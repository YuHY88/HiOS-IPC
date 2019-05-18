/*
 * Checksum routine for Internet Protocol family headers (C Version).
 *
 * Refer to "Computing the Internet Checksum" by R. Braden, D. Borman and
 * C. Partridge, Computer Communication Review, Vol. 19, No. 2, April 1989,
 * pp. 86-101, for additional details on computing this checksum.
 */

#ifdef HAVE_NETINET6_IN_H
#include <netinet6/in.h>
#endif /* HAVE_NETINET6_IN_H */

#include <lib/types.h>
#include <lib/inet_ip.h>
#include "checksum.h"


/* return checksum in 16 bits */
uint16_t in_cksum(void *parg, int nbytes)
{
	u_short *ptr = parg;
	register long	  sum;		/* assumes long == 32 bits */
	u_short			  oddbyte;
	register u_short  answer;	/* assumes u_short == 16 bits */

	/*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */
	sum = 0;
	while (nbytes > 1)  {
		sum += *ptr++;
		nbytes -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nbytes == 1) 
	{
		oddbyte = 0;							  /* make sure top half is zero */
		*((u_char *) &oddbyte) = *(u_char *)ptr;  /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */
	sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;		        /* ones-complement, then truncate to 16 bits */
	return(answer);
}


/* Fletcher Checksum -- Refer to RFC1008. */
#define MODX                 4102   /* 5802 should be fine */


/* To be consistent, offset is 0-based index, rather than the 1-based 
   index required in the specification ISO 8473, Annex C.1 */
/* calling with offset == FLETCHER_CHECKSUM_VALIDATE will validate the checksum
   without modifying the buffer; a valid checksum returns 0 */
u_int16_t fletcher_checksum(u_char * buffer, const int len, const uint16_t offset)
{
    u_int8_t *p;
    int x, y, c0, c1;
    u_int16_t checksum;
    u_int16_t *csum;
    size_t partial_len, i, left = len;

    checksum = 0;


    if (offset != FLETCHER_CHECKSUM_VALIDATE)
    /* Zero the csum in the packet. */
    {
      assert (offset < (len - 1)); /* account for two bytes of checksum */
      csum = (u_int16_t *) (buffer + offset);
      *(csum) = 0;
    }

    p = buffer;
    c0 = 0;
    c1 = 0;

    while (left != 0)
    {
        partial_len = MIN(left, MODX);

        for (i = 0; i < partial_len; i++)
        {
            c0 = c0 + *(p++);
            c1 += c0;
        }

        c0 = c0 % 255;
        c1 = c1 % 255;

        left -= partial_len;
    }

    /* The cast is important, to ensure the mod is taken as a signed value. */
    x = (int)((len - offset - 1) * c0 - c1) % 255;

    if (x <= 0)
        x += 255;
    y = 510 - c0 - x;
    if (y > 255)  
        y -= 255;

    if (offset == FLETCHER_CHECKSUM_VALIDATE)
    {
        checksum = (c1 << 8) + c0;
    }
    else
    {
        /*
        * Now we write this to the packet.
        * We could skip this step too, since the checksum returned would
        * be stored into the checksum field by the caller.
        */
        buffer[offset] = x;
        buffer[offset + 1] = y;

        /* Take care of the endian issue */
        checksum = htons((x << 8) | (y & 0xFF));
    }

    return checksum;
}


uint16_t in_checksum (uint16_t *ptr, int nbytes)
{
	register long sum;
	u_int16_t oddbyte = 0;
	register u_int16_t result;
	u_int8_t *p, *q;

	sum = 0;
	while (nbytes > 1)  
	{
        sum += *ptr++;
        nbytes -= 2;
	}

	if (nbytes == 1) 
	{
        p = (u_int8_t *)&oddbyte;
        q = (u_int8_t *)ptr;
        *p = *q;
        sum += oddbyte;
	}
	
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	result = ~sum;

	return result;
}


#ifdef HAVE_IPV6
uint32_t  in6_checksum (struct prefix *psrc, struct prefix *pdst, 
								uint8_t nxt_hdr, uint16_t *ptr, size_t nbytes)
{
    struct in6_pseudo_hdr in6_ph;
    u_int16_t result;
    int sum;
    u_int16_t tmp_size;
    u_int16_t *tmp_p;
    u_int8_t *p, *q; 
    u_int16_t oddbyte = 0;

    sum = 0;

    /* Prepare IPv6 Pseudo-Header */
    memset (&in6_ph, 0, sizeof (struct in6_pseudo_hdr));
    IPV6_ADDR_COPY (&in6_ph.in6_src, &psrc->u.prefix6);
    IPV6_ADDR_COPY (&in6_ph.in6_dst, &pdst->u.prefix6);
    in6_ph.length = htonl (nbytes);
    in6_ph.nxt_hdr = nxt_hdr;

    /* First checksum IPv6 Pseudo-Header */
    tmp_size = sizeof (struct in6_pseudo_hdr);
    tmp_p = (u_int16_t *) &in6_ph;
  
    while (tmp_size)
    {      
        sum += (*tmp_p++);
        tmp_size -= sizeof (u_int16_t);
    }
  

    /* Next checksum IPv6 PDU */
    tmp_size = nbytes;
    tmp_p = ptr;
    while (tmp_size > 1)
    {
        sum += *tmp_p++;
        tmp_size -= sizeof (u_int16_t);
    }

    if (tmp_size == 1)
    {
        p = (u_int8_t *)&oddbyte;
        q = (u_int8_t *)tmp_p;
        *p = *q;
        sum += oddbyte;
    }

    sum  = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    result = ~sum;

    return result;
}
#endif /* HAVE_IPV6 */

