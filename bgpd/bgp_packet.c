/* BGP packet management routine.
   Copyright (C) 1999 Kunihiro Ishiguro

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

#include <zebra.h>
#include <lib/thread.h>
#include <lib/stream.h>
#include <lib/module_id.h>
#include <lib/network.h>
#include <lib/prefix.h>
#include <lib/command.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/sockunion.h>      /* for inet_ntop () */
#include <lib/sockopt.h>
#include <lib/linklist.h>
#include <lib/plist.h>
#include <lib/filter.h>
#include <lib/timer.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ifm_common.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_tcp.h>
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"

#include "bgpd/bgpd.h"
#include "bgpd/bgp_table.h"
#include "bgpd/bgp_dump.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_fsm.h"
#include "bgpd/bgp_route.h"
#include "bgpd/bgp_packet.h"
#include "bgpd/bgp_open.h"
#include "bgpd/bgp_aspath.h"
#include "bgpd/bgp_community.h"
#include "bgpd/bgp_ecommunity.h"
#include "bgpd/bgp_mplsvpn.h"
#include "bgpd/bgp_encap.h"
#include "bgpd/bgp_advertise.h"
#include "bgpd/bgp_nexthop.h"
#include "bgpd/bgp_vty.h"

int stream_put_prefix (struct stream *, struct prefix *);

int bgp_packet_alloc_port(struct peer *peer)
{
    if(peer->local_sport)
    {
        zlog_debug(BGP_DEBUG_TYPE_OTHER,"%s[%d],local_sport(%d) ",__FUNCTION__,__LINE__,peer->local_sport);
        return 0;
    }
	
    peer->local_sport = pkt_port_alloc(MODULE_ID_BGP,PKT_TYPE_TCP);
    if(0 == peer->local_sport)
    {
        zlog_err("%s[%d], get sport failed! ", __FUNCTION__, __LINE__);
        return -1;
    }
	
    peer->local_dport = BGP_PORT_DEFAULT;
	
    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] local_sport(%d),local_dport(%d) ",peer->host, __FUNCTION__,__LINE__,peer->local_sport,peer->local_dport);
    //}
	
    return 0;
}

int bgp_packet_release_port(struct peer *peer)
{
    int ret = 0;
	
    if(peer->local_sport == 0 || peer->local_sport == BGP_PORT_DEFAULT)
    {
        zlog_debug(BGP_DEBUG_TYPE_OTHER,"%s[%d] ",__FUNCTION__,__LINE__);
        return ret;
    }
	
	//if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] local_sport %d ",peer->host,__FUNCTION__,__LINE__,peer->local_sport);
    //}
	
    ret = pkt_port_relese(MODULE_ID_BGP,PKT_TYPE_TCP, peer->local_sport);
	
    return ret;
}

void peer_close_tcp_send(struct peer *peer)
{
    struct ip_proto ptcp;
	char srcip[IPV6_ADDR_STRLEN] = "";
    char destip[IPV6_ADDR_STRLEN] = "";
	
    memset(&ptcp,0x0,sizeof(struct ip_proto));
	
	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		IPV6_ADDR_COPY(&ptcp.sipv6,&peer->su_local.sin6.sin6_addr);	
		IPV6_ADDR_COPY(&ptcp.dipv6,&peer->su.sin6.sin6_addr);
		ptcp.type = PROTO_TYPE_IPV6;
	}
	else
	{
	    ptcp.dip = ntohl(peer->su.sin.sin_addr.s_addr);
	    ptcp.sip = ntohl(peer->su_local.sin.sin_addr.s_addr);
	}

    ptcp.dport = peer->send_dport;
    ptcp.sport = peer->send_sport;
    ptcp.protocol = IP_P_TCP;
	
    if((peer->su_local.sin.sin_family == AF_INET && (ptcp.sip == 0 || ptcp.dip == 0 ))
		|| (peer->su_local.sin6.sin6_family == AF_INET6 && (ipv6_is_zero(&ptcp.sipv6) || ipv6_is_zero(&ptcp.dipv6)))
		|| ptcp.sport == 0 || ptcp.dport == 0)
    {
        return;
    }
	
	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		inet_ipv6tostr(&ptcp.sipv6, srcip, IPV6_ADDR_STRLEN);
		inet_ipv6tostr(&ptcp.dipv6, destip, IPV6_ADDR_STRLEN);
	}
	else
	{
		inet_ipv4tostr(ptcp.sip, srcip);
		inet_ipv4tostr(ptcp.dip, destip);
	}

    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] sip %s dip %s pro %d sport %d dport %d ",peer->host,__FUNCTION__,__LINE__, srcip, destip, ptcp.protocol, ptcp.sport, ptcp.dport);
    //}
	
    pkt_close_tcp(MODULE_ID_BGP, &ptcp);
	
    peer->send_sport = 0;
    peer->send_dport = 0;
	
    return;
}

void peer_close_tcp_local(struct peer *peer)
{
    struct ip_proto ptcp;
	char srcip[IPV6_ADDR_STRLEN] = "";
    char destip[IPV6_ADDR_STRLEN] = "";
	
    memset(&ptcp,0x0,sizeof(struct ip_proto));
	
	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		IPV6_ADDR_COPY(&ptcp.sipv6,&peer->su_local.sin6.sin6_addr);	
		IPV6_ADDR_COPY(&ptcp.dipv6,&peer->su.sin6.sin6_addr);	
		ptcp.type = PROTO_TYPE_IPV6;
	}
	else
	{
	    ptcp.dip = ntohl(peer->su.sin.sin_addr.s_addr);
	    ptcp.sip = ntohl(peer->su_local.sin.sin_addr.s_addr);
	}
	
    ptcp.dport = peer->local_dport;
    ptcp.sport = peer->local_sport;
    ptcp.protocol = IP_P_TCP;
	
    if((peer->su_local.sin.sin_family == AF_INET && (ptcp.sip == 0 || ptcp.dip == 0 ))
		|| (peer->su_local.sin6.sin6_family == AF_INET6 && (ipv6_is_zero(&ptcp.sipv6) || ipv6_is_zero(&ptcp.dipv6)))
		|| ptcp.sport == 0 || ptcp.dport == 0)
    {
        return;
    }
	
	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		inet_ipv6tostr(&ptcp.sipv6, srcip, IPV6_ADDR_STRLEN);
		inet_ipv6tostr(&ptcp.dipv6, destip, IPV6_ADDR_STRLEN);
	}
	else
	{
		inet_ipv4tostr(ptcp.sip, srcip);
		inet_ipv4tostr(ptcp.dip, destip);
	}
	
    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] sip %s dip %s pro %d sport %d dport %d ",peer->host,__FUNCTION__,__LINE__, srcip, destip, ptcp.protocol, ptcp.sport, ptcp.dport);
    //}

	bgp_packet_release_port(peer);
		
	peer->local_sport = 0;
	peer->local_dport = 0;
	
    pkt_close_tcp(MODULE_ID_BGP, &ptcp);
			
    return;
}

void peer_close_tcp_rcv_ipv4(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport)
{
    struct ip_proto ptcp;
	char srcip[INET_ADDRSTRLEN] = "";
    char destip[INET_ADDRSTRLEN] = "";
	
    memset(&ptcp,0x0,sizeof(struct ip_proto));
	
    ptcp.dip = dip;
    ptcp.sip = sip;
    ptcp.dport = dport;
    ptcp.sport = sport;
    ptcp.protocol = IP_P_TCP;
	
    if(ptcp.sip == 0 || ptcp.dip == 0 || ptcp.sport == 0 || ptcp.dport == 0)
    {
        return;
    }
	
	inet_ipv4tostr(ptcp.sip, srcip);
	inet_ipv4tostr(ptcp.dip, destip);
	
    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s[%d] sip %s dip %s pro %d sport %d dport %d ",__FUNCTION__,__LINE__,srcip, destip, ptcp.protocol, ptcp.sport, ptcp.dport);
    //}
	
    pkt_close_tcp(MODULE_ID_BGP, &ptcp);
	
    return;
}


void peer_close_tcp_rcv_ipv6(struct ipv6_addr sip, struct ipv6_addr dip, uint16_t sport, uint16_t dport)
{
    struct ip_proto ptcp;
	char srcip[IPV6_ADDR_STRLEN] = "";
    char destip[IPV6_ADDR_STRLEN] = "";
	
    memset(&ptcp,0x0,sizeof(struct ip_proto));
	
	IPV6_ADDR_COPY(&ptcp.sipv6,&sip);	
	IPV6_ADDR_COPY(&ptcp.dipv6,&dip);
    ptcp.sport = sport;
    ptcp.dport = dport;
    ptcp.protocol = IP_P_TCP;
	ptcp.type = PROTO_TYPE_IPV6;
	
    if(ipv6_is_zero(&ptcp.sipv6) || ipv6_is_zero(&ptcp.dipv6)  || ptcp.sport == 0 || ptcp.dport == 0)
    {
        return;
    }
	
	inet_ipv6tostr(&ptcp.sipv6, srcip, IPV6_ADDR_STRLEN);
	inet_ipv6tostr(&ptcp.dipv6, srcip, IPV6_ADDR_STRLEN);
	
    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s[%d] sip %s dip %s pro %d sport %d dport %d ",__FUNCTION__,__LINE__,srcip, destip, ptcp.protocol, ptcp.sport, ptcp.dport);
    //}
	
    pkt_close_tcp(MODULE_ID_BGP, &ptcp);
	
    return;
}

/* Set up BGP packet marker and packet type. */
static int
bgp_packet_set_marker (struct stream *s, u_char type)
{
    int i;
	
    /* Fill in marker. */
    for (i = 0; i < BGP_MARKER_SIZE; i++)
    {
        stream_putc (s, 0xff);
    }
	
    /* Dummy total length. This field is should be filled in later on. */
    stream_putw (s, 0);
	
    /* BGP packet type. */
    stream_putc (s, type);
	
    /* Return current stream size. */
    return stream_get_endp (s);
}

/* Set BGP packet header size entry.  If size is zero then use current
   stream size. */
static int
bgp_packet_set_size (struct stream *s)
{
    int cp;
	
    /* Preserve current pointer. */
    cp = stream_get_endp (s);
    stream_putw_at (s, BGP_MARKER_SIZE, cp);
	
    return cp;
}

/* Add new packet to the peer. */
static void
bgp_packet_add (struct peer *peer, struct stream *s)
{
    /* Add packet to the end of list. */
    stream_fifo_push (peer->obuf, s);
}

/* Free first packet. */
static void
bgp_packet_delete (struct peer *peer)
{
    stream_free (stream_fifo_pop (peer->obuf));
}

/* Make BGP update packet.  */
static struct stream *
bgp_update_packet (struct peer *peer, afi_t afi, safi_t safi)
{
    struct stream *s;
    struct stream *snlri;
    struct bgp_adj_out *adj;
    struct bgp_advertise *adv;
    struct stream *packet;
    struct bgp_node *rn = NULL;
    struct bgp_info *binfo = NULL;
    bgp_size_t total_attr_len = 0;
    unsigned long attrlen_pos = 0;
    size_t mpattrlen_pos = 0;

    s = peer->work;
    stream_reset (s);
    snlri = peer->scratch;
    stream_reset (snlri);

    adv = BGP_ADV_FIFO_HEAD (&peer->sync[afi][safi]->update);

    while (adv)
    {
        //assert (adv->rn);
		if(!(adv->rn))
		{
			return NULL;
		}
		
        rn = adv->rn;
        adj = adv->adj;
		
        if (adv->binfo)
        {
            binfo = adv->binfo;
        }
		
        /* When remaining space can't include NLRI and it's length.  */
        if (STREAM_CONCAT_REMAIN (s, snlri, STREAM_SIZE(s)) <=
                (BGP_NLRI_LENGTH + bgp_packet_mpattr_prefix_size(afi,safi,&rn->p)))
        {
            break;
        }
		
        /* If packet is empty, set attribute. */
        if (stream_empty (s))
        {
            struct prefix_rd *prd = NULL;
            u_char *tag = NULL;
            struct peer *from = NULL;
			
            if (rn->prn)
            {
                prd = (struct prefix_rd *) &rn->prn->p;
            }
			
            if (binfo)
            {
                from = binfo->peer;
            }
			
            /* 1: Write the BGP message header - 16 bytes marker, 2 bytes length,
             * one byte message type.
             */
            bgp_packet_set_marker (s, BGP_MSG_UPDATE);
			
            /* 2: withdrawn routes length */
            stream_putw (s, 0);
			
            /* 3: total attributes length - attrlen_pos stores the position */
            attrlen_pos = stream_get_endp (s);
            stream_putw (s, 0);
			
            /* 4: if there is MP_REACH_NLRI attribute, that should be the first
             * attribute, according to draft-ietf-idr-error-handling. Save the
             * position.
             */
			
            /* 5: Encode all the attributes, except MP_REACH_NLRI attr. */
            total_attr_len = bgp_packet_attribute (NULL, peer, s,
                                                   adv->baa->attr,
                                                   ((afi == AFI_IP && safi == SAFI_UNICAST) ?
                                                    &rn->p : NULL),
                                                   afi, safi,
                                                   from, prd, tag);
        }
		
        if (afi == AFI_IP && safi == SAFI_UNICAST)
        {
            stream_put_prefix (s, &rn->p);
        }
        else
        {
            /* Encode the prefix in MP_REACH_NLRI attribute */
            struct prefix_rd *prd = NULL;
            u_char *tag = NULL;
			
            if (rn->prn)
            {
                prd = (struct prefix_rd *) &rn->prn->p;
            }
			
			if (binfo->extra)
			{
				tag = binfo->extra->in_tag;
			}
			
            if (stream_empty(snlri))
                mpattrlen_pos = bgp_packet_mpattr_start(snlri, afi, safi,
                                                        adv->baa->attr);
            bgp_packet_mpattr_prefix(snlri, afi, safi, &rn->p, prd, tag);
        }
		
        if (BGP_DEBUG (update, UPDATE_OUT))
        {
            char buf[INET6_BUFSIZ];
            zlog (peer->log, LOG_DEBUG, "%s send UPDATE %s/%d",
                  peer->host,
                  inet_ntop (rn->p.family, &(rn->p.u.prefix), buf, INET6_BUFSIZ),
                  rn->p.prefixlen);
        }
		
        /* Synchnorize attribute.  */
        if (adj->attr)
        {
            bgp_attr_unintern (&adj->attr);
        }
        else
        {
            peer->scount[afi][safi]++;
        }
		
        adj->attr = bgp_attr_intern (adv->baa->attr);
        adv = bgp_advertise_clean (peer, adj, afi, safi);
    }

    if (! stream_empty (s))
    {
        if (!stream_empty(snlri))
        {
            bgp_packet_mpattr_end(snlri, mpattrlen_pos);
            total_attr_len += stream_get_endp(snlri);
        }
		
        /* set the total attribute length correctly */
        stream_putw_at (s, attrlen_pos, total_attr_len);
        if (!stream_empty(snlri))
        {
            packet = stream_dupcat(s, snlri);
        }
        else
        {
            packet = stream_dup (s);
        }
		
        bgp_packet_set_size (packet);
        bgp_packet_add (peer, packet);
		
         BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
		//if (!(peer->t_write) && (peer->status != Deleted))
        //{
        //   bgp_write((void *)peer);
		//}
        stream_reset (s);
        stream_reset (snlri);
		
        return packet;
    }
    return NULL;
}

static struct stream *
bgp_update_packet_eor (struct peer *peer, afi_t afi, safi_t safi)
{
    struct stream *s;

    //if (BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"send End-of-RIB for %s to %s", afi_safi_print (afi, safi), peer->host);
    //}

    s = stream_new (BGP_MAX_PACKET_SIZE);

    /* Make BGP update packet. */
    bgp_packet_set_marker (s, BGP_MSG_UPDATE);

    /* Unfeasible Routes Length */
    stream_putw (s, 0);

    if (afi == AFI_IP && safi == SAFI_UNICAST)
    {
        /* Total Path Attribute Length */
        stream_putw (s, 0);
    }
    else
    {
        /* Total Path Attribute Length */
        stream_putw (s, 6);
        stream_putc (s, BGP_ATTR_FLAG_OPTIONAL);
        stream_putc (s, BGP_ATTR_MP_UNREACH_NLRI);
        stream_putc (s, 3);
        stream_putw (s, afi);
        stream_putc (s, safi);
    }

    bgp_packet_set_size (s);
    bgp_packet_add (peer, s);
	
    return s;
}

/* Make BGP withdraw packet.  */
/* For ipv4 unicast:
   16-octet marker | 2-octet length | 1-octet type |
    2-octet withdrawn route length | withdrawn prefixes | 2-octet attrlen (=0)
*/
/* For other afi/safis:
   16-octet marker | 2-octet length | 1-octet type |
    2-octet withdrawn route length (=0) | 2-octet attrlen |
     mp_unreach attr type | attr len | afi | safi | withdrawn prefixes
*/
static struct stream *
bgp_withdraw_packet (struct peer *peer, afi_t afi, safi_t safi)
{
    struct stream *s;
    struct stream *packet;
    struct bgp_adj_out *adj;
    struct bgp_advertise *adv;
    struct bgp_node *rn;
    bgp_size_t unfeasible_len;
    bgp_size_t total_attr_len;
    size_t mp_start = 0;
    size_t attrlen_pos = 0;
    size_t mplen_pos = 0;
    u_char first_time = 1;

    s = peer->work;
    stream_reset (s);

    while ((adv = BGP_ADV_FIFO_HEAD (&peer->sync[afi][safi]->withdraw)) != NULL)
    {
        //assert (adv->rn);
		if(!(adv->rn))
		{
			return NULL;
		}
		
        adj = adv->adj;
        rn = adv->rn;
		
        if (STREAM_REMAIN (s)
                < (BGP_NLRI_LENGTH + BGP_TOTAL_ATTR_LEN + PSIZE (rn->p.prefixlen)))
        {
            break;
        }
		
        if (stream_empty (s))
        {
            bgp_packet_set_marker (s, BGP_MSG_UPDATE);
            stream_putw (s, 0); /* unfeasible routes length */
        }
        else
        {
            first_time = 0;
        }
		
        if (afi == AFI_IP && safi == SAFI_UNICAST)
        {
            stream_put_prefix (s, &rn->p);
        }
        else
        {
            struct prefix_rd *prd = NULL;
            if (rn->prn)
            {
                prd = (struct prefix_rd *) &rn->prn->p;
            }
			
            /* If first time, format the MP_UNREACH header */
            if (first_time)
            {
                attrlen_pos = stream_get_endp (s);
				
                /* total attr length = 0 for now. reevaluate later */
                stream_putw (s, 0);
                mp_start = stream_get_endp (s);
                mplen_pos = bgp_packet_mpunreach_start(s, afi, safi);
            }
            bgp_packet_mpunreach_prefix(s, &rn->p, afi, safi, prd, NULL);
        }
		
        if (BGP_DEBUG (update, UPDATE_OUT))
        {
            char buf[INET6_BUFSIZ];
            zlog (peer->log, LOG_DEBUG, "%s send UPDATE %s/%d -- unreachable",
                  peer->host,
                  inet_ntop (rn->p.family, &(rn->p.u.prefix), buf, INET6_BUFSIZ),
                  rn->p.prefixlen);
        }
		
        peer->scount[afi][safi]--;
        bgp_adj_out_remove (rn, adj, peer, afi, safi);
        bgp_unlock_node (rn);
    }

    if (! stream_empty (s))
    {
        if (afi == AFI_IP && safi == SAFI_UNICAST)
        {
            unfeasible_len = stream_get_endp (s) - BGP_HEADER_SIZE - BGP_UNFEASIBLE_LEN;
            stream_putw_at (s, BGP_HEADER_SIZE, unfeasible_len);
            stream_putw (s, 0);
        }
        else
        {
            /* Set the mp_unreach attr's length */
            bgp_packet_mpunreach_end(s, mplen_pos);
			
            /* Set total path attribute length. */
            total_attr_len = stream_get_endp(s) - mp_start;
            stream_putw_at (s, attrlen_pos, total_attr_len);
        }
		
        bgp_packet_set_size (s);
        packet = stream_dup (s);
        bgp_packet_add (peer, packet);
        stream_reset (s);
		
        return packet;
    }

    return NULL;
}

void
bgp_default_update_send (struct peer *peer, struct attr *attr,
                         afi_t afi, safi_t safi, struct peer *from)
{
    struct stream *s;
    struct prefix p;
    unsigned long pos;
    bgp_size_t total_attr_len;
	
    if (afi == AFI_IP)
    {
        str2prefix ("0.0.0.0/0", &p);
    }
    else
    {
        str2prefix ("::/0", &p);
    }
	
    /* Logging the attribute. */
    if (BGP_DEBUG (update, UPDATE_OUT))
    {
        char attrstr[BUFSIZ];
        char buf[INET6_BUFSIZ];
        attrstr[0] = '\0';
        bgp_dump_attr (peer, attr, attrstr, BUFSIZ);
        zlog (peer->log, LOG_DEBUG, "%s[%d] %s send UPDATE %s/%d %s",
              __FUNCTION__,__LINE__,
              peer->host, inet_ntop(p.family, &(p.u.prefix), buf, INET6_BUFSIZ),
              p.prefixlen, attrstr);
    }
	
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make BGP update packet. */
    bgp_packet_set_marker (s, BGP_MSG_UPDATE);
	
    /* Unfeasible Routes Length. */
    stream_putw (s, 0);
	
    /* Make place for total attribute length.  */
    pos = stream_get_endp (s);
    stream_putw (s, 0);
    total_attr_len = bgp_packet_attribute (NULL, peer, s, attr, &p, afi, safi, from, NULL, NULL);
	
    /* Set Total Path Attribute Length. */
    stream_putw_at (s, pos, total_attr_len);
	
    /* NLRI set. */
    if (p.family == AF_INET && safi == SAFI_UNICAST)
    {
        stream_put_prefix (s, &p);
    }
	
    /* Set size. */
    bgp_packet_set_size (s);
	
    /* Add packet to the peer. */
    bgp_packet_add (peer, s);
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
    //if (!(peer->t_write) && (peer->status != Deleted))
    //{
    //   bgp_write((void *)peer);
	//}
}

void
bgp_default_withdraw_send (struct peer *peer, afi_t afi, safi_t safi)
{
    struct stream *s;
    struct prefix p;
    unsigned long attrlen_pos = 0;
    unsigned long cp;
    bgp_size_t unfeasible_len;
    bgp_size_t total_attr_len;
    size_t mp_start = 0;
    size_t mplen_pos = 0;
	
    if (afi == AFI_IP)
    {
        str2prefix ("0.0.0.0/0", &p);
    }
    else
    {
        str2prefix ("::/0", &p);
    }
	
    total_attr_len = 0;
	
    if (BGP_DEBUG (update, UPDATE_OUT))
    {
        char buf[INET6_BUFSIZ];
        zlog (peer->log, LOG_DEBUG, "%s[%d] %s send UPDATE %s/%d -- unreachable",
              __FUNCTION__,__LINE__,
              peer->host, inet_ntop(p.family, &(p.u.prefix), buf, INET6_BUFSIZ),
              p.prefixlen);
    }
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make BGP update packet. */
    bgp_packet_set_marker (s, BGP_MSG_UPDATE);
	
    /* Unfeasible Routes Length. */;
    cp = stream_get_endp (s);
    stream_putw (s, 0);
	
    /* Withdrawn Routes. */
    if (p.family == AF_INET && safi == SAFI_UNICAST)
    {
        stream_put_prefix (s, &p);
        unfeasible_len = stream_get_endp (s) - cp - 2;
		
        /* Set unfeasible len.  */
        stream_putw_at (s, cp, unfeasible_len);
		
        /* Set total path attribute length. */
        stream_putw (s, 0);
    }
    else
    {
        attrlen_pos = stream_get_endp (s);
        stream_putw (s, 0);
        mp_start = stream_get_endp (s);
        mplen_pos = bgp_packet_mpunreach_start(s, afi, safi);
        bgp_packet_mpunreach_prefix(s, &p, afi, safi, NULL, NULL);
		
        /* Set the mp_unreach attr's length */
        bgp_packet_mpunreach_end(s, mplen_pos);
		
        /* Set total path attribute length. */
        total_attr_len = stream_get_endp(s) - mp_start;
        stream_putw_at (s, attrlen_pos, total_attr_len);
    }
    bgp_packet_set_size (s);
	
    /* Add packet to the peer. */
    bgp_packet_add (peer, s);
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);

	//if (!(peer->t_write) && (peer->status != Deleted))
    //{
    //   bgp_write((void *)peer);
	//}
}

/* Get next packet to be written.  */
static struct stream *
bgp_write_packet (struct peer *peer)
{
    afi_t afi;
    safi_t safi;
    struct stream *s = NULL;
    struct bgp_advertise *adv;

    s = stream_fifo_head (peer->obuf);
    if (s)
    {
        return s;
    }

    for (afi = AFI_IP; afi < AFI_MAX; afi++)
    {
        for (safi = SAFI_UNICAST; safi < SAFI_MAX; safi++)
        {
            adv = BGP_ADV_FIFO_HEAD (&peer->sync[afi][safi]->withdraw);
            if (adv)
            {
                s = bgp_withdraw_packet (peer, afi, safi);
                if (s)
                {
                    return s;
                }
            }
        }
    }
	
    for (afi = AFI_IP; afi < AFI_MAX; afi++)
    {
        for (safi = SAFI_UNICAST; safi < SAFI_MAX; safi++)
        {
            adv = BGP_ADV_FIFO_HEAD (&peer->sync[afi][safi]->update);
            if (adv)
            {
                if (adv->binfo && adv->binfo->uptime < peer->synctime)
                {
                    if (CHECK_FLAG (adv->binfo->peer->cap, PEER_CAP_RESTART_RCV)
                        && CHECK_FLAG (adv->binfo->peer->cap, PEER_CAP_RESTART_ADV)
                        && ! (CHECK_FLAG (adv->binfo->peer->cap, PEER_CAP_RESTART_BIT_RCV) 
                        && CHECK_FLAG (adv->binfo->peer->cap, PEER_CAP_RESTART_BIT_ADV))
                        && ! CHECK_FLAG (adv->binfo->flags, BGP_INFO_STALE)
                        && safi != SAFI_MPLS_VPN)
                    {
                        if (CHECK_FLAG (adv->binfo->peer->af_sflags[afi][safi], PEER_STATUS_EOR_RECEIVED))
                        {
                            s = bgp_update_packet (peer, afi, safi);
                        }
                    }
                    else
                    {
                        s = bgp_update_packet (peer, afi, safi);
                    }
                }
                if (s)
                {
                    return s;
                }
            }
            if (CHECK_FLAG (peer->cap, PEER_CAP_RESTART_RCV))
            {
                if (peer->afc_nego[afi][safi] 
					&& peer->synctime
                    && ! CHECK_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_EOR_SEND)
                    && safi != SAFI_MPLS_VPN)
                {
                    SET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_EOR_SEND);
                    return bgp_update_packet_eor (peer, afi, safi);
                }
            }
        }
    }
    return NULL;
}

/* Is there partially written packet or updates we can send right
   now.  */
static int
bgp_write_proceed (struct peer *peer)
{
    afi_t afi;
    safi_t safi;
    struct bgp_advertise *adv;
	
    if (stream_fifo_head (peer->obuf))
    {
        return 1;
    }
	
    for (afi = AFI_IP; afi < AFI_MAX; afi++)
        for (safi = SAFI_UNICAST; safi < SAFI_MAX; safi++)
            if (FIFO_HEAD (&peer->sync[afi][safi]->withdraw))
                return 1;
			
    for (afi = AFI_IP; afi < AFI_MAX; afi++)
        for (safi = SAFI_UNICAST; safi < SAFI_MAX; safi++)
            if ((adv = BGP_ADV_FIFO_HEAD (&peer->sync[afi][safi]->update)) != NULL)
                if (adv->binfo->uptime < peer->synctime)
                    return 1;
    return 0;
}

/* Write packet to the peer. */
int
bgp_write (struct thread * thread)
{
    struct peer *peer = NULL;
    u_char type;
    u_int16_t size = 0;
    struct stream *s;
    union pkt_control pkt_ctrl;
    unsigned int  count = 0;
    int ret = 0;
	char srcip[IPV6_ADDR_STRLEN] = "";
    char destip[IPV6_ADDR_STRLEN] = "";
	
    /* Yes first of all get peer pointer. */
    peer = THREAD_ARG (thread);
    if(!peer)
    {
		return 0;
    }
	
    peer->t_write = NULL;
	
    if(!CHECK_FLAG (peer->flags, PEER_FLAG_ENABLE))
    {
		return 0;
    }
	
    s = bgp_write_packet (peer);
    if (!s)
    {
		return 0;
    }
	
    do
    {
        memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));	
		
		if(peer->su_local.sa.sa_family == AF_INET6)
		{
			IPV6_ADDR_COPY(&pkt_ctrl.ipcb.sipv6,&peer->su_local.sin6.sin6_addr);	
			IPV6_ADDR_COPY(&pkt_ctrl.ipcb.dipv6,&peer->su.sin6.sin6_addr);
		}
		else
		{
	        pkt_ctrl.ipcb.sip = ntohl(peer->su_local.sin.sin_addr.s_addr);
	        pkt_ctrl.ipcb.dip = ntohl(peer->su.sin.sin_addr.s_addr);
		}
		
        pkt_ctrl.ipcb.ttl = peer->ttl;
        pkt_ctrl.ipcb.protocol = IP_P_TCP;
        pkt_ctrl.ipcb.is_changed = 1;
        pkt_ctrl.ipcb.tos = 6;

		type = stream_getc_from (s, BGP_MARKER_SIZE + 2);
        size = stream_getw_from (s, BGP_MARKER_SIZE);
		
		if(peer->vrf_id)
		{
			pkt_ctrl.ipcb.vpn = peer->vrf_id;
		}
		
        if(peer->send_sport && peer->send_dport)
        {
            pkt_ctrl.ipcb.sport = peer->send_sport;
            pkt_ctrl.ipcb.dport = peer->send_dport;
        }
        else
        {			
	        if( peer->local_sport && peer->local_sport != BGP_PORT_DEFAULT && BGP_MSG_OPEN == type)
			{
				peer_close_tcp_local(peer);
			}
			
            if(peer->local_sport == 0)
            {
                ret = bgp_packet_alloc_port(peer);
                if(ret < 0)
                {
                    zlog_err("%s[%d],bgp_packet_alloc_port fail",__FUNCTION__,__LINE__);
					return 0;
                }
            }

            pkt_ctrl.ipcb.sport = peer->local_sport;
            pkt_ctrl.ipcb.dport = peer->local_dport;
        }
        ret = pkt_send(PKT_TYPE_TCP, &pkt_ctrl, STREAM_PNT (s), stream_get_endp(s));
        if(ret < 0)
        {
            zlog_err("%s[%d],pkt_send fail ret %d",__FUNCTION__,__LINE__,ret);
			BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
			//if (!(peer->t_write) && (peer->status != Deleted))
            //{
            //   bgp_write((void *)peer);
    		//}
			return 0;
        }
		
        //if(BGP_DEBUG (normal, NORMAL))
    	//{
            zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] send_sport(%d),send_dport(%d),local_sport(%d),local_dport(%d) ",
                       peer->host,__FUNCTION__,__LINE__,  peer->send_sport,peer->send_dport,peer->local_sport,peer->local_dport);
    	//}
		
		/*close local tcp session*/
        if(peer->send_sport && peer->send_dport && peer->local_sport && peer->local_dport && peer->send_sport != peer->local_sport && peer->send_dport != peer->local_dport)
        {
            peer_close_tcp_local(peer);
        }
		
		memset(srcip, 0x0, IPV6_ADDR_STRLEN * sizeof(char));
		memset(destip, 0x0, IPV6_ADDR_STRLEN * sizeof(char));
		
		if(peer->su_local.sin6.sin6_family == AF_INET6)
		{
			inet_ipv6tostr(&pkt_ctrl.ipcb.sipv6, srcip, IPV6_ADDR_STRLEN);
			inet_ipv6tostr(&pkt_ctrl.ipcb.dipv6, destip, IPV6_ADDR_STRLEN);
		}
		else
		{
			inet_ipv4tostr(pkt_ctrl.ipcb.sip, srcip);
			inet_ipv4tostr(pkt_ctrl.ipcb.dip, destip);
		}
		
        //if(BGP_DEBUG (normal, NORMAL))
        //{
            zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] send %s size %d sip %s dip %s vpn %d ttl %d sport %d dport %d ",peer->host,__FUNCTION__,__LINE__, bgp_type_str[(int) type], size, srcip, destip , pkt_ctrl.ipcb.vpn, pkt_ctrl.ipcb.ttl, pkt_ctrl.ipcb.sport, pkt_ctrl.ipcb.dport);
        //}
		
        switch (type)
        {
	        case BGP_MSG_OPEN:
	            peer->open_out++;
	            break;
	        case BGP_MSG_UPDATE:
	            peer->update_out++;
	            break;
	        case BGP_MSG_NOTIFY:
	            peer->notify_out++;
				
	            /* Double start timer. */
	            peer->v_start *= 2;
				
	            /* Overflow check. */
	            if (peer->v_start >= (60 * 2))
	            {
	                peer->v_start = (60 * 2);
	            }
				
	            /* Flush any existing events */
				peer->event = BGP_Stop;
	            BGP_EVENT_ADD (peer);
				return 0;	            
	        case BGP_MSG_KEEPALIVE:
	            peer->keepalive_out++;
	            break;
	        case BGP_MSG_ROUTE_REFRESH_NEW:
	        case BGP_MSG_ROUTE_REFRESH_OLD:
	            peer->refresh_out++;
	            break;
	        case BGP_MSG_CAPABILITY:
	            peer->dynamic_cap_out++;
	            break;
	        default:
	            break;
        }
		
        /* OK we send packet so delete it. */
        bgp_packet_delete (peer);
    }
    while (++count < BGP_WRITE_PACKET_MAX && (s = bgp_write_packet (peer)) != NULL);
	
    if (bgp_write_proceed (peer))
    {
        BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
        //if (!(peer->t_write) && (peer->status != Deleted))
        //{
        //   bgp_write((void *)peer);
		//}
    }
	
    return 0;
}

/* This is only for sending NOTIFICATION message to neighbor. */
static int
bgp_write_notify (struct peer *peer)
{
    int ret;
    u_char type = 0;
    struct stream *s;
    union pkt_control pkt_ctrl;
	char srcip[IPV6_ADDR_STRLEN] = "";
    char destip[IPV6_ADDR_STRLEN] = "";
	
    /* There should be at least one packet. */
    s = stream_fifo_head (peer->hold_buf);
    if (!s)
    {
		return 0;
    }
	
    //assert (stream_get_endp (s) >= BGP_HEADER_SIZE);
	if(stream_get_endp (s) < BGP_HEADER_SIZE)
	{
		return 0;
	}
	
	//if(BGP_DEBUG (normal, NORMAL))
	//{
    	zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] send_sport %d send_dport %d local_sport %d local_dport %d ",
               		peer->host,__FUNCTION__,__LINE__,peer->send_sport,peer->send_dport,peer->local_sport,peer->local_dport);
	//}
	
    memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
	
	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		IPV6_ADDR_COPY(&pkt_ctrl.ipcb.sipv6,&peer->su_local.sin6.sin6_addr);	
		IPV6_ADDR_COPY(&pkt_ctrl.ipcb.dipv6,&peer->su.sin6.sin6_addr);
	}
	else
	{
    	pkt_ctrl.ipcb.sip = ntohl(peer->su_local.sin.sin_addr.s_addr);
    	pkt_ctrl.ipcb.dip = ntohl(peer->su.sin.sin_addr.s_addr);
	}
	
    pkt_ctrl.ipcb.ttl = peer->ttl;
    pkt_ctrl.ipcb.protocol = IP_P_TCP;
	
    if(peer->send_sport && peer->send_dport)
    {
        pkt_ctrl.ipcb.sport = peer->send_sport;
        pkt_ctrl.ipcb.dport = peer->send_dport;
    }
    else
    {
        pkt_ctrl.ipcb.sport = peer->local_sport;
        pkt_ctrl.ipcb.dport = peer->local_dport;
    }
	
    pkt_ctrl.ipcb.is_changed = 1;
    pkt_ctrl.ipcb.tos = 6;
	
	if(peer->vrf_id)
	{
		pkt_ctrl.ipcb.vpn = peer->vrf_id;
	}
	
    ret = pkt_send(PKT_TYPE_TCP, &pkt_ctrl, STREAM_PNT (s), stream_get_endp(s));
    if(ret < 0)
    {
        zlog_err("%s[%d],ret %d",__FUNCTION__,__LINE__,ret);
		return 0;
    }
	
    /* Retrieve BGP packet type. */
    stream_set_getp (s, BGP_MARKER_SIZE + 2);
    type = stream_getc (s);
	
	memset(srcip, 0x0, IPV6_ADDR_STRLEN * sizeof(char));
	memset(destip, 0x0, IPV6_ADDR_STRLEN * sizeof(char));
	
	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		inet_ipv6tostr(&pkt_ctrl.ipcb.sipv6, srcip, IPV6_ADDR_STRLEN);
		inet_ipv6tostr(&pkt_ctrl.ipcb.dipv6, destip, IPV6_ADDR_STRLEN);
	}
	else
	{
		inet_ipv4tostr(pkt_ctrl.ipcb.sip, srcip);
		inet_ipv4tostr(pkt_ctrl.ipcb.dip, destip);
	}
	
	//if(BGP_DEBUG (normal, NORMAL))
	//{
		zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] send %s sip %s dip %s vpn %d ttl %d sport %d dport %d ",peer->host,__FUNCTION__,__LINE__, bgp_type_str[(int) type], srcip, destip, pkt_ctrl.ipcb.vpn, pkt_ctrl.ipcb.ttl, pkt_ctrl.ipcb.sport, pkt_ctrl.ipcb.dport);
	//}

	//assert (type == BGP_MSG_NOTIFY);
	if(type != BGP_MSG_NOTIFY)
	{
		return 0;
	}
	
    /* Type should be notify. */
    peer->notify_out++;
	
    /* Double start timer. */
    peer->v_start *= 2;
	
    /* Overflow check. */
    if (peer->v_start >= (60 * 2))
    {
        peer->v_start = (60 * 2);
    }
	peer->event = BGP_Stop;
    BGP_EVENT_ADD (peer);

    return 0;
}

#if 0
int bgp_pkt_sendmsg(struct ipc_mesg_n *pMsg, int iLen)
{
    int iresult =0;
    
    iresult = ipc_send_msg_n1(pMsg,  iLen);

    if(iresult != 0) mem_share_free(pMsg, MODULE_ID_BGP);

    return(iresult);
}

static void bgp_pkt_fillhead(struct ipc_msghdr_n  *pMsgHdr) 
{
     pMsgHdr->module_id   = MODULE_ID_FTM;
     pMsgHdr->sender_id   = MODULE_ID_BGP;
     pMsgHdr->msg_type    = IPC_TYPE_PACKET;
     pMsgHdr->msg_subtype = PKT_TYPE_TCP;
     pMsgHdr->opcode      = 0;
     pMsgHdr->data_num    = 1;
     pMsgHdr->priority    = 0;
     pMsgHdr->result      = 0;
}

int bgp_pkt_send( struct stream * data,int stream_length)
{
     struct ipc_mesg_n *pSndMsg =  NULL;
     int len = 0;
         
     pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + stream_length, MODULE_ID_BGP);
     
     if(pSndMsg != NULL)
     {
     
         bgp_pkt_fillhead(&pSndMsg->msghdr);

         len = sizeof(struct ipc_msghdr_n);
                   
         memcpy(pSndMsg->msg_data + len, STREAM_DATA(data) , stream_length);

         len += stream_length;
                   
         pSndMsg->msghdr.data_len = len;

         bgp_pkt_sendmsg(pSndMsg, sizeof(struct ipc_msghdr_n) + len);
     }
     
     return 0;
}

#endif

/* Make keepalive packet and send it to the peer. */
void
bgp_keepalive_send (struct peer *peer)
{
    struct stream *s;
    int length;
	
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make keepalive packet. */
    bgp_packet_set_marker (s, BGP_MSG_KEEPALIVE);
	
    /* Set packet size. */
    length = bgp_packet_set_size (s);
	
    //if (BGP_DEBUG (keepalive, KEEPALIVE))
    //{
        zlog_debug (BGP_DEBUG_TYPE_KEEPALIVE,"%s sending KEEPALIVE", peer->host);
    //}
	
    //if (BGP_DEBUG (normal, NORMAL))
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s send message type %d, length (incl. header) %d",
                    peer->host, BGP_MSG_KEEPALIVE, length);
	
    /* Add packet to the peer. */				
    bgp_packet_add (peer, s);
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);

	//if (!(peer->t_write) && (peer->status != Deleted))
    //{
    //   bgp_write((void *)peer);
	//}
}

/* Make open packet and send it to the peer. */
void
bgp_open_send (struct peer *peer)
{
    struct stream *s;
    int length;
    u_int16_t send_holdtime;
    as_t local_as;
	
    if (CHECK_FLAG (peer->config, PEER_CONFIG_TIMER))
    {
        send_holdtime = peer->holdtime;
    }
    else
    {
        send_holdtime = peer->bgp->default_holdtime;
    }
	
    /* local-as Change */
    if (peer->change_local_as)
    {
        local_as = peer->change_local_as;
    }
    else
    {
        local_as = peer->local_as;
    }
	
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make open packet. */
    bgp_packet_set_marker (s, BGP_MSG_OPEN);
	
    /* Set open packet values. */
    stream_putc (s, BGP_VERSION_4);        /* BGP version */
    stream_putw (s, (local_as <= BGP_AS_MAX) ? (u_int16_t) local_as
                 : BGP_AS_TRANS);
    stream_putw (s, send_holdtime);        /* Hold Time */
    stream_put_in_addr (s, &peer->local_id); /* BGP Identifier */
	
    /* Set capability code. */
    bgp_open_capability (s, peer);
	
    /* Set BGP packet length. */
    length = bgp_packet_set_size (s);
	
    //if (BGP_DEBUG (normal, NORMAL))
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s sending OPEN, version %d, my as %u, holdtime %d, id %s",
                    peer->host, BGP_VERSION_4, local_as,
                    send_holdtime, inet_ntoa (peer->local_id));
	
    //if (BGP_DEBUG (normal, NORMAL))
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s send message type %d, length (incl. header) %d",
                    peer->host, BGP_MSG_OPEN, length);
	
    /* Add packet to the peer. */
    bgp_packet_add (peer, s);
	
    BGP_WRITE_OFF (peer->t_write);
    BGP_WRITE_ON_EXECUTE (peer->t_write, bgp_write, peer->fd);
    //bgp_write((void*)peer);
}

/* Send BGP notify packet with data potion. */
void
bgp_notify_send_with_data (struct peer *peer, u_char code, u_char sub_code,
                           u_char *data, size_t datalen)
{
    struct stream *s;
    int length;
	
    /* Allocate new stream. */
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make nitify packet. */
    bgp_packet_set_marker (s, BGP_MSG_NOTIFY);
	
    /* Set notify packet values. */
    stream_putc (s, code);        /* BGP notify code */
    stream_putc (s, sub_code);    /* BGP notify sub_code */
	
    /* If notify data is present. */
    if (data)
    {
        stream_write (s, data, datalen);
    }
	
    /* Set BGP packet length. */
    length = bgp_packet_set_size (s);
	
    /* Add packet to the peer. */
    //stream_fifo_clean (peer->obuf);
    stream_fifo_push (peer->hold_buf, s);
	
    /* For debug */
    {
        struct bgp_notify bgp_notify;
        int first = 0;
        int i;
        char c[4];
        bgp_notify.code = code;
        bgp_notify.subcode = sub_code;
        bgp_notify.data = NULL;
        bgp_notify.length = length - BGP_MSG_NOTIFY_MIN_SIZE;
        if (bgp_notify.length)
        {
            bgp_notify.data = XMALLOC (MTYPE_TMP, bgp_notify.length * 3);
            for (i = 0; i < bgp_notify.length; i++)
            {
                if (first)
                {
                    sprintf (c, " %02x", data[i]);
                    strcat (bgp_notify.data, c);
                }
                else
                {
                    first = 1;
                    sprintf (c, "%02x", data[i]);
                    strcpy (bgp_notify.data, c);
                }
            }
        }
        bgp_notify_print (peer, &bgp_notify, "sending");
        if (bgp_notify.data)
        {
            XFREE (MTYPE_TMP, bgp_notify.data);
            bgp_notify.data = NULL;
            bgp_notify.length = 0;
        }
    }
	
    //if (BGP_DEBUG (normal, NORMAL))
	//{
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s send message type %d, length (incl. header) %d", peer->host, BGP_MSG_NOTIFY, length);
	//}
	
    /* peer reset cause */
    if (sub_code != BGP_NOTIFY_CEASE_CONFIG_CHANGE)
    {
        if (sub_code == BGP_NOTIFY_CEASE_ADMIN_RESET)
        {
            peer->last_reset = PEER_DOWN_USER_RESET;
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"Notification sent to neighbor %s: User reset", peer->host);
        }
        else if (sub_code == BGP_NOTIFY_CEASE_ADMIN_SHUTDOWN)
        {
            peer->last_reset = PEER_DOWN_USER_SHUTDOWN;
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"Notification sent to neighbor %s: shutdown", peer->host);
        }
        else
        {
            peer->last_reset = PEER_DOWN_NOTIFY_SEND;
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"Notification sent to neighbor %s: type %u/%u", peer->host, code, sub_code);
        }
    }
    else
	{
        zlog_debug (BGP_DEBUG_TYPE_OTHER,"Notification sent to neighbor %s: configuration change",  peer->host);
	}
	
    /* Call immediately. */
    BGP_WRITE_OFF (peer->t_write);
    bgp_write_notify (peer);
    
}

/* Send BGP notify packet. */
void
bgp_notify_send (struct peer *peer, u_char code, u_char sub_code)
{
    bgp_notify_send_with_data (peer, code, sub_code, NULL, 0);
}

/* Send route refresh message to the peer. */
void
bgp_route_refresh_send (struct peer *peer, afi_t afi, safi_t safi,
                        u_char orf_type, u_char when_to_refresh, int remove)
{
    struct stream *s;
    int length;
    struct bgp_filter *filter;
    int orf_refresh = 0;
	
    filter = &peer->filter[afi][safi];
	
    /* Adjust safi code. */
    if (safi == SAFI_MPLS_VPN)
    {
        safi = SAFI_MPLS_LABELED_VPN;
    }
	
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make BGP update packet. */
    if (CHECK_FLAG (peer->cap, PEER_CAP_REFRESH_NEW_RCV))
    {
        bgp_packet_set_marker (s, BGP_MSG_ROUTE_REFRESH_NEW);
    }
    else
    {
        bgp_packet_set_marker (s, BGP_MSG_ROUTE_REFRESH_OLD);
    }
	
    /* Encode Route Refresh message. */
    stream_putw (s, afi);
    stream_putc (s, 0);
    stream_putc (s, safi);
	
    if (orf_type == ORF_TYPE_PREFIX || orf_type == ORF_TYPE_PREFIX_OLD)
    {
        if (remove || filter->plist[FILTER_IN].plist)
        {
            u_int16_t orf_len;
            unsigned long orfp;
            orf_refresh = 1;
            stream_putc (s, when_to_refresh);
            stream_putc (s, orf_type);
            orfp = stream_get_endp (s);
            stream_putw (s, 0);
            if (remove)
            {
                UNSET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_PREFIX_SEND);
                stream_putc (s, ORF_COMMON_PART_REMOVE_ALL);
                //if (BGP_DEBUG (normal, NORMAL))
                    zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s sending REFRESH_REQ to remove ORF(%d) (%s) for afi/safi: %d/%d",
                                peer->host, orf_type,
                                (when_to_refresh == REFRESH_DEFER ? "defer" : "immediate"),
                                afi, safi);
            }
            else
            {
                SET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_PREFIX_SEND);
                prefix_bgp_orf_entry (s, filter->plist[FILTER_IN].plist,
                                      ORF_COMMON_PART_ADD, ORF_COMMON_PART_PERMIT,
                                      ORF_COMMON_PART_DENY);
                //if (BGP_DEBUG (normal, NORMAL))
                    zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s sending REFRESH_REQ with pfxlist ORF(%d) (%s) for afi/safi: %d/%d",
                                peer->host, orf_type,
                                (when_to_refresh == REFRESH_DEFER ? "defer" : "immediate"),
                                afi, safi);
            }
            /* Total ORF Entry Len. */
            orf_len = stream_get_endp (s) - orfp - 2;
            stream_putw_at (s, orfp, orf_len);
        }
	}
		
    /* Set packet size. */
    length = bgp_packet_set_size (s);
	
    //if (BGP_DEBUG (normal, NORMAL))
    //{
        if (! orf_refresh)
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s sending REFRESH_REQ for afi/safi: %d/%d",
                        peer->host, afi, safi);
		
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s send message type %d, length (incl. header) %d",
                    peer->host, CHECK_FLAG (peer->cap, PEER_CAP_REFRESH_NEW_RCV) ?
                    BGP_MSG_ROUTE_REFRESH_NEW : BGP_MSG_ROUTE_REFRESH_OLD, length);
    //}
	
    /* Add packet to the peer. */
    bgp_packet_add (peer, s);
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
    //if (!(peer->t_write) && (peer->status != Deleted))
    //{
    //   bgp_write((void *)peer);
	//}
	
   
}

/* Send capability message to the peer. */
void
bgp_capability_send (struct peer *peer, afi_t afi, safi_t safi,
                     int capability_code, int action)
{
    struct stream *s;
    int length;
	
    /* Adjust safi code. */
    if (safi == SAFI_MPLS_VPN)
    {
        safi = SAFI_MPLS_LABELED_VPN;
    }
	
    s = stream_new (BGP_MAX_PACKET_SIZE);
	
    /* Make BGP update packet. */
    bgp_packet_set_marker (s, BGP_MSG_CAPABILITY);
	
    /* Encode MP_EXT capability. */
    if (capability_code == CAPABILITY_CODE_MP)
    {
        stream_putc (s, action);
        stream_putc (s, CAPABILITY_CODE_MP);
        stream_putc (s, CAPABILITY_CODE_MP_LEN);
        stream_putw (s, afi);
        stream_putc (s, 0);
        stream_putc (s, safi);
		
        //if (BGP_DEBUG (normal, NORMAL))
    	//{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s sending CAPABILITY has %s MP_EXT CAP for afi/safi: %d/%d",
                        peer->host, action == CAPABILITY_ACTION_SET ? "Advertising" : "Removing", afi, safi);
    	//}
    }
	
    /* Set packet size. */
    length = bgp_packet_set_size (s);
	
    /* Add packet to the peer. */
    bgp_packet_add (peer, s);
	
    //if (BGP_DEBUG (normal, NORMAL))
	//{
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s send message type %d, length (incl. header) %d", peer->host, BGP_MSG_CAPABILITY, length);
	//}
	
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
   //bgp_pkt_send(s, length);

	//if (!(peer->t_write) && (peer->status != Deleted))
    //{
    //   bgp_write((void *)peer);
	//}
}

static int
bgp_open_receive (struct peer *peer, bgp_size_t size)
{
    int ret = 0;
    u_char version;
    u_char optlen;
    u_int16_t holdtime;
    u_int16_t send_holdtime;
    u_int16_t keepalive;
    u_int16_t send_keepalive;
    as_t remote_as;
    as_t as4 = 0;
    struct in_addr remote_id;
    int mp_capability;
    u_int8_t notify_data_remote_as[2];
    u_int8_t notify_data_remote_id[4];
	
    /* Parse open packet. */
    version = stream_getc (peer->ibuf);
    memcpy (notify_data_remote_as, stream_pnt (peer->ibuf), 2);
    remote_as  = stream_getw (peer->ibuf);
    holdtime = stream_getw (peer->ibuf);
    memcpy (notify_data_remote_id, stream_pnt (peer->ibuf), 4);
    remote_id.s_addr = stream_get_ipv4 (peer->ibuf);
	
    /* Receive OPEN message log  */
    //if (BGP_DEBUG (normal, NORMAL))
	//{
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s[%d] %s rcv OPEN, version %d, remote-as (in open) %u, holdtime %d, id %s",
                    __func__,__LINE__,peer->host, version, remote_as, holdtime, inet_ntoa (remote_id));
	//}
	
    /* BEGIN to read the capability here, but dont do it yet */
    mp_capability = 0;
    optlen = stream_getc (peer->ibuf);
    if (optlen != 0)
    {
        /* We need the as4 capability value *right now* because
         * if it is there, we have not got the remote_as yet, and without
         * that we do not know which peer is connecting to us now.
         */
        as4 = peek_for_as4_capability (peer, optlen);
    }
	
    /* Just in case we have a silly peer who sends AS4 capability set to 0 */
    if (CHECK_FLAG (peer->cap, PEER_CAP_AS4_RCV) && !as4)
    {
        zlog_err ("%s bad OPEN, got AS4 capability, but AS4 set to 0", peer->host);
		
        bgp_notify_send (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_BAD_PEER_AS);
		
        return -1;
    }
	
    if (remote_as == BGP_AS_TRANS)
    {
        /* Take the AS4 from the capability.  We must have received the
         * capability now!  Otherwise we have a asn16 peer who uses
         * BGP_AS_TRANS, for some unknown reason.
         */
        if (as4 == BGP_AS_TRANS)
        {
            zlog_err ("%s [AS4] NEW speaker using AS_TRANS for AS4, not allowed", peer->host);
			
            bgp_notify_send (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_BAD_PEER_AS);
			
            return -1;
        }
		//if (!as4 && BGP_DEBUG (as4, AS4))
        if (!as4)
    	{
            zlog_debug (BGP_DEBUG_TYPE_AS4,"%s [AS4] OPEN remote_as is AS_TRANS, but no AS4. Odd, but proceeding.", peer->host);
    	}
        else if (as4 < BGP_AS_MAX )      //else if (as4 < BGP_AS_MAX && BGP_DEBUG (as4, AS4))
    	{
            zlog_debug (BGP_DEBUG_TYPE_AS4,"%s [AS4] OPEN remote_as is AS_TRANS, but AS4 (%u) fits in 2-bytes, very odd peer.", peer->host, as4);
    	}
		
        if (as4)
        {
            remote_as = as4;
        }
    }
    else
    {
        /* We may have a partner with AS4 who has an asno < BGP_AS_MAX */
        /* If we have got the capability, peer->as4cap must match remote_as */
        if (CHECK_FLAG (peer->cap, PEER_CAP_AS4_RCV) && as4 != remote_as)
        {
            /* raise error, log this, close session */
            zlog_err ("%s bad OPEN, got AS4 capability, but remote_as %u mismatch with 16bit 'myasn' %u in open", peer->host, as4, remote_as);

			bgp_notify_send (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_BAD_PEER_AS);

			return -1;
        }
    }    
	
    if(peer->status < OpenConfirm)
    {
        peer->status = OpenSent;
		if(peer->local_sport != peer->send_sport)
		{
	        bgp_open_send(peer);
		}
    }
		
    /* Set remote router-id */
    peer->remote_id = remote_id;
	
    /* remote router-id check. */
    if (remote_id.s_addr == 0
	    || IPV4_CLASS_DE (ntohl (remote_id.s_addr))
	    || ntohl (peer->local_id.s_addr) == ntohl (remote_id.s_addr))
    {
        //if (BGP_DEBUG (normal, NORMAL))
    	//{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s bad OPEN, wrong router identifier %s", peer->host, inet_ntoa (remote_id));
    	//}
		
        bgp_notify_send_with_data (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_BAD_BGP_IDENT, notify_data_remote_id, 4);

		return -1;
    }
	
    /* Peer BGP version check. */
    if (version != BGP_VERSION_4)
    {
        u_int16_t maxver = htons(BGP_VERSION_4);
		
        /* XXX this reply may not be correct if version < 4  XXX */
        //if (BGP_DEBUG (normal, NORMAL))
    	//{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s bad protocol version, remote requested %d, local request %d", peer->host, version, BGP_VERSION_4);
    	//}
		
        /* Data must be in network byte order here */
        bgp_notify_send_with_data (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_UNSUP_VERSION, (u_int8_t *) &maxver, 2);

		return -1;
    }
	
    /* Check neighbor as number. */
    if (remote_as != peer->as)
    {
        //if (BGP_DEBUG (normal, NORMAL))
    	//{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s bad OPEN, remote AS is %u, expected %u", peer->host, remote_as, peer->as);
    	//}
		
        bgp_notify_send_with_data (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_BAD_PEER_AS, notify_data_remote_as, 2);
		
        return -1;
    }
	
    /* From the rfc: Upon receipt of an OPEN message, a BGP speaker MUST
       calculate the value of the Hold Timer by using the smaller of its
       configured Hold Time and the Hold Time received in the OPEN message.
       The Hold Time MUST be either zero or at least three seconds.  An
       implementation may reject connections on the basis of the Hold Time. */
    if (holdtime < 3 && holdtime != 0)
    {
        bgp_notify_send (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_UNACEP_HOLDTIME);
		
        return -1;
    }
	
    /* From the rfc: A reasonable maximum time between KEEPALIVE messages
       would be one third of the Hold Time interval.  KEEPALIVE messages
       MUST NOT be sent more frequently than one per second.  An
       implementation MAY adjust the rate at which it sends KEEPALIVE
       messages as a function of the Hold Time interval. */
    if (CHECK_FLAG (peer->config, PEER_CONFIG_TIMER))
    {
        send_holdtime = peer->holdtime;
        send_keepalive = peer->keepalive;
    }
    else
    {
        send_holdtime = peer->bgp->default_holdtime;
        send_keepalive = peer->bgp->default_keepalive;
    }
	
    if (holdtime < send_holdtime)
    {
        peer->v_holdtime = holdtime;
    }
    else
    {
        peer->v_holdtime = send_holdtime;
    }
	
    keepalive = peer->v_holdtime / 3;
	
    if(send_keepalive < keepalive)
    {
        peer->v_keepalive = send_keepalive;
    }
    else
    {
        peer->v_keepalive = keepalive;
    }
	
    /* Open option part parse. */
    if (optlen != 0)
    {
        if ((ret = bgp_open_option_parse (peer, optlen, &mp_capability)) < 0)
        {
            bgp_notify_send (peer, BGP_NOTIFY_OPEN_ERR, BGP_NOTIFY_OPEN_UNSPECIFIC);
			
            return ret;
        }
    }
    else
    {
        //if (BGP_DEBUG (normal, NORMAL))
    	//{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcvd OPEN w/ OPTION parameter len: 0", peer->host);
    	//}
    }
	
    /*
     * Assume that the peer supports the locally configured set of
     * AFI/SAFIs if the peer did not send us any Mulitiprotocol
     * capabilities, or if 'override-capability' is configured.
     */
    if (! mp_capability || CHECK_FLAG (peer->flags, PEER_FLAG_OVERRIDE_CAPABILITY))
    {
        peer->afc_nego[AFI_IP][SAFI_UNICAST] = peer->afc[AFI_IP][SAFI_UNICAST];
        peer->afc_nego[AFI_IP][SAFI_MULTICAST] = peer->afc[AFI_IP][SAFI_MULTICAST];
        peer->afc_nego[AFI_IP6][SAFI_UNICAST] = peer->afc[AFI_IP6][SAFI_UNICAST];
        peer->afc_nego[AFI_IP6][SAFI_MULTICAST] = peer->afc[AFI_IP6][SAFI_MULTICAST];
    }
	peer->event = Receive_OPEN_message;
    BGP_EVENT_EXECUTE (peer);
	
    peer->packet_size = 0;
	
    if (peer->ibuf)
    {
        stream_reset (peer->ibuf);
    }
	
    return 0;
}

/* Frontend for NLRI parsing, to fan-out to AFI/SAFI specific parsers */
int
bgp_nlri_parse (struct peer *peer, struct attr *attr, struct bgp_nlri *packet)
{
    switch (packet->safi)
    {
	    case SAFI_UNICAST:
	    case SAFI_MULTICAST:
	        return bgp_nlri_parse_ip (peer, attr, packet);
	    case SAFI_MPLS_VPN:
	    case SAFI_MPLS_LABELED_VPN:
	        return bgp_nlri_parse_vpn (peer, attr, packet);
	    case SAFI_ENCAP:
	        return bgp_nlri_parse_encap (peer, attr, packet);
    }
    return -1;
}

/* Parse BGP Update packet and make attribute object. */
static int
bgp_update_receive (struct peer *peer, bgp_size_t size)
{
    int ret, nlri_ret = 0;
    u_char *end;
    struct stream *s;
    struct attr attr;
    struct attr_extra extra;
    bgp_size_t attribute_len;
    bgp_size_t update_len;
    bgp_size_t withdraw_len;
    int i;
	
    enum NLRI_TYPES
    {
        NLRI_UPDATE,
        NLRI_WITHDRAW,
        NLRI_MP_UPDATE,
        NLRI_MP_WITHDRAW,
        NLRI_TYPE_MAX,
    };
	
    struct bgp_nlri nlris[NLRI_TYPE_MAX];
	
    /* Status must be Established. */
    if (peer->status != Established)
    {
        zlog_err ("%s [FSM] Update packet received under status %s",
                  peer->host, LOOKUP (bgp_status_msg, peer->status));
        bgp_notify_send (peer, BGP_NOTIFY_FSM_ERR, 0);
        return -1;
    }
	
    /* Set initial values. */
    memset (&attr, 0, sizeof (struct attr));
    memset (&extra, 0, sizeof (struct attr_extra));
    memset (&nlris, 0, sizeof nlris);
    attr.extra = &extra;
	
    s = peer->ibuf;
    end = stream_pnt (s) + size;
	
    /* RFC1771 6.3 If the Unfeasible Routes Length or Total Attribute
       Length is too large (i.e., if Unfeasible Routes Length + Total
       Attribute Length + 23 exceeds the message Length), then the Error
       Subcode is set to Malformed Attribute List.  */
    if (stream_pnt (s) + 2 > end)
    {
        zlog_err ("%s [Error] Update packet error"
                  " (packet length is short for unfeasible length)",
                  peer->host);
        bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR,
                         BGP_NOTIFY_UPDATE_MAL_ATTR);
        return -1;
    }
	
    /* Unfeasible Route Length. */
    withdraw_len = stream_getw (s);
	
    /* Unfeasible Route Length check. */
    if (stream_pnt (s) + withdraw_len > end)
    {
        zlog_err ("%s [Error] Update packet error"
                  " (packet unfeasible length overflow %d)",
                  peer->host, withdraw_len);
        bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR,
                         BGP_NOTIFY_UPDATE_MAL_ATTR);
        return -1;
    }
	
    /* Unfeasible Route packet format check. */
    if (withdraw_len > 0)
    {
        nlris[NLRI_WITHDRAW].afi = AFI_IP;
        nlris[NLRI_WITHDRAW].safi = SAFI_UNICAST;
        nlris[NLRI_WITHDRAW].nlri = stream_pnt (s);
        nlris[NLRI_WITHDRAW].length = withdraw_len;
		
        //if (BGP_DEBUG (packet, PACKET_RECV))
        //{
            zlog_debug (BGP_DEBUG_TYPE_PACKET,"%s [Update:RECV] Unfeasible NLRI received", peer->host);
        //}
		
        stream_forward_getp (s, withdraw_len);
    }
	
    /* Attribute total length check. */
    if (stream_pnt (s) + 2 > end)
    {
        zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s [Error] Packet Error"
                   " (update packet is short for attribute length)",
                   peer->host);
        bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR,
                         BGP_NOTIFY_UPDATE_MAL_ATTR);
        return -1;
    }
	
    /* Fetch attribute total length. */
    attribute_len = stream_getw (s);
	
    /* Attribute length check. */
    if (stream_pnt (s) + attribute_len > end)
    {
        zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s [Error] Packet Error"
                   " (update packet attribute length overflow %d)",
                   peer->host, attribute_len);
		
        bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR,
                         BGP_NOTIFY_UPDATE_MAL_ATTR);
		
        return -1;
    }
	
    /* Certain attribute parsing errors should not be considered bad enough
     * to reset the session for, most particularly any partial/optional
     * attributes that have 'tunneled' over speakers that don't understand
     * them. Instead we withdraw only the prefix concerned.
     *
     * Complicates the flow a little though..
     */
    bgp_attr_parse_ret_t attr_parse_ret = BGP_ATTR_PARSE_PROCEED;
	
    /* This define morphs the update case into a withdraw when lower levels
     * have signalled an error condition where this is best.
     */
#define NLRI_ATTR_ARG (attr_parse_ret != BGP_ATTR_PARSE_WITHDRAW ? &attr : NULL)

    /* Parse attribute when it exists. */
    if (attribute_len)
    {
        attr_parse_ret = bgp_attr_parse (peer, &attr, attribute_len,
                                         &nlris[NLRI_MP_UPDATE], &nlris[NLRI_MP_WITHDRAW]);
        if (attr_parse_ret == BGP_ATTR_PARSE_ERROR)
        {
            bgp_attr_unintern_sub (&attr);
            bgp_attr_flush (&attr);
            return -1;
        }
    }
	
    /* Logging the attribute. */
    if (attr_parse_ret == BGP_ATTR_PARSE_WITHDRAW
            || BGP_DEBUG (update, UPDATE_IN))
    {
        char attrstr[BUFSIZ];
		
        attrstr[0] = '\0';
		
        ret = bgp_dump_attr (peer, &attr, attrstr, BUFSIZ);
        int lvl = (attr_parse_ret == BGP_ATTR_PARSE_WITHDRAW)
                  ? LOG_ERR : LOG_DEBUG;
		
        if (attr_parse_ret == BGP_ATTR_PARSE_WITHDRAW)
    	{
            zlog (peer->log, LOG_ERR,
                  "%s rcvd UPDATE with errors in attr(s)!! Withdrawing route.",
                  peer->host);
    	}
        if (ret)
    	{
            zlog (peer->log, lvl, "%s rcvd UPDATE w/ attr: %s",
                  peer->host, attrstr);
    	}
    }
	
    /* Network Layer Reachability Information. */
    update_len = end - stream_pnt (s);
    if (update_len)
    {
        /* Set NLRI portion to structure. */
        nlris[NLRI_UPDATE].afi = AFI_IP;
        nlris[NLRI_UPDATE].safi = SAFI_UNICAST;
        nlris[NLRI_UPDATE].nlri = stream_pnt (s);
        nlris[NLRI_UPDATE].length = update_len;
        stream_forward_getp (s, update_len);
    }
	
    /* Parse any given NLRIs */
    for (i = NLRI_UPDATE; i < NLRI_TYPE_MAX; i++)
    {
        /* We use afi and safi as indices into tables and what not.  It would
         * be impossible, at this time, to support unknown afi/safis.  And
         * anyway, the peer needs to be configured to enable the afi/safi
         * explicitly which requires UI support.
         *
         * Ignore unknown afi/safi NLRIs.
         *
         * Note: this means nlri[x].afi/safi still can not be trusted for
         * indexing later in this function!
         *
         * Note2: This will also remap the wire code-point for VPN safi to the
         * internal safi_t point, as needs be.
         */
        if (!bgp_afi_safi_valid_indices (nlris[i].afi, &nlris[i].safi))
        {
            plog_info (peer->log,
                       "%s [Info] UPDATE with unsupported AFI/SAFI %u/%u",
                       peer->host, nlris[i].afi, nlris[i].safi);
            continue;
        }
		
        /* NLRI is processed only when the peer is configured specific
           Address Family and Subsequent Address Family. */
        if (!peer->afc[nlris[i].afi][nlris[i].safi])
        {
            plog_info (peer->log,
                       "%s [Info] UPDATE for non-enabled AFI/SAFI %u/%u",
                       peer->host, nlris[i].afi, nlris[i].safi);
            continue;
        }
		
        /* EoR handled later */
        if (nlris[i].length == 0)
        {
            continue;
        }
		
        switch (i)
        {
	        case NLRI_UPDATE:
	        case NLRI_MP_UPDATE:
	            nlri_ret = bgp_nlri_parse (peer, NLRI_ATTR_ARG, &nlris[i]);
	            break;
	        case NLRI_WITHDRAW:
	        case NLRI_MP_WITHDRAW:
	            nlri_ret = bgp_nlri_parse (peer, NULL, &nlris[i]);
				break;
        }
		
        if (nlri_ret < 0)
        {
            plog_err (peer->log,
                      "%s [Error] Error parsing NLRI", peer->host);
			
            if (peer->status == Established)
        	{
                bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR,
                                 i <= NLRI_WITHDRAW
                                 ? BGP_NOTIFY_UPDATE_INVAL_NETWORK
                                 : BGP_NOTIFY_UPDATE_OPT_ATTR_ERR);
        	}
            bgp_attr_unintern_sub (&attr);
			
            return -1;
        }
    }
	
    /* EoR checks.
     *
     * Non-MP IPv4/Unicast EoR is a completely empty UPDATE
     * and MP EoR should have only an empty MP_UNREACH
     */
    if (!update_len && !withdraw_len
            && nlris[NLRI_MP_UPDATE].length == 0)
    {
        afi_t afi = 0;
        safi_t safi;
		
        /* Non-MP IPv4/Unicast is a completely empty UPDATE - already
         * checked update and withdraw NLRI lengths are 0.
         */
        if (!attribute_len)
        {
            afi = AFI_IP;
            safi = SAFI_UNICAST;
        }
		
        /* otherwise MP AFI/SAFI is an empty update, other than an empty
         * MP_UNREACH_NLRI attr (with an AFI/SAFI we recognise).
         */
        else if (attr.flag == BGP_ATTR_MP_UNREACH_NLRI
                 && nlris[NLRI_MP_WITHDRAW].length == 0
                 && bgp_afi_safi_valid_indices (nlris[NLRI_MP_WITHDRAW].afi, &nlris[NLRI_MP_WITHDRAW].safi))
        {
            afi = nlris[NLRI_MP_WITHDRAW].afi;
            safi = nlris[NLRI_MP_WITHDRAW].safi;
        }
		
        if (afi && peer->afc[afi][safi])
        {
            /* End-of-RIB received */
            SET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_EOR_RECEIVED);
			
            /* NSF delete stale route */
            if (peer->nsf[afi][safi])
            {
                bgp_clear_stale_route (peer, afi, safi);
            }
			
            if (BGP_DEBUG (normal, NORMAL))
        	{
                zlog (peer->log, LOG_DEBUG, "rcvd End-of-RIB for %s from %s",  peer->host, afi_safi_print (afi, safi));
        	}
        }
    }
	
    /* Everything is done.  We unintern temporary structures which
       interned in bgp_attr_parse(). */
    bgp_attr_unintern_sub (&attr);
    bgp_attr_flush (&attr);
	
    /* If peering is stopped due to some reason, do not generate BGP
       event.  */
    if (peer->status != Established)
    {
        return 0;
    }
	
    /* Increment packet counter. */
    peer->update_in++;
    peer->update_time = bgp_clock ();
	
    /* Rearm holdtime timer */
    BGP_TIMER_OFF (peer->t_holdtime);
	
    bgp_timer_set (peer);
    return 0;
}

/* Notify message treatment function. */
static void
bgp_notify_receive (struct peer *peer, bgp_size_t size)
{
    struct bgp_notify bgp_notify;
	
    if (peer->notify.data)
    {
        XFREE (MTYPE_TMP, peer->notify.data);
        peer->notify.data = NULL;
        peer->notify.length = 0;
    }
	
    bgp_notify.code = stream_getc (peer->ibuf);
    bgp_notify.subcode = stream_getc (peer->ibuf);
    bgp_notify.length = size - 2;
    bgp_notify.data = NULL;
	
    /* Preserv notify code and sub code. */
    peer->notify.code = bgp_notify.code;
    peer->notify.subcode = bgp_notify.subcode;
	
    /* For further diagnostic record returned Data. */
    if (bgp_notify.length)
    {
        peer->notify.length = size - 2;
        peer->notify.data = XMALLOC (MTYPE_TMP, size - 2);
        memcpy (peer->notify.data, stream_pnt (peer->ibuf), size - 2);
    }
	
    /* For debug */
    {
        int i;
        int first = 0;
        char c[4];
        if (bgp_notify.length)
        {
            bgp_notify.data = XMALLOC (MTYPE_TMP, bgp_notify.length * 3);
            for (i = 0; i < bgp_notify.length; i++)
            {
                if (first)
                {
                    sprintf (c, " %02x", stream_getc (peer->ibuf));
                    strcat (bgp_notify.data, c);
                }
                else
                {
                    first = 1;
                    sprintf (c, "%02x", stream_getc (peer->ibuf));
                    strcpy (bgp_notify.data, c);
                }
            }
        }
        bgp_notify_print(peer, &bgp_notify, "received");
        if (bgp_notify.data)
        {
            XFREE (MTYPE_TMP, bgp_notify.data);
            bgp_notify.data = NULL;
            bgp_notify.length = 0;
        }
    }
	
    /* peer count update */
    peer->notify_in++;
	
    if (peer->status == Established)
    {
        peer->last_reset = PEER_DOWN_NOTIFY_RECEIVED;
    }
	
    /* We have to check for Notify with Unsupported Optional Parameter.
       in that case we fallback to open without the capability option.
       But this done in bgp_stop. We just mark it here to avoid changing
       the fsm tables.  */
    if (bgp_notify.code == BGP_NOTIFY_OPEN_ERR && bgp_notify.subcode == BGP_NOTIFY_OPEN_UNSUP_PARAM )
    {
        UNSET_FLAG (peer->sflags, PEER_STATUS_CAPABILITY_OPEN);
    }
	
	peer->event = Receive_NOTIFICATION_message;
    BGP_EVENT_ADD (peer);
}

/* Keepalive treatment function -- get keepalive send keepalive */
static void
bgp_keepalive_receive (struct peer *peer, bgp_size_t size)
{
    //if (BGP_DEBUG (keepalive, KEEPALIVE))
    //{
        zlog_debug (BGP_DEBUG_TYPE_KEEPALIVE,"%s KEEPALIVE rcvd", peer->host);
    //}

	peer->event = Receive_KEEPALIVE_message;
    BGP_EVENT_EXECUTE (peer);
}

/* Route refresh message is received. */
static void
bgp_route_refresh_receive (struct peer *peer, bgp_size_t size)
{
    afi_t afi;
    safi_t safi;
    struct stream *s;
	
    /* If peer does not have the capability, send notification. */
    if (! CHECK_FLAG (peer->cap, PEER_CAP_REFRESH_ADV))
    {
        plog_err (peer->log, "%s [Error] BGP route refresh is not enabled",
                  peer->host);
        bgp_notify_send (peer,
                         BGP_NOTIFY_HEADER_ERR,
                         BGP_NOTIFY_HEADER_BAD_MESTYPE);
        return;
    }
	
    /* Status must be Established. */
    if (peer->status != Established)
    {
        plog_err (peer->log,
                  "%s [Error] Route refresh packet received under status %s",
                  peer->host, LOOKUP (bgp_status_msg, peer->status));
        bgp_notify_send (peer, BGP_NOTIFY_FSM_ERR, 0);
        return;
    }
	
    s = peer->ibuf;
	
    /* Parse packet. */
    afi = stream_getw (s);
	
    /* reserved byte */
    stream_getc (s);
    safi = stream_getc (s);
	
    //if (BGP_DEBUG (normal, NORMAL))
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcvd REFRESH_REQ for afi/safi: %d/%d",
                    peer->host, afi, safi);
	
    /* Check AFI and SAFI. */
    if ((afi != AFI_IP && afi != AFI_IP6)
            || (safi != SAFI_UNICAST && safi != SAFI_MULTICAST
                && safi != SAFI_MPLS_LABELED_VPN))
    {
        //if (BGP_DEBUG (normal, NORMAL))
        //{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s REFRESH_REQ for unrecognized afi/safi: %d/%d - ignored",
                        peer->host, afi, safi);
        //}
        return;
    }
	
    /* Adjust safi code. */
    if (safi == SAFI_MPLS_LABELED_VPN)
    {
        safi = SAFI_MPLS_VPN;
    }
	
    if (size != BGP_MSG_ROUTE_REFRESH_MIN_SIZE - BGP_HEADER_SIZE)
    {
        u_char *end;
        u_char when_to_refresh;
        u_char orf_type;
        u_int16_t orf_len;
		
        if (size - (BGP_MSG_ROUTE_REFRESH_MIN_SIZE - BGP_HEADER_SIZE) < 5)
        {
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s ORF route refresh length error", peer->host);
            bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
            return;
        }
		
        when_to_refresh = stream_getc (s);
        end = stream_pnt (s) + (size - 5);
        while ((stream_pnt (s) + 2) < end)
        {
            orf_type = stream_getc (s);
            orf_len = stream_getw (s);
			
            /* orf_len in bounds? */
            if ((stream_pnt (s) + orf_len) > end)
            {
                break;    /* XXX: Notify instead?? */
            }
            if (orf_type == ORF_TYPE_PREFIX
                    || orf_type == ORF_TYPE_PREFIX_OLD)
            {
                uint8_t *p_pnt = stream_pnt (s);
                uint8_t *p_end = stream_pnt (s) + orf_len;
                struct orf_prefix orfp;
                u_char common = 0;
                u_int32_t seq;
                int psize;
                char name[BUFSIZ];
                int ret;
                //if (BGP_DEBUG (normal, NORMAL))
                //{
                    zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcvd Prefixlist ORF(%d) length %d",
                                peer->host, orf_type, orf_len);
                //}
				
                /* we're going to read at least 1 byte of common ORF header,
                 * and 7 bytes of ORF Address-filter entry from the stream
                 */
                if (orf_len < 7)
                {
                    break;
                }
				
                /* ORF prefix-list name */
                sprintf (name, "%s.%d.%d", peer->host, afi, safi);
                while (p_pnt < p_end)
                {
                    /* If the ORF entry is malformed, want to read as much of it
                     * as possible without going beyond the bounds of the entry,
                     * to maximise debug information.
                     */
                    int ok;
                    memset (&orfp, 0, sizeof (struct orf_prefix));
                    common = *p_pnt++;
					
                    /* after ++: p_pnt <= p_end */
                    if (common & ORF_COMMON_PART_REMOVE_ALL)
                    {
                        //if (BGP_DEBUG (normal, NORMAL))
                        //{
                            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcvd Remove-All pfxlist ORF request", peer->host);
                        //}
                        prefix_bgp_orf_remove_all (afi, name);
                        break;
                    }
                    ok = ((size_t)(p_end - p_pnt) >= sizeof(u_int32_t)) ;
                    if (ok)
                    {
                        memcpy (&seq, p_pnt, sizeof (u_int32_t));
                        p_pnt += sizeof (u_int32_t);
                        orfp.seq = ntohl (seq);
                    }
                    else
                    {
                        p_pnt = p_end ;
                    }
                    if ((ok = (p_pnt < p_end)))
                    {
                        orfp.ge = *p_pnt++ ;    /* value checked in prefix_bgp_orf_set() */
                    }
                    if ((ok = (p_pnt < p_end)))
                    {
                        orfp.le = *p_pnt++ ;    /* value checked in prefix_bgp_orf_set() */
                    }
                    if ((ok = (p_pnt < p_end)))
                    {
                        orfp.p.prefixlen = *p_pnt++ ;
                    }
                    orfp.p.family = afi2family (afi);   /* afi checked already  */
                    psize = PSIZE (orfp.p.prefixlen);   /* 0 if not ok          */
                    if (psize > prefix_blen(&orfp.p))   /* valid for family ?   */
                    {
                        ok = 0 ;
                        psize = prefix_blen(&orfp.p) ;
                    }
                    if (psize > (p_end - p_pnt))        /* valid for packet ?   */
                    {
                        ok = 0 ;
                        psize = p_end - p_pnt ;
                    }
                    if (psize > 0)
                    {
                        memcpy (&orfp.p.u.prefix, p_pnt, psize);
                    }
                    p_pnt += psize;
                    //if (BGP_DEBUG (normal, NORMAL))
                    //{
                        char buf[INET6_BUFSIZ];
                        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcvd %s %s seq %u %s/%d ge %d le %d%s",
                                    peer->host,
                                    (common & ORF_COMMON_PART_REMOVE ? "Remove" : "Add"),
                                    (common & ORF_COMMON_PART_DENY ? "deny" : "permit"),
                                    orfp.seq,
                                    inet_ntop (orfp.p.family, &orfp.p.u.prefix, buf, INET6_BUFSIZ),
                                    orfp.p.prefixlen, orfp.ge, orfp.le,
                                    ok ? "" : " MALFORMED");
                    //}
                    if (ok)
                        ret = prefix_bgp_orf_set (name, afi, &orfp,
                                                  (common & ORF_COMMON_PART_DENY ? 0 : 1 ),
                                                  (common & ORF_COMMON_PART_REMOVE ? 0 : 1));
                    if (!ok || (ok && ret != CMD_SUCCESS))
                    {
                        //if (BGP_DEBUG (normal, NORMAL))
                            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s Received misformatted prefixlist ORF."
                                        " Remove All pfxlist", peer->host);
                        prefix_bgp_orf_remove_all (afi, name);
                        break;
                    }
                }
                peer->orf_plist[afi][safi] =
                    prefix_bgp_orf_lookup (afi, name);
            }
            stream_forward_getp (s, orf_len);
        }
		
        //if (BGP_DEBUG (normal, NORMAL))
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcvd Refresh %s ORF request", peer->host,
                        when_to_refresh == REFRESH_DEFER ? "Defer" : "Immediate");
        if (when_to_refresh == REFRESH_DEFER)
        {
            return;
        }
    }
	
    /* First update is deferred until ORF or ROUTE-REFRESH is received */
    if (CHECK_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_WAIT_REFRESH))
    {
        UNSET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_WAIT_REFRESH);
    }
	
    /* Perform route refreshment to the peer */
    bgp_announce_route (peer, afi, safi);
}

static int
bgp_capability_msg_parse (struct peer *peer, u_char *pnt, bgp_size_t length)
{
    u_char *end;
    struct capability_mp_data mpc;
    struct capability_header *hdr;
    u_char action;
    afi_t afi;
    safi_t safi;
	
    end = pnt + length;
    while (pnt < end)
    {
        /* We need at least action, capability code and capability length. */
        if (pnt + 3 > end)
        {
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s Capability length error", peer->host);
            bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
            return -1;
        }
        action = *pnt;
        hdr = (struct capability_header *)(pnt + 1);
		
        /* Action value check.  */
        if (action != CAPABILITY_ACTION_SET
                && action != CAPABILITY_ACTION_UNSET)
        {
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s Capability Action Value error %d",
                       peer->host, action);
            bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
            return -1;
        }
        //if (BGP_DEBUG (normal, NORMAL))
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s CAPABILITY has action: %d, code: %u, length %u",
                        peer->host, action, hdr->code, hdr->length);
		
        /* Capability length check. */
        if ((pnt + hdr->length + 3) > end)
        {
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s Capability length error", peer->host);
            bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
            return -1;
        }
		
        /* Fetch structure to the byte stream. */
        memcpy (&mpc, pnt + 3, sizeof (struct capability_mp_data));
		
        /* We know MP Capability Code. */
        if (hdr->code == CAPABILITY_CODE_MP)
        {
            afi = ntohs (mpc.afi);
            safi = mpc.safi;
			
            /* Ignore capability when override-capability is set. */
            if (CHECK_FLAG (peer->flags, PEER_FLAG_OVERRIDE_CAPABILITY))
            {
                continue;
            }
            if (!bgp_afi_safi_valid_indices (afi, &safi))
            {
                //if (BGP_DEBUG (normal, NORMAL))
                    zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s Dynamic Capability MP_EXT afi/safi invalid "
                                "(%u/%u)", peer->host, afi, safi);
                continue;
            }
			
            /* Address family check.  */
            //if (BGP_DEBUG (normal, NORMAL))
                zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s CAPABILITY has %s MP_EXT CAP for afi/safi: %u/%u",
                            peer->host,
                            action == CAPABILITY_ACTION_SET
                            ? "Advertising" : "Removing",
                            ntohs(mpc.afi) , mpc.safi);
            if (action == CAPABILITY_ACTION_SET)
            {
                peer->afc_recv[afi][safi] = 1;
                if (peer->afc[afi][safi])
                {
                    peer->afc_nego[afi][safi] = 1;
                    bgp_announce_route (peer, afi, safi);
                }
            }
            else
            {
                peer->afc_recv[afi][safi] = 0;
                peer->afc_nego[afi][safi] = 0;
                if (peer_active_nego (peer))
                {
                    bgp_clear_route (peer, afi, safi, BGP_CLEAR_ROUTE_NORMAL);
                }
                else
                {   
                    peer->event = BGP_Stop;
                    BGP_EVENT_ADD (peer);
                }
            }
        }
        else
        {
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s unrecognized capability code: %d - ignored",
                       peer->host, hdr->code);
        }
        pnt += hdr->length + 3;
    }
    return 0;
}

/* Dynamic Capability is received.
 *
 * This is exported for unit-test purposes
 */
int
bgp_capability_receive (struct peer *peer, bgp_size_t size)
{
    u_char *pnt;
	
    /* Fetch pointer. */
    pnt = stream_pnt (peer->ibuf);
    //if (BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s rcv CAPABILITY", peer->host);
    //}
	
    /* If peer does not have the capability, send notification. */
    if (! CHECK_FLAG (peer->cap, PEER_CAP_DYNAMIC_ADV))
    {
        plog_err (peer->log, "%s [Error] BGP dynamic capability is not enabled",
                  peer->host);
        bgp_notify_send (peer,
                         BGP_NOTIFY_HEADER_ERR,
                         BGP_NOTIFY_HEADER_BAD_MESTYPE);
        return -1;
    }
	
    /* Status must be Established. */
    if (peer->status != Established)
    {
        plog_err (peer->log,
                  "%s [Error] Dynamic capability packet received under status %s", peer->host, LOOKUP (bgp_status_msg, peer->status));
        bgp_notify_send (peer, BGP_NOTIFY_FSM_ERR, 0);
        return -1;
    }
	
    /* Parse packet. */
    return bgp_capability_msg_parse (peer, pnt, size);
}

/* Marker check. */
static int
bgp_marker_all_one (struct stream *s, int length,unsigned long offset)
{
    int i;
	
    for (i = 0; i < length; i++)
        if (s->data[i+offset] != 0xff)
            return 0;
    return 1;
}

/* Recent thread time.
   On same clock base as bgp_clock (MONOTONIC)
   but can be time of last context switch to bgp_read thread. */
static time_t
bgp_recent_clock (void)
{
    return time_get_recent_relative_time().tv_sec;
}

int bgp_pkt_check_ip(struct peer *peer)
{
    afi_t afi = AFI_IP;
    struct prefix p;
    struct bgp *bgp;
    struct bgp_static *bgp_static;
    struct bgp_node *rn;
	char buf[SU_ADDRSTRLEN];
    struct ifm_l3 l3 = {0};
	
    bgp = bgp_get_default();
    if(!bgp)
    {
        return 0;
    }
	
    memset(&p, 0, sizeof (struct prefix));
	memset(buf,0x0,SU_ADDRSTRLEN);
	
	if(peer->su_local.sa.sa_family == AF_INET6)
	{
		afi = AFI_IP6;
		p.family = AF_INET6;
	    p.prefixlen = IPV6_MAX_BITLEN;
		IPV6_ADDR_COPY(&p.u.prefix6, &peer->su_local.sin6.sin6_addr);
	}
	else
	{
	    p.family = AF_INET;
	    p.prefixlen = IPV4_MAX_BITLEN;
	    p.u.prefix4.s_addr = peer->su_local.sin.sin_addr.s_addr;
	}

    if(bgp->route[afi][SAFI_UNICAST] == NULL)
    {
        return 0;
    }
	
    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] su_local %s ",peer->host,__FUNCTION__,__LINE__, sockunion2str(&peer->su_local, buf, SU_ADDRSTRLEN));
    //}
	
	/*add connect check*/
	if(!bgp_nexthop_connect_check(peer->vrf_id, &peer->su_local))
	{
		return 0;
	}
	
    for(rn = bgp_table_top (bgp->route[afi][SAFI_UNICAST]); rn; rn = bgp_route_next (rn))
    {
    
		for(bgp_static = rn->info; bgp_static; bgp_static = bgp_static->next)
        {
            if((ROUTE_PROTO_CONNECT == bgp_static->type) && prefix_same(&rn->p, &p) && peer->vrf_id == bgp_static->vrf_id)
            {   
            	if(IFM_TYPE_IS_LOOPBCK(bgp_static->ifindex))
                {
                    SET_FLAG(peer->flags,PEER_FLAG_LOOPBACK);
                }
                else
                {
                    UNSET_FLAG(peer->flags,PEER_FLAG_LOOPBACK);
                }
				
				if(ifm_get_l3if(bgp_static->ifindex, MODULE_ID_BGP, &l3) == 0)
				{
					if(afi == AFI_IP6)
					{					
						peer->mask_len = l3.ipv6[0].prefixlen;
					}
					else
					{
						peer->mask_len = l3.ipv4[0].prefixlen;
					}
				}
				
                return 1;
            }
        }
    }
	
    return 0;
}

/* Starting point of packet process function. */
static int bgp_read_pkt (struct pkt_buffer *pkt)
{
    u_char type = 0;
    struct peer *peer = NULL;
    bgp_size_t size = 0;
    struct bgp *bgp = NULL;
    union sockunion su;
    char notify_data_length[2];
    size_t newsize;
    unsigned long offset = 0;
	uint32_t sip = 0, dip = 0;
    struct in6_addr src_v6, dst_v6;
	uint16_t sport = 0, dport = 0;
	char src_ip[IPV6_ADDR_STRLEN];
	char dst_ip[IPV6_ADDR_STRLEN];
	char buf[SU_ADDRSTRLEN];
	
	memset(&src_ip, 0x0, IPV6_ADDR_STRLEN);
	memset(&dst_ip, 0x0, IPV6_ADDR_STRLEN);
	
	memset(&src_v6, 0x0, sizeof(struct in6_addr));
	memset(&dst_v6, 0x0, sizeof(struct in6_addr));

	memset(buf, 0x0, SU_ADDRSTRLEN);
	
	if(pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6)
	{
		IPV6_ADDR_COPY(&src_v6,pkt->cb.ipcb.sipv6.ipv6);
	    IPV6_ADDR_COPY(&dst_v6,pkt->cb.ipcb.dipv6.ipv6);
		inet_ipv6tostr(&pkt->cb.ipcb.sipv6, src_ip, IPV6_ADDR_STRLEN);
		inet_ipv6tostr(&pkt->cb.ipcb.dipv6, dst_ip, IPV6_ADDR_STRLEN);
	}
	else
	{
		sip = pkt->cb.ipcb.dip;
		dip = pkt->cb.ipcb.sip;
		inet_ipv4tostr(pkt->cb.ipcb.sip, src_ip);
		inet_ipv4tostr(pkt->cb.ipcb.dip, dst_ip);
		
		//if(BGP_DEBUG (normal, NORMAL))
	   // {
	        zlog_debug(BGP_DEBUG_TYPE_NORMAL," %s[%d] the packet receive with sip:%s  dip:%s",__FUNCTION__,__LINE__,src_ip,dst_ip);
	   // }
	}
	
	sport = pkt->cb.ipcb.dport;
	dport = pkt->cb.ipcb.sport;

    bgp = bgp_get_default();
    if(!bgp)
    {	
    	if(pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6)
    	{
			peer_close_tcp_rcv_ipv6(pkt->cb.ipcb.sipv6, pkt->cb.ipcb.dipv6, sport, dport);
    	}
		else
		{
    		peer_close_tcp_rcv_ipv4(sip, dip, sport, dport);
		}
		
        return 0;
    }
	
    memset (&su, 0, sizeof (union sockunion));
	
	if(pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6)
	{    
		su.sin.sin_family = AF_INET6;
		IPV6_ADDR_COPY(&su.sin6.sin6_addr,&pkt->cb.ipcb.sipv6);
	}
	else
	{
	    su.sin.sin_family = AF_INET;
	    su.sin.sin_addr.s_addr = htonl(pkt->cb.ipcb.sip);
	}
	
    peer = peer_lookup (bgp, &su);
    if(!peer || !CHECK_FLAG (peer->flags, PEER_FLAG_ENABLE))
    {    	
    	if(pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6)
    	{
			peer_close_tcp_rcv_ipv6(pkt->cb.ipcb.sipv6, pkt->cb.ipcb.dipv6, sport, dport);
    	}
		else
		{
    		peer_close_tcp_rcv_ipv4(sip, dip, sport, dport);
		}
		
        return 0;
    }
	
	if(pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6)
	{
		if(ipv6_is_zero((struct ipv6_addr *)&(peer->su_local.sin6.sin6_addr))| !IPV6_ADDR_SAME(&peer->su_local.sin6.sin6_addr,&pkt->cb.ipcb.dipv6))
	    {
			peer_close_tcp_rcv_ipv6(pkt->cb.ipcb.sipv6, pkt->cb.ipcb.dipv6, sport, dport);
			
			if (BGP_DEBUG (normal, NORMAL))
			{
	        	zlog_err("%s %s[%d] pkt sip %s dip %s ", peer->host, __FUNCTION__, __LINE__, src_ip, dst_ip);
			}
			
	        return 0;
	    }
	}
	else
	{
	    if(peer->su_local.sin.sin_addr.s_addr == 0 || peer->su_local.sin.sin_addr.s_addr != htonl(pkt->cb.ipcb.dip))
	    {
	    	peer_close_tcp_rcv_ipv4(sip, dip, sport, dport);
			
			if (BGP_DEBUG (normal, NORMAL))
			{
	        	zlog_err("%s %s[%d] pkt sip %s dip %s ",peer->host,__FUNCTION__,__LINE__,  src_ip, dst_ip);
			}
			
	        return 0;
	    }
	}
	
    if(bgp_pkt_check_ip(peer) == 0)
    {
    	if(pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6)
		{
    		peer_close_tcp_rcv_ipv6(pkt->cb.ipcb.sipv6, pkt->cb.ipcb.dipv6, sport, dport);
		}
		else
		{
    		peer_close_tcp_rcv_ipv4(sip, dip, sport, dport);			
		}
		
		//if (BGP_DEBUG (normal, NORMAL))
		//{
        	zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] su_local %s ",peer->host,__FUNCTION__,__LINE__, sockunion2str(&peer->su_local, buf, SU_ADDRSTRLEN));
		//}
		
		return 0;
    }
	
    //if(BGP_DEBUG (normal, NORMAL))
    //{
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] data_len %d buf_all %lu sport %d dport %d ",peer->host,__FUNCTION__,__LINE__,  pkt->data_len, stream_get_endp(peer->ibuf_all), pkt->cb.ipcb.sport, pkt->cb.ipcb.dport);
    //}

	if(BGP_PORT_DEFAULT == peer->send_sport &&  peer->send_dport && peer->send_dport != pkt->cb.ipcb.sport)
	{   
	    zlog_debug(BGP_DEBUG_TYPE_NORMAL,"peer send_port %d, pkt->cb.ipcb.sport %d\n",peer->send_dport, pkt->cb.ipcb.sport);
		peer_close_tcp_send(peer);
	}

	peer->t_read = NULL;
    //peer->t_read = 0;
    peer->send_sport = pkt->cb.ipcb.dport;
    peer->send_dport = pkt->cb.ipcb.sport;
	
    stream_reset(peer->ibuf);
    stream_write(peer->ibuf,pkt->data,pkt->data_len);
    if(stream_get_endp(peer->ibuf) >= BGP_HEADER_SIZE && bgp_marker_all_one (peer->ibuf, BGP_MARKER_SIZE,0))
    {
		size = stream_getw_from (peer->ibuf, BGP_MARKER_SIZE);
        type = stream_getc_from (peer->ibuf, BGP_MARKER_SIZE + 2);
		if(size == BGP_HEADER_SIZE && type == BGP_MSG_KEEPALIVE && pkt->data_len == size)
		{
            peer->readtime = bgp_recent_clock ();
            bgp_keepalive_receive (peer, size);
			return 0;
		}

		if(stream_get_endp(peer->ibuf_all) 
			&& ((type == BGP_MSG_OPEN )
		        || (type == BGP_MSG_UPDATE )
		        || (type == BGP_MSG_NOTIFY)
		        || (type == BGP_MSG_ROUTE_REFRESH_NEW )
		        || (type == BGP_MSG_ROUTE_REFRESH_OLD )
		        || (type == BGP_MSG_CAPABILITY)))
		{
			stream_reset(peer->ibuf_all);
		}
	        
    }
	
    newsize = stream_get_endp(peer->ibuf_all) + (size_t)(pkt->data_len);
    if(peer->ibuf_all && (newsize > BGP_MAX_PACKET_SIZE))
    {
        stream_resize(peer->ibuf_all,newsize);
    }
	
    stream_write(peer->ibuf_all,pkt->data,pkt->data_len);
	
    if( stream_get_endp(peer->ibuf_all))    //if(BGP_DEBUG (normal, NORMAL) && stream_get_endp(peer->ibuf_all))
    {
        zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] ibuf_all endp %lu , ibuf endp %lu ",peer->host,__FUNCTION__,__LINE__,  stream_get_endp(peer->ibuf_all), stream_get_endp(peer->ibuf));
    }
	
    if(stream_get_endp(peer->ibuf_all) >= BGP_HEADER_SIZE)
    {
        offset = 0;
        peer->packet_size = 0;
        while(bgp_marker_all_one (peer->ibuf_all, BGP_MARKER_SIZE, offset))
        {
            size = stream_getw_from (peer->ibuf_all, BGP_MARKER_SIZE + offset);
            peer->packet_size += size ;
			
            //if(BGP_DEBUG (normal, NORMAL))
            //{
                zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d], peer->packet_size %lu size %d offset %lu ibuf_all %lu ",peer->host,__FUNCTION__,__LINE__,peer->packet_size,size,offset,stream_get_endp(peer->ibuf_all));
            //}
			
            if(peer->packet_size > stream_get_endp(peer->ibuf_all))
            {
                stream_reset(peer->ibuf);
                return 0;
            }
            else if(peer->packet_size == stream_get_endp(peer->ibuf_all))
            {
                stream_reset(peer->ibuf);
                break;
            }
            else
            {
                offset += size;
            }

			if(offset + BGP_MARKER_SIZE + sizeof(u_int16_t) > stream_get_endp(peer->ibuf_all))
			{
				return 0;
			}
        }
    }
	
    if(stream_get_endp(peer->ibuf_all) != peer->packet_size)
    {
        zlog_err("%s %s[%d] endp %lu offset %lu packet_size %lu ",peer->host,__FUNCTION__,__LINE__, stream_get_endp(peer->ibuf_all),offset,peer->packet_size);
        return 0;
    }
	
    offset = 0;
	
    do
    {
    	/*get packet header*/
        stream_reset(peer->ibuf);
        stream_write(peer->ibuf,(void *)((u_int32_t)peer->ibuf_all->data + offset),BGP_HEADER_SIZE);
		
        size = stream_getw_from (peer->ibuf, BGP_MARKER_SIZE);
        type = stream_getc_from (peer->ibuf, BGP_MARKER_SIZE + 2);
		
        //if(BGP_DEBUG (normal, NORMAL))
        //{
            zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] rcv message type %d size %d ", peer->host, __func__,__LINE__, type, size);
        //}
		
        /* Marker check */
        if(((type == BGP_MSG_OPEN) || (type == BGP_MSG_KEEPALIVE))  && ! bgp_marker_all_one (peer->ibuf, BGP_MARKER_SIZE,0))
        {
            bgp_notify_send (peer, BGP_NOTIFY_HEADER_ERR, BGP_NOTIFY_HEADER_NOT_SYNC);
			
            return 0;
        }
		
        /* BGP type check. */
        if (type != BGP_MSG_OPEN && type != BGP_MSG_UPDATE
            && type != BGP_MSG_NOTIFY && type != BGP_MSG_KEEPALIVE
            && type != BGP_MSG_ROUTE_REFRESH_NEW
            && type != BGP_MSG_ROUTE_REFRESH_OLD
            && type != BGP_MSG_CAPABILITY)
        {
            //if (BGP_DEBUG (normal, NORMAL))
            //{
                zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s unknown message type 0x%02x", peer->host, type);
            //}
			
            bgp_notify_send_with_data (peer, BGP_NOTIFY_HEADER_ERR, BGP_NOTIFY_HEADER_BAD_MESTYPE, &type, 1);
			
            return 0;
        }
		
        /* Mimimum packet length check. */
        if ((size < BGP_HEADER_SIZE)
            || (size > BGP_MAX_PACKET_SIZE)
            || (type == BGP_MSG_OPEN && size < BGP_MSG_OPEN_MIN_SIZE)
            || (type == BGP_MSG_UPDATE && size < BGP_MSG_UPDATE_MIN_SIZE)
            || (type == BGP_MSG_NOTIFY && size < BGP_MSG_NOTIFY_MIN_SIZE)
            || (type == BGP_MSG_KEEPALIVE && size != BGP_MSG_KEEPALIVE_MIN_SIZE)
            || (type == BGP_MSG_ROUTE_REFRESH_NEW && size < BGP_MSG_ROUTE_REFRESH_MIN_SIZE)
            || (type == BGP_MSG_ROUTE_REFRESH_OLD && size < BGP_MSG_ROUTE_REFRESH_MIN_SIZE)
            || (type == BGP_MSG_CAPABILITY && size < BGP_MSG_CAPABILITY_MIN_SIZE))
        {
            //if(BGP_DEBUG (normal, NORMAL))
            //{
                zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s bad message length - %d for %s", peer->host, size, type == 128 ? "ROUTE-REFRESH" :  bgp_type_str[(int) type]);
            //}
			
            stream_forward_getp (peer->ibuf, BGP_MARKER_SIZE);
            memcpy (notify_data_length, stream_pnt (peer->ibuf), 2);
            bgp_notify_send_with_data (peer, BGP_NOTIFY_HEADER_ERR, BGP_NOTIFY_HEADER_BAD_MESLEN, (u_char *) notify_data_length, 2);

			return 0;
        }
		
		/*get the packet*/
        stream_reset(peer->ibuf);
        stream_write(peer->ibuf,(void *)((u_int32_t)peer->ibuf_all->data + offset),size);
        offset += size;
		
        if (stream_get_endp (peer->ibuf) < BGP_HEADER_SIZE)
        {			
        	zlog_err("%s %s[%d] ibuf endp %lu offset %lu size %d ",peer->host,__FUNCTION__,__LINE__, stream_get_endp(peer->ibuf),offset,size);
            return 0;
        }
		
        stream_forward_getp (peer->ibuf, BGP_HEADER_SIZE);
        size -= BGP_HEADER_SIZE;
		
        //if(BGP_DEBUG (normal, NORMAL))
        //{
            zlog_debug(BGP_DEBUG_TYPE_NORMAL,"%s %s[%d] status %s rcv %s offset = %lu, size = %d,local_sport(%d),local_dport(%d),send_sport(%d),send_dport(%d)\n",peer->host,__FUNCTION__,__LINE__, 
				LOOKUP (bgp_status_msg, peer->status), bgp_type_str[(int) type], offset, size + BGP_HEADER_SIZE,peer->local_sport,peer->local_dport,peer->send_sport,peer->send_dport );
        //}
		
        /* Read rest of the packet and call each sort of packet routine */
        switch (type)
        {
	        case BGP_MSG_OPEN:
	            peer->open_in++;
	            bgp_open_receive (peer, size); /* XXX return value ignored! */
	            break;
	        case BGP_MSG_UPDATE:
	            peer->readtime = bgp_recent_clock ();
	            bgp_update_receive (peer, size);
	            break;
	        case BGP_MSG_NOTIFY:
	            bgp_notify_receive (peer, size);
	            break;
	        case BGP_MSG_KEEPALIVE:
	            peer->readtime = bgp_recent_clock ();
	            bgp_keepalive_receive (peer, size);
	            break;
	        case BGP_MSG_ROUTE_REFRESH_NEW:
	        case BGP_MSG_ROUTE_REFRESH_OLD:
	            peer->refresh_in++;
	            bgp_route_refresh_receive (peer, size);
	            break;
	        case BGP_MSG_CAPABILITY:
	            peer->dynamic_cap_in++;
	            bgp_capability_receive (peer, size);
	            break;
        }
		
        /* Clear input buffer. */
        if (peer->ibuf)
        {
            stream_reset (peer->ibuf);
        }
		
    }
    while(offset < peer->packet_size);
	
    peer->packet_size = 0;
    stream_reset(peer->ibuf_all);
	
    return 0;
}


int bgp_pkt_rcv(struct ipc_mesg_n *pmsg)
{
    struct pkt_buffer *pkt = NULL;

	 pkt = pkt_rcv_n(pmsg);
     //pkt =  (struct pkt_buffer *) (pmsg->msg_data);
     
     if( pkt == NULL
	||( pkt->cb.ipcb.pkt_type == PKT_TYPE_IP && pkt->cb.ipcb.sip == 0 )
	||( pkt->cb.ipcb.pkt_type == PKT_TYPE_IP && pkt->cb.ipcb.dip == 0 )
	||( pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6 )
	//||( pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6 && ipv6_is_zero(&pkt->cb.ipcb.sipv6))
	//||( pkt->cb.ipcb.pkt_type == PKT_TYPE_IPV6 && ipv6_is_zero(&pkt->cb.ipcb.dipv6))
    ||( pkt->cb.ipcb.pkt_type != PKT_TYPE_IP && pkt->cb.ipcb.pkt_type != PKT_TYPE_IPV6)
	|| pkt->cb.ipcb.sport == 0
	|| pkt->cb.ipcb.dport == 0 
	|| pkt->data == NULL 
	|| pkt->data_len > BGP_MAX_PACKET_SIZE 
	|| pkt->data_len == 0)
    {
        return -1;
    }
	
    bgp_read_pkt(pkt); 
	   
#if 0	
out:
    usleep(1000);
    thread_add_event (bm->master, bgp_pkt_rcv, NULL, 0);
#endif

    return 0;
}
