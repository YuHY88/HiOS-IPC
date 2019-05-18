#ifndef _CHECKSUM_H
#define _CHECKSUM_H


#include <lib/types.h>
#include "lib/prefix.h"


#ifdef HAVE_IPV6
struct in6_pseudo_hdr
{
  struct in6_addr in6_src;
  struct in6_addr in6_dst;
  u_int32_t length;
  u_int8_t zero [3];
  u_int8_t nxt_hdr;
}u __attribute__ ((aligned (8)));

#endif /* HAVE_IPV6 */





uint16_t in_cksum(void *, int);

#define FLETCHER_CHECKSUM_VALIDATE 0xffff
u_int16_t fletcher_checksum(u_char *, const int len, const uint16_t offset);

uint16_t in_checksum (uint16_t *ptr, int nbytes);

extern uint32_t  in6_checksum (struct prefix *, struct prefix *, 
								uint8_t , uint16_t *, size_t );



#endif /* def _CHECKSUM_H */

