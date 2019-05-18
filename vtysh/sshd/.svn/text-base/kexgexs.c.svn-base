/* $OpenBSD: kexgexs.c,v 1.11 2009/01/01 21:17:36 djm Exp $ */
/*
 * Copyright (c) 2000 Niels Provos.  All rights reserved.
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

#include <sys/param.h>

#include <stdarg.h>
#include <stdio.h>
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
//#include "compat.h"
#ifdef GSSAPI
#include "ssh-gss.h"
#endif
//#include "monitor_wrap.h"
#include "ssh_version.h"

void
kexgex_server(Kex *kex, struct ssh_packet *ssh_packet)
{
	BIGNUM *shared_secret = NULL, *dh_client_pub = NULL;
	Key *server_host_key;
	DH *dh;
	u_char *kbuf, *hash, *signature = NULL, *server_host_key_blob = NULL;
	u_int sbloblen, klen, slen, hashlen;
	int omin = -1, min = -1, omax = -1, max = -1, onbits = -1, nbits = -1;
	int type, kout;

	zlog_debug(VD_SSH, "kexgex_server\n");
	if (kex->load_host_key == NULL)
		fatal("Cannot load hostkey");
	server_host_key = kex->load_host_key(kex->hostkey_type);
	if (server_host_key == NULL)
		fatal("Unsupported hostkey type %d", kex->hostkey_type);

	/*等待并接收client发送的SSH2_MSG_KEX_DH_GEX_REQUEST，包含DH GEX:min,max,numbers of bits*/
	type = packet_read(ssh_packet);
	switch (type) {
	case SSH2_MSG_KEX_DH_GEX_REQUEST:
		zlog_debug(VD_SSH, "SSH2_MSG_KEX_DH_GEX_REQUEST received");
		omin = min = packet_get_int(ssh_packet);
		onbits = nbits = packet_get_int(ssh_packet);
		omax = max = packet_get_int(ssh_packet);
		min = MAX(DH_GRP_MIN, min);
		max = MIN(DH_GRP_MAX, max);
		nbits = MAX(DH_GRP_MIN, nbits);
		nbits = MIN(DH_GRP_MAX, nbits);
		break;
	case SSH2_MSG_KEX_DH_GEX_REQUEST_OLD:
		zlog_debug(VD_SSH, "SSH2_MSG_KEX_DH_GEX_REQUEST_OLD received");
		onbits = nbits = packet_get_int(ssh_packet);
		/* unused for old GEX */
		omin = min = DH_GRP_MIN;
		omax = max = DH_GRP_MAX;
		break;
	default:
		
		/* lipf modity, to avoid exitting process */
		
		//fatal("protocol error during kex, no DH_GEX_REQUEST: %d", type);
		packet_disconnect(ssh_packet, "protocol error during kex, no DH_GEX_REQUEST");
		return;
	}
	packet_check_eom(ssh_packet);

	if (omax < omin || onbits < omin || omax < onbits)
		fatal("DH_GEX_REQUEST, bad parameters: %d !< %d !< %d",
		    omin, onbits, omax);
	/* Contact privileged parent */
	dh = choose_dh(min, nbits, max);
	if (dh == NULL)
		{
		packet_disconnect(ssh_packet, "Protocol error: no matching DH grp found");
		}
	
	zlog_debug(VD_SSH, "SSH2_MSG_KEX_DH_GEX_GROUP sent");
	/*SSH2_MSG_KEX_DH_GEX_GROUP,发送g和p*/
	packet_start(SSH2_MSG_KEX_DH_GEX_GROUP, ssh_packet);
	packet_put_bignum2(dh->p, ssh_packet);
	packet_put_bignum2(dh->g, ssh_packet);
	packet_send(ssh_packet);

	/* flush */
//	packet_write_wait();
	packet_write_poll_ipc(ssh_packet);


	/* Compute our exchange value in parallel with the client */
	/* server生成私钥x，同时client会生成私钥y */
	dh_gen_key(dh, kex->we_need * 8);

	zlog_debug(VD_SSH, "expecting SSH2_MSG_KEX_DH_GEX_INIT");
	/* 等待client答复SSH2_MSG_KEX_DH_GEX_INIT，client的公钥e */
	packet_read_expect(SSH2_MSG_KEX_DH_GEX_INIT, ssh_packet);

	/* key, cert */
	if ((dh_client_pub = BN_new()) == NULL)
		fatal("dh_client_pub == NULL");
	/* 获取client的公钥e */
	packet_get_bignum2(dh_client_pub, ssh_packet);
	packet_check_eom(ssh_packet);

#ifdef DEBUG_KEXDH
	fprintf(stderr, "dh_client_pub= ");
	BN_print_fp(stderr, dh_client_pub);
	fprintf(stderr, "\n");
	printf("bits %d", BN_num_bits(dh_client_pub));
#endif

#ifdef DEBUG_KEXDH
	DHparams_print_fp(stderr, dh);
	fprintf(stderr, "pub= ");
	BN_print_fp(stderr, dh->pub_key);
	fprintf(stderr, "\n");
#endif
	if (!dh_pub_is_valid(dh, dh_client_pub))
		{
		packet_disconnect(ssh_packet, "bad client public DH value");
		}

	klen = DH_size(dh);
	kbuf = xmalloc(klen);
	//根据对方公钥和己方DH 密钥来生成共享密钥的函数，结果保存在kbuf中
	if ((kout = DH_compute_key(kbuf, dh_client_pub, dh)) < 0)
		{
		fatal("DH_compute_key: failed");
		}
#ifdef DEBUG_KEXDH
	dump_digest("shared secret", kbuf, kout);
#endif
	if ((shared_secret = BN_new()) == NULL)
		fatal("kexgex_server: BN_new failed");
	/*转换为big nums，shared_secret现在存放秘钥*/
	if (BN_bin2bn(kbuf, kout, shared_secret) == NULL)
		fatal("kexgex_server: BN_bin2bn failed");
	memset(kbuf, 0, klen);
	xfree(kbuf);

	/*生成RSA公钥: n 、e*/
	key_to_blob(server_host_key, &server_host_key_blob, &sbloblen);

	if (type == SSH2_MSG_KEX_DH_GEX_REQUEST_OLD)
		omin = min = omax = max = -1;

	/* calc H */
	/* 使用sha256散列算法计算hash 
	 * H=hash(V_C||V_S||I_C||I_S||K_S||e||f||K);
	 * V_C:客户端的初始报文;I_C:客户端 SSH_MSG_KEX_INIT的有效载荷
	 * K_S:server生成的RSA主机秘钥*/
	kexgex_hash(
	    kex->evp_md,
	    kex->client_version_string,
	    kex->server_version_string,
	    buffer_ptr(&kex->peer), buffer_len(&kex->peer),
	    buffer_ptr(&kex->my), buffer_len(&kex->my),
	    server_host_key_blob, sbloblen,
	    omin, onbits, omax,
	    dh->p, dh->g,
	    dh_client_pub,
	    dh->pub_key,
	    shared_secret,
	    &hash, &hashlen
	);
	BN_clear_free(dh_client_pub);

	/* save session id := H */
	/* session id为第一次秘钥交换计算出来的H */
	if (kex->session_id == NULL) {
		kex->session_id_len = hashlen;
		kex->session_id = xmalloc(kex->session_id_len);
		memcpy(kex->session_id, hash, kex->session_id_len);
	}

	/* sign H */
	/* server 用主机rsa私钥对H进行加密签名，client用公钥解密 */
	key_sign(server_host_key, &signature, &slen, hash, hashlen);

	/* destroy_sensitive_data(); */

	/* send server hostkey, DH pubkey 'f' and singed H */
	zlog_debug(VD_SSH, "SSH2_MSG_KEX_DH_GEX_REPLY sent");
	/* 答复 server dh公钥f; 主机rsa公钥; 使用rsa私钥签名后的H */
	packet_start(SSH2_MSG_KEX_DH_GEX_REPLY, ssh_packet);
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


