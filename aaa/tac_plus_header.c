/* header.c - Create pre-filled header for TACACS+ request.
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

//#include <arpa/inet.h>
#include "lib/types.h"
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#if defined(HAVE_OPENSSL_RAND_H) && defined(HAVE_LIBCRYPTO)
# include <openssl/rand.h>
#elif defined(HAVE_GETRANDOM)
# if defined(HAVE_LINUX_RANDOM_H)
#  include <linux/random.h>
# elif defined(HAVE_SYS_RANDOM_H)
#  include <sys/random.h>
# endif
#else
//# include "magic.h"
extern u_int32_t magic(void);
#endif

#include "aaa_tac_plus.h"
#include "xalloc.h"
#include "tacplus.h"


/* Miscellaneous variables that are global, because we need
 * store their values between different functions and connections.
 */

/* Encryption flag. */
int tac_encryption = 1;


/* Returns pre-filled TACACS+ packet header of given type.
 * 1. you MUST fill th->datalength and th->version
 * 2. you MAY fill th->encryption
 * 3. you are responsible for freeing allocated header 
 * By default packet encryption is enabled. The version
 * field depends on the TACACS+ request type and thus it
 * cannot be predefined.
 */
int _tac_req_header(char type, int cont_session, TAC_HDR *th)
{
	int session_id = 0;

    if(NULL == th) return AAA_FAIL;
	//if(id > TAC_PLUS_SESS_MAX || id < 0) return AAA_ERROR;

    /* preset some packet options in header */
	th->version = TAC_PLUS_VER_0; /* default */
    th->type = type;
    th->seq_no = 1; /* always 1 for request */
    th->encryption = TAC_PLUS_ENCRYPTED_FLAG;
 
    /* make g_tac_session_id from pseudo-random number */
    if (!cont_session)
	{
#if defined(HAVE_OPENSSL_RAND_H) && defined(HAVE_LIBCRYPTO)
    	// the preferred way is to use OpenSSL abstraction as we are linking it anyway for MD5
        RAND_pseudo_bytes((char *) &session_id, sizeof(session_id));
#elif defined(HAVE_GETRANDOM)
        // experimental
        getrandom((void *) &session_id, sizeof(session_id), GRND_NONBLOCK);
#else
        // if everything fails use the legacy code
        session_id = magic();
#endif
		//g_tac_session[id].session_id = session_id;
    }
    th->session_id = htonl(session_id);

    return AAA_OK;
}

/* Checks given reply header for possible inconsistencies:
 *  1. reply type other than expected
 *  2. sequence number other than 2 or 4
 *  3. session_id different from one sent in request
 * Returns pointer to error message
 * or NULL when the header seems to be correct
 */
char *_tac_check_header(TAC_HDR *th)
{
    /*if(th->type != type)
	{
        zlog_err("%s: unrelated reply, type %d, expected %d\n",\
            __FUNCTION__, th->type, type);
        return PROTOCOL_ERR_MSG;
    }
	else */if (1 == (th->seq_no % 2))
	{
        zlog_err("%s: not a reply - seq_no %d not even\n",\
            __FUNCTION__, th->seq_no);
        return NULL;
    }
    
    return NULL; /* header is ok */    
} /* check header */

