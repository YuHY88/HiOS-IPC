/* setsockopt functions
 * Copyright (C) 1999 Kunihiro Ishiguro
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

#include <zebra.h>
#include "log.h"
#include "sockopt.h"
#include "sockunion.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>



int
setsockopt_so_recvbuf (int sock, int size)
{
  int ret;
  
  if ( (ret = setsockopt (sock, SOL_SOCKET, SO_RCVBUF, (char *)
                          &size, sizeof (int))) < 0)
    zlog_err ("fd %d: can't setsockopt SO_RCVBUF to %d: %s",
	      sock,size,safe_strerror(errno));

  return ret;
}

int
setsockopt_so_sendbuf (const int sock, int size)
{
  int ret = setsockopt (sock, SOL_SOCKET, SO_SNDBUF,
    (char *)&size, sizeof (int));
  
  if (ret < 0)
    zlog_err ("fd %d: can't setsockopt SO_SNDBUF to %d: %s",
      sock, size, safe_strerror (errno));

  return ret;
}

int
getsockopt_so_sendbuf (const int sock)
{
  u_int32_t optval;
  socklen_t optlen = sizeof (optval);
  int ret = getsockopt (sock, SOL_SOCKET, SO_SNDBUF,
    (char *)&optval, &optlen);
  if (ret < 0)
  {
    zlog_err ("fd %d: can't getsockopt SO_SNDBUF: %d (%s)",
      sock, errno, safe_strerror (errno));
    return ret;
  }
  return optval;
}

//static void *
//getsockopt_cmsg_data (struct msghdr *msgh, int level, int type)
//{
//  struct cmsghdr *cmsg;
//  void *ptr = NULL;
//  
//  for (cmsg = ZCMSG_FIRSTHDR(msgh); 
//       cmsg != NULL;
//       cmsg = CMSG_NXTHDR(msgh, cmsg))
//    if (cmsg->cmsg_level == level && cmsg->cmsg_type)
//      return (ptr = CMSG_DATA(cmsg));

//  return NULL;
//}

#ifdef HAVE_IPV6
/* Set IPv6 packet info to the socket. */
int
setsockopt_ipv6_pktinfo (int sock, int val)
{
  int ret;
    
#ifdef IPV6_RECVPKTINFO		/*2292bis-01*/
  ret = setsockopt(sock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &val, sizeof(val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_RECVPKTINFO : %s", safe_strerror (errno));
#else	/*RFC2292*/
  ret = setsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, &val, sizeof(val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_PKTINFO : %s", safe_strerror (errno));
#endif /* INIA_IPV6 */
  return ret;
}

/* Set multicast hops val to the socket. */
int
setsockopt_ipv6_checksum (int sock, int val)
{
  int ret;

#ifdef GNU_LINUX
  ret = setsockopt(sock, IPPROTO_RAW, IPV6_CHECKSUM, &val, sizeof(val));
#else
  ret = setsockopt(sock, IPPROTO_IPV6, IPV6_CHECKSUM, &val, sizeof(val));
#endif /* GNU_LINUX */
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_CHECKSUM");
  return ret;
}

/* Set multicast hops val to the socket. */
int
setsockopt_ipv6_multicast_hops (int sock, int val)
{
  int ret;

  ret = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &val, sizeof(val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_MULTICAST_HOPS");
  return ret;
}

/* Set multicast hops val to the socket. */
int
setsockopt_ipv6_unicast_hops (int sock, int val)
{
  int ret;

  ret = setsockopt(sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &val, sizeof(val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_UNICAST_HOPS");
  return ret;
}

int
setsockopt_ipv6_hoplimit (int sock, int val)
{
  int ret;

#ifdef IPV6_RECVHOPLIMIT	/*2292bis-01*/
  ret = setsockopt (sock, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &val, sizeof(val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_RECVHOPLIMIT");
#else	/*RFC2292*/
  ret = setsockopt (sock, IPPROTO_IPV6, IPV6_HOPLIMIT, &val, sizeof(val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_HOPLIMIT");
#endif
  return ret;
}

/* Set multicast loop zero to the socket. */
int
setsockopt_ipv6_multicast_loop (int sock, int val)
{
  int ret;
    
  ret = setsockopt (sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &val,
		    sizeof (val));
  if (ret < 0)
    zlog_warn ("can't setsockopt IPV6_MULTICAST_LOOP");
  return ret;
}

//static int
//getsockopt_ipv6_ifindex (struct msghdr *msgh)
//{
//  struct in6_pktinfo *pktinfo;
//  
//  pktinfo = getsockopt_cmsg_data (msgh, IPPROTO_IPV6, IPV6_PKTINFO);
//  
//  return pktinfo->ipi6_ifindex;
//}

int
setsockopt_ipv6_tclass(int sock, int tclass)
{
  int ret = 0;

#ifdef IPV6_TCLASS /* RFC3542 */
  ret = setsockopt (sock, IPPROTO_IPV6, IPV6_TCLASS, &tclass, sizeof (tclass));
  if (ret < 0)
    zlog_warn ("Can't set IPV6_TCLASS option for fd %d to %#x: %s",
	       sock, tclass, safe_strerror(errno));
#endif
  return ret;
}
#endif /* HAVE_IPV6 */


int
setsockopt_ipv4_tos(int sock, int tos)
{
  int ret;

  ret = setsockopt (sock, IPPROTO_IP, IP_TOS, &tos, sizeof (tos));
  if (ret < 0)
    zlog_warn ("Can't set IP_TOS option for fd %d to %#x: %s",
	       sock, tos, safe_strerror(errno));
  return ret;
}


/* swab iph between order system uses for IP_HDRINCL and host order */
void
sockopt_iphdrincl_swab_htosys (struct ip *iph)
{
  /* BSD and derived take iph in network order, except for 
   * ip_len and ip_off
   */
#ifndef HAVE_IP_HDRINCL_BSD_ORDER
  iph->ip_len = htons(iph->ip_len);
  iph->ip_off = htons(iph->ip_off);
#endif /* HAVE_IP_HDRINCL_BSD_ORDER */

  iph->ip_id = htons(iph->ip_id);
}

void
sockopt_iphdrincl_swab_systoh (struct ip *iph)
{
#ifndef HAVE_IP_HDRINCL_BSD_ORDER
  iph->ip_len = ntohs(iph->ip_len);
  iph->ip_off = ntohs(iph->ip_off);
#endif /* HAVE_IP_HDRINCL_BSD_ORDER */

  iph->ip_id = ntohs(iph->ip_id);
}

int
sockopt_tcp_rtt (int sock)
{
#ifdef TCP_INFO
  struct tcp_info ti;
  socklen_t len = sizeof(ti);

  if (getsockopt (sock, IPPROTO_TCP, TCP_INFO, &ti, &len) != 0)
    return 0;

  return ti.tcpi_rtt / 1000;
#else
  return 0;
#endif
}

int
sockopt_tcp_signature (int sock, union sockunion *su, const char *password)
{
#if defined(HAVE_TCP_MD5_LINUX24) && defined(GNU_LINUX)
  /* Support for the old Linux 2.4 TCP-MD5 patch, taken from Hasso Tepper's
   * version of the Quagga patch (based on work by Rick Payne, and Bruce
   * Simpson)
   */
#define TCP_MD5_AUTH 13
#define TCP_MD5_AUTH_ADD 1
#define TCP_MD5_AUTH_DEL 2
  struct tcp_rfc2385_cmd {
    u_int8_t     command;    /* Command - Add/Delete */
    u_int32_t    address;    /* IPV4 address associated */
    u_int8_t     keylen;     /* MD5 Key len (do NOT assume 0 terminated ascii) */
    void         *key;       /* MD5 Key */
  } cmd;
  struct in_addr *addr = &su->sin.sin_addr;
  
  cmd.command = (password != NULL ? TCP_MD5_AUTH_ADD : TCP_MD5_AUTH_DEL);
  cmd.address = addr->s_addr;
  cmd.keylen = (password != NULL ? strlen (password) : 0);
  cmd.key = password;
  
  return setsockopt (sock, IPPROTO_TCP, TCP_MD5_AUTH, &cmd, sizeof cmd);
  
#elif HAVE_DECL_TCP_MD5SIG
  int ret;
#ifndef GNU_LINUX
  /*
   * XXX Need to do PF_KEY operation here to add/remove an SA entry,
   * and add/remove an SP entry for this peer's packet flows also.
   */
  int md5sig = password && *password ? 1 : 0;
#else
  int keylen = password ? strlen (password) : 0;
  struct tcp_md5sig md5sig;
  union sockunion *su2, *susock;
  
  /* Figure out whether the socket and the sockunion are the same family..
   * adding AF_INET to AF_INET6 needs to be v4 mapped, you'd think..
   */
  if (!(susock = sockunion_getsockname (sock)))
    return -1;
  
  if (susock->sa.sa_family == su->sa.sa_family)
    su2 = su;
  else
    {
      /* oops.. */
      su2 = susock;
      
      if (su2->sa.sa_family == AF_INET)
        {
          sockunion_free (susock);
          return 0;
        }
      
#ifdef HAVE_IPV6
      /* If this does not work, then all users of this sockopt will need to
       * differentiate between IPv4 and IPv6, and keep seperate sockets for
       * each. 
       *
       * Sadly, it doesn't seem to work at present. It's unknown whether
       * this is a bug or not.
       */
      if (su2->sa.sa_family == AF_INET6
          && su->sa.sa_family == AF_INET)
        {
           su2->sin6.sin6_family = AF_INET6;
           /* V4Map the address */
           memset (&su2->sin6.sin6_addr, 0, sizeof (struct in6_addr));
           su2->sin6.sin6_addr.s6_addr32[2] = htonl(0xffff);
           memcpy (&su2->sin6.sin6_addr.s6_addr32[3], &su->sin.sin_addr, 4);
        }
#endif
    }
  
  memset (&md5sig, 0, sizeof (md5sig));
  memcpy (&md5sig.tcpm_addr, su2, sizeof (*su2));
  md5sig.tcpm_keylen = keylen;
  if (keylen)
    memcpy (md5sig.tcpm_key, password, keylen);
  sockunion_free (susock);
#endif /* GNU_LINUX */
  if ((ret = setsockopt (sock, IPPROTO_TCP, TCP_MD5SIG, &md5sig, sizeof md5sig)) < 0)
    {
      /* ENOENT is harmless.  It is returned when we clear a password for which
	 one was not previously set. */
      if (ENOENT == errno)
	ret = 0;
      else
	zlog_err ("sockopt_tcp_signature: setsockopt(%d): %s",
		  sock, safe_strerror(errno));
    }
  return ret;
#else /* HAVE_TCP_MD5SIG */
  return -2;
#endif /* !HAVE_TCP_MD5SIG */
}

/*liufy add*/
/* Set up a multicast socket options for IPv4
   This is here so that people only have to do their OS multicast mess 
   in one place rather than all through zebra, ospfd, and ripd 
   NB: This is a hookpoint for specific OS functionality */
int
setsockopt_multicast_ipv4(int sock, 
			int optname, 
			struct in_addr if_addr,
			unsigned int mcast_addr,
			unsigned int ifindex)
	{
	
	  /* Linux 2.2.0 and up */
#if defined(GNU_LINUX) && LINUX_VERSION_CODE > 131584
	  /* This is better because it uses ifindex directly */
	  struct ip_mreqn mreqn;
	  
	  switch (optname)
		{
		case IP_MULTICAST_IF:
		case IP_ADD_MEMBERSHIP:
		case IP_DROP_MEMBERSHIP:
		  memset (&mreqn, 0, sizeof(mreqn));
	
		  if (mcast_addr)
		mreqn.imr_multiaddr.s_addr = mcast_addr;
		  
		  if (ifindex)
		mreqn.imr_ifindex = ifindex;
		  else
		mreqn.imr_address = if_addr;
		  
		  return setsockopt(sock, IPPROTO_IP, optname, (void *)&mreqn, sizeof(mreqn));
		  break;
	
		default:
		  /* Can out and give an understandable error */
		  errno = EINVAL;
		  return -1;
		  break;
		}
	
	  /* Example defines for another OS, boilerplate off other code in this
		 function, AND handle optname as per other sections for consistency !! */
	  /* #elif	defined(BOGON_NIX) && EXAMPLE_VERSION_CODE > -100000 */
	  /* Add your favourite OS here! */
	
#else /* #if OS_TYPE */ 
	  /* default OS support */
	
	  struct in_addr m;
	  struct ip_mreq mreq;
	
	  switch (optname)
		{
		case IP_MULTICAST_IF:
		  m = if_addr;
		  
		  return setsockopt (sock, IPPROTO_IP, optname, (void *)&m, sizeof(m)); 
		  break;
	
		case IP_ADD_MEMBERSHIP:
		case IP_DROP_MEMBERSHIP:
		  memset (&mreq, 0, sizeof(mreq));
		  mreq.imr_multiaddr.s_addr = mcast_addr;
		  mreq.imr_interface = if_addr;
		  
		  return setsockopt (sock, 
				 IPPROTO_IP, 
				 optname, 
				 (void *)&mreq, 
				 sizeof(mreq));
		  break;
		  
		default:
		  /* Can out and give an understandable error */
		  errno = EINVAL;
		  return -1;
		  break;
		}
#endif /* #if OS_TYPE */
	
	}



