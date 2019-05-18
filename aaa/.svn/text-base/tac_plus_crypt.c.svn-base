/* crypt.c - TACACS+ encryption related functions
 *
 * Copyright (C) 2010, Pawel Krawczyk <pawel.krawczyk@hush.com> and
 * Jeroen Nijhof <jeroen@jeroennijhof.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program - see the file COPYING.
 *
 * See `CHANGES' file for revision history.
 */

#include <string.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "aaa_tac_plus.h"
#include "xalloc.h"
#include "tacplus.h"

#include "aaa_config.h"

#if defined(HAVE_OPENSSL_MD5_H) && defined(HAVE_LIBCRYPTO)
# include <openssl/md5.h>
#else
#include "md5.h"
#endif

#define tac_secret g_aaa_cfgs.tac_plus.authen_key


/* Produce MD5 pseudo-random pad for TACACS+ encryption.
   Use data from packet header and secret, which
   should be a global variable */
char *_tac_md5_pad(int len, TAC_HDR *hdr)
{
    int n, i, bufsize;
    int bp = 0; /* buffer pointer */
    int pp = 0; /* pad pointer */
    char *pad;
    char *buf;
    MD5_CTX mdcontext;

    /* make pseudo pad */
    n = (int)(len / 16) + 1;  /* number of MD5 runs */
    bufsize = sizeof(hdr->session_id) + strlen(tac_secret) + sizeof(hdr->version)
        + sizeof(hdr->seq_no) + MD5_LBLOCK + 10;
    buf = (char *) xcalloc(1, bufsize);
    pad = (char *) xcalloc(n, MD5_LBLOCK);

    for (i = 0; i < n; i++)
	{
        /* MD5_1 = MD5{session_id, secret, version, seq_no}
           MD5_2 = MD5{session_id, secret, version, seq_no, MD5_1} */

        /* place session_id, key, version and seq_no in buffer */
        bp = 0;
        bcopy(&hdr->session_id, buf, sizeof(hdr->session_id));
        bp += sizeof(hdr->session_id);
        bcopy(tac_secret, buf+bp, strlen(tac_secret));
        bp += strlen(tac_secret);
        bcopy(&hdr->version, buf+bp, sizeof(hdr->version));
        bp += sizeof(hdr->version);
        bcopy(&hdr->seq_no, buf+bp, sizeof(hdr->seq_no));
        bp += sizeof(hdr->seq_no);

        /* append previous pad if this is not the first run */
        if (i)
		{
            bcopy(pad+((i-1)*MD5_LBLOCK), buf+bp, MD5_LBLOCK);
            bp += MD5_LBLOCK;
        }

#if defined(HAVE_OPENSSL_MD5_H) && defined(HAVE_LIBCRYPTO)
        MD5_Init(&mdcontext);
        MD5_Update(&mdcontext, buf, bp);
        MD5_Final(pad+pp, &mdcontext);
#else
		MD5Init(&mdcontext);
		MD5Update(&mdcontext, buf, bp);
		MD5Final((uint8_t *)(pad + pp), &mdcontext);
#endif
        pp += MD5_LBLOCK;
    }

    free(buf);
    return pad;

}    /* _tac_md5_pad */

/* Perform encryption/decryption on buffer. This means simply XORing
   each byte from buffer with according byte from pseudo-random pad. */
void _tac_crypt(char *buf, TAC_HDR *th, int length)
{
    int i;
    char *pad;

    /* null operation if no encryption requested */
    if((tac_secret[0] != 0) && (th->encryption == TAC_PLUS_ENCRYPTED_FLAG))
	{
        pad = _tac_md5_pad(length, th);

        for (i=0; i<length; i++)
		{
            *(buf+i) ^= pad[i];
        }

        free(pad);
    }
	else
	{
        zlog_debug(AAA_DBG_TACACS, "%s: using no TACACS+ encryption\n", __FUNCTION__);
    }
}    /* _tac_crypt */

