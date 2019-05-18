/*
 *  $Id: libnet_link_pf.c,v 1.1.1.1 2004/04/05 10:15:04 gkong Exp $
 *
 *  libnet
 *  libnet_pf.c - pf routines
 *
 *  Copyright (c) 1998 - 2001 Mike D. Schiffman <mike@infonexus.com>
 *  All rights reserved.
 *
 * Copyright (c) 1990, 1991, 1992, 1993, 1994, 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * packet filter subroutines for tcpdump
 *	Extraction/creation by Jeffrey Mogul, DECWRL
 */

#if (HAVE_CONFIG_H)
#include "config.h"
#endif
//#include "low_libnet.h"
#include <netinet/if_ether.h>
#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#if 1	/*add by zhangjj 2016-4-21*/
#include "enet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libnet-structures.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/*
 *  Bits in mode word modified by EIOCMBIS and EIOCMBIC.
 */
//#define	ENHOLDSIG	0x0001	/* don't disable signal after sending */
#define	ENBATCH		0x0002	/* group as many packets into single read */
#define	ENTSTAMP	0x0004	/* prepend packet with a header containing
				   time stamp and other stuff */
#define	ENPROMISC	0x0008	/* accept promiscuously received packets */
#define	ENNONEXCL	0x0010	/* non-exclusive reception */
#define	ENCOPYALL	0x0020	/* hear all packets addressed to this host */
#define	ENBPFHDR	0x0040	/* like ENTSTAMP but use BPF header format */
#define DLT_EN10MB 1 /* Ethernet (10Mb)*/

/*
 * Data-link level type codes.
 */
#define DLT_NULL	0	/* no link-layer encapsulation */
#define DLT_EN10MB	1	/* Ethernet (10Mb) */
#define DLT_EN3MB	2	/* Experimental Ethernet (3Mb) */
#define DLT_AX25	3	/* Amateur Radio AX.25 */
#define DLT_PRONET	4	/* Proteon ProNET Token Ring */
#define DLT_CHAOS	5	/* Chaos */
#define DLT_IEEE802	6	/* IEEE 802 Networks */
#define DLT_ARCNET	7	/* ARCNET */
#define DLT_SLIP	8	/* Serial Line IP */
#define DLT_PPP		9	/* Point-to-point Protocol */
#define DLT_FDDI	10	/* FDDI */
#define DLT_ATM_RFC1483	11	/* LLC/SNAP encapsulated atm */
#define DLT_RAW		12	/* raw IP */
#define DLT_APPLE_IP_OVER_IEEE1394      138

#endif

struct libnet_link_int *
libnet_open_link_interface(char *device, char *ebuf)
{
    register struct libnet_link_int *l;
    short enmode;
    int backlog = -1;   /* request the most */
    struct enfilter Filter;
    struct endevp devparams;

    l = (struct libnet_link_int *)malloc(sizeof(*l));
    if (l == NULL)
    {
        sprintf(ebuf, "libnet_open_link_int: %s", ll_strerror(errno));
        return (0);
    }
    memset(l, 0, sizeof(*l));
    l->fd = pfopen(device, O_RDWR);
    if (l->fd < 0)
    {
        sprintf(ebuf, "pf open: %s: %s\n\
your system may not be properly configured; see \"man packetfilter(4)\"\n",
            device, ll_strerror(errno));
        goto bad;
    }

    enmode = ENTSTAMP|ENBATCH|ENNONEXCL;
    if (ioctl(l->fd, EIOCMBIS, (caddr_t)&enmode) < 0)
    {
        sprintf(ebuf, "EIOCMBIS: %s", ll_strerror(errno));
        goto bad;
    }
#ifdef	ENCOPYALL
    /* Try to set COPYALL mode so that we see packets to ourself */
    enmode = ENCOPYALL;
    ioctl(l->fd, EIOCMBIS, (caddr_t)&enmode);   /* OK if this fails */
#endif
	/* set the backlog */
    if (ioctl(l->fd, EIOCSETW, (caddr_t)&backlog) < 0)
    {
        sprintf(ebuf, "EIOCSETW: %s", ll_strerror(errno));
        goto bad;
    }
    /*
     *  discover interface type
     */
    if (ioctl(l->fd, EIOCDEVP, (caddr_t)&devparams) < 0)
    {
        sprintf(ebuf, "EIOCDEVP: %s", ll_strerror(errno));
        goto bad;
    }

    /* HACK: to compile prior to Ultrix 4.2 */
#ifndef	ENDT_FDDI
#define	ENDT_FDDI   4
#endif
    switch (devparams.end_dev_type)
    {
        case ENDT_10MB:
            l->linktype = DLT_EN10MB;
            break;
        case ENDT_FDDI:
            l->linktype = DLT_FDDI;
            break;
        default:
            /*
             * XXX
             * Currently, the Ultrix packet filter supports only
             * Ethernet and FDDI.  Eventually, support for SLIP and PPP
             * (and possibly others: T1?) should be added.
             */
            l->linktype = DLT_EN10MB;
            break;
	}
    /*
     *  accept all packets
     */
    bzero((char *)&Filter, sizeof(Filter));
    Filter.enf_Priority = 37;	/* anything > 2 */
    Filter.enf_FilterLen = 0;	/* means "always true" */
    if (ioctl(l->fd, EIOCSETF, (caddr_t)&Filter) < 0)
    {
        sprintf(ebuf, "EIOCSETF: %s", ll_strerror(errno));
        goto bad;
    }

    return (l);
bad:
    free(l);
    return (NULL);
}


int
libnet_close_link_interface(struct libnet_link_int *l)
{
    if (close(l->fd) == 0)
    {
        return (1);
    }
    else
    {
        return (-1);
    }
}


int
libnet_write_link_layer(struct libnet_link_int *l, const char *device,
            const u_char *buf, int len)
{
    int c;

    c = write(l->fd, buf, len);
    if (c != len)
    {
#if (__DEBUG)
        fprintf(stderr, "libnet_write_link_layer: %d bytes written (%s)\n", c,
            strerror(errno));
#endif
    }
    return (c);
}