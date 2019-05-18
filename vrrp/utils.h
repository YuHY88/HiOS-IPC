/*! @file 
 *  $Source: /home/cvsroot/fw/src/zebra/vrrpd/utils.h,v $
*/

#ifndef _VRRP_COMMON_H
#define _VRRP_COMMON_H

#include <netinet/in.h>

#include "log.h"
#include "zlog.h"


//#define ENABLE_DEBUG 1

#ifdef ENABLE_DEBUG
#define DBG(x) x
#else
#define DBG(x) (void) 0
#endif

#define LOG_PPP(x,y) zlog_ex(LOG_TYPE_PPP, x, y)
#define DBG_PPP(x,y) zdebug_ex(LOG_TYPE_PPP, x, y)

#define LOG_PPPOE(x,y) zlog_ex(LOG_TYPE_PPPOE, x, y)
#define DBG_PPPOE(x,y) zdebug_ex(LOG_TYPE_PPPOE, x, y)


#define ETH_ALEN     6               /* Octets in one ethernet addr   */
#define MAX_NAME_LEN 64
#define MD5LEN 16               /* Length of MD5 hash */
#define MAX_FDS 256
#define SMALLBUF 256


/*
 * Inline versions of get/put char/short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be u_char *.
 */
#define GETCHAR(c, cp) { \
	(c) = *(cp)++; \
}
#define PUTCHAR(c, cp) { \
	*(cp)++ = (u_char) (c); \
}
#define GETSHORT(s, cp) { \
	(s) = *(cp)++ << 8; \
	(s) |= *(cp)++; \
}
#define PUTSHORT(s, cp) { \
	*(cp)++ = (u_char) ((s) >> 8); \
	*(cp)++ = (u_char) (s); \
}
#define GETLONG(l, cp) { \
	(l) = *(cp)++ << 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; \
}
#define PUTLONG(l, cp) { \
	*(cp)++ = (u_char) ((l) >> 24); \
	*(cp)++ = (u_char) ((l) >> 16); \
	*(cp)++ = (u_char) ((l) >> 8); \
	*(cp)++ = (u_char) (l); \
}

/*
 * SET_SA_FAMILY - set the sa_family field of a struct sockaddr,
 * if it exists.
 */

#define SET_SA_FAMILY(addr, family)			\
    memset ((char *) &(addr), '\0', sizeof(addr));	\
    addr.sa_family = (family);


#define SIN_ADDR(x)	(((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)
#define MAX_IFS		100

#define FLAGS_GOOD (IFF_UP          | IFF_BROADCAST)
#define FLAGS_MASK (IFF_UP          | IFF_BROADCAST | \
		    IFF_POINTOPOINT | IFF_LOOPBACK  | IFF_NOARP)

extern int sock_ioctl;

extern int open_ioctl_sock();
extern void close_ioctl_sock();
extern int is_bad_ip_address(char *ip);
extern int is_address(char *address);
extern uint32_t convert2netip (uint32_t ip, int masklen);
extern int getmacbyip(int ioctl_sock, struct in_addr *ip, char *interface, unsigned char *mac);
extern int is_same_net(unsigned long ip1, unsigned long ip2, int masklen);
extern void masklen_to_netip (int masklen, struct in_addr *netmask);
extern void masklen_to_broadip (unsigned long ip, int masklen, unsigned long *broadip);
extern int addr_to_ip(char *ip_addr, unsigned long num, unsigned long net_ip, int masklen);
extern uint32_t ifname_to_ip( char *ifname );
extern int ifname_to_idx( char *ifname );
extern int rcvhwaddr_op( char *ifname, char *addr, int addrlen, int addF );
extern int hwaddr_set( char *ifname, uint16_t hw_type, unsigned char *addr, int addrlen );
extern int hwaddr_get( char *ifname, char *addr, int addrlen );
extern int get_ether_addr (struct sockaddr *hwaddr, char const *ifname);
extern int cifproxyarp (uint32_t ipaddr, char const *ifname);
extern int sifproxyarp (uint32_t ipaddr, char const *ifname);
extern char *ip_ntoa(uint32_t ip);

#endif /*_VRRP_COMMON_H*/
