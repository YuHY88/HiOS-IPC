/*
 * OSPF Link State Advertisement
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "lib/linklist.h"
#include "lib/prefix.h"
#include "lib/if.h"
#include "lib/table.h"
#include "lib/memory.h"
#include "lib/stream.h"
#include "lib/log.h"
#include "lib/thread.h"
#include "lib/timer.h"
#include "lib/route_types.h"
#include "lib/checksum.h"
#include "lib/route_com.h"
#include "lib/stream.h"
#include "lib/memtypes.h"


#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_abr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_flood.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_ase.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_ipc.h"
#include "lib/devm_com.h"



//extern struct ospf_statics ospf_sta;



u_int32_t
get_metric (u_char *metric)
{
    u_int32_t m;
    m = metric[0];
    m = (m << 8) + metric[1];
    m = (m << 8) + metric[2];
    return m;
}


struct timeval
tv_adjust (struct timeval a)
{
    while (a.tv_usec >= 1000000)
    {
        a.tv_usec -= 1000000;
        a.tv_sec++;
    }

    while (a.tv_usec < 0)
    {
        a.tv_usec += 1000000;
        a.tv_sec--;
    }

    return a;
}

int
tv_ceil (struct timeval a)
{
    a = tv_adjust (a);
    return (a.tv_usec ? a.tv_sec + 1 : a.tv_sec);
}

int
tv_floor (struct timeval a)
{
    a = tv_adjust (a);
    return a.tv_sec;
}

struct timeval
int2tv (int a)
{
    struct timeval ret;

    ret.tv_sec = a;
    ret.tv_usec = 0;

    return ret;
}

struct timeval
msec2tv (int a)
{
    struct timeval ret;

    ret.tv_sec = 0;
    ret.tv_usec = a * 1000;

    return tv_adjust (ret);
}

struct timeval
tv_add (struct timeval a, struct timeval b)
{
    struct timeval ret;

    ret.tv_sec = a.tv_sec + b.tv_sec;
    ret.tv_usec = a.tv_usec + b.tv_usec;

    return tv_adjust (ret);
}

struct timeval
tv_sub (struct timeval a, struct timeval b)
{
    struct timeval ret;

    ret.tv_sec = a.tv_sec - b.tv_sec;
    ret.tv_usec = a.tv_usec - b.tv_usec;

    return tv_adjust (ret);
}

int
tv_cmp (struct timeval a, struct timeval b)
{
    return (a.tv_sec == b.tv_sec ?
            a.tv_usec - b.tv_usec : a.tv_sec - b.tv_sec);
}

int
ospf_lsa_refresh_delay (struct ospf_lsa *lsa)
{
    struct timeval delta, now;
    int delay = 0;
    time_get_time (TIME_CLK_MONOTONIC, &now);
    delta = tv_sub (now, lsa->tv_orig);
    if (tv_cmp (delta, msec2tv (OSPF_MIN_LS_INTERVAL)) < 0)
    {
        delay = tv_ceil (tv_sub (msec2tv (OSPF_MIN_LS_INTERVAL), delta));
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Refresh timer delay %d seconds",
                        lsa->data->type, inet_ntoa (lsa->data->id), delay);
		if(delay <= 0)
		{
			zlog_warn("%s[%d] delay <= 0",__func__,__LINE__);
			return 0;
		}
        assert (delay > 0);
    }
    return delay;
}


int
get_age (struct ospf_lsa *lsa)
{
    int age;
    age = ntohs (lsa->data->ls_age)
          + tv_floor (tv_sub (time_get_recent_relative_time (), lsa->tv_recv));
    return age;
}


/* Fletcher Checksum -- Refer to RFC1008. */

/* All the offsets are zero-based. The offsets in the RFC1008 are
   one-based. */
u_int16_t
ospf_lsa_checksum (struct lsa_header *lsa)
{
    u_char *buffer = (u_char *) &lsa->options;
    int options_offset = buffer - (u_char *) &lsa->ls_age; /* should be 2 */
    /* Skip the AGE field */
    u_int16_t len = ntohs(lsa->length) - options_offset;
    /* Checksum offset starts from "options" field, not the beginning of the
       lsa_header struct. The offset is 14, rather than 16. */
    int checksum_offset = (u_char *) &lsa->checksum - buffer;
    return fletcher_checksum(buffer, len, checksum_offset);
}

int
ospf_lsa_checksum_valid (struct lsa_header *lsa)
{
    u_char *buffer = (u_char *) &lsa->options;
    int options_offset = buffer - (u_char *) &lsa->ls_age; /* should be 2 */
    /* Skip the AGE field */
    u_int16_t len = ntohs(lsa->length) - options_offset;
    return(fletcher_checksum(buffer, len, FLETCHER_CHECKSUM_VALIDATE) == 0);
}



/* Create OSPF LSA. */
struct ospf_lsa *
ospf_lsa_new ()
{
    struct ospf_lsa *new_lsa;

	do
	{		
		new_lsa = XCALLOC (MTYPE_OSPF_LSA, sizeof (struct ospf_lsa));
	
		if (new_lsa == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(new_lsa == NULL);
	if(new_lsa == NULL)
    {
        zlog_err("Can't creat ospf_lsa : malloc failed");
        return (struct ospf_lsa *)NULL;
    }
	memset(new_lsa, 0, sizeof(struct ospf_lsa));

    new_lsa->flags = 0;
	new_lsa->opaque_lsa_flag = 0;
    new_lsa->lock = 1;
    new_lsa->retransmit_counter = 0;
    new_lsa->tv_recv = time_get_recent_relative_time ();
    new_lsa->tv_orig = new_lsa->tv_recv;
    new_lsa->refresh_list = -1;
    ospf_sta.os_lsa++;

    return new_lsa;
}

/* Duplicate OSPF LSA. */
struct ospf_lsa *
ospf_lsa_dup (struct ospf_lsa *lsa)
{
    struct ospf_lsa *new_lsa;

    if (lsa == NULL)
    {
        return NULL;
    }

	do
	{		
		new_lsa = XCALLOC (MTYPE_OSPF_LSA, sizeof (struct ospf_lsa));
	
		if (new_lsa == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(new_lsa == NULL);
	if(new_lsa == NULL)
    {
        zlog_err("Can't creat ospf_lsa : malloc failed");
        return (struct ospf_lsa *)NULL;
    }

    memcpy (new_lsa, lsa, sizeof (struct ospf_lsa));
    UNSET_FLAG (new_lsa->flags, OSPF_LSA_DISCARD);
    new_lsa->lock = 1;
    new_lsa->retransmit_counter = 0;
    new_lsa->data = ospf_lsa_data_dup (lsa->data);

    /* kevinm: Clear the refresh_list, otherwise there are going
       to be problems when we try to remove the LSA from the
       queue (which it's not a member of.)
       XXX: Should we add the LSA to the refresh_list queue? */
    new_lsa->refresh_list = -1;

    if (IS_DEBUG_OSPF (lsa, LSA))
    {
        zlog_debug (OSPF_DBG_LSA, "LSA: duplicated %p (new: %p)", (void *)lsa, (void *)new_lsa);
    }

    return new_lsa;
}

/* Free OSPF LSA. */
void
ospf_lsa_free (struct ospf_lsa *lsa)
{
	if(lsa->lock != 0)
	{
		zlog_warn("%s[%d] lsa->lock != 0",__func__,__LINE__);
		return ;
	}
    assert (lsa->lock == 0);
    if (IS_DEBUG_OSPF (lsa, LSA))
    {
        zlog_debug (OSPF_DBG_LSA, "LSA: freed %p", (void *)lsa);
    }
    /* Delete LSA data. */
    if (lsa->data != NULL)
    {
        ospf_lsa_data_free (lsa->data);
    }
	lsa->data = NULL;
	if(lsa->refresh_list >= 0)
	{
		zlog_warn("%s[%d] lsa->refresh_list >= 0",__func__,__LINE__);
		return ;
	}
    assert (lsa->refresh_list < 0);
	
    memset (lsa, 0, sizeof (struct ospf_lsa));
    XFREE (MTYPE_OSPF_LSA, lsa);
    ospf_sta.os_lsa--;
}

/* Lock LSA. */
struct ospf_lsa *
ospf_lsa_lock (struct ospf_lsa *lsa)
{
    lsa->lock++;
    return lsa;
}

/* Unlock LSA. */
void
ospf_lsa_unlock (struct ospf_lsa **lsa)
{
    /* This is sanity check. */
    if (!lsa || !*lsa)
    {
        return;
    }
    (*lsa)->lock--;
    assert ((*lsa)->lock >= 0);
    if ((*lsa)->lock == 0)
    {
        assert (CHECK_FLAG ((*lsa)->flags, OSPF_LSA_DISCARD));
        ospf_lsa_free (*lsa);
        *lsa = NULL;
    }
}

/* Check discard flag. */
void
ospf_lsa_discard (struct ospf_lsa *lsa)
{
    if (!CHECK_FLAG (lsa->flags, OSPF_LSA_DISCARD))
    {
        SET_FLAG (lsa->flags, OSPF_LSA_DISCARD);
        ospf_lsa_unlock (&lsa);
    }
}

/* Create LSA data. */
struct lsa_header *
ospf_lsa_data_new (size_t size)
{	
	struct lsa_header *lsah = NULL;

	do
	{		
		lsah = XCALLOC (MTYPE_OSPF_LSA_DATA, size); 
	
		if (lsah == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(lsah == NULL);
    return lsah;
}

/* Duplicate LSA data. */
struct lsa_header *
ospf_lsa_data_dup (struct lsa_header *lsah)
{
    struct lsa_header *new_lsa;

    new_lsa = ospf_lsa_data_new (ntohs (lsah->length));
	if(new_lsa == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa, lsah, ntohs (lsah->length));

    return new_lsa;
}

/* Free LSA data. */
void
ospf_lsa_data_free (struct lsa_header *lsah)
{
    if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug (OSPF_DBG_LSA, "LSA[Type%d:%s]: data freed %p",
                    lsah->type, inet_ntoa (lsah->id), (void *)lsah);
    XFREE (MTYPE_OSPF_LSA_DATA, lsah);
}


/* LSA general functions. */

const char *
dump_lsa_key (struct ospf_lsa *lsa)
{
    static char buf[] =
    {
        "Type255,id(255.255.255.255),ar(255.255.255.255)"
    };
    struct lsa_header *lsah;
    if (lsa != NULL && (lsah = lsa->data) != NULL)
    {
        char id[INET_ADDRSTRLEN], ar[INET_ADDRSTRLEN];
        strcpy (id, inet_ntoa (lsah->id));
        strcpy (ar, inet_ntoa (lsah->adv_router));
        sprintf (buf, "Type%d,id(%s),ar(%s)", lsah->type, id, ar);
    }
    else
    {
        strcpy (buf, "NULL");
    }
    return buf;
}

u_int32_t
lsa_seqnum_increment (struct ospf_lsa *lsa)
{
    u_int32_t seqnum;
    seqnum = ntohl (lsa->data->ls_seqnum) + 1;
    return htonl (seqnum);
}

void
lsa_header_set (struct stream *s, u_char options,
                u_char type, struct in_addr id, struct in_addr router_id)
{
    struct lsa_header *lsah;
    lsah = (struct lsa_header *) STREAM_DATA (s);
    lsah->ls_age = htons (OSPF_LSA_INITIAL_AGE);
    lsah->options = options;
    lsah->type = type;
    lsah->id = id;
    lsah->adv_router = router_id;
    lsah->ls_seqnum = htonl (OSPF_INITIAL_SEQUENCE_NUMBER);
    stream_forward_endp (s, OSPF_LSA_HEADER_SIZE);
}


/* router-LSA related functions. */
/* Get router-LSA flags. */
static u_char
router_lsa_flags (struct ospf_area *area)
{
    u_char flags;
    flags = area->ospf->flags;
    /* Set virtual link flag. */
    if (ospf_full_virtual_nbrs (area))
    {
        SET_FLAG (flags, ROUTER_LSA_VIRTUAL);
    }
    else
        /* Just sanity check */
    {
        UNSET_FLAG (flags, ROUTER_LSA_VIRTUAL);
    }
    /* Set Shortcut ABR behabiour flag. */
    UNSET_FLAG (flags, ROUTER_LSA_SHORTCUT);
    if (area->ospf->abr_type == OSPF_ABR_SHORTCUT)
        if (!OSPF_IS_AREA_BACKBONE (area))
            if ((area->shortcut_configured == OSPF_SHORTCUT_DEFAULT &&
                    area->ospf->backbone == NULL) ||
                    area->shortcut_configured == OSPF_SHORTCUT_ENABLE)
            {
                SET_FLAG (flags, ROUTER_LSA_SHORTCUT);
            }
    /* ASBR can't exit in stub area. */
    if (area->external_routing == OSPF_AREA_STUB)
    {
        UNSET_FLAG (flags, ROUTER_LSA_EXTERNAL);
    }
    /* If ASBR set External flag */
    else if (IS_OSPF_ASBR (area->ospf))
    {
        SET_FLAG (flags, ROUTER_LSA_EXTERNAL);
    }
    /* Set ABR dependent flags */
    if (IS_OSPF_ABR (area->ospf))
    {
        SET_FLAG (flags,  ROUTER_LSA_BORDER);
        /* If Area is NSSA and we are both ABR and unconditional translator,
         * set Nt bit to inform other routers.
         */
        if ( (area->external_routing == OSPF_AREA_NSSA)
                && (area->NSSATranslatorRole == OSPF_NSSA_ROLE_ALWAYS))
        {
            SET_FLAG (flags, ROUTER_LSA_NT);
        }
    }
    return flags;
}

/* Lookup neighbor other than myself.
   And check neighbor count,
   Point-to-Point link must have only 1 neighbor. */
struct ospf_neighbor *
ospf_nbr_lookup_ptop (struct ospf_interface *oi)
{
    struct ospf_neighbor *nbr = NULL;
    struct route_node *rn;

    /* Search neighbor, there must be one of two nbrs. */
    for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
    {
        if ((nbr = rn->info))
        {
            if (!IPV4_ADDR_SAME (&nbr->router_id, &oi->ospf->router_id))
            {
                if (nbr->state == NSM_Full)
                {
                    route_unlock_node (rn);
                    break;
                }
            }
        }
    }

    /* PtoP link must have only 1 neighbor. */
    if (ospf_nbr_count (oi, 0) > 1)
    {
        zlog_warn ("Point-to-Point link has more than 1 neighobrs.");
    }

    return nbr;
}

/* Determine cost of link, taking RFC3137 stub-router support into
 * consideration
 */
static u_int16_t
ospf_link_cost (struct ospf_interface *oi)
{
    /* RFC3137 stub router support */
    if (!CHECK_FLAG (oi->area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED))
    {
        return oi->output_cost;
    }
    else
    {
        return OSPF_OUTPUT_COST_INFINITE;
    }
}

/* Set a link information. */
static char
link_info_set (struct stream *s, struct in_addr id,
               struct in_addr data, u_char type, u_char tos, u_int16_t cost)
{
    /* LSA stream is initially allocated to OSPF_MAX_LSA_SIZE, suits
     * vast majority of cases. Some rare routers with lots of links need more.
     * we try accomodate those here.
     */
    if (STREAM_WRITEABLE(s) < OSPF_ROUTER_LSA_LINK_SIZE)
    {
        size_t ret = OSPF_MAX_LSA_SIZE;
        /* Can we enlarge the stream still? */
        if (STREAM_SIZE(s) == OSPF_MAX_LSA_SIZE)
        {
            /* we futz the size here for simplicity, really we need to account
             * for just:
             * IP Header - (sizeof (struct ip))
             * OSPF Header - OSPF_HEADER_SIZE
             * LSA Header - OSPF_LSA_HEADER_SIZE
             * MD5 auth data, if MD5 is configured - OSPF_AUTH_MD5_SIZE.
             *
             * Simpler just to subtract OSPF_MAX_LSA_SIZE though.
             */
            ret = stream_resize (s, OSPF_MAX_PACKET_SIZE - OSPF_MAX_LSA_SIZE);
        }
        if (ret == OSPF_MAX_LSA_SIZE)
        {
            zlog_warn ("%s: Out of space in LSA stream, left %zd, size %zd",
                       __func__, STREAM_REMAIN (s), STREAM_SIZE (s));
            return 0;
        }
    }
    /* TOS based routing is not supported. */
    stream_put_ipv4 (s, id.s_addr);       /* Link ID. */
    stream_put_ipv4 (s, data.s_addr);     /* Link Data. */
    stream_putc (s, type);            /* Link Type. */
    stream_putc (s, tos);             /* TOS = 0. */
    stream_putw (s, cost);            /* Link Cost. */
    return 1;
}

/* Describe Point-to-Point link (Section 12.4.1.1). */
static int
lsa_link_ptop_set (struct stream *s, struct ospf_interface *oi)
{
    int links = 0;
    struct ospf_neighbor *nbr;
    struct in_addr id, mask;
	struct listnode *node = NULL;
	struct u0_device_info_local *u0_device = NULL;
    u_int16_t cost = ospf_link_cost (oi);
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Set link Point-to-Point");
    }
    if ((nbr = ospf_nbr_lookup_ptop (oi)))
    {
        if (nbr->state == NSM_Full)
        {
            /* For unnumbered point-to-point networks, the Link Data field
               should specify the interface's MIB-II ifIndex value. */
            links += link_info_set (s, nbr->router_id, oi->address->u.prefix4,
                                    LSA_LINK_TYPE_POINTOPOINT, 0, cost);
        }
		else//add by zzl 20190103 for u0 manger
		{
			//printf("%s[%d]:before add u0 p2p links 0\n", __FUNCTION__, __LINE__);
			if(om->u0_flag && (listcount(om->u0_list) > 0))
			{
				//printf("%s[%d]:before add u0 p2p links\n", __FUNCTION__, __LINE__);
				for (ALL_LIST_ELEMENTS_RO (om->u0_list, node, u0_device))
				{
					if(u0_device->u0_info.ifindex == oi->ifp->ifindex)
					{	
						//printf("%s[%d]: add u0 p2p links: u0 ne-ip:%s\n", __FUNCTION__, __LINE__, inet_ntoa(u0_device->ne_ip));
						links += link_info_set (s, u0_device->u0_info.ne_ip, oi->address->u.prefix4,
	                                    LSA_LINK_TYPE_POINTOPOINT, 0, cost);
						break;
					}
				}
			}
		}
    }
	
    /* Regardless of the state of the neighboring router, we must
       add a Type 3 link (stub network).
       N.B. Options 1 & 2 share basically the same logic. */
    masklen2ip (oi->address->prefixlen, &mask);
    id.s_addr = CONNECTED_PREFIX(oi->connected)->u.prefix4.s_addr & mask.s_addr;
    links += link_info_set (s, id, mask, LSA_LINK_TYPE_STUB, 0,
                            oi->output_cost);
    return links;
}

/* Describe Broadcast Link. */
static int
lsa_link_broadcast_set (struct stream *s, struct ospf_interface *oi)
{
    struct ospf_neighbor *dr;
    struct in_addr id, mask;
    u_int16_t cost = ospf_link_cost (oi);
    /* Describe Type 3 Link. */
    if (oi->state == ISM_Waiting)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Interface %s is in state Waiting. "
                        "Adding stub interface", oi->ifp->name);
        }
        masklen2ip (oi->address->prefixlen, &mask);
        id.s_addr = oi->address->u.prefix4.s_addr & mask.s_addr;
        return link_info_set (s, id, mask, LSA_LINK_TYPE_STUB, 0,
                              oi->output_cost);
    }
    dr = ospf_nbr_lookup_by_addr (oi->nbrs, &DR (oi));
    /* Describe Type 2 link. */
    if (dr && (dr->state == NSM_Full ||
               IPV4_ADDR_SAME (&oi->address->u.prefix4, &DR (oi))) &&
            ospf_nbr_count (oi, NSM_Full) > 0)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Interface %s has a DR. "
                        "Adding transit interface", oi->ifp->name);
        }
        return link_info_set (s, DR (oi), oi->address->u.prefix4,
                              LSA_LINK_TYPE_TRANSIT, 0, cost);
    }
    /* Describe type 3 link. */
    else
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Interface %s has no DR. "
                        "Adding stub interface", oi->ifp->name);
        }
        masklen2ip (oi->address->prefixlen, &mask);
        id.s_addr = oi->address->u.prefix4.s_addr & mask.s_addr;
        return link_info_set (s, id, mask, LSA_LINK_TYPE_STUB, 0,
                              oi->output_cost);
    }
}

static int
lsa_link_loopback_set (struct stream *s, struct ospf_interface *oi)
{
    struct in_addr id, mask;
    /* Describe Type 3 Link. */
    if (oi->state != ISM_Loopback)
    {
        return 0;
    }
    mask.s_addr = 0xffffffff;
    id.s_addr = oi->address->u.prefix4.s_addr;
    /******** new **********/
    return link_info_set(s, id, mask, LSA_LINK_TYPE_STUB, 0, 0);
}

/* Describe Virtual Link. */
static int
lsa_link_virtuallink_set (struct stream *s, struct ospf_interface *oi)
{
    struct ospf_neighbor *nbr;
    u_int16_t cost = ospf_link_cost (oi);
    if (oi->state == ISM_PointToPoint)
    {
        if ((nbr = ospf_nbr_lookup_ptop (oi)))
        {
            if (nbr->state == NSM_Full)
            {
                return link_info_set (s, nbr->router_id, oi->address->u.prefix4,
                                      LSA_LINK_TYPE_VIRTUALLINK, 0, cost);
            }
        }
    }
    return 0;
}

#define lsa_link_nbma_set(S,O)  lsa_link_broadcast_set (S, O)

/* this function add for support point-to-multipoint ,see rfc2328
12.4.1.4.*/
/* from "edward rrr" <edward_rrr@hotmail.com>
   http://marc.theaimsgroup.com/?l=zebra&m=100739222210507&w=2 */
static int
lsa_link_ptomp_set (struct stream *s, struct ospf_interface *oi)
{
    int links = 0;
    struct route_node *rn;
    struct ospf_neighbor *nbr = NULL;
    struct in_addr id, mask;
    u_int16_t cost = ospf_link_cost (oi);
    mask.s_addr = 0xffffffff;
    id.s_addr = oi->address->u.prefix4.s_addr;
    links += link_info_set (s, id, mask, LSA_LINK_TYPE_STUB, 0, 0);
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "PointToMultipoint: running ptomultip_set");
    }
    /* Search neighbor, */
    for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
    {
        if ((nbr = rn->info) != NULL)
        {
            /* Ignore myself. */
            if (!IPV4_ADDR_SAME (&nbr->router_id, &oi->ospf->router_id))
                if (nbr->state == NSM_Full)
                {
                    links += link_info_set (s, nbr->router_id, oi->address->u.prefix4,
                                            LSA_LINK_TYPE_POINTOPOINT, 0, cost);
                    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                        zlog_debug (OSPF_DBG_LSA_GENERATE, "PointToMultipoint: set link to %s",
                                    inet_ntoa(oi->address->u.prefix4));
                }
        }
    }
    return links;
}

/* Set router-LSA link information. */
static int
router_lsa_link_set (struct stream *s, struct ospf_area *area)
{
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;
    int links = 0;
    for (ALL_LIST_ELEMENTS_RO (area->oiflist, node, oi))
    {
        struct interface *ifp = oi->ifp;
        /* Check interface is up, OSPF is enable. */
        if (if_is_operative (ifp))
        {
            if (oi->state != ISM_Down)
            {
                oi->lsa_pos_beg = links;
                /* Describe each link. */
                switch (oi->type)
                {
                case OSPF_IFTYPE_POINTOPOINT:
                    links += lsa_link_ptop_set (s, oi);
                    break;
                case OSPF_IFTYPE_BROADCAST:
                    links += lsa_link_broadcast_set (s, oi);
                    break;
                case OSPF_IFTYPE_NBMA:
                    links += lsa_link_nbma_set (s, oi);
                    break;
                case OSPF_IFTYPE_POINTOMULTIPOINT:
                    links += lsa_link_ptomp_set (s, oi);
                    break;
                case OSPF_IFTYPE_VIRTUALLINK:
                    links += lsa_link_virtuallink_set (s, oi);
                    break;
                case OSPF_IFTYPE_LOOPBACK:
                    links += lsa_link_loopback_set (s, oi);
					break;
				default:
					break;
                }
                oi->lsa_pos_end = links;
            }
        }
    }
    return links;
}

/* Set router-LSA body. */
static void
ospf_router_lsa_body_set (struct stream *s, struct ospf_area *area)
{
    unsigned long putp;
    u_int16_t cnt;
    /* Set flags. */
    stream_putc (s, router_lsa_flags (area));
    /* Set Zero fields. */
    stream_putc (s, 0);
    /* Keep pointer to # links. */
    putp = stream_get_endp(s);
    /* Forward word */
    stream_putw(s, 0);
    /* Set all link information. */
    cnt = router_lsa_link_set (s, area);
    /* Set # of links here. */
    stream_putw_at (s, putp, cnt);
}

static int
ospf_stub_router_timer (void *t)
{
    struct ospf_area *area = (struct ospf_area *)(t);
    if(area == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(area);
	
    area->t_stub_router = 0;
    SET_FLAG (area->stub_router_state, OSPF_AREA_WAS_START_STUB_ROUTED);
    /* clear stub route state and generate router-lsa refresh, don't
     * clobber an administratively set stub-router state though.
     */
    if (CHECK_FLAG (area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED))
    {
        return 0;
    }
    UNSET_FLAG (area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED);
    ospf_router_lsa_update_area (area);
    return 0;
}

static void
ospf_stub_router_check (struct ospf_area *area)
{
    /* area must either be administratively configured to be stub
     * or startup-time stub-router must be configured and we must in a pre-stub
     * state.
     */
    if (CHECK_FLAG (area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED))
    {
        SET_FLAG (area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED);
        return;
    }
    /* not admin-stubbed, check whether startup stubbing is configured and
     * whether it's not been done yet
     */
    if (CHECK_FLAG (area->stub_router_state, OSPF_AREA_WAS_START_STUB_ROUTED))
    {
        return;
    }
    if (area->ospf->stub_router_startup_time == OSPF_STUB_ROUTER_UNCONFIGURED)
    {
        /* stub-router is hence done forever for this area, even if someone
         * tries configure it (take effect next restart).
         */
        SET_FLAG (area->stub_router_state, OSPF_AREA_WAS_START_STUB_ROUTED);
        return;
    }
    /* startup stub-router configured and not yet done */
    SET_FLAG (area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED);
    OSPF_AREA_TIMER_ON (area->t_stub_router, ospf_stub_router_timer,
                        area->ospf->stub_router_startup_time);
}

/* Create new router-LSA. */
static struct ospf_lsa *
ospf_router_lsa_new (struct ospf_area *area)
{
    struct ospf *ospf = area->ospf;
    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new_lsa;
    int length;

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Create router-LSA instance");
    }

    /* check whether stub-router is desired, and if this is the first
    * router LSA.
    */
    ospf_stub_router_check (area);

    /* Create a stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}
	
    /* Set LSA common header fields. */
    lsa_header_set (s, LSA_OPTIONS_GET (area) | LSA_OPTIONS_NSSA_GET (area),
                    OSPF_ROUTER_LSA, ospf->router_id, ospf->router_id);

    /* Set router-LSA body fields. */
    ospf_router_lsa_body_set (s, area);

    /* Set length. */
    length = stream_get_endp (s);
    lsah = (struct lsa_header *) STREAM_DATA (s);
    lsah->length = htons (length);

    /* Now, create OSPF LSA instance. */
    if ( (new_lsa = ospf_lsa_new ()) == NULL)
    {
        zlog_err ("%s: Unable to create new lsa", __func__);
        return NULL;
    }

    new_lsa->area = area;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

    /* Copy LSA data to store, discard stream. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    return new_lsa;
}

/* Originate Router-LSA. */
static struct ospf_lsa *
ospf_router_lsa_originate (struct ospf_area *area)
{
    struct ospf_lsa *new_lsa;

    /* Create new router-LSA instance. */
    if ( (new_lsa = ospf_router_lsa_new (area)) == NULL)
    {
        zlog_err ("%s: ospf_router_lsa_new returned NULL", __func__);
        return NULL;
    }

    /* Sanity check. */
    if (new_lsa->data->adv_router.s_addr == 0)
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "LSA[Type1]: AdvRouter is 0, discard");
        }
        ospf_lsa_discard (new_lsa);
        return NULL;
    }

    /* Install LSA to LSDB. */
    new_lsa = ospf_lsa_install (area->ospf, NULL, new_lsa);

    /* Update LSA origination count. */
    area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    ospf_flood_through_area (area, NULL, new_lsa);

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate router-LSA %p",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id), (void *)new_lsa);
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

/* Refresh router-LSA. */
struct ospf_lsa *ospf_router_lsa_refresh (struct ospf_lsa *lsa)
{
    struct ospf_area *area = lsa->area;
    struct ospf_lsa *new_lsa;

    /* Sanity check. */
    assert (lsa->data);

    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_area (area, lsa);

    /* Unregister LSA from refresh-list */
    ospf_refresher_unregister_lsa (area->ospf, lsa);

    /* Create new router-LSA instance. */
    if ( (new_lsa = ospf_router_lsa_new (area)) == NULL)
    {
        zlog_err ("%s: ospf_router_lsa_new returned NULL", __func__);
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

    ospf_lsa_install (area->ospf, NULL, new_lsa);

    /* Flood LSA through area. */
    ospf_flood_through_area (area, NULL, new_lsa);

    /* Debug logging. */
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: router-LSA refresh",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id));
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

int
ospf_router_lsa_update_area (struct ospf_area *area)
{
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "[router-LSA]: (router-LSA area update)");
    }
    
    if(area == NULL)
    {
        return 0;
    }
    
    /* Now refresh router-LSA. */
    if (area->router_lsa_self)
    {
        
        if(area->act_ints > 0)
        {
            ospf_lsa_refresh (area->ospf, area->router_lsa_self);
        }
        else
        {
            ospf_lsa_flush_area(area->router_lsa_self, area);            
			ospf_lsa_unlock (&area->router_lsa_self);
            area->router_lsa_self = NULL;
        }
    }
    /* Newly originate router-LSA. */
    else
    {
        ospf_router_lsa_originate (area);
    }
    return 0;
}

int
ospf_router_lsa_update (struct ospf *ospf)
{
    struct listnode *node, *nnode;
    struct ospf_area *area;
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "Timer[router-LSA Update]: (timer expire)");
    }
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        struct ospf_lsa *lsa = area->router_lsa_self;
        struct router_lsa *rl;
        const char *area_str;
        /* Keep Area ID string. */
        area_str = AREA_NAME (area);
        /* If LSA not exist in this Area, originate new. */
        if (lsa == NULL)
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            {
                zlog_debug(OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Create router-LSA for Area %s", area_str);
            }
            ospf_router_lsa_originate (area);
        }
        /* If router-ID is changed, Link ID must change.
            First flush old LSA, then originate new. */
        else if (!IPV4_ADDR_SAME (&lsa->data->id, &ospf->router_id))
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                zlog_debug(OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Refresh router-LSA for Area %s",
                           lsa->data->type, inet_ntoa (lsa->data->id), area_str);
            ospf_refresher_unregister_lsa (ospf, lsa);
            ospf_lsa_flush_area (lsa, area);
            ospf_lsa_unlock (&area->router_lsa_self);
            area->router_lsa_self = NULL;
            /* Refresh router-LSA, (not install) and flood through area. */
            ospf_router_lsa_update_area (area);
        }
        else
        {
            rl = (struct router_lsa *) lsa->data;
            /* Refresh router-LSA, (not install) and flood through area. */
            if(!IS_AREA_STUB(lsa->area))
            {
                if (rl->flags != ospf->flags)
                {
                    ospf_router_lsa_update_area (area);
                }
            }
            else
            {
            	if(CHECK_FLAG(rl->flags,OSPF_FLAG_ASBR))
                //if(((rl->flags) & OSPF_FLAG_ASBR) == 1)
                {
                    if (rl->flags != ospf->flags)
                    {
                        ospf_router_lsa_update_area (area);
                    }
                }
            }
        }
    }
    return 0;
}

/**************************router-lsa handle for u0 manger start*******************************/
/* Describe Point-to-Point link (Section 12.4.1.1). */
static int
lsa_link_ptop_set_for_u0 (struct stream *s, struct ospf_interface *oi, struct u0_device_info *u0_device)
{
    int links = 0;
    //struct ospf_neighbor *nbr;
    struct in_addr id, mask;
	
    u_int16_t cost = ospf_link_cost (oi);
	if(NULL == u0_device)
	{
		return 0;
	}
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "%s[%d]:LSA[Type1]: Set link Point-to-Point", __FUNCTION__, __LINE__);
    }

    /* host access list Describe Type 3 Link. */
    mask.s_addr = 0xffffffff;
    id.s_addr = u0_device->ne_ip.s_addr;
    links += link_info_set(s, id, mask, LSA_LINK_TYPE_STUB, 0, 0);
	
	if(u0_device->ifindex == oi->ifp->ifindex)
	{
		//link ID == local router_id, link data == u0 ne-ip
		links += link_info_set (s, oi->ospf->router_id, u0_device->ne_ip,
                        LSA_LINK_TYPE_POINTOPOINT, 0, cost);
	}

    /* Regardless of the state of the neighboring router, we must
       add a Type 3 link (stub network).
       N.B. Options 1 & 2 share basically the same logic. */
    masklen2ip (oi->address->prefixlen, &mask);
    id.s_addr = u0_device->ne_ip.s_addr & mask.s_addr;
    links += link_info_set (s, id, mask, LSA_LINK_TYPE_STUB, 0,
                            oi->output_cost);
    return links;
}


static int
router_lsa_link_set_for_u0 (struct stream *s, struct ospf_area *area, struct u0_device_info *u0_device)
{
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;
    int links = 0;

	if(NULL == u0_device)
	{
		zlog_debug(OSPF_DBG_LSA_GENERATE, "%s[%d] u0_device == NULL, return!\n",  __FUNCTION__, __LINE__);
		return 0;	
	}
	
    for (ALL_LIST_ELEMENTS_RO (area->oiflist, node, oi))
    {
        struct interface *ifp = oi->ifp;
        /* Check interface is up, OSPF is enable. */
        if ((ifp->ifindex == u0_device->ifindex) && (if_is_operative (ifp)))
        {
            if (oi->state != ISM_Down)
            {
                oi->lsa_pos_beg = links;
                /* Describe each link. */
                switch (oi->type)
                {
                case OSPF_IFTYPE_POINTOPOINT:
                    links += lsa_link_ptop_set_for_u0 (s, oi, u0_device);
                    break;
                case OSPF_IFTYPE_BROADCAST:
                    links += lsa_link_broadcast_set (s, oi);
                    break;
                case OSPF_IFTYPE_NBMA:
                    links += lsa_link_nbma_set (s, oi);
                    break;
                case OSPF_IFTYPE_POINTOMULTIPOINT:
                    links += lsa_link_ptomp_set (s, oi);
                    break;
                case OSPF_IFTYPE_VIRTUALLINK:
                    links += lsa_link_virtuallink_set (s, oi);
                    break;
                case OSPF_IFTYPE_LOOPBACK:
                    links += lsa_link_loopback_set (s, oi);
					break;
				default:
					break;
                }
                oi->lsa_pos_end = links;
            }
        }
    }
    return links;
}


static void
ospf_router_lsa_body_set_for_u0 (struct stream *s, struct ospf_area *area, struct u0_device_info *u0_device)
{
    unsigned long putp;
    u_int16_t cnt;
    /* Set flags. */
    stream_putc (s, router_lsa_flags (area));
    /* Set Zero fields. */
    stream_putc (s, 0);
    /* Keep pointer to # links. */
    putp = stream_get_endp(s);
    /* Forward word */
    stream_putw(s, 0);
    /* Set all link information. */
    cnt = router_lsa_link_set_for_u0 (s, area, u0_device);
    /* Set # of links here. */
    stream_putw_at (s, putp, cnt);
}

static struct ospf_lsa *
ospf_router_lsa_new_for_u0 (struct ospf_area *area, struct u0_device_info *u0_device)
{
    //struct ospf *ospf = area->ospf;
    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new_lsa;
    int length;

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: Create router-LSA instance");
    }

    /* check whether stub-router is desired, and if this is the first
    * router LSA.
    */
    ospf_stub_router_check (area);

    /* Create a stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}
	
    /* Set LSA common header fields. */
    lsa_header_set (s, LSA_OPTIONS_GET (area) | LSA_OPTIONS_NSSA_GET (area),
                    OSPF_ROUTER_LSA, u0_device->ne_ip, u0_device->ne_ip);

    /* Set router-LSA body fields. */
    ospf_router_lsa_body_set_for_u0 (s, area, u0_device);

    /* Set length. */
    length = stream_get_endp (s);
    lsah = (struct lsa_header *) STREAM_DATA (s);
    lsah->length = htons (length);

    /* Now, create OSPF LSA instance. */
    if ( (new_lsa = ospf_lsa_new ()) == NULL)
    {
        zlog_err ("%s: Unable to create new lsa", __func__);
        return NULL;
    }

    new_lsa->area = area;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

    /* Copy LSA data to store, discard stream. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    return new_lsa;
}


struct ospf_lsa *
ospf_router_lsa_originate_for_u0 (struct ospf_area *area, struct u0_device_info *u0_device)
{
    struct ospf_lsa *new_lsa;

    /* Create new router-LSA instance. */
    if ( (new_lsa = ospf_router_lsa_new_for_u0 (area, u0_device)) == NULL)
    {
        zlog_err ("%s: ospf_router_lsa_new returned NULL", __func__);
        return NULL;
    }

    /* Sanity check. */
    if (new_lsa->data->adv_router.s_addr == 0)
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type1]: AdvRouter is 0, discard");
        }
        ospf_lsa_discard (new_lsa);
        return NULL;
    }

    /* Install LSA to LSDB. */
    new_lsa = ospf_lsa_install (area->ospf, NULL, new_lsa);

    /* Update LSA origination count. */
    area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    ospf_flood_through_area (area, NULL, new_lsa);

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate router-LSA %p",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id), (void *)new_lsa);
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

/* Refresh router-LSA. */
struct ospf_lsa *ospf_router_lsa_refresh_for_u0 (struct ospf_lsa *lsa)
{
	//printf("%s[%d]:lsa refresh(%s)\n", __FUNCTION__, __LINE__, dump_lsa_key(lsa));
    struct ospf_area *area = lsa->area;
    struct ospf_lsa *new_lsa = NULL;
	struct u0_device_info_local *u0_device = NULL;

    /* Sanity check. */
    assert (lsa->data);

    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_area (area, lsa);

    /* Unregister LSA from refresh-list */
    ospf_refresher_unregister_lsa (area->ospf, lsa);

	/* lookup target u0 device info*/
	if((u0_device = lookup_u0_device_by_adv_router(lsa->data->adv_router)) == NULL)
	{
		zlog_err("%s()[%d]: Target u0 device is not exist!", __FUNCTION__, __LINE__);
	  	return NULL;
	}
	
    /* Create new router-LSA instance. */
    if ( (new_lsa = ospf_router_lsa_new_for_u0 (area, &(u0_device->u0_info))) == NULL)
    {
        zlog_err ("%s: ospf_router_lsa_new returned NULL", __func__);
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

    ospf_lsa_install (area->ospf, NULL, new_lsa);

    /* Flood LSA through area. */
    ospf_flood_through_area (area, NULL, new_lsa);

    /* Debug logging. */
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: router-LSA refresh",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id));
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

/**************************router-lsa handle for u0 manger end*******************************/


/* network-LSA related functions. */
/* Originate Network-LSA. */
void ospf_network_lsa_body_set (struct stream *s, struct ospf_interface *oi)
{
    struct in_addr mask;
    struct route_node *rn;
    struct ospf_neighbor *nbr;
    masklen2ip (oi->address->prefixlen, &mask);
    stream_put_ipv4 (s, mask.s_addr);
    /* The network-LSA lists those routers that are fully adjacent to
      the Designated Router; each fully adjacent router is identified by
      its OSPF Router ID.  The Designated Router includes itself in this
      list. RFC2328, Section 12.4.2 */
    for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
    {
        if ((nbr = rn->info) != NULL)
        {
            if (nbr->state == NSM_Full || nbr == oi->nbr_self)
            {
                stream_put_ipv4 (s, nbr->router_id.s_addr);
            }
        }
    }
}

struct ospf_lsa *ospf_network_lsa_new (struct ospf_interface *oi)
{
    struct stream *s;
    struct ospf_lsa *new_lsa;
    struct lsa_header *lsah;
    struct ospf_if_params *oip;
    int length;

    /* If there are no neighbours on this network (the net is stub),
       the router does not originate network-LSA (see RFC 12.4.2) */
    if (oi->full_nbrs == 0)
    {
        return NULL;
    }

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type2]: Create network-LSA instance");
    }

    /* Create new stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}
    lsah = (struct lsa_header *) STREAM_DATA (s);

    lsa_header_set (s, (OPTIONS (oi) | LSA_OPTIONS_GET (oi->area)),
                    OSPF_NETWORK_LSA, DR (oi), oi->ospf->router_id);

    /* Set network-LSA body fields. */
    ospf_network_lsa_body_set (s, oi);

    /* Set length. */
    length = stream_get_endp (s);
    lsah->length = htons (length);

    /* Create OSPF LSA instance. */
    if ( (new_lsa = ospf_lsa_new ()) == NULL)
    {
        zlog_err ("%s: ospf_lsa_new returned NULL", __func__);
        return NULL;
    }

    new_lsa->area = oi->area;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

    /* Copy LSA to store. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    /* Remember prior network LSA sequence numbers, even if we stop
     * originating one for this oi, to try avoid re-originating LSAs with a
     * prior sequence number, and thus speed up adjency forming & convergence.
     */
    if ((oip = ospf_lookup_if_params (oi->ifp, oi->address->u.prefix4)))
    {
        new_lsa->data->ls_seqnum = oip->network_lsa_seqnum;
        new_lsa->data->ls_seqnum = lsa_seqnum_increment (new_lsa);
    }
    else
    {
        oip = ospf_get_if_params (oi->ifp, oi->address->u.prefix4);
        ospf_if_update_params (oi->ifp, oi->address->u.prefix4);
    }
    oip->network_lsa_seqnum = new_lsa->data->ls_seqnum;

    return new_lsa;
}

/* Originate network-LSA. */
void
ospf_network_lsa_update (struct ospf_interface *oi)
{
    struct ospf_lsa *new_lsa;
    if (oi->network_lsa_self != NULL)
    {
        ospf_lsa_refresh (oi->ospf, oi->network_lsa_self);
        return;
    }
    /* Create new network-LSA instance. */
    new_lsa = ospf_network_lsa_new (oi);
    if (new_lsa == NULL)
    {
        return;
    }
    /* Install LSA to LSDB. */
    new_lsa = ospf_lsa_install (oi->ospf, oi, new_lsa);
    /* Update LSA origination count. */
    oi->ospf->lsa_originate_count++;
    /* Flooding new LSA through area. */
    ospf_flood_through_area (oi->area, NULL, new_lsa);
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate network-LSA %p",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id), (void *)new_lsa);
        ospf_lsa_header_dump (new_lsa->data);
    }
    return;
}

struct ospf_lsa *ospf_network_lsa_refresh (struct ospf_lsa *lsa)
{
    struct ospf_area *area = lsa->area;
    struct ospf_lsa *new1, *new2;
    struct ospf_if_params *oip;
    struct ospf_interface *oi;

    assert (lsa->data);

    /* Retrieve the oi for the network LSA */
    oi = ospf_if_lookup_by_local_addr (area->ospf, NULL, lsa->data->id);
    if (oi == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: network-LSA refresh: "
                        "no oi found, ick, ignoring.",
                        lsa->data->type, inet_ntoa (lsa->data->id));
            ospf_lsa_header_dump (lsa->data);
        }
        return NULL;
    }
    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_area (area, lsa);

    /* Unregister LSA from refresh-list */
    ospf_refresher_unregister_lsa (area->ospf, lsa);

    /* Create new network-LSA instance. */
    new1 = ospf_network_lsa_new (oi);
    if (new1 == NULL)
    {
        return NULL;
    }

    oip = ospf_lookup_if_params (oi->ifp, oi->address->u.prefix4);
	if(oip == NULL)
	{
		zlog_warn("%s %d oip is NULL",__func__,__LINE__);
		return NULL;
	}
    assert (oip != NULL);
    oip->network_lsa_seqnum = new1->data->ls_seqnum = lsa_seqnum_increment (lsa);

    new2 = ospf_lsa_install (area->ospf, oi, new1);

    assert (new2 == new1);

    /* Flood LSA through aera. */
    ospf_flood_through_area (area, NULL, new1);

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: network-LSA refresh",
                    new1->data->type, inet_ntoa (new1->data->id));
        ospf_lsa_header_dump (new1->data);
    }

    return new1;
}

void stream_put_ospf_metric (struct stream *s, u_int32_t metric_value)
{
    u_int32_t metric;
    char *mp;
    /* Put 0 metric. TOS metric is not supported. */
    metric = htonl (metric_value);
    mp = (char *) &metric;
    mp++;
    stream_put (s, mp, 3);
}

/* summary-LSA related functions. */
void ospf_summary_lsa_body_set (struct stream *s, struct prefix *p, u_int32_t metric)
{
    struct in_addr mask;
    masklen2ip (p->prefixlen, &mask);
    /* Put Network Mask. */
    stream_put_ipv4 (s, mask.s_addr);
    /* Set # TOS. */
    stream_putc (s, (u_char) 0);
    /* Set metric. */
    stream_put_ospf_metric (s, metric);
}

struct ospf_lsa * ospf_summary_lsa_new (struct ospf_area *area, struct prefix *p,
                      u_int32_t metric, struct in_addr id)
{
    struct stream *s = NULL;
    struct ospf_lsa *new_lsa;
    struct lsa_header *lsah;
    int length;

    if (id.s_addr == 0xffffffff)
    {
        /* Maybe Link State ID not available. */
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d]: LS ID not available, can't originate",
                        OSPF_SUMMARY_LSA);
        return NULL;
    }

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type3]: Create summary-LSA instance");
    }

    /* Create new stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}

    lsah = (struct lsa_header *) STREAM_DATA (s);

    lsa_header_set (s, LSA_OPTIONS_GET (area), OSPF_SUMMARY_LSA,
                    id, area->ospf->router_id);

    /* Set summary-LSA body fields. */
    ospf_summary_lsa_body_set (s, p, metric);

    /* Set length. */
    length = stream_get_endp (s);
    lsah->length = htons (length);

    /* Create OSPF LSA instance. */
    new_lsa = ospf_lsa_new ();
    new_lsa->area = area;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

    /* Copy LSA to store. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    return new_lsa;
}

/* Originate Summary-LSA. */
struct ospf_lsa *
ospf_summary_lsa_originate (struct prefix_ipv4 *p, u_int32_t metric,
                            struct ospf_area *area)
{
    struct ospf_lsa *new_lsa;
    struct in_addr id;
    u_int32_t ls_seqnum = OSPF_MAX_SEQUENCE_NUMBER;

    id = ospf_lsa_unique_id (area->ospf, area->lsdb, OSPF_SUMMARY_LSA, p, &ls_seqnum, area);

    if (id.s_addr == 0xffffffff)
    {
        /* Maybe Link State ID not available. */
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d]: LS ID not available, can't originate",
                        OSPF_SUMMARY_LSA);
        return NULL;
    }

    /* Create new summary-LSA instance. */
    if ( !(new_lsa = ospf_summary_lsa_new (area, (struct prefix *) p, metric, id)))
    {
        return NULL;
    }

    if (ls_seqnum != OSPF_MAX_SEQUENCE_NUMBER)
    {
        new_lsa->data->ls_seqnum = htonl(ls_seqnum);
    }

    /* Instlal LSA to LSDB. */
    new_lsa = ospf_lsa_install (area->ospf, NULL, new_lsa);

    /* Update LSA origination count. */
    area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    ospf_flood_through_area (area, NULL, new_lsa);

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate summary-LSA %p",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id), (void *)new_lsa);
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

struct ospf_lsa *ospf_summary_lsa_refresh (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct ospf_lsa *new_lsa;
    struct summary_lsa *sl;
    struct prefix p;

    /* Sanity check. */
    assert (lsa->data);

    sl = (struct summary_lsa *)lsa->data;
    p.prefixlen = ip_masklen (sl->mask);
    new_lsa = ospf_summary_lsa_new (lsa->area, &p, GET_METRIC (sl->metric),
                                sl->header.id);

    if (!new_lsa)
    {
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

    ospf_lsa_install (ospf, NULL, new_lsa);

    /* Flood LSA through AS. */
    ospf_flood_through_area (new_lsa->area, NULL, new_lsa);

    /* Debug logging. */
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: summary-LSA refresh",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id));
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}


/* summary-ASBR-LSA related functions. */
void ospf_summary_asbr_lsa_body_set (struct stream *s, struct prefix *p,
                                u_int32_t metric)
{
    /* Put Network Mask. */
    stream_put_ipv4 (s, (u_int32_t) 0);
    /* Set # TOS. */
    stream_putc (s, (u_char) 0);
    /* Set metric. */
    stream_put_ospf_metric (s, metric);
}

struct ospf_lsa *ospf_summary_asbr_lsa_new (struct ospf_area *area, struct prefix *p,
                           u_int32_t metric, struct in_addr id)
{
    struct stream *s;
    struct ospf_lsa *new_lsa;
    struct lsa_header *lsah;
    int length;

    if (id.s_addr == 0xffffffff)
    {
        /* Maybe Link State ID not available. */
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d]: LS ID not available, can't originate",
                        OSPF_ASBR_SUMMARY_LSA);
        return NULL;
    }

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type3]: Create summary-LSA instance");
    }

    /* Create new stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}

    lsah = (struct lsa_header *) STREAM_DATA (s);

    lsa_header_set (s, LSA_OPTIONS_GET (area), OSPF_ASBR_SUMMARY_LSA,
                    id, area->ospf->router_id);

    /* Set summary-LSA body fields. */
    ospf_summary_asbr_lsa_body_set (s, p, metric);

    /* Set length. */
    length = stream_get_endp (s);
    lsah->length = htons (length);

    /* Create OSPF LSA instance. */
    new_lsa = ospf_lsa_new ();
    new_lsa->area = area;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

    /* Copy LSA to store. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    return new_lsa;
}

/* Originate summary-ASBR-LSA. */
struct ospf_lsa *
ospf_summary_asbr_lsa_originate (struct prefix_ipv4 *p, u_int32_t metric,
                                 struct ospf_area *area)
{
    struct ospf_lsa *new_lsa;
    struct in_addr id;
    u_int32_t ls_seqnum = OSPF_MAX_SEQUENCE_NUMBER;

    id = ospf_lsa_unique_id (area->ospf, area->lsdb, OSPF_ASBR_SUMMARY_LSA, p, &ls_seqnum, area);

    if (id.s_addr == 0xffffffff)
    {
        /* Maybe Link State ID not available. */
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d]: LS ID not available, can't originate",
                        OSPF_ASBR_SUMMARY_LSA);
        return NULL;
    }

    /* Create new summary-LSA instance. */
    new_lsa = ospf_summary_asbr_lsa_new (area, (struct prefix *) p, metric, id);
    if (!new_lsa)
    {
        return NULL;
    }

    if (ls_seqnum != OSPF_MAX_SEQUENCE_NUMBER)
    {
        new_lsa->data->ls_seqnum = htonl(ls_seqnum);
    }

    /* Install LSA to LSDB. */
    new_lsa = ospf_lsa_install (area->ospf, NULL, new_lsa);

    /* Update LSA origination count. */
    area->ospf->lsa_originate_count++;

    /* Flooding new LSA through area. */
    ospf_flood_through_area (area, NULL, new_lsa);

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate summary-ASBR-LSA %p",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id), (void *)new_lsa);
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

struct ospf_lsa *ospf_summary_asbr_lsa_refresh (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct ospf_lsa *new_lsa;
    struct summary_lsa *sl;
    struct prefix p;

    /* Sanity check. */
    assert (lsa->data);

    sl = (struct summary_lsa *)lsa->data;
    p.prefixlen = ip_masklen (sl->mask);
    new_lsa = ospf_summary_asbr_lsa_new (lsa->area, &p, GET_METRIC (sl->metric),
                                     sl->header.id);
    if (!new_lsa)
    {
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

    ospf_lsa_install (ospf, NULL, new_lsa);

    /* Flood LSA through area. */
    ospf_flood_through_area (new_lsa->area, NULL, new_lsa);

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: summary-ASBR-LSA refresh",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id));
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

/* AS-external-LSA related functions. */

/* Get nexthop for AS-external-LSAs.  Return nexthop if its interface
   is connected, else 0*/
struct in_addr ospf_external_lsa_nexthop_get (struct ospf *ospf, struct in_addr nexthop)
{
    struct in_addr fwd;
    struct prefix nh;
    struct listnode *node;
    struct ospf_interface *oi;

    fwd.s_addr = 0;

    if (!nexthop.s_addr)
    {
        return fwd;
    }

    /* Check whether nexthop is covered by OSPF network. */
    nh.family = AF_INET;
    nh.u.prefix4 = nexthop;
    nh.prefixlen = IPV4_MAX_BITLEN;

    /* XXX/SCALE: If there were a lot of oi's on an ifp, then it'd be
     * better to make use of the per-ifp table of ois.
     */
    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        if (if_is_operative (oi->ifp))
        {
            if (oi->address->family == AF_INET)
            {
                if (prefix_match (oi->address, &nh))
                {
                    if(oi->type == OSPF_IFTYPE_BROADCAST || oi->type == OSPF_IFTYPE_NBMA)
                    {
                        if (OSPF_IF_PASSIVE_STATUS (oi) == OSPF_IF_ACTIVE)
                        {
                            return nexthop;
                        }
                    }
                }
            }
        }
    }

    return fwd;
}


int
ospf_external_lsa_check_ei_and_lsa(struct ospf * ospf, 
                        struct external_info *ei , struct ospf_lsa *lsa)
{
    assert(ei);
    assert(lsa);
    struct as_external_lsa *al = NULL;
    struct in_addr fwd_addr;
	
	if(ei == NULL)
	{
		return 0;
	}
    al = (struct as_external_lsa *) lsa->data;
    
    if((int)(GET_METRIC (al->e[0].metric)) !=  (metric_value (ospf, ei->type,ei->instance)))
    {
        return 1;
    }
    if((IS_EXTERNAL_METRIC (al->e[0].tos) ? EXTERNAL_METRIC_TYPE_2 : EXTERNAL_METRIC_TYPE_1)
                    != (metric_type(ospf, ei->type,ei->instance)))
    {
        return 1;
    }

    fwd_addr = ospf_external_lsa_nexthop_get (ospf, ei->nexthop);
    if(al->e[0].fwd_addr.s_addr != fwd_addr.s_addr)
    {
        return 1;
    }
    return 0;

}


/* NSSA-external-LSA related functions. */

/* Get 1st IP connection for Forward Addr */

struct in_addr
ospf_get_ip_from_ifp (struct ospf_interface *oi)
{
    struct in_addr fwd;

    fwd.s_addr = 0;

    if (if_is_operative (oi->ifp))
    {
        return oi->address->u.prefix4;
    }

    return fwd;
}

/* Get 1st IP connection for Forward Addr */
static struct in_addr
ospf_get_nssa_ip (struct ospf_area *area)
{
    struct in_addr fwd;
    struct in_addr best_default;
    struct listnode *node;
    struct ospf_interface *oi;

    fwd.s_addr = 0;
    best_default.s_addr = 0;

    for (ALL_LIST_ELEMENTS_RO (area->ospf->oiflist, node, oi))
    {
        if (if_is_operative (oi->ifp))
        {
            if (oi->area->external_routing == OSPF_AREA_NSSA)
            {
                if (oi->address && oi->address->family == AF_INET)
                {
                    if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
                    {
                        continue;
                    }
					
                    if (best_default.s_addr == 0)
                    {
                        best_default = oi->address->u.prefix4;
                    }
                    if (oi->area == area)
                    {
                        return oi->address->u.prefix4;
                    }
                }
            }
        }
    }
    if (best_default.s_addr != 0)
    {
        return best_default;
    }

    return fwd;
}

#define DEFAULT_DEFAULT_METRIC               20
#define DEFAULT_DEFAULT_ORIGINATE_METRIC     10
#define DEFAULT_DEFAULT_ALWAYS_METRIC         1

#define DEFAULT_METRIC_TYPE          EXTERNAL_METRIC_TYPE_2

int
metric_type (struct ospf *ospf, u_char src, u_int8_t instance)
{
    return (ospf->dmetric[src][instance].type < 0 ?
            DEFAULT_METRIC_TYPE : ospf->dmetric[src][instance].type);
}

int
metric_value (struct ospf *ospf, u_char src, u_int8_t instance)
{
    if (ospf->dmetric[src][instance].value < 0)
    {
        if (src == DEFAULT_ROUTE)
        {
            if (ospf->default_originate == DEFAULT_ORIGINATE_ZEBRA)
            {
                return DEFAULT_DEFAULT_ORIGINATE_METRIC;
            }
            else
            {
                return DEFAULT_DEFAULT_ALWAYS_METRIC;
            }
        }
        else if (ospf->default_metric < 0)
        {
            return DEFAULT_DEFAULT_METRIC;
        }
        else
        {
            return ospf->default_metric;
        }
    }
    return ospf->dmetric[src][instance].value;
}

/* Set AS-external-LSA body. */
void ospf_external_lsa_body_set (struct stream *s, struct external_info *ei, struct ospf *ospf)
{
    int type;
    int mtype;
    u_int32_t mvalue;
    struct in_addr mask, fwd_addr;
    struct prefix_ipv4 *p = &ei->p;
    struct ospf_summary *summary = NULL;
	
    /* Put Network Mask. */
    masklen2ip (p->prefixlen, &mask);
    stream_put_ipv4 (s, mask.s_addr);
    /* If prefix is default, specify DEFAULT_ROUTE. */
    type = is_prefix_default (&ei->p) ? DEFAULT_ROUTE : ei->type;
	
	/*summary route*/
	if(ROUTE_PROTO_OSPF == ei->type && ei->instance == ospf->ospf_id)
	{
		summary = ospf_summary_lookup (ospf, p);
		if(summary != NULL)
		{
			mtype = summary->type;
			mvalue = summary->cost;
		}
		else
		{
			zlog_warn("%s %d summary route is NULL,%s/%d",__func__,__LINE__,inet_ntoa (p->prefix), p->prefixlen);
			mtype = (ROUTEMAP_METRIC_TYPE (ei) != -1) ?
	            ROUTEMAP_METRIC_TYPE (ei) : metric_type (ospf, type, ei->instance);
			mvalue = (ROUTEMAP_METRIC (ei) != -1) ?
	             ROUTEMAP_METRIC (ei) : metric_value (ospf, type ,ei->instance);
		}
	}
	else
	{
	    mtype = (ROUTEMAP_METRIC_TYPE (ei) != -1) ?
	            ROUTEMAP_METRIC_TYPE (ei) : metric_type (ospf, type, ei->instance);
	    mvalue = (ROUTEMAP_METRIC (ei) != -1) ?
	             ROUTEMAP_METRIC (ei) : metric_value (ospf, type ,ei->instance);
	}
    /* Put type of external metric. */
    stream_putc (s, (mtype == EXTERNAL_METRIC_TYPE_2 ? 0x80 : 0));
    /* Put 0 metric. TOS metric is not supported. */
    stream_put_ospf_metric (s, mvalue);
    /* Get forwarding address to nexthop if on the Connection List, else 0. */
    fwd_addr = ospf_external_lsa_nexthop_get (ospf, ei->nexthop);
    /* Put forwarding address. */
    stream_put_ipv4 (s, fwd_addr.s_addr);
    /* Put route tag */
    stream_putl(s, ei->tag);
}

/* Create new external-LSA. */
struct ospf_lsa *
ospf_external_lsa_new (struct ospf *ospf,
                       struct external_info *ei, struct in_addr *old_id)
{
    struct stream *s;
    struct lsa_header *lsah;
    struct ospf_lsa *new_lsa;
    struct in_addr id;
    int length;
    u_int32_t ls_seqnum = OSPF_MAX_SEQUENCE_NUMBER;

    if (ei == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type5]: External info is NULL, can't originate");
        }
        return NULL;
    }

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type5]: Originate AS-external-LSA instance");
    }

    /* If old Link State ID is specified, refresh LSA with same ID. */
    if (old_id)
    {
        id = *old_id;
    }
    /* Get Link State with unique ID. */
    else
    {
        id = ospf_lsa_unique_id (ospf, ospf->lsdb, OSPF_AS_EXTERNAL_LSA, &ei->p, &ls_seqnum, NULL);
        if (id.s_addr == 0xffffffff)
        {
            /* Maybe Link State ID not available. */
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            {
                zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type5]: LS ID not available, can't originate");
            }
            return NULL;
        }
    }

    /* Create new stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}
    lsah = (struct lsa_header *) STREAM_DATA (s);

    /* Set LSA common header fields. */
    lsa_header_set (s, OSPF_OPTION_E, OSPF_AS_EXTERNAL_LSA,
                    id, ospf->router_id);
    if (ls_seqnum != OSPF_MAX_SEQUENCE_NUMBER)
    {
        lsah->ls_seqnum = htonl(ls_seqnum);
    }

    /* Set AS-external-LSA body fields. */
    ospf_external_lsa_body_set (s, ei, ospf);

    /* Set length. */
    length = stream_get_endp (s);
    lsah->length = htons (length);

    /* Now, create OSPF LSA instance. */
    new_lsa = ospf_lsa_new ();
    new_lsa->area = NULL;
	new_lsa->adv_type7.s_addr = 0;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_APPROVED | OSPF_LSA_SELF_CHECKED);

    /* Copy LSA data to store, discard stream. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    return new_lsa;
}

/*
RFC 14.4.4.1 diag 16 for NSSA
return 0:存在相同目标、距离值和非零的转发地址
return -1:不存在
*/
static int
ospf_nssa_lsa_originate_condiation (struct ospf_area *area, struct ospf_lsa *nssalsa)
{
    struct route_node *rn;
    struct ospf_route *or;
    struct listnode *node;
    struct ospf_lsa *lsa;
    struct as_external_lsa *al;
    struct as_external_lsa *al_new;
    int flag = 0;
    al_new = (struct as_external_lsa *)nssalsa->data;
    if (al_new->e[0].fwd_addr.s_addr == 0)
    {
        return -1;
    }

	if(area->ospf->new_rtrs == NULL)
	{
		zlog_warn("ospf->new_rtrs is NULL %s %d",__func__,__LINE__);
		return -1;
	}
	
    for (rn = route_top (area->ospf->new_rtrs); rn; rn = route_next (rn))
    {
        flag = 0;
        if (rn->info)
        {
            for (ALL_LIST_ELEMENTS_RO ((struct list *)rn->info, node, or))
            {
                if (or->u.std.flags & ROUTER_LSA_EXTERNAL)
                {
                    /*找到可达的ASBR*/
                    flag = 1;
                    break;
                }
            }
        }
        /*如果没有找到ASBR，则继续查找ASBR*/
        if (flag == 0)
        {
            continue;
        }
        /*是否存在由可达路由器生成的相同目标的LSA*/
        lsa = ospf_lsdb_lookup_by_id (area->lsdb, OSPF_AS_NSSA_LSA,
                                      nssalsa->data->id, rn->p.u.prefix4);
        if (lsa)
        {
            al = (struct as_external_lsa *)lsa->data;
            /*是否相同目标、距离值和非零的转发地址*/
            if (IS_LSA_MAXAGE(lsa))
            {
                continue;
            }
            /*
            如果存在相同目标、距离值和非零的转发地址且
            OSPF 路由器标识大于本路由器标识的AS-external-LSA，则
            无须再生成该5类LSA。
            */
            if(IPV4_ADDR_CMP(&area->ospf->router_id, &rn->p.u.prefix4) < 0 &&
                    ((int)(GET_METRIC (al->e[0].metric)) ==  (int)(GET_METRIC (al_new->e[0].metric))) &&
                    ((IS_EXTERNAL_METRIC (al->e[0].tos) ? EXTERNAL_METRIC_TYPE_2 : EXTERNAL_METRIC_TYPE_1)
                     == (IS_EXTERNAL_METRIC (al_new->e[0].tos) ? EXTERNAL_METRIC_TYPE_2 : EXTERNAL_METRIC_TYPE_1)) &&
                    al->e[0].fwd_addr.s_addr == al_new->e[0].fwd_addr.s_addr)
            {
                return 0;
            }
        }
    }
    return -1;
}


/*
    As Type-7 originate(when ei is null)or refresh(when ei is not null)
*/
void
ospf_install_flood_nssa (struct ospf *ospf,
                         struct ospf_lsa *lsa, struct external_info *ei)
{
    struct ospf_lsa *new_lsa;
    struct ospf_lsa *old;
    struct prefix p;
    struct as_external_lsa *exl = NULL;
    unsigned char plength = 0;
    struct as_external_lsa *extlsa;
    struct ospf_area *area;
    struct listnode *node, *nnode;
    /* LSA may be a Type-5 originated via translation of a Type-7 LSA
    * which originated from an NSSA area. In which case it should not be
    * flooded back to NSSA areas.
    */
    if (CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT))
    {
        return;
    }
    if(IS_LSA_MAXAGE(lsa))
    {
        return;
    }
    p.family = AF_INET;
    p.u.prefix4.s_addr = lsa->data->id.s_addr;
    exl = (struct as_external_lsa *)(lsa->data);
    plength = ip_masklen(exl->mask);
    p.prefixlen = MIN(IPV4_MAX_PREFIXLEN,plength);
    /* NSSA Originate or Refresh (If anyNSSA)

    LSA is self-originated. And just installed as Type-5.
    Additionally, install as Type-7 LSDB for every attached NSSA.

    P-Bit controls which ABR performs translation to outside world; If
    we are an ABR....do not set the P-bit, because we send the Type-5,
    not as the ABR Translator, but as the ASBR owner within the AS!

    If we are NOT ABR, Flood through NSSA as Type-7 w/P-bit set.  The
    elected ABR Translator will see the P-bit, Translate, and re-flood.

    Later, ABR_TASK and P-bit will scan Type-7 LSDB and translate to
    Type-5's to non-NSSA Areas.  (it will also attempt a re-install) */
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        /* Don't install Type-7 LSA's into nonNSSA area */
        if (!IS_AREA_NSSA(area))
        {
            continue;
        }
        if(ei == NULL)
        {
            old = ospf_lsa_lookup_by_prefix (area->lsdb, OSPF_AS_NSSA_LSA,
                                             (struct prefix_ipv4 *)&p,
                                             area->ospf->router_id);
            if(old != NULL && !IS_LSA_MAXAGE(old))
            {
                continue;
            }
        }
        /* make lsa duplicate, lock=1 */
        new_lsa = ospf_lsa_dup (lsa);
        new_lsa->area = area;
        new_lsa->data->type = OSPF_AS_NSSA_LSA;
        new_lsa->tv_recv = time_get_recent_relative_time ();
        new_lsa->tv_orig = new_lsa->tv_recv;
        new_lsa->data->ls_age = htons (OSPF_LSA_INITIAL_AGE);
        UNSET_FLAG(lsa->flags, OSPF_LSA_IN_MAXAGE);
        /* fix BugID=39582 by zhangyun start*/
        UNSET_FLAG(new_lsa->data->options, OSPF_OPTION_E);
        /* fix BugID=39582 by zhangyun end*/
        /* set P-bit if not ABR */
        if (! IS_OSPF_ABR (ospf))
        {
            SET_FLAG(new_lsa->data->options, OSPF_OPTION_NP);
            /* set non-zero FWD ADDR

                draft-ietf-ospf-nssa-update-09.txt

                if the network between the NSSA AS boundary router and the
                adjacent AS is advertised into OSPF as an internal OSPF route,
                the forwarding address should be the next op address as is cu
                currently done with type-5 LSAs.  If the intervening network is
                not adversited into OSPF as an internal OSPF route and the
                type-7 LSA's P-bit is set a forwarding address should be
                selected from one of the router's active OSPF inteface addresses
                which belong to the NSSA.  If no such addresses exist, then
                no type-7 LSA's with the P-bit set should originate from this
                router.   */
            /* kevinm: not updating lsa anymore, just new */
            extlsa = (struct as_external_lsa *)(new_lsa->data);
            if (extlsa->e[0].fwd_addr.s_addr == 0)
            {
                extlsa->e[0].fwd_addr = ospf_get_nssa_ip(area);    /* this NSSA area in ifp */
            }
            if (extlsa->e[0].fwd_addr.s_addr == 0)
            {
                if (IS_DEBUG_OSPF_NSSA)
                {
                    zlog_debug (OSPF_DBG_NSSA, "LSA[Type-7]: Could not build FWD-ADDR");
                }
                ospf_lsa_discard (new_lsa);
                return;
            }
        }
        SET_FLAG (new_lsa->flags, OSPF_LSA_SELF);
        /*如果forwarding address 不为零，判断是否需要安装*/
        if (ospf_nssa_lsa_originate_condiation(area, new_lsa) == 0)
        {
            ospf_lsa_discard (new_lsa);
            continue;
        }
        /* install also as Type-7 */
        ospf_lsa_install (ospf, NULL, new_lsa);   /* Remove Old, Lock New = 2 */
        /* will send each copy, lock=2+n */
        ospf_flood_through_as (ospf, NULL, new_lsa); /* all attached NSSA's, no AS/STUBs */
    }
}

/*orignate Type-7 LSA ONLY*/
void
ospf_nssa_lsa_orignate_only(struct ospf *ospf, struct external_info *ei)
{
    struct ospf_lsa *new_lsa;
    /* Create new AS-external-LSA instance. */
    if ((new_lsa = ospf_external_lsa_new (ospf, ei, &ei->p.prefix)) == NULL)
    {
        return ;
    }
    /* orignate type 7 LSA ,delete the new*/
    ospf_install_flood_nssa (ospf, new_lsa, NULL); /* Install/Flood Type-7 to all NSSAs */
    ospf_lsa_discard(new_lsa);
    return ;
}


struct ospf_lsa *
ospf_nssa_lsa_refresh (struct ospf *ospf, struct ospf_lsa *lsa,
                       struct external_info *ei, int force)
{
    struct ospf_lsa *lsa_tmp = NULL;
    int changed;
    struct ospf_summary *summary = NULL;

    if(ei == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed, "
                        "external info is NULL",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        ospf_nssa_lsa_flush (ospf, &ei->p);
        return NULL;
    }

    /* Check the NSSA LSA should be originated. */
    if (!ospf_redistribute_check (ospf, ei, &changed))
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed, "
                        "redist check fail",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        ospf_nssa_lsa_flush (ospf, &ei->p);
        return NULL;
    }

    if (!changed && !force)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Not refreshed, not changed/forced",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        return NULL;
    }

    /* Check the NSSA LSA is in the summary list*/
    if(!(ei->type == ROUTE_PROTO_OSPF && ei->instance == ospf->ospf_id))
    {
        if ((summary = ospf_summary_match (ospf, &ei->p)))
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Not refreshed, in summary list",
                            lsa->data->type, inet_ntoa (lsa->data->id));
            return NULL;
        }
    }

    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_as (ospf, lsa);

    /* Unregister AS-external-LSA from refresh-list. */
    ospf_refresher_unregister_lsa (ospf, lsa);

    lsa_tmp = ospf_external_lsa_new (ospf, ei, &lsa->data->id);

    if (lsa_tmp == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed", lsa->data->type,
                        inet_ntoa (lsa->data->id));
        return NULL;
    }

    lsa_tmp->data->ls_seqnum = lsa_seqnum_increment (lsa);
    ospf_install_flood_nssa (ospf, lsa_tmp, ei);
    return lsa_tmp;
}

void
ospf_nssa_lsa_refresh_all(struct ospf *ospf)
{
    struct ospf_area *area = NULL;
    struct listnode *node = NULL,
                    *nnode = NULL;
    struct route_node *rn = NULL;
    struct ospf_lsa * lsa = NULL;
    struct external_info * ei =  NULL;
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        if (! IS_AREA_NSSA(area))
        {
            continue;
        }
        LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
        {
            if(IS_LSA_SELF(lsa))
            {
                ei = ospf_external_info_check (ospf, lsa);
                if(ei != NULL)
                {
                    ospf_nssa_lsa_refresh (ospf, lsa, ei, LSA_REFRESH_FORCE);
                }
                else if(!IS_LSA_MAXAGE(lsa))
                {
                    if (lsa->data->id.s_addr != OSPF_DEFAULT_DESTINATION)
                    {
                        struct prefix_ipv4 p;
                        struct as_external_lsa *al = (struct as_external_lsa*)lsa->data;
                        p.family = AF_INET;
                        p.prefix = lsa->data->id;
                        p.prefixlen = ip_masklen (al->mask);
                        ospf_nssa_lsa_flush(ospf, &p);
                    }
                }
            }
        }
    }
}


static struct ospf_lsa *
ospf_lsa_translated_nssa_new (struct ospf *ospf,
                              struct ospf_lsa *type7)
{

    struct ospf_lsa *new_lsa;
    struct ospf_lsa *lsa;
    struct as_external_lsa *ext, *extnew;
    struct external_info ei;
    struct ospf_summary *summary = NULL;

    ext = (struct as_external_lsa *)(type7->data);

    /* need external_info struct, fill in bare minimum */
    ei.p.family = AF_INET;
    ei.p.prefix = type7->data->id;
    ei.p.prefixlen = ip_masklen (ext->mask);
    ei.type = ROUTE_PROTO_OSPF;
    ei.instance = ospf->ospf_id;
    ei.nexthop = ext->header.adv_router;
    ei.route_map_set.metric = GET_METRIC (ext->e[0].metric);
    ei.route_map_set.metric_type = -1;
    ei.tag = 0;
    if(!((ei.type == ROUTE_PROTO_OSPF) && (ei.instance == ospf->ospf_id)))
    {
        if ((summary = ospf_summary_match (ospf, &ei.p)))
        {
            if (IS_DEBUG_OSPF_EVENT)
                zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:%s]: Could not originate AS-external-LSA:This lsa is summary lsa ",
                            inet_ntoa (ei.p.prefix));
            return NULL;
        }
    }
    if ( (new_lsa = ospf_external_lsa_new (ospf, &ei, &type7->data->id)) == NULL)
    {
        if (IS_DEBUG_OSPF_NSSA)
            zlog_debug (OSPF_DBG_NSSA, "ospf_nssa_translate_originate(): Could not originate "
                        "Translated Type-5 for %s",
                        inet_ntoa (ei.p.prefix));
        return NULL;
    }

    extnew = (struct as_external_lsa *)(new_lsa->data);

    /* copy over Type-7 data to new */
    extnew->e[0].tos = ext->e[0].tos;
    extnew->e[0].route_tag = ext->e[0].route_tag;
    extnew->e[0].fwd_addr.s_addr = ext->e[0].fwd_addr.s_addr;

    /* Check existence of LSA instance. */
    lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA, ei.p.prefix, ospf->router_id);
    if (lsa)
    {
        new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);;
    }
    else
    {
        new_lsa->data->ls_seqnum = htonl(OSPF_INITIAL_SEQUENCE_NUMBER);
    }

    /* add by zhangyun for translating type7 to type5 start */
    if (IS_LSA_MAXAGE(type7))
    {
        new_lsa->data->ls_age = OSPF_LSA_MAXAGE;
    }
    /* add by zhangyun for translating type7 to type5 end */

    /* add translated flag, checksum and lock new lsa */

    new_lsa->adv_type7.s_addr = type7->data->adv_router.s_addr;
    SET_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT); /* Translated from 7  */
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF);
    new_lsa = ospf_lsa_lock (new_lsa);

    return new_lsa;
}

/* Originate Translated Type-5 for supplied Type-7 NSSA LSA */
struct ospf_lsa *
ospf_translated_nssa_originate (struct ospf *ospf, struct ospf_lsa *type7)
{
    struct ospf_lsa *new_lsa;
    struct as_external_lsa *extnew;

    /* we cant use ospf_external_lsa_originate() as we need to set
     * the OSPF_LSA_LOCAL_XLT flag, must originate by hand
     */

    if ( (new_lsa = ospf_lsa_translated_nssa_new (ospf, type7)) == NULL)
    {
        if (IS_DEBUG_OSPF_NSSA)
        {
            zlog_debug (OSPF_DBG_NSSA, "ospf_translated_nssa_originate(): Could not translate "
                        "Type-7, Id %s, to Type-5",
                        inet_ntoa (type7->data->id));
        }
        return NULL;
    }

    extnew = (struct as_external_lsa *)new_lsa;

    if (IS_DEBUG_OSPF_NSSA)
    {
        zlog_debug (OSPF_DBG_NSSA, "ospf_translated_nssa_originate(): "
                    "translated Type 7, installed:");
        ospf_lsa_header_dump (new_lsa->data);
        zlog_debug (OSPF_DBG_NSSA, "   Network mask: %d",ip_masklen (extnew->mask));
        zlog_debug (OSPF_DBG_NSSA, "   Forward addr: %s", inet_ntoa (extnew->e[0].fwd_addr));
    }

    if ( (new_lsa = ospf_lsa_install (ospf, NULL, new_lsa)) == NULL)
    {
        if (IS_DEBUG_OSPF_NSSA)
        {
            zlog_debug (OSPF_DBG_NSSA, "ospf_lsa_translated_nssa_originate(): "
                        "Could not install LSA "
                        "id %s", inet_ntoa (type7->data->id));
        }
        return NULL;
    }

    ospf->lsa_originate_count++;
    ospf_flood_through_as (ospf, NULL, new_lsa);

    return new_lsa;
}

/* Refresh Translated from NSSA AS-external-LSA. 7->5*/
struct ospf_lsa *
ospf_translated_nssa_refresh (struct ospf *ospf, struct ospf_lsa *type7,
                              struct ospf_lsa *type5)
{
    struct ospf_lsa *new_lsa = NULL;

    /* Sanity checks. */
    assert (type7 || type5);
    if (!(type7 || type5))
    {
        return NULL;
    }
    if (type7)
    {
        assert (type7->data);
    }
    if (type5)
    {
        assert (type5->data);
    }
    assert (ospf->anyNSSA);

    /* get required data according to what has been given */
    if (type7 && type5 == NULL)
    {
        /* find the translated Type-5 for this Type-7 */
        struct as_external_lsa *ext = (struct as_external_lsa *)(type7->data);
        struct prefix_ipv4 p =
        {
            .prefix = type7->data->id,
            .prefixlen = ip_masklen (ext->mask),
            .family = AF_INET,
        };
        type5 = ospf_external_info_find_lsa (ospf, &p);
    }
    else if (type5 && type7 == NULL)
    {
        /* find the type-7 from which supplied type-5 was translated,
         * ie find first type-7 with same LSA Id.
         */
        struct listnode *ln, *lnn;
        struct route_node *rn;
        struct ospf_lsa *lsa;
        struct ospf_area *area;
        for (ALL_LIST_ELEMENTS (ospf->areas, ln, lnn, area))
        {
            if (area->external_routing != OSPF_AREA_NSSA
                    && !type7)
            {
                continue;
            }
            LSDB_LOOP (NSSA_LSDB(area), rn, lsa)
            {
                if (lsa->data->id.s_addr == type5->data->id.s_addr)
                {
                    type7 = lsa;
                    break;
                }
            }
        }
    }

    /* do we have type7? */
    if (!type7)
    {
        if (IS_DEBUG_OSPF_NSSA)
            zlog_debug (OSPF_DBG_NSSA, "ospf_translated_nssa_refresh(): no Type-7 found for "
                        "Type-5 LSA Id %s",
                        inet_ntoa (type5->data->id));
        return NULL;
    }

    /* do we have valid translated type5? */
    //if (type5 == NULL || !CHECK_FLAG (type5->flags, OSPF_LSA_LOCAL_XLT) )
    if (type5 == NULL)
    {
        if (IS_DEBUG_OSPF_NSSA)
            zlog_debug (OSPF_DBG_NSSA, "ospf_translated_nssa_refresh(): No translated Type-5 "
                        "found for Type-7 with Id %s",
                        inet_ntoa (type7->data->id));
        return NULL;
    }

    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_as (ospf, type5);

    /* create new translated type 5 LSA */
    if ( (new_lsa = ospf_lsa_translated_nssa_new (ospf, type7)) == NULL)
    {
        if (IS_DEBUG_OSPF_NSSA)
            zlog_debug (OSPF_DBG_NSSA, "ospf_translated_nssa_refresh(): Could not translate "
                        "Type-7 for %s to Type-5",
                        inet_ntoa (type7->data->id));
        return NULL;
    }

    if ( !(new_lsa = ospf_lsa_install (ospf, NULL, new_lsa)) )
    {
        if (IS_DEBUG_OSPF_NSSA)
            zlog_debug (OSPF_DBG_NSSA, "ospf_translated_nssa_refresh(): Could not install "
                        "translated LSA, Id %s",
                        inet_ntoa (type7->data->id));
        return NULL;
    }

    /* Flood LSA through area. */
    ospf_flood_through_as (ospf, NULL, new_lsa);

    return new_lsa;
}

int
is_prefix_default (struct prefix_ipv4 *p)
{
    struct prefix_ipv4 q;
    q.family = AF_INET;
    q.prefix.s_addr = 0;
    q.prefixlen = 0;
    return prefix_same ((struct prefix *) p, (struct prefix *) &q);
}

struct ospf_lsa *
ospf_as_nssa_defaults_lsa_originate (struct ospf *ospf)
{
    struct stream *s = NULL;
    struct ospf_lsa *new_lsa = NULL;
    struct lsa_header *lsah = NULL;
    struct prefix_ipv4 p;
    int length;
    struct ospf_area *area = NULL;
    struct in_addr id;

    p.family = AF_INET;
    p.prefix.s_addr = OSPF_DEFAULT_DESTINATION;
    p.prefixlen = 0;

    id = p.prefix;

    if (!IS_OSPF_ABR(ospf))
    {
        return NULL;
    }

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type7]: Create default AS-NSSA-LSA instance");
    }

    /* Create new stream for LSA. */
    s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}

    lsah = (struct lsa_header *) STREAM_DATA (s);

    lsa_header_set (s, 0x00, OSPF_AS_NSSA_LSA, id, ospf->router_id);
    stream_put_ipv4 (s, OSPF_DEFAULT_DESTINATION);
    /* Put type of external metric. */
    stream_putc (s, 0x80);
    /* Put 0 metric. TOS metric is not supported. */
    stream_put_ospf_metric (s, 1);

    /* Put forwarding address. */
    stream_put_ipv4 (s, OSPF_DEFAULT_DESTINATION);

    /* Put route tag -- This value should be introduced from configuration. */
    stream_putl (s, 1);

    /* Set length. */
    length = stream_get_endp (s);
    lsah->length = htons (length);

    /* Create OSPF LSA instance. */
    new_lsa = ospf_lsa_new ();
    new_lsa->area = area;
    SET_FLAG (new_lsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

    //set_nssa_metric (new, area->default_cost);

    /* Copy LSA to store. */
    new_lsa->data = ospf_lsa_data_new (length);
	if(new_lsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
    memcpy (new_lsa->data, lsah, length);
    stream_free (s);

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_abr_announce_nssa_defaults(): Stop");
    }

    return new_lsa;
}

void ospf_as_nssa_defaults_lsa_flush(struct ospf *ospf)
{
    struct prefix_ipv4 p;
    struct ospf_area *area = NULL;
    struct ospf_lsa *old = NULL;
    struct listnode *node = NULL;
    if(ospf->anyNSSA == 0)
    {
        return ;
    }
    if(IS_OSPF_ABR(ospf))
    {
        return;
    }
    p.family = AF_INET;
    p.prefix.s_addr = OSPF_DEFAULT_DESTINATION;
    p.prefixlen = 0;
    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        if (!IS_AREA_NSSA(area))
        {
            continue;
        }
        old = ospf_lsa_lookup_by_prefix (area->lsdb, OSPF_AS_NSSA_LSA,
                                         &p, area->ospf->router_id);
        if(old == NULL)
        {
            continue;
        }
        if (IS_LSA_MAXAGE(old))
        {
            continue;
        }
        ospf_nssa_lsa_flush(ospf, &p);
    }
    return ;
}

/* Originate an AS-NSSA-LSA, install and flood. 5->7 */
struct ospf_lsa *
ospf_as_nssa_lsa_originate (struct ospf *ospf)
{
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;

    if (IS_DEBUG_OSPF_NSSA)
    {
        zlog_debug (OSPF_DBG_NSSA, "ospf_abr_process_nssa_translate_type5totype7(): Start");
    }
    if(ospf->anyNSSA == 0)
    {
        return NULL;
    }

    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
        if (lsa && IS_LSA_SELF(lsa)&& !IS_LSA_MAXAGE(lsa))
        {
            /* If there is any attached NSSA, do special handling */
            if (ospf->anyNSSA &&
                    /* stay away from translated LSAs! */
                    !(CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT)))
            {
                ospf_install_flood_nssa (ospf, lsa, NULL); /* Install/Flood Type-7 to all NSSAs */
                if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                {
                    zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate AS-NSSA-LSA %p",
                                lsa->data->type, inet_ntoa (lsa->data->id), (void *)lsa);
                }
            }
        }
    }

    if (IS_DEBUG_OSPF_NSSA)
    {
        zlog_debug (OSPF_DBG_NSSA, "ospf_abr_process_nssa_translate_type5totype7(): Stop");
    }

    return lsa;
}

/* Flush any NSSA LSAs for given prefix */
void
ospf_nssa_lsa_flush (struct ospf *ospf, struct prefix_ipv4 *p)
{
    struct listnode *node, *nnode;
    struct ospf_lsa *lsa;
    struct ospf_area *area;
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        if (area->external_routing == OSPF_AREA_NSSA)
        {
            if (!(lsa = ospf_lsa_lookup (area, OSPF_AS_NSSA_LSA, p->prefix,
                                         ospf->router_id)))
            {
                if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
                {
                    zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA: There is no such AS-NSSA-LSA %s/%d in LSDB",
                                inet_ntoa (p->prefix), p->prefixlen);
                }
                continue;
            }
            ospf_ls_retransmit_delete_nbr_area (area, lsa);
            if (!IS_LSA_MAXAGE (lsa))
            {
                ospf_refresher_unregister_lsa (ospf, lsa);
                ospf_lsa_flush_area (lsa, area);
            }
        }
    }
}




/*
RFC 12.4.4.1 diag 16
return 0:存在相同目标、距离值和非零的转发地址
return -1:不存在
*/
int ospf_external_lsa_originate_condiation (struct ospf *ospf, struct external_info *ei)
{
    struct route_node *rn;
    struct ospf_route *or;
    struct listnode *node;
    struct ospf_lsa *lsa;
    struct as_external_lsa *al;
    struct in_addr fwd_addr;
    int flag = 0;
	
	if(ospf->new_rtrs == NULL)
	{
		zlog_warn("ospf->new_rtrs is NULL %s %d",__func__,__LINE__);
		return -1;
	}
	
    for (rn = route_top (ospf->new_rtrs); rn; rn = route_next (rn))
    {
        flag = 0;
        if (rn->info)
        {
            for (ALL_LIST_ELEMENTS_RO ((struct list *)rn->info, node, or))
            {
                if (or->u.std.flags & ROUTER_LSA_EXTERNAL)
                {
                    /*找到可达的ASBR*/
                    flag = 1;
                    break;
                }
            }
        }
        /*如果没有找到ASBR，则继续查找ASBR*/
        if (flag == 0)
        {
            continue;
        }
        /*是否存在由可达路由器生成的相同目标的LSA*/
        lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                      ei->p.prefix, rn->p.u.prefix4);
        if (lsa)
        {
            al = (struct as_external_lsa *)lsa->data;
            /*是否相同目标、距离值和非零的转发地址*/
            if (al->e[0].fwd_addr.s_addr == 0)
            {
                continue;
            }
            if (IS_LSA_MAXAGE(lsa))
            {
                continue;
            }
            /*
            如果存在相同目标、距离值和非零的转发地址且
            OSPF 路由器标识大于本路由器标识的AS-external-LSA，则
            无须再生成该5类LSA。
            */
            fwd_addr = ospf_external_lsa_nexthop_get (ospf, ei->nexthop);
			
			//solve bug#57621 by zzl 20181128
            if(/*IPV4_ADDR_CMP(&ospf->router_id, &rn->p.u.prefix4) < 0 && */((int)(GET_METRIC (al->e[0].metric)) ==  (metric_value (ospf, ei->type,ei->instance)))
                    && ((IS_EXTERNAL_METRIC (al->e[0].tos) ? EXTERNAL_METRIC_TYPE_2 : EXTERNAL_METRIC_TYPE_1)
                        == (metric_type(ospf, ei->type,ei->instance))) && al->e[0].fwd_addr.s_addr == fwd_addr.s_addr)
            {
            	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
				{
		        	zlog_debug (OSPF_DBG_EVENT, "fun:%s[%d]: LSA found the same target, distance value, and non-zero forwarding address",\
								__FUNCTION__, __LINE__);
					zlog_debug (OSPF_DBG_EVENT, "fun:%s[%d]: the lsa route_id: %s, type:%d ",\
								__FUNCTION__, __LINE__, inet_ntoa(al->header.id), al->header.type);
					zlog_debug (OSPF_DBG_EVENT, "fun:%s[%d]: adv_router: %s",\
								__FUNCTION__, __LINE__, inet_ntoa(al->header.adv_router));
				}
                return 0;
            }
        }
    }
    return -1;
}


/* Originate an AS-external-LSA, install and flood. */
struct ospf_lsa *
ospf_external_lsa_originate (struct ospf *ospf, struct external_info *ei)
{
    struct ospf_lsa *new_lsa = NULL;
    struct ospf_summary *summary = NULL;

    /* Added for NSSA project....

       External LSAs are originated in ASBRs as usual, but for NSSA systems.
        there is the global Type-5 LSDB and a Type-7 LSDB installed for
        every area.  The Type-7's are flooded to every IR and every ABR; We
        install the Type-5 LSDB so that the normal "refresh" code operates
        as usual, and flag them as not used during ASE calculations.  The
        Type-7 LSDB is used for calculations.  Each Type-7 has a Forwarding
        Address of non-zero.

        If an ABR is the elected NSSA translator, following SPF and during
        the ABR task it will translate all the scanned Type-7's, with P-bit
        ON and not-self generated, and translate to Type-5's throughout the
        non-NSSA/STUB AS.

        A difference in operation depends whether this ASBR is an ABR
        or not.  If not an ABR, the P-bit is ON, to indicate that any
        elected NSSA-ABR can perform its translation.

        If an ABR, the P-bit is OFF;  No ABR will perform translation and
        this ASBR will flood the Type-5 LSA as usual.

        For the case where this ASBR is not an ABR, the ASE calculations
        are based on the Type-5 LSDB;  The Type-7 LSDB exists just to
        demonstrate to the user that there are LSA's that belong to any
        attached NSSA.

        Finally, it just so happens that when the ABR is translating every
        Type-7 into Type-5, it installs it into the Type-5 LSDB as an
        approved Type-5 (translated from Type-7);  at the end of translation
        if any Translated Type-5's remain unapproved, then they must be
        flushed from the AS.

        */

    /* Check the AS-external-LSA should be originated. */
    if (!ospf_redistribute_check (ospf, ei, NULL))
    {
        return NULL;
    }

    /* Create new AS-external-LSA instance. */

    if(!((ei->type == ROUTE_PROTO_OSPF) && (ei->instance == ospf->ospf_id)))
    {    
        if ((summary = ospf_summary_match (ospf, &ei->p)))
        {
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:%s]: Could not originate AS-external-LSA:This lsa is summary lsa ",
                            inet_ntoa (ei->p.prefix));
            }
			
            return NULL;
        }
    }

    /*
    RFC 14.4.4.1 diag 16
    */
    if (ospf_external_lsa_originate_condiation(ospf, ei) == 0)
    {
        return NULL;
    }

    /* Create new AS-external-LSA instance. */
    if ((new_lsa = ospf_external_lsa_new (ospf, ei, NULL)) == NULL)
    {
        if (IS_DEBUG_OSPF_EVENT)
            zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:%s]: Could not originate AS-external-LSA",
                        inet_ntoa (ei->p.prefix));
        return NULL;
    }

    /* Install newly created LSA into Type-5 LSDB, lock = 1. */
    //if (ospf->anyNSSA == 0 && ospf->anySTUB == 0)/* stay away from translated LSAs! */
    ospf_lsa_install (ospf, NULL, new_lsa);

    /* Update LSA origination count. */
    ospf->lsa_originate_count++;

    /* Flooding new LSA. only to AS (non-NSSA/STUB) */
    ospf_flood_through_as (ospf, NULL, new_lsa);

    /* If there is any attached NSSA, do special handling */
    if (ospf->anyNSSA &&
            /* stay away from translated LSAs! */
            !(CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT)))
    {
        ospf_install_flood_nssa (ospf, new_lsa, ei);    /* Install/Flood Type-7 to all NSSAs */
    }

    /* Debug logging. */
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate AS-external-LSA %p",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id), (void *)new_lsa);
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

/* Originate AS-external-LSA from external info with initial flag. */
int
ospf_external_lsa_originate_timer (void *thread)
{
    struct ospf *ospf = (struct ospf *)(thread);
    struct route_node *rn;
    struct external_info *ei;
    struct route_table *rt;
    int type, instance;
	
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    ospf->t_external_lsa = 0;
    /* Originate As-external-LSA from all type of distribute source. */
    for (type = 0; type < ZEBRA_ROUTE_MAX; type++)
    {
        for (instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance++)
        {
            if (ospf_is_type_redistributed (ospf, type, instance))
            {
                if ((rt = EXTERNAL_INFO (ospf->vpn, type)))
                {
                    for (rn = route_top (rt); rn; rn = route_next (rn))
                    {
                        for(ei = rn->info ; ei != NULL ; ei = ei->next)
                        {
                            if (ospf_is_type_redistributed (ospf,ei->type,ei->instance))
                            {
                                if (!is_prefix_default ((struct prefix_ipv4 *)&ei->p))
                                {
                                    if (!ospf_external_lsa_originate (ospf, ei))
                                    {
                                        zlog_warn ("LSA: AS-external-LSA was not originated.");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

static struct external_info *
ospf_external_info_check_by_fw (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct as_external_lsa *al = NULL;
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
    struct external_info *ei = NULL, *ei1= NULL;
    int type,instance;

    struct in_addr fwd_addr;

    if(!lsa)
    {
		zlog_warn("%s[%d] lsa is NULL",__func__,__LINE__);
        return NULL;
    }
    assert(lsa);

    al = (struct as_external_lsa *) lsa->data;

    p.family = AF_INET;
    p.prefix.s_addr = ((lsa->data->id.s_addr) & (al->mask.s_addr));
    p.prefixlen = ip_masklen (al->mask);

    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        int redist_type = is_prefix_default (&p) ? DEFAULT_ROUTE : type;
        for(instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance ++)
        {
            if (ospf_is_type_redistributed (ospf, redist_type,instance) ||
                    (type == ROUTE_PROTO_OSPF && instance == ospf->ospf_id))
            {
                if (EXTERNAL_INFO (ospf->vpn, type))
                {
                    rn = route_node_lookup (EXTERNAL_INFO (ospf->vpn, type),
                                            (struct prefix *) &p);
                    if (rn)
                    {
                        fwd_addr.s_addr = 0;
                        route_unlock_node (rn);
                        for(ei = rn->info ; ei != NULL ; ei = ei->next)
                        {
                        	fwd_addr = ospf_external_lsa_nexthop_get (ospf, ei->nexthop);
                            if (ospf_is_type_redistributed (ospf,ei->type,ei->instance))
                            {
                                if(fwd_addr.s_addr != 0)
                                {
                                	/*if(al->e[0].fwd_addr.s_addr == fwd_addr.s_addr)
									{
										return NULL;
									}
									else*/
                                	{
                                    	return ei;
                                	}
                                }
                                else
                                {
                                    ei1 = ei;
                                }    
                            }
                            else if(ei->type == ROUTE_PROTO_OSPF && ei->instance == ospf->ospf_id)
                            {
                                if(ei->flags == ei->instance)
                                {
                                    if(al->e[0].fwd_addr.s_addr == 0)
                                    {
                                        if(fwd_addr.s_addr != 0)
                                        {
                                            return ei;
                                        }
                                    }
                                    else
                                    {
                                        if(fwd_addr.s_addr == 0)
                                        {
                                            return ei;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return ei1;
}

static int
ospf_external_lsa_fw_update_timer (void *thread)
{
    struct ospf *ospf = (struct ospf *)(thread);
    struct route_node *rn = NULL;
    struct ospf_lsa *lsa = NULL;
    struct external_info *ei = NULL;
    struct in_addr fwd_addr;
	
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    
    ospf->t_external_fw = 0;
	
	LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
	{
		if(IS_LSA_SELF(lsa)&&(!IS_LSA_MAXAGE(lsa)))
		{
			ei = ospf_external_info_check_by_fw (ospf, lsa);
			if(ei != NULL)
			{
				struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;
				/* Get forwarding address to nexthop if on the Connection List, else 0. */
				fwd_addr = ospf_external_lsa_nexthop_get (ospf, ei->nexthop);
				if(al->e[0].fwd_addr.s_addr != fwd_addr.s_addr)
				{
					ospf_external_lsa_refresh (ospf, lsa, ei, LSA_REFRESH_FORCE);
					continue;
				}
			}
		}
	}

    return 0;
}

/*******************************************************************************************

*Function: 	Delay 1s update the external lsa forwarding address
*Parameter:	struct ospf* ospf //specific ospf instance
*return: 	NULL

********************************************************************************************/
void
ospf_external_lsa_fw_update(struct ospf* ospf)
{
	if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return ;
    }
	
	if(ospf->t_external_fw == 0)
	{
		/*ospf->t_external_fw = thread_add_timer (master_ospf, ospf_external_lsa_fw_update_timer,
                                               ospf, 1);*/
		ospf->t_external_fw = high_pre_timer_add ((char *)"ospf_ext_fw_timer", LIB_TIMER_TYPE_NOLOOP,\
								ospf_external_lsa_fw_update_timer, ospf, 1000);
	} 
}



static int
ospf_nssa_lsa_fw_update_timer (void *thread)
{
	struct route_node *rn = NULL;
    struct ospf_lsa *lsa = NULL;
    struct external_info *ei = NULL;
    struct in_addr fwd_addr;
    struct ospf_area *area = (struct ospf_area *)(thread);

	if(area == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    
    area->ospf->t_nssa_fw = 0;
	
	LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
    {
		zlog_debug(OSPF_DBG_NSSA, "%s %d ",__func__,__LINE__);
        if(IS_LSA_SELF(lsa)&&(!IS_LSA_MAXAGE(lsa))&&(lsa->data->id.s_addr != OSPF_DEFAULT_DESTINATION))
        {
        	
            ei = ospf_external_info_check_by_fw (area->ospf,lsa);
            if(ei != NULL)
            {
                struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;
                /* Get forwarding address to nexthop if on the Connection List, else 0. */
                fwd_addr = ospf_get_nssa_ip(area);
				zlog_debug(OSPF_DBG_NSSA, "%s old:%s ",inet_ntoa(fwd_addr),inet_ntoa(al->e[0].fwd_addr));
				if(fwd_addr.s_addr == 0)
				{
					ospf_nssa_lsa_flush (area->ospf, &ei->p);
                    continue;
				}
                else if(al->e[0].fwd_addr.s_addr != fwd_addr.s_addr)
                {
                    ospf_nssa_lsa_refresh (area->ospf, lsa, ei, LSA_REFRESH_FORCE);
                    continue;
                }
            }
        }
    }
    return 0;

}

/*******************************************************************************************

*Function: 	Delay 1s update the nssa lsa forwarding address
*Parameter:	ospf_area* area //specific area
*return: 	NULL

********************************************************************************************/
void
ospf_nssa_lsa_fw_update(struct ospf_area* area)
{   
	if(area == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return ;
    }
	
	if(area->ospf->t_nssa_fw == 0)
	{
		/*area->ospf->t_nssa_fw = thread_add_timer (master_ospf, ospf_nssa_lsa_fw_update_timer,
                                               area, 1);*/
		area->ospf->t_nssa_fw = high_pre_timer_add ((char *)"ospf_nssa_fw_timer", LIB_TIMER_TYPE_NOLOOP,\
								ospf_nssa_lsa_fw_update_timer, area, 1000);
	} 

}

static struct external_info *
ospf_default_external_info (struct ospf *ospf)
{
    int type;
    struct route_node *rn;
    struct prefix_ipv4 p;

    p.family = AF_INET;
    p.prefix.s_addr = 0;
    p.prefixlen = 0;

    /* First, lookup redistributed default route. */
    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        if (EXTERNAL_INFO (ospf->vpn, type) && type != ROUTE_PROTO_OSPF)
        {
            rn = route_node_lookup (EXTERNAL_INFO (ospf->vpn, type), (struct prefix *) &p);
            if (rn != NULL)
            {
                route_unlock_node (rn);
                assert (rn->info);
                if (ospf_redistribute_check (ospf, rn->info, NULL))
                {
                    return rn->info;
                }
            }
        }
    }

    return NULL;
}

int
ospf_default_originate_timer (void *thread)
{
    struct prefix_ipv4 p;
    struct in_addr nexthop;
    struct external_info *ei = NULL;
    struct ospf *ospf = NULL;
    ospf = (struct ospf *)(thread);
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    p.family = AF_INET;
    p.prefix.s_addr = 0;
    p.prefixlen = 0;
    if (ospf->default_originate == DEFAULT_ORIGINATE_ALWAYS)
    {
        /* If there is no default route via redistribute,
        then originate AS-external-LSA with nexthop 0 (self). */
        nexthop.s_addr = 0;
        ospf_external_info_add (ospf->vpn, DEFAULT_ROUTE,1, p, 0, nexthop, 0);
    }
    if ((ei = ospf_default_external_info (ospf)))
    {
        ospf_external_lsa_originate (ospf, ei);
    }
    return 0;
}


/* Flush an AS-external-LSA from LSDB and routing domain. */
void
ospf_external_lsa_flush (struct ospf *ospf,
                         u_char type, struct prefix_ipv4 *p,
                         ifindex_t ifindex /*, struct in_addr nexthop */)
{
    struct ospf_lsa *lsa = NULL;
    struct ospf_lsa *lsa_tmp = NULL;
    struct ospf_summary *summary = NULL;
	
    if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
    {
        zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA: Find Flushing AS-external-LSA %s/%d",
                    inet_ntoa (p->prefix), p->prefixlen);
    }
    if ((summary = ospf_summary_match (ospf, p)))
    {
        if (IS_DEBUG_OSPF_EVENT)
            zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:%s]: Could not flush AS-external-LSA:This lsa is summary lsa ",
                        inet_ntoa (p->prefix));
        return;
    }
    /* First lookup LSA from LSDB. */
    if (!(lsa = ospf_external_info_find_lsa_specific (ospf, p)))
    {
        if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
            zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA: There is no such AS-external-LSA %s/%d in LSDB",
                        inet_ntoa (p->prefix), p->prefixlen);
        return;
    }

	if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
    {
		struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;
        zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA: Flushing AS-external-LSA %s/%d",
                    inet_ntoa (lsa->data->id),ip_masklen (al->mask));
    }
	
    if(ospf->anyNSSA)
    {
        /* If LSA is selforiginated, not a translated LSA, and there is
          * NSSA area, flush Type-7 LSA's at first.
           */
        p->prefix.s_addr = lsa->data->id.s_addr;
        ospf_nssa_lsa_flush (ospf, p);
        if(IS_OSPF_ABR(ospf))
        {
            /* check the type 7 LSA if exist, if exist ,do nothing else flush the type 5 LSA*/
            lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &lsa->data->id);
            if (lsa_tmp && !IS_LSA_SELF(lsa_tmp))
            {
                if(!CHECK_FLAG(lsa->flags,OSPF_LSA_LOCAL_XLT))
                {
                    ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                }
                return;
            }
        }
    }
    /* Sweep LSA from Link State Retransmit List. */
    ospf_ls_retransmit_delete_nbr_as (ospf, lsa);
    /* There must be no self-originated LSA in rtrs_external. */
    /* Remove External route from Zebra. */
    //ospf_zebra_delete ((struct prefix_ipv4 *) p, &nexthop);
    if (!IS_LSA_MAXAGE (lsa))
    {
        /* Unregister LSA from Refresh queue. */
        ospf_refresher_unregister_lsa (ospf, lsa);
        /* Flush AS-external-LSA through AS. */
        ospf_lsa_flush_as (ospf, lsa);
    }
    if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
    {
        zlog_debug (OSPF_DBG_LSA_FLOODING, "ospf_external_lsa_flush(): stop");
    }
}

/* Flush an AS-external-LSA from LSDB and routing domain. */
void
ospf_external_summary_lsa_flush (struct ospf *ospf,
                                 u_char type, struct prefix_ipv4 *p,
                                 ifindex_t ifindex /*, struct in_addr nexthop */)
{
    struct ospf_lsa *lsa = NULL;
    struct ospf_lsa *lsa_tmp = NULL;
    if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
        zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA: Flushing AS-external-LSA %s/%d",
                    inet_ntoa (p->prefix), p->prefixlen);
    /* First lookup LSA from LSDB. */
    if (!(lsa = ospf_external_info_find_lsa (ospf, p)))
    {
        if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
            zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA: There is no such AS-external-LSA %s/%d in LSDB",
                        inet_ntoa (p->prefix), p->prefixlen);
        return;
    }
    if(ospf->anyNSSA )
    {
        /* If LSA is selforiginated, not a translated LSA, and there is
          * NSSA area, flush Type-7 LSA's at first.
          */
        p->prefix.s_addr = lsa->data->id.s_addr;
        ospf_nssa_lsa_flush (ospf, p);
        if(IS_OSPF_ABR(ospf))
        {
            /* check the type 7 LSA if exist, if exist ,do nothing else flush the type 5 LSA*/
            lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &lsa->data->id);
            if (lsa_tmp && !IS_LSA_SELF(lsa_tmp))
            {
                if(!CHECK_FLAG(lsa->flags,OSPF_LSA_LOCAL_XLT))
                {
                    ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                }
                return;
            }
        }
    }
    /* Sweep LSA from Link State Retransmit List. */
    ospf_ls_retransmit_delete_nbr_as (ospf, lsa);
    /* There must be no self-originated LSA in rtrs_external. */
    /* Remove External route from Zebra. */
    //ospf_zebra_delete ((struct prefix_ipv4 *) p, &nexthop);
    if (!IS_LSA_MAXAGE (lsa))
    {
        /* Unregister LSA from Refresh queue. */
        ospf_refresher_unregister_lsa (ospf, lsa);
        /* Flush AS-external-LSA through AS. */
        ospf_lsa_flush_as (ospf, lsa);
    }
    if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
    {
        zlog_debug (OSPF_DBG_LSA_FLOODING, "ospf_external_lsa_flush(): stop");
    }
}


/* NSSA-LSA flush 7->5 LSA. If there is self exist just refresh the lsa*/
int
ospf_translated_external_lsa_flush_byarea (struct ospf *ospf, struct ospf_area *area)
{
    struct route_node *rn = NULL;
    struct ospf_lsa *lsa = NULL;
    struct ospf_lsa *external_lsa = NULL;
    struct external_info *ei = NULL;
    if (!IS_AREA_NSSA(area))
    {
        return 0;
    }
    if (ospf_lsdb_count_self (area->lsdb, OSPF_AS_NSSA_LSA) == 0 ||
            (ospf_lsdb_count (area->lsdb, OSPF_AS_NSSA_LSA) == 0))
    {
        return 0;
    }
    LSDB_LOOP(NSSA_LSDB(area), rn ,lsa)
    {
        if (lsa->data->id.s_addr == OSPF_DEFAULT_DESTINATION)
        {
            continue;
        }
        external_lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                               lsa->data->id, ospf->router_id);
        if (external_lsa == NULL)
        {
            continue;
        }
        if (CHECK_FLAG (external_lsa->flags, OSPF_LSA_LOCAL_XLT))
        {
            ei = ospf_external_info_check(ospf,external_lsa);
            if(ei)
            {
                ospf_external_lsa_refresh_only(ospf, lsa, ei, LSA_REFRESH_FORCE);
            }
            else
            {
                /* Sweep LSA from Link State Retransmit List. */
                ospf_ls_retransmit_delete_nbr_as (ospf, external_lsa);
                if (!IS_LSA_MAXAGE (external_lsa))
                {
                    /* Unregister LSA from Refresh queue. */
                    ospf_refresher_unregister_lsa (ospf, external_lsa);
                    /* Flush AS-external-LSA through AS. */
                    ospf_lsa_flush_as (ospf, external_lsa);
                }
            }
        }
    }
    return 0;
}


void
ospf_external_lsa_refresh_default (struct ospf *ospf)
{
    struct prefix_ipv4 p;
    struct external_info *ei;
    struct ospf_lsa *lsa;
    p.family = AF_INET;
    p.prefixlen = 0;
    p.prefix.s_addr = 0;
    ei = ospf_default_external_info (ospf);
    lsa = ospf_external_info_find_lsa (ospf, &p);
    if (ei)
    {
        if (lsa)
        {
            if (IS_DEBUG_OSPF_EVENT)
                zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:0.0.0.0]: Refresh AS-external-LSA %p",
                            (void *)lsa);
            ospf_external_lsa_refresh (ospf, lsa, ei, LSA_REFRESH_FORCE);
        }
        else
        {
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:0.0.0.0]: Originate AS-external-LSA");
            }
            ospf_external_lsa_originate (ospf, ei);
        }
    }
    else
    {
        if (lsa)
        {
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "LSA[Type5:0.0.0.0]: Flush AS-external-LSA");
            }
            ospf_refresher_unregister_lsa (ospf, lsa);
            ospf_lsa_flush_as (ospf, lsa);
        }
    }
    ospf_schedule_asbr_task (ospf);
}

void
ospf_external_lsa_refresh_type (struct ospf *ospf, u_char type, int force, u_int8_t instance)
{
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
    struct ospf_lsa *lsa_tmp = NULL;
    struct ospf_lsa *old = NULL;
    if(type == ROUTE_PROTO_OSPF && instance == ospf->ospf_id)
    {
        return ;
    }
    if (type != DEFAULT_ROUTE)
    {
        if (EXTERNAL_INFO(ospf->vpn, type))
        {
            /* Refresh each redistributed AS-external-LSAs. */
            for (rn = route_top (EXTERNAL_INFO (ospf->vpn, type)); rn; rn = route_next (rn))
            {
                for(ei = rn->info ; ei != NULL ; ei = ei->next)
                {
                    if (!is_prefix_default (&ei->p) && ei->instance == instance)
                    {
                        struct ospf_lsa *lsa = NULL;
                        if ((lsa = ospf_external_info_find_lsa (ospf, &ei->p)))
                        {
                            if (ospf->anyNSSA)
                            {
                                /*  NSSA ABR 和ASBR 有7转5的LSA  */
                                if (IS_OSPF_ABR (ospf) && IS_OSPF_ASBR (ospf))
                                {
                                    old = ospf_as_nssa_info_find_lsa(ospf, &ei->p);
                                    if(old == NULL)
                                    {
                                        ospf_nssa_lsa_orignate_only(ospf, ei);
                                    }
                                    else
                                    {
                                        ospf_nssa_lsa_refresh(ospf, old, ei, force);
                                    }
                                    lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &ei->p.prefix);
                                    if (lsa_tmp && IS_LSA_SELF(lsa_tmp))
                                    {
                                        /*refresh Type-5 LSA ONLY*/
                                        ospf_external_lsa_refresh_only(ospf, lsa, ei, force);
                                    }
                                    else if (lsa_tmp && !IS_LSA_SELF(lsa_tmp))
                                    {
                                        if(!CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT))
                                        {
                                            ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                                        }
                                    }
                                }
                                else
                                {
                                    ospf_external_lsa_refresh (ospf, lsa, ei, force);
                                }
                            }
                            else
                            {
                                ospf_external_lsa_refresh (ospf, lsa, ei, force);
                            }
                        }
                        else
                        {
                            ospf_external_lsa_originate (ospf, ei);
                        }
                    }
                }
            }
        }
    }
}

/* Refresh AS-external-LSA ONLY. */
struct ospf_lsa *
ospf_external_lsa_refresh_only (struct ospf *ospf, struct ospf_lsa *lsa,
                                struct external_info *ei, int force)
{
    struct ospf_lsa *new_lsa = NULL;
    int changed;
    struct ospf_summary *summary = NULL;

    /* Check the AS-external-LSA should be originated. */
    if (!ospf_redistribute_check (ospf, ei, &changed))
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed, "
                        "redist check fail",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        ospf_external_lsa_flush (ospf, ei->type, &ei->p,
                                 ei->ifindex /*, ei->nexthop */);
        return NULL;
    }

    if (!changed && !force)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Not refreshed, not changed/forced",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        return NULL;
    }

    if(!((ei->type == ROUTE_PROTO_OSPF) && (ei->instance == ospf->ospf_id)))
    {
        if ((summary = ospf_summary_match (ospf, &ei->p)))
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Not refreshed, in summary list",
                            lsa->data->type, inet_ntoa (lsa->data->id));
            return NULL;
        }
    }

    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_as (ospf, lsa);

    /* Unregister AS-external-LSA from refresh-list. */
    ospf_refresher_unregister_lsa (ospf, lsa);

    new_lsa = ospf_external_lsa_new (ospf, ei, &lsa->data->id);

    if (new_lsa == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed", lsa->data->type,
                        inet_ntoa (lsa->data->id));
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

    ospf_lsa_install (ospf, NULL, new_lsa);   /* As type-5. */

    /* Flood LSA through AS. */
    ospf_flood_through_as (ospf, NULL, new_lsa);


    /* Register self-originated LSA to refresh queue.
     * Translated LSAs should not be registered, but refreshed upon
     * refresh of the Type-7
     */
    if ( !CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT) )
    {
        ospf_refresher_register_lsa (ospf, new_lsa);
    }

    /* Debug logging. */
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: AS-external-LSA refresh",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id));
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}


/* Refresh AS-external-LSA. */
struct ospf_lsa *
ospf_external_lsa_refresh (struct ospf *ospf, struct ospf_lsa *lsa,
                           struct external_info *ei, int force)
{
    struct ospf_lsa *new_lsa = NULL;
    int changed;
    struct ospf_summary *summary = NULL;

    /* Check the AS-external-LSA should be originated. */
    if (!ospf_redistribute_check (ospf, ei, &changed))
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed, "
                        "redist check fail",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        ospf_external_lsa_flush (ospf, ei->type, &ei->p,
                                 ei->ifindex /*, ei->nexthop */);
        return NULL;
    }

    if (!changed && !force)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Not refreshed, not changed/forced",
                        lsa->data->type, inet_ntoa (lsa->data->id));
        return NULL;
    }

    if(!((ei->type == ROUTE_PROTO_OSPF) && (ei->instance == ospf->ospf_id)))
    {
        if ((summary = ospf_summary_match (ospf, &ei->p)))
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Not refreshed, in summary list",
                            lsa->data->type, inet_ntoa (lsa->data->id));
            return NULL;
        }
    }

    /* Delete LSA from neighbor retransmit-list. */
    ospf_ls_retransmit_delete_nbr_as (ospf, lsa);

    /* Unregister AS-external-LSA from refresh-list. */
    ospf_refresher_unregister_lsa (ospf, lsa);

    new_lsa = ospf_external_lsa_new (ospf, ei, &lsa->data->id);

    if (new_lsa == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Could not be refreshed", lsa->data->type,
                        inet_ntoa (lsa->data->id));
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

    ospf_lsa_install (ospf, NULL, new_lsa);   /* As type-5. */

    /* Flood LSA through AS. */
    ospf_flood_through_as (ospf, NULL, new_lsa);

    /* If any attached NSSA, install as Type-7, flood to all NSSA Areas */
    if (ospf->anyNSSA && !(CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT)))
    {
        ospf_install_flood_nssa (ospf, new_lsa, ei);    /* Install/Flood per new rules */
    }

    /* Register self-originated LSA to refresh queue.
     * Translated LSAs should not be registered, but refreshed upon
     * refresh of the Type-7
     */
    if ( !CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT) )
    {
        ospf_refresher_register_lsa (ospf, new_lsa);
    }

    /* Debug logging. */
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: AS-external-LSA refresh",
                    new_lsa->data->type, inet_ntoa (new_lsa->data->id));
        ospf_lsa_header_dump (new_lsa->data);
    }

    return new_lsa;
}

struct ospf_lsa*
ospf_as_nssa_default_lsa_refresh (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct ospf_lsa *new_lsa;

    new_lsa = ospf_as_nssa_defaults_lsa_originate(ospf);

    if (!new_lsa)
    {
        return NULL;
    }

    new_lsa->data->ls_seqnum = lsa_seqnum_increment (lsa);
    new_lsa->area = lsa->area;
    ospf_lsa_install (ospf, NULL, new_lsa);

    /* Flood LSA through AS. */
    ospf_flood_through_area (new_lsa->area, NULL, new_lsa);

    return new_lsa;
}


/*******************************************************************************************
*Function: 	以计算路由自身为起点，根据route_lsa里携带的连接描述（link）来递归计算当前在线的网
			元router_id列表。
*Parameter:	struct ospf_area *area //指定区域
			struct list *dcn_ne_online //用来存放在线网元router_id的链表
			struct router_lsa *route_lsa //当前在线网元的route_lsa
*return: 	NULL
********************************************************************************************/
void
ospf_dcn_ne_online_lookup(struct ospf_area *area, struct list *dcn_ne_online, struct router_lsa *route_lsa)
{
	int len = 0;
	u_int16 i = 0;
	struct ospf_lsa *lsa_temp = NULL;
	struct router_lsa *rl = NULL;

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': router_lsa id = %s\n",\
							__FILE__, __LINE__, __func__, inet_ntoa(route_lsa->header.id));
	}

	if(route_lsa->links != 0)
	{
		len = ntohs (route_lsa->header.length) - 4;
		for (i = 0; (i < ntohs (route_lsa->links)) && (len > 0); len -= 12, i++)
		{
			if(route_lsa->link[i].type != LSA_LINK_TYPE_POINTOPOINT)//只需判断类型为p2p的连接描述即可
			{
				continue;
			}
			
			if((route_lsa->link[i].link_id.s_addr == route_lsa->header.id.s_addr)
				&& (listnode_lookup ( dcn_ne_online, (void *)(route_lsa->link[i].link_id.s_addr) ) == NULL))
			{
				
				listnode_add(dcn_ne_online, (void *)(route_lsa->link[i].link_id.s_addr));
				continue;
			}

			
			if((listnode_lookup ( dcn_ne_online, (void *)(route_lsa->link[i].link_id.s_addr) ) == NULL)
				&& ((lsa_temp = ospf_lsa_lookup_by_id(area, OSPF_ROUTER_LSA, route_lsa->link[i].link_id)) != NULL))
			{
				
				rl = (struct router_lsa *) lsa_temp->data;
				listnode_add(dcn_ne_online,  (void *)(route_lsa->link[i].link_id.s_addr));
				ospf_dcn_ne_online_lookup(area, dcn_ne_online, rl);
			}
		}
	}
	return ;
}


/*******************************************************************************************
*Function: 	以计算路由自身为起点，根据route_lsa里携带的连接描述（link）来递归计算当前在线的网
			元router_id列表，然后将数据库里所有type10-lsa与该列表对比，将在线网元的type10-lsa进
			行标记处理，已经不在线的网元上报网管。
*Parameter:	struct ospf *ospf //实例号为255的ospf实例数据结构
*return: 	NULL
********************************************************************************************/
static void
ospf_dcn_ne_status_handle(struct ospf *ospf)
{
	u_int32_t lsid = 0;
	u_char opaque_type = 0;
	u_int32_t *link_id = NULL;
	struct ospf_area *area = NULL;
	struct in_addr area_id;
	struct in_addr online_router_id;
	struct ospf_lsa *lsa = NULL;
	struct ospf_lsa *lsa_temp = NULL;
	struct route_node *rn = NULL;
	struct listnode *node = NULL;
	struct ospf_dcn_ne_info ne_info;
	struct list *dcn_ne_online = list_new ();

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	
	area_id.s_addr = 0;
	area = ospf_area_lookup_by_area_id(ospf, area_id);
	if(NULL == area)
	{
		OSPF_LOG_ERROR(" area 0 is not exist, dcn ne calculate stop!!!");
		return;
	}
	lsa = area->router_lsa_self;
	if(NULL == lsa)
	{
		OSPF_LOG_ERROR(" area->router_lsa_self == NULL, dcn ne calculate stop!!!");
		return;
	}
	struct router_lsa *rl = (struct router_lsa *) lsa->data;
	ospf_dcn_ne_online_lookup(area, dcn_ne_online, rl);//递归计算在线网元列表

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' :The list dcn_ne_online count = %d \n", \
							__FILE__, __LINE__, __func__, listcount(dcn_ne_online));
		for (ALL_LIST_ELEMENTS_RO (dcn_ne_online, node, link_id))
		{
			online_router_id.s_addr = (u_int32_t)link_id;
			zlog_debug(OSPF_DBG_DCN, " The online router_id:%s \n",inet_ntoa(online_router_id));
		}
	}

	LSDB_LOOP (OPAQUE_AREA_LSDB(area), rn, lsa_temp)
	{
		lsid = ntohl (lsa_temp->data->id.s_addr);
		opaque_type = GET_OPAQUE_TYPE (lsid);
		if(opaque_type == OPAQUE_TYPE_NMS_AUTOREPORT)
		{
			if(listnode_lookup ( dcn_ne_online,  (void *)(lsa_temp->data->adv_router.s_addr) ) == NULL)
			{
				if(ospf->dcn_report)
				{
					memset(&ne_info, 0, sizeof(struct ospf_dcn_ne_info));
					if(!get_dcn_ne_info_from_tlv(lsa_temp, NULL, &ne_info))
					{
						ospf_dcn_ne_info_trap(&ne_info, OSPF_DCN_NE_OFFLINE);
					}
				}

				//Note here:opaque_lsa_flag == 0x1 mark offline
				if(!CHECK_FLAG(lsa_temp->opaque_lsa_flag, OPAQUE_LSA_FOR_DCN))
				{
					SET_FLAG(lsa_temp->opaque_lsa_flag, OPAQUE_LSA_FOR_DCN);
				}
			}
			else
			{	
				//Note here:opaque_lsa_flag == 0 mark online
				if(CHECK_FLAG(lsa_temp->opaque_lsa_flag, OPAQUE_LSA_FOR_DCN))
				{
					UNSET_FLAG(lsa_temp->opaque_lsa_flag, OPAQUE_LSA_FOR_DCN);
				}
			}
		}
	}
	
	list_delete(dcn_ne_online);
	dcn_ne_online = NULL;
	return;
}

/*
展示网元厂家标识
*/
static u_int16_t
show_vty_manufacturer_id (struct vty *vty, struct opaque_tlv_header *tlvh)
{
	struct opaque_tlv_manuid *top = (struct opaque_tlv_manuid *) tlvh;
	unsigned char manuid[128];

	memset(manuid, 0, sizeof(manuid));
  	memcpy(manuid, &(top->value), ntohs(top->header.length));

  	if (vty != NULL)
  	{
    	vty_out (vty, "  manufacturer: %s%s", manuid, VTY_NEWLINE);
  	}
  	else
    	zlog_debug(OSPF_DBG_DCN, "    manufacturer: %s", manuid);

  	return OPAQUE_TLV_SIZE (tlvh);
}

/*
网元设备型号
*/
static u_int16_t
show_vty_dev_id (struct vty *vty, struct opaque_tlv_header *tlvh)
{
	struct opaque_tlv_devid *top = (struct opaque_tlv_devid *) tlvh;
	unsigned char devid[128];

  	memset(devid, 0, sizeof(devid));
  	memcpy(devid, &(top->value), ntohs(top->header.length));

  	if (vty != NULL)
    	vty_out (vty, "  dev: %s%s", devid, VTY_NEWLINE);
  	else
    	zlog_debug (OSPF_DBG_DCN, "    dev: %s", devid);

  	return OPAQUE_TLV_SIZE (tlvh);
}

/*
网元设备桥MAC
*/
static u_int16_t
show_vty_dev_mac (struct vty *vty, struct opaque_tlv_header *tlvh)
{
	struct opaque_tlv_devmac *top = (struct opaque_tlv_devmac *) tlvh;

	if (vty != NULL)
    	vty_out (vty, "  dev mac: %02X:%02X:%02X:%02X:%02X:%02X%s", 
    		top->value[0], top->value[1], top->value[2], top->value[3], 
    		top->value[4], top->value[5], VTY_NEWLINE);
  	else
		zlog_debug (OSPF_DBG_DCN, "    dev mac: %02X:%02X:%02X:%02X:%02X:%02X", 
			top->value[0], top->value[1], top->value[2], top->value[3],
			top->value[4], top->value[5]);

  	return OPAQUE_TLV_SIZE (tlvh);
}

/*
网元的NE ID
*/
static u_int16_t
show_vty_dev_neid (struct vty *vty, struct opaque_tlv_header *tlvh)
{
	struct opaque_tlv_neid *top = (struct opaque_tlv_neid *) tlvh;

  	if (vty != NULL)
    	vty_out (vty, "  NEID: %08X%s", ntohl(top->value.s_addr), VTY_NEWLINE);
  	else
		zlog_debug (OSPF_DBG_DCN, "    NEID: %08X", ntohl(top->value.s_addr));

  	return OPAQUE_TLV_SIZE (tlvh);
}

/*
网元的NE IP
*/	
static u_int16_t
show_vty_dev_neipv4 (struct vty *vty, struct opaque_tlv_header *tlvh)
{
	struct opaque_tlv_neipv4 *top = (struct opaque_tlv_neipv4 *) tlvh;

	if (vty != NULL)
		vty_out (vty, "  NEIP: %s%s", inet_ntoa (top->value), VTY_NEWLINE);
	else
		zlog_debug (OSPF_DBG_DCN, "	 NEIP: %s", inet_ntoa (top->value));

  	return OPAQUE_TLV_SIZE (tlvh);
}

/*
网元的NE IP v6
*/	
static u_int16_t
show_vty_dev_neipv6 (struct vty *vty, struct opaque_tlv_header *tlvh)
{
	//struct opaque_tlv_neipv6 *top = (struct opaque_tlv_neipv6 *) tlvh;

  	return OPAQUE_TLV_SIZE (tlvh);
}

/*
未知的网元信息
*/	
static u_int16_t
show_vty_unknown_opaque_tlv (struct vty *vty, struct opaque_tlv_header *tlvh)
{
  if (vty != NULL)
	vty_out (vty, "  Unknown TLV: [type(0x%x), length(0x%x)]%s", ntohs (tlvh->type), ntohs (tlvh->length), VTY_NEWLINE);
  else
	zlog_debug (OSPF_DBG_DCN, "	 Unknown TLV: [type(0x%x), length(0x%x)]", ntohs (tlvh->type), ntohs (tlvh->length));

  return OPAQUE_TLV_SIZE (tlvh);
}


/* 
Set opaque-LSA body. 
*/
static void
ospf_opaque_type10_lsa_body_set (struct stream *s, struct ospf_area *area)
{
	int length = 0;
	char dcn_ne_id_temp[32] = "";
	char dcn_ne_ip_temp[32] = "";
	char dcn_ne_mac_temp[32] = "";

  	/*  厂家标识*/
  	stream_putw(s, 0x8000);
  	stream_putw(s, strlen(om->company_name));
  	stream_put(s, om->company_name, strlen(om->company_name));

	if((length = strlen(om->company_name)%4) !=0)
    {
        for(int j = 0; j< 4-length; j++)
        {
  	        stream_putc(s, 0x0);
        }    
    }

  	/*  设备型号*/
  	stream_putw(s, 0x8001);
  	stream_putw(s, strlen(om->device_name));
  	stream_put(s, om->device_name, strlen(om->device_name));

	if((length = strlen(om->device_name)%4) !=0)
    {
        for(int j = 0; j< 4-length; j++)
        {
  	        stream_putc(s, 0x0);
        }    
    }

  	/*  设备桥MAC  */
  	stream_putw(s, 0x8002);
  	stream_putw(s, 0x06);
  	stream_put(s, om->mac, 6);
	stream_putc(s, 0x0);
	stream_putc(s, 0x0);
	
	memset(dcn_ne_mac_temp, 0, sizeof(dcn_ne_mac_temp));
	sprintf(dcn_ne_mac_temp, "%02X:%02X:%02X:%02X:%02X:%02X", om->mac[0], om->mac[1],
							om->mac[2],om->mac[3],om->mac[4],om->mac[5]);

  	/*  NE ID  */
  	stream_putw(s, 0x8003);
  	stream_putw(s, 0x04);
	if(om->ne_id_static.s_addr == 0)
	{
  		stream_put(s, &(om->ne_id_dynamic), 4);
		memset(dcn_ne_id_temp, 0, sizeof(dcn_ne_id_temp));
		sprintf(dcn_ne_id_temp, "%s", inet_ntoa(om->ne_id_dynamic));
	}
	else
	{
		stream_put(s, &(om->ne_id_static), 4);
		memset(dcn_ne_id_temp, 0, sizeof(dcn_ne_id_temp));
		sprintf(dcn_ne_id_temp, "%s", inet_ntoa(om->ne_id_static));
	}

  	/*  NE IPv4  */
  	stream_putw(s, 0x8004);
  	stream_putw(s, 0x04);
	if(om->ne_ip_static.s_addr == 0)
	{
  		stream_put(s, &(om->ne_ip_dynamic), 4);
		memset(dcn_ne_ip_temp, 0, sizeof(dcn_ne_ip_temp));
		sprintf(dcn_ne_ip_temp, "%s", inet_ntoa(om->ne_ip_dynamic));
	}
	else
	{
		stream_put(s, &(om->ne_ip_static), 4);
		memset(dcn_ne_ip_temp, 0, sizeof(dcn_ne_ip_temp));
		sprintf(dcn_ne_ip_temp, "%s", inet_ntoa(om->ne_ip_static));
	}

  	/*  NE IPv6  */
  	stream_putw(s, 0x8005);
  	stream_putw(s, 0x10);
    
    for(int i = 0; i < 16; i++)
  	    stream_putc(s, 0x0);

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "DCN lsa body:company_name:%s dev_name:%s mac:%s ne-id:%s ne-ip:%s\n",
				om->company_name, om->device_name, dcn_ne_mac_temp, dcn_ne_id_temp, dcn_ne_ip_temp);
	}
}


/*
Create new opaque-LSA. 
*/
struct ospf_lsa *ospf_opaque_type10_lsa_new (struct ospf_area *area)
{
	struct ospf *ospf = area->ospf;
	struct stream *s;
	struct lsa_header *lsah;
	struct ospf_lsa *newlsa;
	int length;
  	struct in_addr opaque_id;
  	struct in_addr opaque_id_tmp;
	struct in_addr dcn_opaque_id;

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
  	/* Create a stream for LSA. */
  	s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}

	/* 网管自通LSA ID 为202.255.238.0 */
	
	//opaque_id_tmp.s_addr = SET_OPAQUE_LSID (OPAQUE_TYPE_NMS_AUTOREPORT, GET_OPAQUE_ID(htonl(om->ne_id_dynamic.s_addr)));
	inet_aton ("202.255.238.0", &dcn_opaque_id);
	opaque_id_tmp.s_addr = SET_OPAQUE_LSID (OPAQUE_TYPE_NMS_AUTOREPORT, htonl(dcn_opaque_id.s_addr));
	opaque_id.s_addr = htonl (opaque_id_tmp.s_addr);

	if (IS_DEBUG_OSPF (lsa, LSA_GENERATE) || IS_DEBUG_OSPF_DCN)
  	{
		zlog_debug ((OSPF_DBG_LSA_GENERATE|OSPF_DBG_DCN) , "LSA[Type%d:%s]: Create an Opaque-LSA/OSPF-DCN instance", OSPF_OPAQUE_AREA_LSA, inet_ntoa (opaque_id));
  	}
  	/* Set LSA common header fields. */
  	lsa_header_set (s, 
		LSA_OPTIONS_GET (area) | LSA_OPTIONS_NSSA_GET (area) | OSPF_OPTION_O,
		OSPF_OPAQUE_AREA_LSA, opaque_id, ospf->router_id);

  	/* Set opaque-LSA body fields. */
  	ospf_opaque_type10_lsa_body_set (s, area);

    if((length = (stream_get_endp (s))%4) !=0)
    {
        for(int j = 0; j< 4-length; j++)
        {
  	        stream_putc(s, 0x0);
        }
            
    }

  	/* Set length. */
  	length = stream_get_endp (s);
  	lsah = (struct lsa_header *) STREAM_DATA (s);
  	lsah->length = htons (length);

  	/* Now, create OSPF LSA instance. */
  	if ( (newlsa = ospf_lsa_new ()) == NULL)
    {
		zlog_err("%s():%d: failed!", __FUNCTION__, __LINE__);
		return NULL;
    }
  
  	newlsa->area = area;
  	SET_FLAG (newlsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

  	/* Copy LSA data to store, discard stream. */
  	newlsa->data = ospf_lsa_data_new (length);
	if(newlsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
  	memcpy (newlsa->data, lsah, length);
  	stream_free (s);

  	return newlsa;
}


/*
展示type 10 LSA 详细信息回调函数
*/
void ospf_opaque_tyep10_lsa_show_info (struct vty *vty, struct ospf_lsa *lsa)
{
	struct lsa_header *lsah = (struct lsa_header *) lsa->data;
  	struct opaque_tlv_header *tlvh, *next;
  	u_int16_t sum, total;
	int offset = 0;

  	sum = 0;
  	total = ntohs (lsah->length) - OSPF_LSA_HEADER_SIZE;

  	for (tlvh = OPAQUE_TLV_HDR_TOP (lsah); sum < total;
					tlvh = (next ? next : OPAQUE_TLV_HDR_NEXT (tlvh, offset)))
    {
      	next = NULL;
		offset = 0;
      	switch (ntohs (tlvh->type))
        {
		case OPAQUE_TLV_TYPE_MANUID:
			if(ntohs(tlvh->length)%4 != 0)
			{
				offset = (4 - ntohs(tlvh->length)%4);
			}
        	sum += (show_vty_manufacturer_id (vty, tlvh) + offset);
			
          	break;
		case OPAQUE_TLV_TYPE_DEVID:
			if(ntohs(tlvh->length)%4 != 0)
			{
				offset = (4 - ntohs(tlvh->length)%4);
			}
        	sum += (show_vty_dev_id (vty, tlvh) + offset);
          	break;
		case OPAQUE_TLV_TYPE_DEVMAC:
			if(ntohs(tlvh->length)%4 != 0)
			{
				offset = (4 - ntohs(tlvh->length)%4);
			}
        	sum += (show_vty_dev_mac (vty, tlvh) + offset);
          	break;
		case OPAQUE_TLV_TYPE_NEID:
        	sum += show_vty_dev_neid (vty, tlvh);
          	break;
		case OPAQUE_TLV_TYPE_NEIPV4:
        	sum += show_vty_dev_neipv4 (vty, tlvh);
          	break;
		case OPAQUE_TLV_TYPE_NEIPV6:
        	sum += show_vty_dev_neipv6 (vty, tlvh);
          	break;
        default:
          	sum += show_vty_unknown_opaque_tlv (vty, tlvh);
          	break;
        }
    }
					
	return;
}

/*
type 10 Opaque-LSA 生成回调函数
*/
int ospf_opaque_type10_lsa_generate (void *arg)
{
	struct ospf_area *area;
	struct ospf_lsa *newlsa;

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	area = (struct ospf_area *)arg;

  	/* Create new opaque-LSA instance. */
  	if ((newlsa = ospf_opaque_type10_lsa_new (area)) == NULL)
	{
		zlog_err("%s():%d: failed!", __FUNCTION__, __LINE__);
	  	return -1;
	}

  	/* Sanity check. */
  	if (newlsa->data->adv_router.s_addr == 0)
	{
		if (IS_DEBUG_OSPF_EVENT)
		{
			zlog_debug (OSPF_DBG_EVENT, "LSA[Type10]: AdvRouter is 0, discard");
		}
		
	  	ospf_lsa_discard (newlsa);
		
	  	return -1;
	}

  	/* Install LSA to LSDB. */
  	newlsa = ospf_lsa_install (area->ospf, NULL, newlsa);

  	/* Update LSA origination count. */
  	area->ospf->lsa_originate_count++;

  	/* Flooding new LSA through area. */
  	ospf_flood_through_area (area, NULL, newlsa);

  	if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
	{
		zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate opaque-type10-LSA %p",
					newlsa->data->type, inet_ntoa (newlsa->data->id),
					(void *)newlsa);
	  	ospf_lsa_header_dump (newlsa->data);
	}

  	return 0;
}

/*
type 10 Opaque-LSA 刷新回调函数
*/
struct ospf_lsa *ospf_opaque_type10_lsa_refresh (struct ospf_lsa *lsa)
{
	struct ospf_area *area = lsa->area;
	struct ospf_lsa *newlsa;

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
  	/* Sanity check. */
  	assert (lsa->data);

  	/* Delete LSA from neighbor retransmit-list. */
  	ospf_ls_retransmit_delete_nbr_area (area, lsa);

  	/* Unregister LSA from refresh-list */
  	ospf_refresher_unregister_lsa (area->ospf, lsa);
  
  	/* Create new router-LSA instance. */
  	if ( (newlsa = ospf_opaque_type10_lsa_new (area)) == NULL)
	{
		zlog_err("%s():%d: failed!", __FUNCTION__, __LINE__);
	  	return NULL;
	}
  
  	newlsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

  	ospf_lsa_install (area->ospf, NULL, newlsa);

  	/* Flood LSA through area. */
  	ospf_flood_through_area (area, NULL, newlsa);

  	/* Debug logging. */
  	if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
	{
		zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: opaque-type10-LSA refresh",
		 	newlsa->data->type, inet_ntoa (newlsa->data->id));
	  	ospf_lsa_header_dump (newlsa->data);
	}

  	return NULL;
}

/*
注册type 10 LSA 回调函数
*/
int
ospf_opaque_type10_init (void)
{
	int rc;

	rc = ospf_register_opaque_functab (
				OSPF_OPAQUE_AREA_LSA,
                OPAQUE_TYPE_NMS_AUTOREPORT,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
        		ospf_opaque_tyep10_lsa_show_info,
        		ospf_opaque_type10_lsa_generate,
        		ospf_opaque_type10_lsa_refresh,
				NULL,
				NULL);
  	if (rc != 0)
    {
		zlog_err("%s():%d: Register opaque type 10 LSA callback function failed!", __FUNCTION__, __LINE__);
		goto out;
    }

out:
	return rc;
}

struct u0_device_info_local * 
lookup_u0_device_by_adv_router(struct in_addr adv_router)
{
	struct listnode *node = NULL;
	struct u0_device_info_local *target_p = NULL;

	for (ALL_LIST_ELEMENTS_RO(om->u0_list, node, target_p))
	{
		if(target_p->u0_info.ne_ip.s_addr == adv_router.s_addr)
		{
			return target_p;
		}
	}
	return NULL;
}


static void
ospf_opaque_type10_lsa_body_set_for_u0 (struct stream *s, struct u0_device_info *u0_device)
{
	int length = 0;
	char dcn_ne_id_temp[32] = "";
	char dcn_ne_ip_temp[32] = "";
	char dcn_ne_mac_temp[32] = "";

  	/*  厂家标识*/
  	stream_putw(s, 0x8000);
  	stream_putw(s, strlen(u0_device->dcn_ne_vendor));
  	stream_put(s, u0_device->dcn_ne_vendor, strlen(u0_device->dcn_ne_vendor));

	if((length = strlen(u0_device->dcn_ne_vendor)%4) !=0)
    {
        for(int j = 0; j< 4-length; j++)
        {
  	        stream_putc(s, 0x0);
        }    
    }

  	/*  设备型号*/
  	stream_putw(s, 0x8001);
  	stream_putw(s, strlen(u0_device->dcn_ne_device_type));
  	stream_put(s, u0_device->dcn_ne_device_type, strlen(u0_device->dcn_ne_device_type));

	if((length = strlen(u0_device->dcn_ne_device_type)%4) !=0)
    {
        for(int j = 0; j< 4-length; j++)
        {
  	        stream_putc(s, 0x0);
        }    
    }

  	/*  设备桥MAC  */
  	stream_putw(s, 0x8002);
  	stream_putw(s, 0x06);
  	stream_put(s, u0_device->mac, 6);
	stream_putc(s, 0x0);
	stream_putc(s, 0x0);
	
	memset(dcn_ne_mac_temp, 0, sizeof(dcn_ne_mac_temp));
	sprintf(dcn_ne_mac_temp, "%02X:%02X:%02X:%02X:%02X:%02X", u0_device->mac[0], u0_device->mac[1],
							u0_device->mac[2],u0_device->mac[3],u0_device->mac[4],u0_device->mac[5]);

  	/*  NE ID  */
  	stream_putw(s, 0x8003);
  	stream_putw(s, 0x04);
	stream_put(s, &(u0_device->ne_id), 4);
	memset(dcn_ne_id_temp, 0, sizeof(dcn_ne_id_temp));
	sprintf(dcn_ne_id_temp, "%s", inet_ntoa(u0_device->ne_id));


  	/*  NE IPv4  */
  	stream_putw(s, 0x8004);
  	stream_putw(s, 0x04);
	stream_put(s, &(u0_device->ne_ip), 4);
	memset(dcn_ne_ip_temp, 0, sizeof(dcn_ne_ip_temp));
	sprintf(dcn_ne_ip_temp, "%s", inet_ntoa(u0_device->ne_ip));


  	/*  NE IPv6  */
  	stream_putw(s, 0x8005);
  	stream_putw(s, 0x10);
    
    for(int i = 0; i < 16; i++)
  	    stream_putc(s, 0x0);

	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG("DCN lsa body:company_name:%s dev_name:%s mac:%s ne-id:%s ne-ip:%s\n",
				u0_device->dcn_ne_vendor, u0_device->dcn_ne_device_type, dcn_ne_mac_temp, dcn_ne_id_temp, dcn_ne_ip_temp);
	}
}

static struct ospf_lsa *
ospf_opaque_type10_lsa_new_for_u0 (struct ospf_area *area, struct u0_device_info *u0_device)
{
	//struct ospf *ospf = area->ospf;
	struct stream *s;
	struct lsa_header *lsah;
	struct ospf_lsa *newlsa;
	int length;
  	struct in_addr opaque_id;
  	struct in_addr opaque_id_tmp;
	struct in_addr dcn_opaque_id;

	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG();
	}
  	/* Create a stream for LSA. */
  	s = stream_new (OSPF_MAX_LSA_SIZE);
	if(s == NULL)
	{
       zlog_err("%s %d: fatal error: ospf_router_lsa_new(%u) failed allocating s",
        __func__,__LINE__,OSPF_MAX_LSA_SIZE);
        return NULL;
	}

	/* 网管自通LSA ID 为202.255.238.0 */
	
	//opaque_id_tmp.s_addr = SET_OPAQUE_LSID (OPAQUE_TYPE_NMS_AUTOREPORT, GET_OPAQUE_ID(htonl(om->ne_id_dynamic.s_addr)));
	inet_aton ("202.255.238.0", &dcn_opaque_id);
	opaque_id_tmp.s_addr = SET_OPAQUE_LSID (OPAQUE_TYPE_NMS_AUTOREPORT, htonl(dcn_opaque_id.s_addr));
	opaque_id.s_addr = htonl (opaque_id_tmp.s_addr);

	if (IS_DEBUG_OSPF (lsa, LSA_GENERATE) || IS_DEBUG_OSPF_DCN)
  	{
		zlog_debug ((OSPF_DBG_DCN|OSPF_DBG_LSA_GENERATE),"LSA[Type%d:%s]: Create an Opaque-LSA/OSPF-DCN instance", OSPF_OPAQUE_AREA_LSA, inet_ntoa (opaque_id));
  	}
  	/* Set LSA common header fields. */
  	lsa_header_set (s, 
		LSA_OPTIONS_GET (area) | LSA_OPTIONS_NSSA_GET (area) | OSPF_OPTION_O,
		OSPF_OPAQUE_AREA_LSA, opaque_id, u0_device->ne_ip);

  	/* Set opaque-LSA body fields. */
  	ospf_opaque_type10_lsa_body_set_for_u0 (s, u0_device);

    if((length = (stream_get_endp (s))%4) !=0)
    {
        for(int j = 0; j< 4-length; j++)
        {
  	        stream_putc(s, 0x0);
        }
            
    }

  	/* Set length. */
  	length = stream_get_endp (s);
  	lsah = (struct lsa_header *) STREAM_DATA (s);
  	lsah->length = htons (length);

  	/* Now, create OSPF LSA instance. */
  	if ( (newlsa = ospf_lsa_new ()) == NULL)
    {
		zlog_err("%s():%d: failed!", __FUNCTION__, __LINE__);
		return NULL;
    }
  
  	newlsa->area = area;
  	SET_FLAG (newlsa->flags, OSPF_LSA_SELF | OSPF_LSA_SELF_CHECKED);

  	/* Copy LSA data to store, discard stream. */
  	newlsa->data = ospf_lsa_data_new (length);
	if(newlsa->data == NULL)
	{
		zlog_err("%s %d Can't creat ospf_lsa_data_new : malloc failed",__func__,__LINE__);
		return NULL;
	}
  	memcpy (newlsa->data, lsah, length);
  	stream_free (s);

  	return newlsa;
}

int ospf_opaque_type10_lsa_generate_for_u0 (void *arg, struct u0_device_info *u0_device)
{
	struct ospf_area *area = NULL;
	struct ospf_lsa *newlsa = NULL;

	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG();
	}
	area = (struct ospf_area *)arg;

	if(!om->u0_flag)
	{
		zlog_warn ("%s[%d]: has no u0 info add, error!",__FUNCTION__, __LINE__);
		//printf("%s[%d]: ospf dcn is disabled now.can't generate type 10 lsa for dcn!\n",__FUNCTION__, __LINE__);
		return 0;
	}
  	/* Create new opaque-LSA instance. */
  	if ((newlsa = ospf_opaque_type10_lsa_new_for_u0 (area, u0_device)) == NULL)
	{
		zlog_err("%s():%d: failed!", __FUNCTION__, __LINE__);
	  	return -1;
	}

  	/* Sanity check. */
  	if (newlsa->data->adv_router.s_addr == 0)
	{
		if (IS_DEBUG_OSPF_EVENT)
		{
			zlog_debug (OSPF_DBG_EVENT, "LSA[Type10]: AdvRouter is 0, discard");
		}
		
	  	ospf_lsa_discard (newlsa);
		
	  	return -1;
	}

  	/* Install LSA to LSDB. */
  	newlsa = ospf_lsa_install (area->ospf, NULL, newlsa);

  	/* Update LSA origination count. */
  	area->ospf->lsa_originate_count++;

  	/* Flooding new LSA through area. */
  	ospf_flood_through_area (area, NULL, newlsa);

  	if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
	{
		zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Originate opaque-type10-LSA %p",
					newlsa->data->type, inet_ntoa (newlsa->data->id),
					(void *)newlsa);
	  	ospf_lsa_header_dump (newlsa->data);
	}

  	return 0;
}

struct ospf_lsa *ospf_opaque_type10_lsa_refresh_for_u0 (struct ospf_lsa *lsa)
{
	struct ospf_area *area = lsa->area;
	struct ospf_lsa *newlsa = NULL;
	struct u0_device_info_local *u0_device = NULL;

	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG();
	}
  	/* Sanity check. */
  	assert (lsa->data);

  	/* Delete LSA from neighbor retransmit-list. */
  	ospf_ls_retransmit_delete_nbr_area (area, lsa);

  	/* Unregister LSA from refresh-list */
  	ospf_refresher_unregister_lsa (area->ospf, lsa);

	/* lookup target u0 device info*/
	if((u0_device = lookup_u0_device_by_adv_router(lsa->data->adv_router)) == NULL)
	{
		zlog_err("%s()[%d]: Target u0 device is not exist!", __FUNCTION__, __LINE__);
	  	return NULL;
	}
  
  	/* Create new router-LSA instance. */
  	if ( (newlsa = ospf_opaque_type10_lsa_new_for_u0 (area, &(u0_device->u0_info))) == NULL)
	{
		zlog_err("%s():%d: failed!", __FUNCTION__, __LINE__);
	  	return NULL;
	}
  
  	newlsa->data->ls_seqnum = lsa_seqnum_increment (lsa);

  	ospf_lsa_install (area->ospf, NULL, newlsa);

  	/* Flood LSA through area. */
  	ospf_flood_through_area (area, NULL, newlsa);

  	/* Debug logging. */
  	if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
	{
		zlog_debug (OSPF_DBG_LSA_GENERATE, "%s[%d]:LSA[Type%d:%s]: opaque-type10-LSA refresh",
			__FUNCTION__, __LINE__,
		 	newlsa->data->type, inet_ntoa (newlsa->data->id));
	  	ospf_lsa_header_dump (newlsa->data);
	}

  	return NULL;
}


/* LSA installation functions. */

/* Install router-LSA to an area. */
struct ospf_lsa *ospf_router_lsa_install (struct ospf *ospf, struct ospf_lsa *new_lsa, int rt_recalc)
{
    struct ospf_area *area = new_lsa->area;

    if (IS_OSPF_ABR (ospf) && IS_AREA_NSSA(new_lsa->area))
    {
        if (ospf->restarting == 0 && area->NSSATranslatorRole == OSPF_NSSA_ROLE_CANDIDATE)
        {
            if (ospf_abr_nssa_am_elected (area) > 0)
            {
                if(area->NSSATranslatorState != OSPF_NSSA_TRANSLATE_ENABLED)
                {
                    area->NSSATranslatorState = OSPF_NSSA_TRANSLATE_ENABLED;
                    ospf_schedule_abr_task(ospf);
                    if (IS_DEBUG_OSPF (nssa, NSSA))
                    {
                        zlog_debug (OSPF_DBG_NSSA, "ospf_abr_nssa_check_status: elected translator");
                    }
                }
            }
            else
            {
                if(area->NSSATranslatorState != OSPF_NSSA_TRANSLATE_DISABLED)
                {
                    ospf_translated_external_lsa_flush_byarea(ospf, area);
                    area->NSSATranslatorState = OSPF_NSSA_TRANSLATE_DISABLED;
                    if (IS_DEBUG_OSPF (nssa, NSSA))
                    {
                        zlog_debug (OSPF_DBG_NSSA, "ospf_abr_nssa_check_status: " "not elected");
                    }
                }
            }
        }
    }
    else if (!IS_OSPF_ABR (area->ospf) && IS_AREA_NSSA(new_lsa->area))
    {
        if(area->NSSATranslatorState == OSPF_NSSA_TRANSLATE_ENABLED)
        {
            if (IS_DEBUG_OSPF (nssa, NSSA))
                zlog_debug (OSPF_DBG_NSSA, "ospf_abr_nssa_check_status: "
                            "not ABR");
            ospf_translated_external_lsa_flush_byarea(ospf, area);
            area->NSSATranslatorState = OSPF_NSSA_TRANSLATE_DISABLED;
        }
    }

    /* RFC 2328 Section 13.2 Router-LSAs and network-LSAs
       The entire routing table must be recalculated, starting with
       the shortest path calculations for each area (not just the
       area whose link-state database has changed).
    */

    if (IS_LSA_SELF (new_lsa))
    {
        /* Only install LSA if it is originated/refreshed by us.
         * If LSA was received by flooding, the RECEIVED flag is set so do
         * not link the LSA */
        if (CHECK_FLAG (new_lsa->flags, OSPF_LSA_RECEIVED))
        {
            return new_lsa;    /* ignore stale LSA */
        }
        /* Set self-originated router-LSA. */
		if(new_lsa->data->adv_router.s_addr == ospf->router_id.s_addr)//add by zzl 20190103 for u0 manager
		{
	        ospf_lsa_unlock (&area->router_lsa_self);
	        area->router_lsa_self = ospf_lsa_lock (new_lsa);
		}
        ospf_refresher_register_lsa (ospf, new_lsa);
    }
    if (rt_recalc)
    {
        ospf_spf_calculate_schedule (ospf, SPF_FLAG_ROUTER_LSA_INSTALL);
    }
    return new_lsa;
}

/*
#define OSPF_INTERFACE_TIMER_ON(T,F,V) \
    if (!(T)) \
      (T) = thread_add_timer (master_ospf, (F), oi, (V))
*/

/* Install network-LSA to an area. */
struct ospf_lsa *ospf_network_lsa_install (struct ospf *ospf,
                          struct ospf_interface *oi,
                          struct ospf_lsa *new_lsa,
                          int rt_recalc)
{

    /* RFC 2328 Section 13.2 Router-LSAs and network-LSAs
       The entire routing table must be recalculated, starting with
       the shortest path calculations for each area (not just the
       area whose link-state database has changed).
    */
    if (IS_LSA_SELF (new_lsa))
    {
        /* We supposed that when LSA is originated by us, we pass the int
        for which it was originated. If LSA was received by flooding,
         the RECEIVED flag is set, so we do not link the LSA to the int. */
        if (CHECK_FLAG (new_lsa->flags, OSPF_LSA_RECEIVED))
        {
            return new_lsa;    /* ignore stale LSA */
        }
        ospf_lsa_unlock (&oi->network_lsa_self);
        oi->network_lsa_self = ospf_lsa_lock (new_lsa);
        ospf_refresher_register_lsa (ospf, new_lsa);
    }
    if (rt_recalc)
    {
        ospf_spf_calculate_schedule (ospf, SPF_FLAG_NETWORK_LSA_INSTALL);
    }

    return new_lsa;
}

/* Install summary-LSA to an area. */
struct ospf_lsa *ospf_summary_lsa_install (struct ospf *ospf, struct ospf_lsa *new_lsa,
                          int rt_recalc)
{
    if (rt_recalc && !IS_LSA_SELF (new_lsa))
    {
        /* RFC 2328 Section 13.2 Summary-LSAs
        The best route to the destination described by the summary-
         LSA must be recalculated (see Section 16.5).  If this
         destination is an AS boundary router, it may also be
         necessary to re-examine all the AS-external-LSAs.
            */
#if 0
        /* This doesn't exist yet... */
        ospf_summary_incremental_update(new);
        */
#else /* #if 0 */
        ospf_spf_calculate_schedule (ospf, SPF_FLAG_SUMMARY_LSA_INSTALL);
#endif /* #if 0 */
    }

    if (IS_LSA_SELF (new_lsa))
    {
        ospf_refresher_register_lsa (ospf, new_lsa);
    }

    return new_lsa;
}

/* Install ASBR-summary-LSA to an area. */
static struct ospf_lsa *
ospf_summary_asbr_lsa_install (struct ospf *ospf, struct ospf_lsa *new_lsa,
                               int rt_recalc)
{
    if (rt_recalc && !IS_LSA_SELF (new_lsa))
    {
        /* RFC 2328 Section 13.2 Summary-LSAs
        The best route to the destination described by the summary-
         LSA must be recalculated (see Section 16.5).  If this
         destination is an AS boundary router, it may also be
         necessary to re-examine all the AS-external-LSAs.
            */
#if 0
        /* These don't exist yet... */
        ospf_summary_incremental_update(new);
        /* Isn't this done by the above call?
        - RFC 2328 Section 16.5 implies it should be */
        /* ospf_ase_calculate_schedule(); */
#else  /* #if 0 */
        ospf_spf_calculate_schedule (ospf, SPF_FLAG_ASBR_SUMMARY_LSA_INSTALL);
#endif /* #if 0 */
    }

    /* register LSA to refresh-list. */
    if (IS_LSA_SELF (new_lsa))
    {
        ospf_refresher_register_lsa (ospf, new_lsa);
    }

    return new_lsa;
}

struct ospf_lsa *
ospf_find_nssa_lsa_highestprio (struct ospf *ospf, struct in_addr *lsa_id)
{
    struct listnode *node = NULL;
    struct route_node *rn = NULL;
    struct ospf_area *area = NULL;
    struct ospf_lsa *lsa = NULL;
    struct ospf_lsa *lsa_best = NULL;

    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        if (! IS_AREA_NSSA(area))
        {
            continue;
        }
        if(area->NSSATranslatorState == OSPF_NSSA_TRANSLATE_DISABLED)
        {
            LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
            {
                if (IPV4_ADDR_SAME(&lsa->data->id, lsa_id) && !IS_LSA_MAXAGE(lsa) && IS_LSA_SELF(lsa))
                {
                    if (lsa_best == NULL)
                    {
                        lsa_best = lsa;
                    }
                    else
                    {
                        struct as_external_lsa *al = (struct as_external_lsa *)lsa->data,
                                                *al_tmp = (struct as_external_lsa *)lsa_best->data;
                        if (IS_EXTERNAL_METRIC(al->e[0].tos) < IS_EXTERNAL_METRIC(al_tmp->e[0].tos))
                        {
                            lsa_best = lsa;
                        }
                        else if (IS_EXTERNAL_METRIC(al->e[0].tos) == IS_EXTERNAL_METRIC(al_tmp->e[0].tos))
                        {
                            if (GET_METRIC(al->e[0].metric) < GET_METRIC(al_tmp->e[0].metric))
                            {
                                lsa_best = lsa;
                            }
                            else if (GET_METRIC(al->e[0].metric) == GET_METRIC(al_tmp->e[0].metric))
                            {
                                if (IPV4_ADDR_CMP (&lsa->data->adv_router.s_addr, &lsa_best->data->adv_router.s_addr) > 0)
                                {
                                    lsa_best = lsa;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
            {
                if (IPV4_ADDR_SAME(&lsa->data->id, lsa_id) && !IS_LSA_MAXAGE(lsa))
                {
                    //ospf_lsa_header_dump(lsa->data);
                    if (lsa_best == NULL)
                    {
                        lsa_best = lsa;
                    }
                    else
                    {
                        struct as_external_lsa *al = (struct as_external_lsa *)lsa->data,
                                                *al_tmp = (struct as_external_lsa *)lsa_best->data;
                        if (IS_EXTERNAL_METRIC(al->e[0].tos) < IS_EXTERNAL_METRIC(al_tmp->e[0].tos))
                        {
                            lsa_best = lsa;
                        }
                        else if (IS_EXTERNAL_METRIC(al->e[0].tos) == IS_EXTERNAL_METRIC(al_tmp->e[0].tos))
                        {
                            if (GET_METRIC(al->e[0].metric) < GET_METRIC(al_tmp->e[0].metric))
                            {
                                lsa_best = lsa;
                            }
                            else if (GET_METRIC(al->e[0].metric) == GET_METRIC(al_tmp->e[0].metric))
                            {
                                if (IPV4_ADDR_CMP (&lsa->data->adv_router.s_addr, &lsa_best->data->adv_router.s_addr) > 0)
                                {
                                    lsa_best = lsa;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return lsa_best;
}


/* Install AS-external-LSA. */
struct ospf_lsa *ospf_external_lsa_install (struct ospf *ospf, struct ospf_lsa *new_lsa, int rt_recalc)
{
    struct ospf_lsa *lsa_tmp;
    struct ospf_lsa *type5;
    struct external_info *ei;
    struct as_external_lsa *ext7;
    struct prefix_ipv4 p;

    ospf_ase_register_external_lsa (new_lsa, ospf);
    /* If LSA is not self-originated, calculate an external route. */
    if (rt_recalc == 1)
    {
        /* RFC 2328 Section 13.2 AS-external-LSAs
              The best route to the destination described by the AS-
              external-LSA must be recalculated (see Section 16.6).
        */
        if (!IS_LSA_SELF (new_lsa))
        {
            ospf_ase_incremental_update (ospf, new_lsa);
        }
    }
    else if (rt_recalc == 2)
    {
        /* RFC 2328 Section 13.2 AS-external-LSAs
              The best route to the destination described by the AS-
              external-LSA must be recalculated (see Section 16.6).
        */
        if (!IS_LSA_SELF (new_lsa))
        {
            ospf_ase_calculate_schedule (ospf);
            ospf_ase_calculate_timer_add (ospf);
        }
    }

    if (new_lsa->data->type == OSPF_AS_NSSA_LSA)
    {
        /* There is no point to register selforiginate Type-7 LSA for
         * refreshing. We rely on refreshing Type-5 LSA's
         */
        if (IS_LSA_SELF (new_lsa))
        {
            if (!CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT))
            {
                ospf_refresher_register_lsa (ospf, new_lsa);
            }
            return new_lsa;
        }
        else
        {
            /* Try refresh type-5 translated LSA for this LSA, if one exists.
             * New translations will be taken care of by the abr_task.
             */
            if(IS_OSPF_ABR(ospf))
            {
                ext7 = (struct as_external_lsa *)(new_lsa->data);
				
				memset(&p, 0, sizeof(struct prefix_ipv4));
				p.family = AF_INET;
                p.prefix = new_lsa->data->id;
                p.prefixlen = ip_masklen (ext7->mask);
				
                type5 = ospf_external_info_find_lsa (ospf, &p);
                lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &new_lsa->data->id);

				if (lsa_tmp == NULL)
                {
		            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
		            {
						zlog_debug(OSPF_DBG_LSA_GENERATE, "LSA TMP is NULL %s %d %s\n",__func__,__LINE__,inet_ntoa(new_lsa->data->id));
		            }
                    /*cannot find highest priority Type-7 LSA,but Type 5 is not NULL,then flush Type 5*/
                    if (type5 != NULL)
                    {
                        /* Sweep LSA from Link State Retransmit List. */
                        ospf_ls_retransmit_delete_nbr_as (ospf, type5);
                        if (!IS_LSA_MAXAGE (type5))
                        {
                            /* Unregister LSA from Refresh queue. */
                            ospf_refresher_unregister_lsa (ospf, type5);
                            /* Flush AS-external-LSA through AS. */
                            ospf_lsa_flush_as (ospf, type5);
                        }
                    }
                }
                else/*find the higheset priority Type-7 LSA*/
				{
					if (!IS_LSA_SELF(lsa_tmp))
		            {
		                if(type5 == NULL)
		                {
							if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
							{
								zlog_debug(OSPF_DBG_LSA_GENERATE, "lsa_tmp is exist,type5 is NULL %s %d %s\n",__func__,__LINE__,inet_ntoa(lsa_tmp->data->id));
							}
		                    /* Recive Type-7 LSA but can't find Type 5 ,then 7->5*/
		                    ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
		                }
		                else
		                {
							if(lsa_tmp == new_lsa)
							{
								if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
								{
									zlog_debug(OSPF_DBG_LSA_GENERATE, "the coming lsa is the best %s %d %s\n",__func__,__LINE__,inet_ntoa(lsa_tmp->data->id));
								}
								/* The  higheset priority LSA is the new_lsa recive,then 7->5*/
								ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
							}
							else
							{
								/*Type 5 LSA is translat from Type 7,but the higest priority is changed,then 7->5*/
		                       	if(!(IPV4_ADDR_SAME(&lsa_tmp->data->adv_router, &type5->adv_type7)))
		                        {
									if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
									{
										zlog_debug(OSPF_DBG_LSA_GENERATE, "the best is changed %s %d %s\n",__func__,__LINE__,inet_ntoa(lsa_tmp->data->id));
									}
		                            ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
		                        }
							}
							
		                }
		            }
	                else if (IS_LSA_SELF(lsa_tmp))
	                {
	                    ei = ospf_external_info_check (ospf, new_lsa);
	                    if (ei)
	                    {
	                        if(type5 != NULL)
	                        {
	                            if(CHECK_FLAG (type5->flags, OSPF_LSA_LOCAL_XLT)||IS_LSA_MAXAGE(type5))
	                            {
	                                zlog_debug(OSPF_DBG_LSA_REFRESH, "%s[%d] Choose the self originate LSA ,refresh it",__func__,__LINE__);
	                                ospf_external_lsa_refresh_only(ospf, type5, ei, LSA_REFRESH_FORCE);
	                            }
	                        }
	                    }
	                }
                }
            }
            /* IF is not ABR ,do nothing*/
        }
    }

    /* Register self-originated LSA to refresh queue.
     * Leave Translated LSAs alone if NSSA is enabled
     */
    if (IS_LSA_SELF (new_lsa) && !CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT ) )
    {
        ospf_refresher_register_lsa (ospf, new_lsa);
    }

    return new_lsa;
}

#if 0
static struct ospf_lsa *
ospf_nssa_lsa_install(struct ospf *ospf, struct ospf_lsa *new_lsa,
                      int rt_recalc)
{
    struct ospf_lsa *lsa_tmp;
    struct ospf_lsa *type5;
    struct external_info *ei;
    struct as_external_lsa *ext7;
    struct prefix_ipv4 p;
    /* If LSA is not self-originated, calculate an external route. */
    if (rt_recalc)
    {
        /* RFC 2328 Section 13.2 AS-external-LSAs
              The best route to the destination described by the AS-
              external-LSA must be recalculated (see Section 16.6).
        */
        if (!IS_LSA_SELF (new_lsa))
        {
            ospf_ase_incremental_update (ospf, new_lsa);
        }
    }

    /* There is no point to register selforiginate Type-7 LSA for
       * refreshing. We rely on refreshing Type-5 LSA's
       */
    if (IS_LSA_SELF (new_lsa))
    {
        if (!CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT))
        {
            ospf_refresher_register_lsa (ospf, new_lsa);
        }
        return new_lsa;
    }
    else
    {
        /* Try refresh type-5 translated LSA for this LSA, if one exists.
         * New translations will be taken care of by the abr_task.
         */
        if(IS_OSPF_ABR(ospf))
        {
            ext7 = (struct as_external_lsa *)(new_lsa->data);
            p.prefix = new_lsa->data->id;
            p.prefixlen = ip_masklen (ext7->mask);
            type5 = ospf_external_info_find_lsa (ospf, &p);
            lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &new_lsa->data->id);
            if (lsa_tmp && !IS_LSA_SELF(lsa_tmp))
            {
                if(type5 == NULL)
                {
                    /* Recive Type-7 LSA but can't find Type 5 ,then 7->5*/
                    ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                }
                else
                {
                    /* Type 5 LSA is Not translate from Type 7 ,then 7->5*/
                    if(!CHECK_FLAG (type5->flags, OSPF_LSA_LOCAL_XLT))
                    {
                        ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                    }
                    else
                    {
                        /*coming LSA is the Type 5 LSA ,then 7->5*/
                        if(type5->type7 && type5->type7->data == NULL)
                        {
                            ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                        }
                        else if(type5->type7 && type5->type7->data)
                        {
                            /*Type 5 LSA is translat from Type 7,but the higest priority is changed,then 7->5*/
                            if(!(IPV4_ADDR_SAME(&lsa_tmp->data->id, &type5->type7->data->id)
                                    && IPV4_ADDR_SAME(&lsa_tmp->data->adv_router, &type5->type7->data->adv_router)))
                            {
                                ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                            }
                        }
                    }
                }
            }
            else if (lsa_tmp && IS_LSA_SELF(lsa_tmp))
            {
                ei = ospf_external_info_check (ospf, new_lsa);
                if (ei)
                {
                    if(type5 != NULL)
                    {
                        if(CHECK_FLAG (type5->flags, OSPF_LSA_LOCAL_XLT)||IS_LSA_MAXAGE(type5))
                        {
                            zlog_debug("%s[%d] Choose the self originate LSA ,refresh it",__func__,__LINE__);
                            ospf_external_lsa_refresh_only(ospf, type5, ei, LSA_REFRESH_FORCE);
                        }
                    }
                }
            }
            if (lsa_tmp == NULL)
            {
                /*cannot find highest priority Type-7 LSA,but Type 5 is not NULL,then flush Type 5*/
                if (type5 != NULL)
                {
                    /* Sweep LSA from Link State Retransmit List. */
                    ospf_ls_retransmit_delete_nbr_as (ospf, type5);
                    if (!IS_LSA_MAXAGE (type5))
                    {
                        /* Unregister LSA from Refresh queue. */
                        ospf_refresher_unregister_lsa (ospf, type5);
                        /* Flush AS-external-LSA through AS. */
                        ospf_lsa_flush_as (ospf, type5);
                    }
                }
            }
        }
    }

}
#endif

void
ospf_discard_from_db (struct ospf *ospf,
                      struct ospf_lsdb *lsdb, struct ospf_lsa *lsa)
{
    struct ospf_lsa *old;
    if (!lsdb)
    {
        zlog_warn ("%s: Called with NULL lsdb!", __func__);
        if (!lsa)
        {
            zlog_warn ("%s: and NULL LSA!", __func__);
        }
        else
            zlog_warn ("LSA[Type%d:%s]: not associated with LSDB!",
                       lsa->data->type, inet_ntoa (lsa->data->id));
        return;
    }
    old = ospf_lsdb_lookup (lsdb, lsa);
    if (!old)
    {
        return;
    }
    if (old->refresh_list >= 0)
    {
        ospf_refresher_unregister_lsa (ospf, old);
    }
    switch (old->data->type)
    {
    case OSPF_AS_EXTERNAL_LSA:
        ospf_ase_unregister_external_lsa (old, ospf);
        ospf_ls_retransmit_delete_nbr_as (ospf, old);
        break;
    case OSPF_OPAQUE_AS_LSA:
        ospf_ls_retransmit_delete_nbr_as (ospf, old);
        break;
    case OSPF_AS_NSSA_LSA:
        ospf_ls_retransmit_delete_nbr_area (old->area, old);
        ospf_ase_unregister_external_lsa (old, ospf);
        break;
    default:
        ospf_ls_retransmit_delete_nbr_area (old->area, old);
        break;
    }
    ospf_lsa_maxage_delete (ospf, old);
    ospf_lsa_discard (old);
}

struct ospf_lsa *
ospf_lsa_install (struct ospf *ospf, struct ospf_interface *oi,
                  struct ospf_lsa *lsa)
{
    int rt_recalc;
	int ret = 0;
    struct ospf_lsa *new_lsa = NULL;
    struct ospf_lsa *old = NULL;
    struct ospf_lsdb *lsdb = NULL;
    struct as_external_lsa *al;
	struct ospf_dcn_ne_info ne_info;
	struct ospf_dcn_ne_info ne_info_old;
	

    /* Set LSDB. */
    switch (lsa->data->type)
    {
        /* kevinm */
    case OSPF_AS_NSSA_LSA:
        if (lsa->area)
        {
            lsdb = lsa->area->lsdb;
        }
        else
        {
            lsdb = ospf->lsdb;
        }
        break;
    case OSPF_AS_EXTERNAL_LSA:
    case OSPF_OPAQUE_AS_LSA:
        lsdb = ospf->lsdb;
        break;
    default:
        lsdb = lsa->area->lsdb;
        break;
    }
	if(lsdb == NULL)
	{
		zlog_warn("%s[%d] lsdb == NULL",__func__,__LINE__);
		return NULL;
	}

    assert (lsdb);

    /*  RFC 2328 13.2.  Installing LSAs in the database

          Installing a new LSA in the database, either as the result of
          flooding or a newly self-originated LSA, may cause the OSPF
          routing table structure to be recalculated.  The contents of the
          new LSA should be compared to the old instance, if present.  If
          there is no difference, there is no need to recalculate the
          routing table. When comparing an LSA to its previous instance,
          the following are all considered to be differences in contents:

              o   The LSA's Options field has changed.

              o   One of the LSA instances has LS age set to MaxAge, and
                  the other does not.

              o   The length field in the LSA header has changed.

              o   The body of the LSA (i.e., anything outside the 20-byte
                  LSA header) has changed. Note that this excludes changes
                  in LS Sequence Number and LS Checksum.

    */
    /* Look up old LSA and determine if any SPF calculation or incremental
       update is needed */
    old = ospf_lsdb_lookup (lsdb, lsa);

    /* Do comparision and record if recalc needed. */
    rt_recalc = 0;
    if (  old == NULL || ospf_lsa_different(old, lsa))
    {
        rt_recalc = 1;
    }

    /*
       Sequence number check (Section 14.1 of rfc 2328)
       "Premature aging is used when it is time for a self-originated
        LSA's sequence number field to wrap.  At this point, the current
        LSA instance (having LS sequence number MaxSequenceNumber) must
        be prematurely aged and flushed from the routing domain before a
        new instance with sequence number equal to InitialSequenceNumber
        can be originated. "
     */

    if (ntohl(lsa->data->ls_seqnum) - 1 == OSPF_MAX_SEQUENCE_NUMBER)
    {
        if (ospf_lsa_is_self_originated(ospf, lsa))
        {
            lsa->data->ls_seqnum = htonl(OSPF_MAX_SEQUENCE_NUMBER);
            if (!IS_LSA_MAXAGE(lsa))
            {
                lsa->flags |= OSPF_LSA_PREMATURE_AGE;
            }
            lsa->data->ls_age = htons (OSPF_LSA_MAXAGE);
            if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
            {
                zlog_debug (OSPF_DBG_LSA_REFRESH, "ospf_lsa_install() Premature Aging "
                            "lsa 0x%p, seqnum 0x%x",
                            (void *)lsa, ntohl(lsa->data->ls_seqnum));
                ospf_lsa_header_dump (lsa->data);
            }
        }
        else
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
            {
                zlog_debug (OSPF_DBG_LSA_GENERATE, "ospf_lsa_install() got an lsa with seq 0x80000000 "
                            "that was not self originated. Ignoring\n");
                ospf_lsa_header_dump (lsa->data);
            }
            return old;
        }
    }

	memset(&ne_info_old, 0, sizeof(struct ospf_dcn_ne_info));
    /* discard old LSA from LSDB */
    if (old != NULL)
    {
        if(IS_OPAQUE_AREA_LSA(old->data->type) && (ospf->dcn_report))
		    get_dcn_ne_info_from_tlv(old, NULL, &ne_info_old);//获取旧lsdb中携带的网元信息
        /*
            当新旧LSA仅掩码不同时，不采用增量更新路由
        */
        if (lsa->data->type == OSPF_AS_EXTERNAL_LSA || lsa->data->type == OSPF_AS_NSSA_LSA)
        {
            struct as_external_lsa *al_new = (struct as_external_lsa *)lsa->data;
            struct as_external_lsa *al_old = (struct as_external_lsa *)old->data;
            if (al_new->mask.s_addr != al_old->mask.s_addr)
            {
                rt_recalc = 2;
            }
        }
        ospf_discard_from_db (ospf, lsdb, lsa);
    }

    /* Calculate Checksum if self-originated?. */
    if (IS_LSA_SELF (lsa))
    {
        ospf_lsa_checksum (lsa->data);
    }

    ospf_lsdb_add (lsdb, lsa);
    lsa->lsdb = lsdb;

    /* Do LSA specific installation instance. */
    switch (lsa->data->type)
    {
    case OSPF_ROUTER_LSA:
        new_lsa = ospf_router_lsa_install (ospf, lsa, rt_recalc);
		if(ospf->ospf_id == OSPF_MAX_PROCESS_ID && ospf->dcn_enable)
		{
			ospf_dcn_ne_status_handle(ospf);
		}
        break;
    case OSPF_NETWORK_LSA:
		if(oi == NULL)
		{
			zlog_warn("%s[%d] oi == NULL",__func__,__LINE__);
			return NULL;
		}
        assert (oi);
        new_lsa = ospf_network_lsa_install (ospf, oi, lsa, rt_recalc);
        break;
    case OSPF_SUMMARY_LSA:
        new_lsa = ospf_summary_lsa_install (ospf, lsa, rt_recalc);
        break;
    case OSPF_ASBR_SUMMARY_LSA:
        new_lsa = ospf_summary_asbr_lsa_install (ospf, lsa, rt_recalc);
        break;
    case OSPF_AS_EXTERNAL_LSA:
        new_lsa = ospf_external_lsa_install (ospf, lsa, rt_recalc);
        break;
    case OSPF_OPAQUE_LINK_LSA:
        if (IS_LSA_SELF (lsa))
        {
            lsa->oi = oi;    /* Specify outgoing ospf-interface for this LSA. */
        }
        else
        {
            /* Incoming "oi" for this LSA has set at LSUpd reception. */
        }
        /* Fallthrough */
    case OSPF_OPAQUE_AREA_LSA:
    case OSPF_OPAQUE_AS_LSA:
        new_lsa = ospf_opaque_lsa_install (lsa, rt_recalc);
	
		if((new_lsa != NULL) && (IS_OPAQUE_AREA_LSA(new_lsa->data->type)) && (ospf->dcn_report))
		{
			memset(&ne_info, 0, sizeof(struct ospf_dcn_ne_info));
			ret = get_dcn_ne_info_from_tlv(new_lsa, NULL, &ne_info);
			//如果本次lsa携带的网元信息与旧的lsa一致，则不用再trap
			if((ne_info.dcn_ne_id != ne_info_old.dcn_ne_id)
				|| (ne_info.dcn_ne_ip != ne_info_old.dcn_ne_ip))
			{
				if(!ret)
				{
					ospf_dcn_ne_info_trap(&ne_info, OSPF_DCN_NE_ONLINE);
				}
			}
		}
        break;
    case OSPF_AS_NSSA_LSA:
        new_lsa = ospf_external_lsa_install (ospf, lsa, rt_recalc);
		break;
    default: /* type-6,8,9....nothing special */
        break;
    }

    if (new_lsa == NULL)
    {
        return new_lsa;    /* Installation failed, cannot proceed further -- endo. */
    }

    /* Debug logs. */
    if (IS_DEBUG_OSPF (lsa, LSA_INSTALL))
    {
        char area_str[INET_ADDRSTRLEN];
        switch (lsa->data->type)
        {
        case OSPF_AS_EXTERNAL_LSA:
        case OSPF_OPAQUE_AS_LSA:
        case OSPF_AS_NSSA_LSA:
            zlog_debug (OSPF_DBG_LSA, "LSA[%s]: Install %s",
                        dump_lsa_key (new_lsa),
                        LOOKUP (ospf_lsa_type_msg, new_lsa->data->type));
            break;
        default:
            strcpy (area_str, inet_ntoa (new_lsa->area->area_id));
            zlog_debug (OSPF_DBG_LSA, "LSA[%s]: Install %s to Area %s",
                        dump_lsa_key (new_lsa),
                        LOOKUP (ospf_lsa_type_msg, new_lsa->data->type), area_str);
            break;
        }
    }

    /*
       If received LSA' ls_age is MaxAge, or lsa is being prematurely aged
       (it's getting flushed out of the area), set LSA on MaxAge LSA list.
     */
    if (IS_LSA_MAXAGE (new_lsa))
    {
        if (lsa->data->type == OSPF_AS_EXTERNAL_LSA)
        {
            al = (struct as_external_lsa *)lsa->data;
            if (al->e[0].fwd_addr.s_addr != 0)
            {
                ospf_schedule_asbr_task(ospf);
            }
        }
        if (lsa->data->type == OSPF_AS_NSSA_LSA)
        {
            al = (struct as_external_lsa *)lsa->data;
            if (al->e[0].fwd_addr.s_addr != 0)
            {
                ospf_schedule_abr_task(ospf);
            }
        }
        if (IS_DEBUG_OSPF (lsa, LSA_INSTALL))
            zlog_debug (OSPF_DBG_LSA, "LSA[Type%d:%s]: Install LSA 0x%p, MaxAge",
                        new_lsa->data->type,
                        inet_ntoa (new_lsa->data->id),
                        (void *)lsa);
        ospf_lsa_maxage (ospf, lsa);
    }

    return new_lsa;
}


int
ospf_check_nbr_status (struct ospf *ospf)
{
    struct listnode *node, *nnode;
    struct ospf_interface *oi;
    for (ALL_LIST_ELEMENTS (ospf->oiflist, node, nnode, oi))
    {
        struct route_node *rn;
        struct ospf_neighbor *nbr;
        if (ospf_if_is_enable (oi))
        {
            for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
            {
                if ((nbr = rn->info) != NULL)
                {
                    if (nbr->state == NSM_Exchange || nbr->state == NSM_Loading)
                    {
                        route_unlock_node (rn);
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}



static int
ospf_maxage_lsa_remover (void *thread)
{
    struct ospf *ospf = (struct ospf *)(thread);
    struct ospf_lsa *lsa;
    struct route_node *rn;
    int reschedule = 0;
	//struct ospf_dcn_ne_info ne_info;
	//int ret = 0;
	
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    //ospf->t_maxage = NULL;
	ospf->t_maxage = 0;
    if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
    {
        zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA[MaxAge]: remover Start");
    }
    reschedule = !ospf_check_nbr_status (ospf);
    if (!reschedule)
    {
        for (rn = route_top(ospf->maxage_lsa); rn; rn = route_next(rn))
        {
            if ((lsa = rn->info) == NULL)
            {
                continue;
            }
            /* There is at least one neighbor from which we still await an ack
             * for that LSA, so we are not allowed to remove it from our lsdb yet
             * as per RFC 2328 section 14 para 4 a) */
            if (lsa->retransmit_counter > 0)
            {
                zlog_warn("%s %d retransmit_counter is not 0 is:%d",__func__,__LINE__,lsa->retransmit_counter);
                reschedule = 1;
                continue;
            }
            /* TODO: maybe convert this function to a work-queue */
#if 0			
            if (thread_should_yield (thread))
            {
                zlog_warn("%s %d thread_should_yield error:",__func__,__LINE__);
                //OSPF_TIMER_ON (ospf->t_maxage, ospf_maxage_lsa_remover, 0);
                route_unlock_node(rn); /* route_top/route_next */
				ospf_maxage_lsa_remover(ospf);
                return 0;
            }
#endif
            /* Remove LSA from the LSDB */
            if (IS_LSA_SELF (lsa))
                if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
                    zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA[Type%d:%s]: LSA 0x%lx is self-originated: ",
                                lsa->data->type, inet_ntoa (lsa->data->id), (u_long)lsa);
            if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
                zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA[Type%d:%s]: MaxAge LSA removed from list",
                            lsa->data->type, inet_ntoa (lsa->data->id));
            if (CHECK_FLAG (lsa->flags, OSPF_LSA_PREMATURE_AGE))
            {
                if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
                {
                    zlog_debug (OSPF_DBG_LSA_FLOODING, "originating new lsa for lsa 0x%p\n", (void *)lsa);
                }
                ospf_lsa_refresh (ospf, lsa);
            }
            /* Remove from lsdb. */
            if (lsa->lsdb)
            {
                ospf_discard_from_db (ospf, lsa->lsdb, lsa);
                ospf_lsdb_delete (lsa->lsdb, lsa);
            }
            else
                zlog_warn ("%s: LSA[Type%d:%s]: No associated LSDB!", __func__,
                           lsa->data->type, inet_ntoa (lsa->data->id));
        }
    }
    /*    A MaxAge LSA must be removed immediately from the router's link
          state database as soon as both a) it is no longer contained on any
          neighbor Link state retransmission lists and b) none of the router's
          neighbors are in states Exchange or Loading. */
    ospf->maxage_delay = OSPF_LSA_MAXAGE_REMOVE_DELAY_DEFAULT;
    if (reschedule)
        OSPF_TIMER_ON (ospf->t_maxage, ospf_maxage_lsa_remover,
                       ospf->maxage_delay);
    return 0;
}

void
ospf_lsa_maxage_delete (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct route_node *rn;
    struct prefix_ptr lsa_prefix;
    lsa_prefix.family = 0;
    lsa_prefix.prefixlen = sizeof(lsa_prefix.prefix) * CHAR_BIT;
    lsa_prefix.prefix = (uintptr_t) lsa;
    if ((rn = route_node_lookup(ospf->maxage_lsa,
                                (struct prefix *)&lsa_prefix)))
    {
        if (rn->info == lsa)
        {
            UNSET_FLAG(lsa->flags, OSPF_LSA_IN_MAXAGE);
            ospf_lsa_unlock (&lsa); /* maxage_lsa */
            rn->info = NULL;
            route_unlock_node (rn); /* unlock node because lsa is deleted */
        }
        route_unlock_node (rn); /* route_node_lookup */
    }
}

/* Add LSA onto the MaxAge list, and schedule for removal.
 * This does *not* lead to the LSA being flooded, that must be taken
 * care of elsewhere, see, e.g., ospf_lsa_flush* (which are callers of this
 * function).
 */
void
ospf_lsa_maxage (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct prefix_ptr lsa_prefix;
    struct route_node *rn;
    /* When we saw a MaxAge LSA flooded to us, we put it on the list
       and schedule the MaxAge LSA remover. */
    if (CHECK_FLAG(lsa->flags, OSPF_LSA_IN_MAXAGE))
    {
        if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
            zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA[Type%d:%s]: %p already exists on MaxAge LSA list",
                        lsa->data->type, inet_ntoa (lsa->data->id), (void *)lsa);
        return;
    }
    lsa_prefix.family = 0;
    lsa_prefix.prefixlen = sizeof(lsa_prefix.prefix) * CHAR_BIT;
    lsa_prefix.prefix = (uintptr_t) lsa;
    if ((rn = route_node_get (ospf->maxage_lsa,
                              (struct prefix *)&lsa_prefix)) != NULL)
    {
        if (rn->info != NULL)
        {
            if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
                zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA[%s]: found LSA (%p) in table for LSA %p %d",
                            dump_lsa_key (lsa), rn->info, (void *)lsa,
                            lsa_prefix.prefixlen);
            route_unlock_node (rn);
        }
        else
        {
            rn->info = ospf_lsa_lock(lsa);
            SET_FLAG(lsa->flags, OSPF_LSA_IN_MAXAGE);
			zlog_debug (OSPF_DBG_LSA_FLOODING, "%s[%d]:LSA[%s]: add into maxage_lsa list", __FUNCTION__, __LINE__, dump_lsa_key (lsa));
        }
    }
    else
    {
        zlog_err("Unable to allocate memory for maxage lsa\n");
        assert(0);
    }
    if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
    {
        zlog_debug (OSPF_DBG_LSA_FLOODING, "LSA[%s]: MaxAge LSA remover scheduled.", dump_lsa_key (lsa));
    }
    OSPF_TIMER_ON (ospf->t_maxage, ospf_maxage_lsa_remover,
                   ospf->maxage_delay);
}

static int
ospf_lsa_maxage_walker_remover (struct ospf *ospf, struct ospf_lsa *lsa)
{
    /* Stay away from any Local Translated Type-7 LSAs */
    if (CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT))
    {
        return 0;
    }
    if (IS_LSA_MAXAGE (lsa))
    {
        /* Self-originated LSAs should NOT time-out instead,
           they're flushed and submitted to the max_age list explicitly. */
        if (!ospf_lsa_is_self_originated (ospf, lsa))
        {
            if (IS_DEBUG_OSPF (lsa, LSA_FLOODING))
            {
                zlog_debug(OSPF_DBG_LSA_FLOODING, "LSA[%s]: is MaxAge", dump_lsa_key (lsa));
            }
            switch (lsa->data->type)
            {
            case OSPF_OPAQUE_LINK_LSA:
            case OSPF_OPAQUE_AREA_LSA:
            case OSPF_OPAQUE_AS_LSA:
                /*
                 * As a general rule, whenever network topology has changed
                 * (due to an LSA removal in this case), routing recalculation
                 * should be triggered. However, this is not true for opaque
                 * LSAs. Even if an opaque LSA instance is going to be removed
                 * from the routing domain, it does not mean a change in network
                 * topology, and thus, routing recalculation is not needed here.
                 */
                break;
            case OSPF_AS_EXTERNAL_LSA:
            case OSPF_AS_NSSA_LSA:
                ospf_ase_incremental_update (ospf, lsa);
                break;
            default:
                ospf_spf_calculate_schedule (ospf, SPF_FLAG_MAXAGE);
                break;
            }
            ospf_lsa_maxage (ospf, lsa);
        }
    }
    if (IS_LSA_MAXAGE (lsa) && !ospf_lsa_is_self_originated (ospf, lsa))
    {
        if (LS_AGE (lsa) > OSPF_LSA_MAXAGE + 30)
        {
            printf ("Eek! Shouldn't happen!\n");
        }
    }
    return 0;
}

/* Periodical check of MaxAge LSA. */
int
ospf_lsa_maxage_walker (void *thread)
{
    struct ospf *ospf = (struct ospf *)(thread);
    struct route_node *rn;
    struct ospf_lsa *lsa;
    struct ospf_area *area;
    struct listnode *node, *nnode;
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    //ospf->t_maxage_walker = NULL;
	ospf->t_maxage_walker = 0;
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        LSDB_LOOP (ROUTER_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (NETWORK_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (SUMMARY_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (ASBR_SUMMARY_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (OPAQUE_AREA_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (OPAQUE_LINK_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
    }
    /* for AS-external-LSAs. */
    if (ospf->lsdb)
    {
        LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
        LSDB_LOOP (OPAQUE_AS_LSDB (ospf), rn, lsa)
        {
            ospf_lsa_maxage_walker_remover (ospf, lsa);
        }
    }
    OSPF_TIMER_ON (ospf->t_maxage_walker, ospf_lsa_maxage_walker,
                   OSPF_LSA_MAXAGE_CHECK_INTERVAL);
    return 0;
}

struct ospf_lsa *
ospf_lsa_lookup_by_prefix (struct ospf_lsdb *lsdb, u_char type,
                           struct prefix_ipv4 *p, struct in_addr router_id)
{
    struct ospf_lsa *lsa;
    struct in_addr mask, id;
    struct lsa_header_mask
    {
        struct lsa_header header;
        struct in_addr mask;
    } *hmask;

    lsa = ospf_lsdb_lookup_by_id (lsdb, type, p->prefix, router_id);
    /*OSPF_ASBR_SUMMARY_LSA 不能应用附录E的方式查找*/
    if (type == OSPF_ASBR_SUMMARY_LSA)
    {
        return lsa;
    }

    if (lsa == NULL)
    {
        masklen2ip (p->prefixlen, &mask);
        id.s_addr = p->prefix.s_addr | (~mask.s_addr);
        lsa = ospf_lsdb_lookup_by_id (lsdb, type, id, router_id);
        if (!lsa)
        {
            return NULL;
        }
        hmask = (struct lsa_header_mask *) lsa->data;
        if (mask.s_addr != hmask->mask.s_addr)
        {
            return NULL;
        }
    }
    else
    {
        if(IS_LSA_MAXAGE(lsa))
        {
            return lsa;
        }
        masklen2ip (p->prefixlen, &mask);
        hmask = (struct lsa_header_mask *) lsa->data;
        if (mask.s_addr != hmask->mask.s_addr)
        {
            id.s_addr = p->prefix.s_addr | (~mask.s_addr);
            lsa = ospf_lsdb_lookup_by_id (lsdb, type, id, router_id);
            if (!lsa)
            {
                return NULL;
            }
            hmask = (struct lsa_header_mask *) lsa->data;
            if (mask.s_addr != hmask->mask.s_addr)
            {
                return NULL;
            }
        }
    }
    return lsa;
}

struct ospf_lsa *
ospf_lsa_lookup (struct ospf_area *area, u_int32_t type,
                 struct in_addr id, struct in_addr adv_router)
{

    struct ospf *ospf = NULL;
	
	if(area == NULL)
	{
		zlog_warn("%s[%d] area == NULL",__func__,__LINE__);
		return NULL;
	}

	ospf = area->ospf;
	if(ospf == NULL)
	{
		zlog_warn("%s[%d] ospf == NULL",__func__,__LINE__);
		return NULL;
	}

    switch (type)
    {
    case OSPF_ROUTER_LSA:
    case OSPF_NETWORK_LSA:
    case OSPF_SUMMARY_LSA:
    case OSPF_ASBR_SUMMARY_LSA:
    case OSPF_AS_NSSA_LSA:
    case OSPF_OPAQUE_LINK_LSA:
    case OSPF_OPAQUE_AREA_LSA:
        return ospf_lsdb_lookup_by_id (area->lsdb, type, id, adv_router);
    case OSPF_AS_EXTERNAL_LSA:
    case OSPF_OPAQUE_AS_LSA:
        return ospf_lsdb_lookup_by_id (ospf->lsdb, type, id, adv_router);
    default:
        break;
    }

    return NULL;
}

struct ospf_lsa *
ospf_lsa_lookup_by_id (struct ospf_area *area, u_int32_t type,
                       struct in_addr id)
{
    struct ospf_lsa *lsa;
    struct route_node *rn;

    switch (type)
    {
    case OSPF_ROUTER_LSA:
        return ospf_lsdb_lookup_by_id (area->lsdb, type, id, id);
    case OSPF_NETWORK_LSA:
        for (rn = route_top (NETWORK_LSDB (area)); rn; rn = route_next (rn))
        {
            if ((lsa = rn->info))
            {
                if (IPV4_ADDR_SAME (&lsa->data->id, &id))
                {
                    if(IS_LSA_MAXAGE(lsa))
                    {
                        return NULL;
                    }
                    route_unlock_node (rn);
                    return lsa;
                }
            }
        }
        break;
    case OSPF_SUMMARY_LSA:
    case OSPF_ASBR_SUMMARY_LSA:
        /* Currently not used. */
        assert (1);
        return ospf_lsdb_lookup_by_id (area->lsdb, type, id, id);
    case OSPF_AS_EXTERNAL_LSA:
    case OSPF_AS_NSSA_LSA:
    case OSPF_OPAQUE_LINK_LSA:
    case OSPF_OPAQUE_AREA_LSA:
    case OSPF_OPAQUE_AS_LSA:
        /* Currently not used. */
        break;
    default:
        break;
    }

    return NULL;
}

struct ospf_lsa *
ospf_lsa_lookup_by_header (struct ospf_area *area, struct lsa_header *lsah)
{
    struct ospf_lsa *match;

    /*
     * Strictly speaking, the LSA-ID field for Opaque-LSAs (type-9/10/11)
     * is redefined to have two subfields; opaque-type and opaque-id.
     * However, it is harmless to treat the two sub fields together, as if
     * they two were forming a unique LSA-ID.
     */

    match = ospf_lsa_lookup (area, lsah->type, lsah->id, lsah->adv_router);

    if (match == NULL)
	{
        zlog_debug (OSPF_DBG_LSA, "LSA[Type%d:%s]: Lookup by header, NO MATCH",
                    lsah->type, inet_ntoa (lsah->id));
	}
    return match;
}

/* return +n, l1 is more recent.
   return -n, l2 is more recent.
   return 0, l1 and l2 is identical. */
int
ospf_lsa_more_recent (struct ospf_lsa *l1, struct ospf_lsa *l2)
{
    int r;
    int x, y;
    if (l1 == NULL && l2 == NULL)
    {
        return 0;
    }
    if (l1 == NULL)
    {
        return -1;
    }
    if (l2 == NULL)
    {
        return 1;
    }
    /* compare LS sequence number. */
    x = (int) ntohl (l1->data->ls_seqnum);
    y = (int) ntohl (l2->data->ls_seqnum);
    if (x > y)
    {
        return 1;
    }
    if (x < y)
    {
        return -1;
    }
    /* compare LS checksum. */
    r = ntohs (l1->data->checksum) - ntohs (l2->data->checksum);
    if (r)
    {
        return r;
    }
    /* compare LS age. */
    if (IS_LSA_MAXAGE (l1) && !IS_LSA_MAXAGE (l2))
    {
        return 1;
    }
    else if (!IS_LSA_MAXAGE (l1) && IS_LSA_MAXAGE (l2))
    {
        return -1;
    }
    /* compare LS age with MaxAgeDiff. */
    if (LS_AGE (l1) - LS_AGE (l2) > OSPF_LSA_MAXAGE_DIFF)
    {
        return -1;
    }
    else if (LS_AGE (l2) - LS_AGE (l1) > OSPF_LSA_MAXAGE_DIFF)
    {
        return 1;
    }
    /* LSAs are identical. */
    return 0;
}

/* If two LSAs are different, return 1, otherwise return 0. */
int
ospf_lsa_different (struct ospf_lsa *l1, struct ospf_lsa *l2)
{
    char *p1, *p2;

    assert (l1);
    assert (l2);
    assert (l1->data);
    assert (l2->data);
    if (l1->data->options != l2->data->options)
    {
        return 1;
    }
    if (IS_LSA_MAXAGE (l1) && !IS_LSA_MAXAGE (l2))
    {
        return 1;
    }
    if (IS_LSA_MAXAGE (l2) && !IS_LSA_MAXAGE (l1))
    {
        return 1;
    }
    if (l1->data->length != l2->data->length)
    {
        return 1;
    }
    if (l1->data->length ==  0)
    {
        return 1;
    }
    if (CHECK_FLAG ((l1->flags ^ l2->flags), OSPF_LSA_RECEIVED))
    {
        return 1;    /* May be a stale LSA in the LSBD */
    }
    assert ( ntohs(l1->data->length) > OSPF_LSA_HEADER_SIZE);
    p1 = (char *) l1->data;
    p2 = (char *) l2->data;
    if (memcmp (p1 + OSPF_LSA_HEADER_SIZE, p2 + OSPF_LSA_HEADER_SIZE,
                ntohs( l1->data->length ) - OSPF_LSA_HEADER_SIZE) != 0)
    {
        return 1;
    }
    return 0;
}

#ifdef ORIGINAL_CODING
void
ospf_lsa_flush_self_originated (struct ospf_neighbor *nbr,
                                struct ospf_lsa *self,
                                struct ospf_lsa *new_lsa)
{
    u_int32_t seqnum;
    /* Adjust LS Sequence Number. */
    seqnum = ntohl (new_lsa->data->ls_seqnum) + 1;
    self->data->ls_seqnum = htonl (seqnum);
    /* Recalculate LSA checksum. */
    ospf_lsa_checksum (self->data);
    /* Reflooding LSA. */
    /*  RFC2328  Section 13.3
        On non-broadcast networks, separate   Link State Update
        packets must be sent, as unicasts, to each adjacent   neighbor
        (i.e., those in state Exchange or greater).    The destination
        IP addresses for these packets are the neighbors' IP
        addresses.   */
    if (nbr->oi->type == OSPF_IFTYPE_NBMA)
    {
        struct route_node *rn;
        struct ospf_neighbor *onbr;
        for (rn = route_top (nbr->oi->nbrs); rn; rn = route_next (rn))
        {
            if ((onbr = rn->info) != NULL)
            {
                if (onbr != nbr->oi->nbr_self && onbr->status >= NSM_Exchange)
                {
                    ospf_ls_upd_send_lsa (onbr, self, OSPF_SEND_PACKET_DIRECT);
                }
            }
        }
    }
    else
    {
        ospf_ls_upd_send_lsa (nbr, self, OSPF_SEND_PACKET_INDIRECT);
    }
    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type%d:%s]: Flush self-originated LSA",
                    self->data->type, inet_ntoa (self->data->id));
}
#else /* ORIGINAL_CODING */
static int
ospf_lsa_flush_schedule (struct ospf *ospf, struct ospf_lsa *lsa)
{
    if (lsa == NULL || !IS_LSA_SELF (lsa))
    {
        return 0;
    }
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "LSA[Type%d:%s]: Schedule self-originated LSA to FLUSH", lsa->data->type, inet_ntoa (lsa->data->id));
    }
    /* Force given lsa's age to MaxAge. */
    lsa->data->ls_age = htons (OSPF_LSA_MAXAGE);
    switch (lsa->data->type)
    {
        /* Opaque wants to be notified of flushes */
    case OSPF_OPAQUE_LINK_LSA:
    //case OSPF_OPAQUE_AREA_LSA://add by zzl
    case OSPF_OPAQUE_AS_LSA:
        ospf_opaque_lsa_refresh (lsa,ospf);
        break;
    default:
        ospf_refresher_unregister_lsa (ospf, lsa);
        ospf_lsa_flush (ospf, lsa);
        break;
    }
    return 0;
}

void
ospf_flush_self_originated_lsas_now (struct ospf *ospf)
{
    struct listnode *node, *nnode;
    struct listnode *node2, *nnode2;
    struct ospf_area *area;
    struct ospf_interface *oi;
    struct ospf_lsa *lsa;
    struct route_node *rn;
    int need_to_flush_ase = 0;
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        if ((lsa = area->router_lsa_self) != NULL)
        {
            if (IS_DEBUG_OSPF_EVENT)
                zlog_debug (OSPF_DBG_EVENT, "LSA[Type%d:%s]: Schedule self-originated LSA to FLUSH",
                            lsa->data->type, inet_ntoa (lsa->data->id));
            ospf_refresher_unregister_lsa (ospf, lsa);
            ospf_lsa_flush_area (lsa, area);
            ospf_lsa_unlock (&area->router_lsa_self);
            area->router_lsa_self = NULL;
        }
        for (ALL_LIST_ELEMENTS (area->oiflist, node2, nnode2, oi))
        {
            if ((lsa = oi->network_lsa_self) != NULL
                    &&   oi->state == ISM_DR
                    &&   oi->full_nbrs > 0)
            {
                if (IS_DEBUG_OSPF_EVENT)
                    zlog_debug (OSPF_DBG_EVENT, "LSA[Type%d:%s]: Schedule self-originated LSA to FLUSH",
                                lsa->data->type, inet_ntoa (lsa->data->id));
                ospf_refresher_unregister_lsa (ospf, oi->network_lsa_self);
                ospf_lsa_flush_area (oi->network_lsa_self, area);
                ospf_lsa_unlock (&oi->network_lsa_self);
                oi->network_lsa_self = NULL;
            }
            if (oi->type != OSPF_IFTYPE_VIRTUALLINK
                    &&  area->external_routing == OSPF_AREA_DEFAULT)
            {
                need_to_flush_ase = 1;
            }
        }
        LSDB_LOOP (SUMMARY_LSDB (area), rn, lsa)
        {
            ospf_lsa_flush_schedule (ospf, lsa);
        }
        LSDB_LOOP (ASBR_SUMMARY_LSDB (area), rn, lsa)
        {
            ospf_lsa_flush_schedule (ospf, lsa);
        }
        LSDB_LOOP (NSSA_LSDB(area), rn, lsa)
        {
            ospf_lsa_flush_schedule (ospf, lsa);
        }
        LSDB_LOOP (OPAQUE_LINK_LSDB (area), rn, lsa)
        {
            ospf_lsa_flush_schedule (ospf, lsa);
        }
        LSDB_LOOP (OPAQUE_AREA_LSDB (area), rn, lsa)
        {
            if(CHECK_FLAG (ospf->config, OSPF_OPAQUE_CAPABLE))// && (ospf->dcn_enable == OSPF_DCN_ENABLE)
            {
                ospf_lsa_flush_schedule (ospf, lsa);
            }
            else
            {
                if(IS_LSA_SELF(lsa))
                {
                    ospf_lsa_flush_area(lsa, lsa->area);
                }
                else
                {
                    ospf_discard_from_db (area->ospf, area->lsdb, lsa);
                }
            }
        }
    }
    if (need_to_flush_ase)
    {
        LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
        {
            ospf_lsa_flush_schedule (ospf, lsa);
        }
        LSDB_LOOP (OPAQUE_AS_LSDB (ospf), rn, lsa)
        {
            ospf_lsa_flush_schedule (ospf, lsa);
        }
    }
    /*
     * Make sure that the MaxAge LSA remover is executed immediately,
     * without conflicting to other threads.
     */
    if (ospf->t_maxage != 0)
    {
        OSPF_TIMER_OFF (ospf->t_maxage);
        //thread_execute (master_ospf, ospf_maxage_lsa_remover, ospf, 0);
        //high_pre_timer_add ((char *)"ospf_event", LIB_TIMER_TYPE_NOLOOP, ospf_maxage_lsa_remover, ospf, 0);
        ospf_maxage_lsa_remover(ospf);
    }
    return;
}
#endif /* ORIGINAL_CODING */
void
ospf_external_self_originated_lsas_now(struct ospf *ospf)
{
    struct route_node *rn = NULL;
    struct ospf_lsa *lsa = NULL;
    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
        ospf_lsa_flush_schedule (ospf, lsa);
    }
    LSDB_LOOP (OPAQUE_AS_LSDB (ospf), rn, lsa)
    {
        ospf_lsa_flush_schedule (ospf, lsa);
    }
}


void
ospf_area_self_originated_lsas_now(struct ospf_area *area)
{
    struct route_node *rn = NULL;
    struct ospf_lsa *lsa = NULL;
    //int need_to_flush_ase = 1;
    struct ospf *ospf = area->ospf;
    /* Type 1 lsa refresh */
    LSDB_LOOP (NSSA_LSDB(area), rn, lsa)
    {
        ospf_lsa_flush_schedule (ospf, lsa);
    }
    LSDB_LOOP (OPAQUE_LINK_LSDB (area), rn, lsa)
    {
        ospf_lsa_flush_schedule (ospf, lsa);
    }
    /* when there is Type 5 lsa flush */
    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
        ospf_lsa_flush_schedule (ospf, lsa);
    }
    /*
     * Make sure that the MaxAge LSA remover is executed immediately,
     * without conflicting to other threads.
     */
    if (ospf->t_maxage != 0)
    {
        OSPF_TIMER_OFF (ospf->t_maxage);
        //thread_execute (master_ospf, ospf_maxage_lsa_remover, ospf, 0);
        //high_pre_timer_add ((char *)"ospf_event", LIB_TIMER_TYPE_NOLOOP, ospf_maxage_lsa_remover, ospf, 0);
        ospf_maxage_lsa_remover(ospf);
    }
    return;
}

/* flush an AS-NSSA-LSA, install and flood. */
void
ospf_area_as_nssa_lsa_flush (struct ospf_area *area)
{
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;
    struct ospf_lsa * type5 = NULL;
    struct external_info *ei = NULL;
    if (IS_DEBUG_OSPF_NSSA)
    {
        zlog_debug (OSPF_DBG_NSSA, "ospf_area_as_nssa_lsa_flush(): Start");
    }
    LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
    {
        /* 老化掉7->5 的5 类LSA*/
        type5 = ospf_lsa_lookup (area, OSPF_AS_EXTERNAL_LSA,lsa->data->id, area->ospf->router_id);
        if(type5 != NULL)
        {
            if(CHECK_FLAG (type5->flags, OSPF_LSA_LOCAL_XLT))
            {
                ei = ospf_external_info_check (area->ospf,type5);
                if(ei == NULL)
                {
                    ospf_ls_retransmit_delete_nbr_as (area->ospf, type5);
                    if (!IS_LSA_MAXAGE (type5))
                    {
                        ospf_refresher_unregister_lsa (area->ospf, type5);
                        ospf_lsa_flush_as (area->ospf, type5);
                    }
                }
                else
                {
                    ospf_external_lsa_refresh (area->ospf, type5, ei, LSA_REFRESH_FORCE);
                }
            }
        }
        /* 老化掉自生成的7类LSA */
        if (IS_LSA_SELF(lsa) && IS_AREA_NSSA(area))
        {
            ospf_ls_retransmit_delete_nbr_area (area, lsa);
            if (!IS_LSA_MAXAGE (lsa))
            {
                ospf_refresher_unregister_lsa (area->ospf, lsa);
                ospf_lsa_flush_area (lsa, area);
            }
        }
        else if (!IS_LSA_SELF(lsa) && IS_AREA_NSSA(area))
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
            ospf_lsdb_delete (lsa->lsdb, lsa);
        }
    }
    if (IS_DEBUG_OSPF_NSSA)
    {
        zlog_debug (OSPF_DBG_NSSA, "ospf_area_as_nssa_lsa_flush(): Stop");
    }
}



/* If there is self-originated LSA, then return 1, otherwise return 0. */
/* An interface-independent version of ospf_lsa_is_self_originated */
int
ospf_lsa_is_self_originated (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct listnode *node;
    struct ospf_interface *oi;
    /* This LSA is already checked. */
    if (CHECK_FLAG (lsa->flags, OSPF_LSA_SELF_CHECKED))
    {
        return IS_LSA_SELF (lsa);
    }
    /* Make sure LSA is self-checked. */
    SET_FLAG (lsa->flags, OSPF_LSA_SELF_CHECKED);
    /* AdvRouter and Router ID is the same. */
    if (IPV4_ADDR_SAME (&lsa->data->adv_router, &ospf->router_id))
    {
        SET_FLAG (lsa->flags, OSPF_LSA_SELF);
    }
    /* LSA is router-LSA. */
    else if (lsa->data->type == OSPF_ROUTER_LSA &&
             IPV4_ADDR_SAME (&lsa->data->id, &ospf->router_id))
    {
        SET_FLAG (lsa->flags, OSPF_LSA_SELF);
    }
    /* LSA is network-LSA.  Compare Link ID with all interfaces. */
    else if (lsa->data->type == OSPF_NETWORK_LSA)
    {
        for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
        {
            /* Ignore virtual link. */
            if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
                if (oi->address->family == AF_INET)
                    if (IPV4_ADDR_SAME (&lsa->data->id, &oi->address->u.prefix4))
                    {
                        /* to make it easier later */
                        SET_FLAG (lsa->flags, OSPF_LSA_SELF);
                        return IS_LSA_SELF (lsa);
                    }
        }
    }
    return IS_LSA_SELF (lsa);
}


/* Get unique Link State ID. */
struct in_addr
ospf_lsa_unique_id (struct ospf *ospf,
                    struct ospf_lsdb *lsdb, u_char type, struct prefix_ipv4 *p,
                    u_int32_t *ls_seqnum, struct ospf_area *area)
{
    struct ospf_lsa *lsa = NULL;
    struct external_info *ei = NULL;
	struct as_external_lsa *al = NULL;
    struct in_addr mask, id;

    id = p->prefix;

    /* Check existence of LSA instance. */
    lsa = ospf_lsdb_lookup_by_id (lsdb, type, id, ospf->router_id);
    if (lsa)
    {
        al = (struct as_external_lsa *) lsa->data;
        if (ip_masklen (al->mask) == p->prefixlen)
        {
            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                zlog_debug (OSPF_DBG_LSA_GENERATE, "ospf_lsa_unique_id(): "
                            "Can't get Link State ID for %s/%d",
                            inet_ntoa (p->prefix), p->prefixlen);
            /*    id.s_addr = 0; */
            id.s_addr = 0xffffffff;
            return id;
        }
        /* Masklen differs, then apply wildcard mask to Link State ID. */
        else
        {
            if (CHECK_FLAG(lsa->flags, OSPF_LSA_IN_MAXAGE))
            {
                struct prefix_ptr lsa_prefix;
                struct route_node *rn;
                lsa_prefix.family = 0;
                lsa_prefix.prefixlen = sizeof(lsa_prefix.prefix) * CHAR_BIT;
                lsa_prefix.prefix = (uintptr_t) lsa;
                if ((rn = route_node_get (ospf->maxage_lsa,
                                          (struct prefix *)&lsa_prefix)) != NULL)
                {
                    if (rn->info != NULL)
                    {
                        route_unlock_node (rn);
                        /* Remove from lsdb. */
                        if (lsa->lsdb)
                        {
                            *ls_seqnum = ntohl(lsa->data->ls_seqnum) + 1;
                            ospf_discard_from_db (ospf, lsa->lsdb, lsa);
                            ospf_lsdb_delete (lsa->lsdb, lsa);
                        }
                    }
                }
                return id;
            }
            else
            {
                masklen2ip (p->prefixlen, &mask);
                //add by zhangyun start
                if (lsa->data->type == OSPF_AS_EXTERNAL_LSA
                        || lsa->data->type == OSPF_SUMMARY_LSA || lsa->data->type == OSPF_AS_NSSA_LSA)
                {
                    al = (struct as_external_lsa *) lsa->data;
                    if (p->prefixlen < ip_masklen(al->mask))
                    {
                        struct ospf_lsa *new_lsa = ospf_lsa_dup (lsa);
                        struct ospf_lsa *old = NULL;
                        new_lsa->tv_recv = time_get_recent_relative_time ();
                        new_lsa->tv_orig = new_lsa->tv_recv;
                        if (lsa->lsdb)
                        {
                            ospf_discard_from_db (ospf, lsa->lsdb, lsa);
                            ospf_lsdb_delete (lsa->lsdb, lsa);
                        }
                        new_lsa->data->id.s_addr |= (~al->mask.s_addr);
                        *ls_seqnum = ntohl(new_lsa->data->ls_seqnum) + 1;
                        old = ospf_lsdb_lookup_by_id (lsdb, type, new_lsa->data->id, ospf->router_id);
                        if((old != NULL) && IS_LSA_MAXAGE(old))
                        {
                            return id;
                        }
                        new_lsa->data->ls_age = htons (OSPF_LSA_INITIAL_AGE);
                        if(old == NULL)
                        {
                            new_lsa->data->ls_seqnum = htonl (OSPF_INITIAL_SEQUENCE_NUMBER);
                        }
                        else
                        {
                            new_lsa->data->ls_seqnum = old->data->ls_seqnum+1;
                        }
                        ospf_lsa_install (ospf, NULL, new_lsa);
                        if (new_lsa->data->type == OSPF_AS_EXTERNAL_LSA)
                        {
                            /* Flooding new LSA. only to AS (non-NSSA/STUB) */
                            ospf_flood_through_as (ospf, NULL, new_lsa);
                            /* If there is any attached NSSA, do special handling */
                            if (ospf->anyNSSA &&
                                    /* stay away from translated LSAs! */
                                    !(CHECK_FLAG (new_lsa->flags, OSPF_LSA_LOCAL_XLT)))
                            {
                                ei = ospf_external_info_check (ospf,new_lsa);
                                if (ei)
                                {
                                    ospf_install_flood_nssa (ospf, new_lsa, ei);    /* Install/Flood Type-7 to all NSSAs */
                                }
                            }
                        }
                        else if (new_lsa->data->type == OSPF_SUMMARY_LSA ||
                                 new_lsa->data->type == OSPF_AS_NSSA_LSA)
                        {
                            /* Flooding new LSA through area. */
                            if(area)
                            {
                                ospf_flood_through_area (area, NULL, new_lsa);
                            }
                        }
                    }
                    else
                    {
                        id.s_addr = p->prefix.s_addr | (~mask.s_addr);
                        lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, type,
                                                      id, ospf->router_id);
                        if (lsa)
                        {

							if (CHECK_FLAG(lsa->flags, OSPF_LSA_IN_MAXAGE))
				            {
								zlog_debug(OSPF_DBG_OTHER, "%s %d \n",__func__,__LINE__);
				                struct prefix_ptr lsa_prefix;
				                struct route_node *rn;
				                lsa_prefix.family = 0;
				                lsa_prefix.prefixlen = sizeof(lsa_prefix.prefix) * CHAR_BIT;
				                lsa_prefix.prefix = (uintptr_t) lsa;
				                if ((rn = route_node_get (ospf->maxage_lsa,
				                                          (struct prefix *)&lsa_prefix)) != NULL)
				                {
				                    if (rn->info != NULL)
				                    {
				                        route_unlock_node (rn);
				                        /* Remove from lsdb. */
				                        if (lsa->lsdb)
				                        {
				                            *ls_seqnum = ntohl(lsa->data->ls_seqnum) + 1;
				                            ospf_discard_from_db (ospf, lsa->lsdb, lsa);
				                            ospf_lsdb_delete (lsa->lsdb, lsa);
				                        }
				                    }
				                }
				                return id;
				            }
							al = (struct as_external_lsa *) lsa->data;
							if(p->prefixlen < ip_masklen(al->mask))
							{
								zlog_debug(OSPF_DBG_OTHER, "%s %d \n",__func__,__LINE__);
								
                                return id;
							}
							
                            /*if (id.s_addr == p->prefix.s_addr)
                            {
                                *ls_seqnum = ntohl(lsa->data->ls_seqnum) + 1;
                                zlog_warn ("%s[%d]: Could not originate %s %s/%d",__func__,__LINE__,
                                            inet_ntoa(id),inet_ntoa(p->prefix),p->prefixlen);
                                return id;
                            }*/
                            if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
                                zlog_debug (OSPF_DBG_LSA_GENERATE, "ospf_lsa_unique_id(): "
                                            "Can't get Link State ID for %s/%d",
                                            inet_ntoa (p->prefix), p->prefixlen);
                            /*        id.s_addr = 0; */
                            id.s_addr = 0xffffffff;
                            return id;
                        }
						else
						{
							zlog_debug(OSPF_DBG_OTHER, "%s %d \n",__func__,__LINE__);
							return id;
						}
                    }
                }
                //add by zhangyun end
            }
        }
    }

    return id;
}


#define LSA_ACTION_FLOOD_AREA 1
#define LSA_ACTION_FLUSH_AREA 2

struct lsa_action
{
    u_char action;
    struct ospf_area *area;
    struct ospf_lsa *lsa;
};

static int
ospf_lsa_action (struct thread *t)
{
    struct lsa_action *data;
    data = THREAD_ARG (t);
    if(data == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(data);

	zlog_debug (OSPF_DBG_LSA, "LSA[Action]: Performing scheduled LSA action: %d",
                    data->action);
    switch (data->action)
    {
    case LSA_ACTION_FLOOD_AREA:
        ospf_flood_through_area (data->area, NULL, data->lsa);
        break;
    case LSA_ACTION_FLUSH_AREA:
        ospf_lsa_flush_area (data->lsa, data->area);
        break;
	default:
		break;
    }
    ospf_lsa_unlock (&data->lsa); /* Message */
    XFREE (MTYPE_OSPF_MESSAGE, data);
	data = NULL;
    return 0;
}

void
ospf_schedule_lsa_flood_area (struct ospf_area *area, struct ospf_lsa *lsa)
{
    struct lsa_action *data = NULL;
	do
	{		
		data = XCALLOC (MTYPE_OSPF_MESSAGE, sizeof (struct lsa_action));	
	
		if (data == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(data == NULL);	
	
	if(data == NULL)
    {
        zlog_err("Can't creat lsa_action : malloc failed");
        return ;
    }
	
	memset(data, 0, sizeof(struct lsa_action));
    data->action = LSA_ACTION_FLOOD_AREA;
    data->area = area;
    data->lsa  = ospf_lsa_lock (lsa); /* Message / Flood area */
    thread_add_event (master_ospf, ospf_lsa_action, data, 0);
}

void
ospf_schedule_lsa_flush_area (struct ospf_area *area, struct ospf_lsa *lsa)
{
    struct lsa_action *data;
	do
	{		
		data = XCALLOC (MTYPE_OSPF_MESSAGE, sizeof (struct lsa_action));	
	
		if (data == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(data == NULL);		
	if(data == NULL)
    {
        zlog_err("Can't creat lsa_action : malloc failed");
        return ;
    }
	
	memset(data, 0, sizeof(struct lsa_action));
    data->action = LSA_ACTION_FLUSH_AREA;
    data->area = area;
    data->lsa  = ospf_lsa_lock (lsa); /* Message / Flush area */
    thread_add_event (master_ospf, ospf_lsa_action, data, 0);
}


/* LSA Refreshment functions. */
struct ospf_lsa *
ospf_lsa_refresh (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct external_info *ei = NULL;
    struct ospf_lsa *new_lsa = NULL;
	struct u0_device_info_local *u0_device = NULL;
    assert (CHECK_FLAG (lsa->flags, OSPF_LSA_SELF));
    assert (IS_LSA_SELF (lsa));
    assert (lsa->lock > 0);

    switch (lsa->data->type)
    {
        /* Router and Network LSAs are processed differently. */
    case OSPF_ROUTER_LSA:
		if((om->u0_flag)
			&&(lsa->data->adv_router.s_addr != ospf->router_id.s_addr)
			&& ((u0_device = lookup_u0_device_by_adv_router(lsa->data->adv_router)) != NULL))
		{
			new_lsa = ospf_router_lsa_refresh_for_u0 (lsa);
		}
		else
		{
        	new_lsa = ospf_router_lsa_refresh (lsa);
		}
        break;
    case OSPF_NETWORK_LSA:
        new_lsa = ospf_network_lsa_refresh (lsa);
        break;
    case OSPF_SUMMARY_LSA:
        new_lsa = ospf_summary_lsa_refresh (ospf, lsa);
        break;
    case OSPF_ASBR_SUMMARY_LSA:
        new_lsa = ospf_summary_asbr_lsa_refresh (ospf, lsa);
        break;
    case OSPF_AS_EXTERNAL_LSA:
        /* Translated from NSSA Type-5s are refreshed when
         * from refresh of Type-7 - do not refresh these directly.
         */
        if (CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT))
        {
            break;
        }
        ei = ospf_external_info_check (ospf,lsa);
        if (ei)
        {
            new_lsa = ospf_external_lsa_refresh_only(ospf, lsa, ei, LSA_REFRESH_FORCE);
        }
        else
        {
            ospf_lsa_flush_as (ospf, lsa);
        }
        break;
    case OSPF_AS_NSSA_LSA:
        ei = ospf_external_info_check (ospf,lsa);
        if(ei)
        {
            new_lsa = ospf_nssa_lsa_refresh(ospf, lsa, ei, LSA_REFRESH_FORCE);
        }
        else
        {
            if (lsa->data->id.s_addr == OSPF_DEFAULT_DESTINATION)
            {
                if(IS_OSPF_ABR(ospf))
                {
                    ospf_as_nssa_default_lsa_refresh(ospf, lsa);
                }
                else
                {
                    ospf_lsa_flush_as (ospf, lsa);
                }
            }
            else
            {
                ospf_lsa_flush_as (ospf, lsa);
            }
            

        }
        break;
    case OSPF_OPAQUE_LINK_LSA:
    case OSPF_OPAQUE_AREA_LSA:
    case OSPF_OPAQUE_AS_LSA:
        new_lsa = ospf_opaque_lsa_refresh (lsa,ospf);
        break;
    default:
        break;
    }
    return new_lsa;
}


void
ospf_refresher_register_lsa (struct ospf *ospf, struct ospf_lsa *lsa)
{
    u_int16_t index = 0;
    if (lsa->lock <= 0)
    {
#ifdef DEBUG_ROBUST
        zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
        return;
    }
    assert (lsa->lock > 0);
    if (!IS_LSA_SELF (lsa))
    {
#ifdef DEBUG_ROBUST
        zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
        return;
    }
    assert (IS_LSA_SELF (lsa));
    if (lsa->refresh_list < 0)
    {
        if (LS_AGE(lsa) >= ospf->refresh_interval)
        {
            index = (ospf->lsa_refresh_queue.index + 1) % (ospf->refresh_interval / OSPF_LSA_REFRESHER_GRANULARITY -1);
            
        }
        else
        {
            if(LS_AGE(lsa) == 0)
            {
                index = (ospf->refresh_interval - LS_AGE(lsa)-1) / OSPF_LSA_REFRESHER_GRANULARITY;
            }
            else
            {
                index = (ospf->refresh_interval - LS_AGE(lsa)) / OSPF_LSA_REFRESHER_GRANULARITY;
            }
            
            index = index + ospf->lsa_refresh_queue.index ;
            
        }
        
        index = index % (ospf->refresh_interval/OSPF_LSA_REFRESHER_GRANULARITY);
        
        if (index >= OSPF_LSA_REFRESHER_SLOTS)
        {
#ifdef DEBUG_ROBUST
            zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
            index = 0;
        }
		
        if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
        {
        	if(lsa->area)
        	{
            	zlog_debug (OSPF_DBG_LSA_REFRESH, "%s[%d]:LSA[Refresh]: lsa %s with age %d added to index %d",
                        __FUNCTION__, __LINE__, dump_lsa_key(lsa), LS_AGE (lsa), index);
				zlog_debug (OSPF_DBG_LSA_REFRESH, "%s[%d]:LSA[Refresh]:in area %s", __FUNCTION__, __LINE__, inet_ntoa (lsa->area->area_id));
        	}
			else
			{
				zlog_debug (OSPF_DBG_LSA_REFRESH, "%s[%d]:LSA[Refresh]: lsa %s with age %d added to index %d ",
										__FUNCTION__, __LINE__, dump_lsa_key(lsa), LS_AGE (lsa), index);
			}
        }
		
        if (!ospf->lsa_refresh_queue.qs[index])
        {
            ospf->lsa_refresh_queue.qs[index] = list_new ();
        }
        listnode_add (ospf->lsa_refresh_queue.qs[index], ospf_lsa_lock (lsa)); /* lsa_refresh_queue */
        lsa->refresh_list = index;
        
        if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
            zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh:%s]: ospf_refresher_register_lsa(): "
                        "setting refresh_list on lsa %p (slod %d)",
                        dump_lsa_key(lsa), (void *)lsa, index);
    }
}


void
ospf_refresher_unregister_lsa (struct ospf *ospf, struct ospf_lsa *lsa)
{
    assert (lsa->lock > 0);
    assert (IS_LSA_SELF (lsa));
    if (lsa->refresh_list >= 0)
    {
        struct list *refresh_list = ospf->lsa_refresh_queue.qs[lsa->refresh_list];
        if (refresh_list == NULL)
        {
#ifdef DEBUG_ROBUST
            zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
            return;
        }
        listnode_delete (refresh_list, lsa);
        if (!listcount (refresh_list))
        {
            list_free (refresh_list);
            ospf->lsa_refresh_queue.qs[lsa->refresh_list] = NULL;
        }

        ospf_lsa_unlock (&lsa); /* lsa_refresh_queue */
        lsa->refresh_list = -1;
    }
}


int
ospf_lsa_refresh_walker (void *t)
{
    struct list *refresh_list = NULL;
    struct listnode *node = NULL,
                    *nnode = NULL;
    struct ospf *ospf = (struct ospf *)(t);
    struct ospf_lsa *lsa = NULL;
    int refresh_index;
    
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    ospf->t_lsa_refresher = 0;
    
    struct list *lsa_to_refresh = list_new ();
    if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
    {
        zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]:ospf_lsa_refresh_walker(): start");
    }
    
    refresh_index = ospf->lsa_refresh_queue.index;
    if (refresh_index < 0 || refresh_index >= OSPF_LSA_REFRESHER_SLOTS)
    {
        zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]: ospf_lsa_refresh_walker(): "
                    "refresh index %d", refresh_index);
        return 0;
    }
    
    
    if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
        zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]: ospf_lsa_refresh_walker(): next index %d",
                    ospf->lsa_refresh_queue.index);
    if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
        zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]: ospf_lsa_refresh_walker(): "
                    "refresh index %d", refresh_index);
    assert (refresh_index >= 0);
    
    refresh_list = ospf->lsa_refresh_queue.qs [refresh_index];
    ospf->lsa_refresh_queue.qs [refresh_index] = NULL;

    ospf->lsa_refresh_queue.index =
        (ospf->lsa_refresh_queue.index + 1)%(ospf->refresh_interval/OSPF_LSA_REFRESHER_GRANULARITY);
	
    if (refresh_list)
    {
        for (ALL_LIST_ELEMENTS (refresh_list, node, nnode, lsa))
        {
            if ((lsa == NULL) || (lsa->data == NULL))
            {
#ifdef DEBUG_ROBUST
                zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
                continue;
            }
            if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
            {
                if (lsa->area != NULL)
                {
                    zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh:%s]: ospf_lsa_refresh_walker(): "
                                "refresh lsa %p (slot %d)",
                                dump_lsa_key(lsa), (void *)lsa, refresh_index);
					zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]: ospf_lsa_refresh_walker(): "
                                "in area %s", inet_ntoa(lsa->area->area_id));
					
                }
                else
                {
                    zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh:%s]: ospf_lsa_refresh_walker(): "
                                "refresh lsa %p (slot %d)",
                                dump_lsa_key(lsa), (void *)lsa, refresh_index);
                }
            }
            if (IS_DEBUG_OSPF_PACKET (OSPF_MSG_HELLO - 1, SEND))
            {
                zlog_debug(OSPF_DBG_SEND, "%s(): %d", __FUNCTION__, __LINE__);
                if (lsa->area != NULL)
                    zlog_debug (OSPF_DBG_SEND, "LSA[Refresh:%s]: ospf_lsa_refresh_walker(): "
                                "refresh lsa %p (slot %d) in area %s",
                                dump_lsa_key(lsa), (void *)lsa, refresh_index, inet_ntoa(lsa->area->area_id));
                else
                    zlog_debug (OSPF_DBG_SEND, "LSA[Refresh:%s]: ospf_lsa_refresh_walker(): "
                                "refresh lsa %p (slot %d)",
                                dump_lsa_key(lsa), (void *)lsa, refresh_index);
            }
            assert (lsa->lock > 0);
            list_delete_node (refresh_list, node);
            lsa->refresh_list = -1;
            listnode_add (lsa_to_refresh, lsa);
        }
        list_free (refresh_list);
    }
    /*ospf->t_lsa_refresher = thread_add_timer (master_ospf, ospf_lsa_refresh_walker,
                           ospf, ospf->lsa_refresh_interval);*/
	ospf->t_lsa_refresher = high_pre_timer_add ((char *)"ospf_lsa_fresh_timer", LIB_TIMER_TYPE_NOLOOP,\
							ospf_lsa_refresh_walker, ospf, (ospf->lsa_refresh_interval)*1000);
	
    ospf->lsa_refresher_started = time_get_real_time (NULL);
    for (ALL_LIST_ELEMENTS (lsa_to_refresh, node, nnode, lsa))
    {
        if (lsa == NULL)
        {
#ifdef DEBUG_ROBUST
            zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
            continue;
        }
        
        ospf_lsa_refresh (ospf, lsa);
		//zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]: %s()[%d]: refresh lsa: %s, lsa->lock = %d", __FUNCTION__, __LINE__, dump_lsa_key(lsa), lsa->lock);
        assert (lsa->lock > 0);
        ospf_lsa_unlock (&lsa); /* lsa_refresh_queue & temp for lsa_to_refresh*/
    }
    list_delete (lsa_to_refresh);
    lsa_to_refresh = NULL;
    if (IS_DEBUG_OSPF (lsa, LSA_REFRESH))
    {
        zlog_debug (OSPF_DBG_LSA_REFRESH, "LSA[Refresh]: ospf_lsa_refresh_walker(): end");
    }
    return 0;
}

#if 0
void
ospf_remove_lsa_now(struct ospf *ospf)
{
    //thread_execute (master_ospf, ospf_maxage_lsa_remover, ospf, 0);
    //high_pre_timer_add ((char *)"ospf_event", LIB_TIMER_TYPE_NOLOOP, ospf_maxage_lsa_remover, ospf, 0);
    ospf_maxage_lsa_remover(ospf);
}

#endif


