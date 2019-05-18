/* $OpenBSD: kexdhs.c,v 1.9 2006/11/06 21:25:28 markus Exp $ */
/*
 * Copyright (c) 2001 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include "includes.h"

#include <sys/types.h>

#include <stdarg.h>
#include <string.h>
#include <signal.h>

#include "xmalloc.h"
#include "buffer_ssh.h"
#include "key.h"
#include "cipher.h"
#include "kex.h"
#include "log.h"
#include "packet.h"
#include "dh.h"
#include "ssh2.h"
#ifdef GSSAPI
#include "ssh-gss.h"
#endif
#include "ssh_version.h"

//#include "monitor_wrap.h"

void
kexdh_server(Kex *kex, struct ssh_packet *ssh_packet)
{
	BIGNUM *shared_secret = NULL, *dh_client_pub = NULL;
	DH *dh;
	Key *server_host_key;
	u_char *kbuf, *hash, *signature = NULL, *server_host_key_blob = NULL;
	u_int sbloblen, klen, hashlen, slen;
	int kout;

zlog_debug(VD_SSH, "kexdh_server\n");

	/* generate server DH public key */
	switch (kex->kex_type) {
	case KEX_DH_GRP1_SHA1:
		dh = dh_new_group1();
		break;
	case KEX_DH_GRP14_SHA1:
		/* server����P(������)��G(���뷢����) */
		dh = dh_new_group14();
		break;
	default:
		fatal("%s: Unexpected KEX type %d", __func__, kex->kex_type);
	}
	dh_gen_key(dh, kex->we_need * 8);

	zlog_debug(VD_SSH, "expecting SSH2_MSG_KEXDH_INIT");
	/*����packet_read_seqnr�Ƚ�P��G���͸�client��������client�ظ�
	 *������e SSH2_MSG_KEXDH_INIT��Ϣ*/
	packet_read_expect(SSH2_MSG_KEXDH_INIT, ssh_packet);

	if (kex->load_host_key == NULL)
		fatal("Cannot load hostkey");
	server_host_key = kex->load_host_key(kex->hostkey_type);
	if (server_host_key == NULL)
		fatal("Unsupported hostkey type %d", kex->hostkey_type);

	/* key, cert */
	if ((dh_client_pub = BN_new()) == NULL)
		fatal("dh_client_pub == NULL");
	packet_get_bignum2(dh_client_pub, ssh_packet);
	packet_check_eom(ssh_packet);

#ifdef DEBUG_KEXDH
	fprintf(stderr, "dh_client_pub= ");
	BN_print_fp(stderr, dh_client_pub);
	fprintf(stderr, "\n");
	zlog_debug(VD_SSH, "bits %d", BN_num_bits(dh_client_pub));
#endif

#ifdef DEBUG_KEXDH
	DHparams_print_fp(stderr, dh);
	fprintf(stderr, "pub= ");
	BN_print_fp(stderr, dh->pub_key);
	fprintf(stderr, "\n");
#endif
	if (!dh_pub_is_valid(dh, dh_client_pub))
		packet_disconnect(ssh_packet, "bad client public DH value");

	klen = DH_size(dh);
	kbuf = xmalloc(klen);
	if ((kout = DH_compute_key(kbuf, dh_client_pub, dh)) < 0)
		fatal("DH_compute_key: failed");
#ifdef DEBUG_KEXDH
	dump_digest("shared secret", kbuf, kout);
#endif
	if ((shared_secret = BN_new()) == NULL)
		fatal("kexdh_server: BN_new failed");
	if (BN_bin2bn(kbuf, kout, shared_secret) == NULL)
		fatal("kexdh_server: BN_bin2bn failed");
	memset(kbuf, 0, klen);
	xfree(kbuf);

	key_to_blob(server_host_key, &server_host_key_blob, &sbloblen);

	/* calc H */
	kex_dh_hash(
	    kex->client_version_string,
	    kex->server_version_string,
	    buffer_ptr(&kex->peer), buffer_len(&kex->peer),
	    buffer_ptr(&kex->my), buffer_len(&kex->my),
	    server_host_key_blob, sbloblen,
	    dh_client_pub,
	    dh->pub_key,
	    shared_secret,
	    &hash, &hashlen
	);
	BN_clear_free(dh_client_pub);

	/* save session id := H */
	if (kex->session_id == NULL) {
		kex->session_id_len = hashlen;
		kex->session_id = xmalloc(kex->session_id_len);
		memcpy(kex->session_id, hash, kex->session_id_len);
	}

	/* sign H */
	key_sign(server_host_key, &signature, &slen, hash, hashlen);

	/* destroy_sensitive_data(); */

	/* send server hostkey, DH pubkey 'f' and singed H */
	packet_start(SSH2_MSG_KEXDH_REPLY, ssh_packet);
	packet_put_string(server_host_key_blob, sbloblen, ssh_packet);
	packet_put_bignum2(dh->pub_key, ssh_packet);	/* f */
	packet_put_string(signature, slen, ssh_packet);
	packet_send(ssh_packet);

	xfree(signature);
	xfree(server_host_key_blob);
	/* have keys, free DH */
	DH_free(dh);

	kex_derive_keys(kex, hash, hashlen, shared_secret, ssh_packet);
	BN_clear_free(shared_secret);
	kex_finish(kex, ssh_packet);
}
