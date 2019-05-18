/* $OpenBSD: kex.c,v 1.80 2008/09/06 12:24:13 djm Exp $ */
/*
 * Copyright (c) 2000, 2001 Markus Friedl.  All rights reserved.
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

#include "includes.h"

#include <sys/param.h>

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/crypto.h>

#include "xmalloc.h"
#include "ssh2.h"
//#include "buffer_ssh.h"
#include "vtysh_sshd.h"
#include "packet.h"
//#include "compat.h"
#include "cipher.h"
#include "key.h"
#include "kex.h"
#include "log.h"
#include "mac.h"
#include "match.h"
#include "dispatch.h"
#include <lib/vty.h>
#include "ssh_version.h"
#include <lib/str.h>

//#include "monitor.h"

#define KEX_COOKIE_LEN	16

#if OPENSSL_VERSION_NUMBER >= 0x00907000L
# if defined(HAVE_EVP_SHA256)
# define evp_ssh_sha256 EVP_sha256
# else
extern const EVP_MD *evp_ssh_sha256(void);
# endif
#endif


/* prototype */
static void kex_kexinit_finish(Kex *, struct ssh_packet *ssh_packet);
static void kex_choose_conf(Kex *, struct ssh_packet *ssh_packet);

/* put algorithm proposal into buffer */
static void
kex_prop2buf(Buffer *b, char *proposal[PROPOSAL_MAX])
{
	u_int i;

	buffer_clear(b);
	/*
	 * add a dummy cookie, the cookie will be overwritten by
	 * kex_send_kexinit(), each time a kexinit is set
	 */
	 /*这里添加16字节长度的空cookie，后面将被kex_send_kexinit()函数覆写*/
	for (i = 0; i < KEX_COOKIE_LEN; i++)
		buffer_put_char(b, 0);
	/*添加算法字符串*/
	for (i = 0; i < PROPOSAL_MAX; i++)
		buffer_put_cstring(b, proposal[i]);
	/*协议规定，在算法字符串后面表明是否有一个猜测的密钥交换数据包跟随*/
	buffer_put_char(b, 0);			/* first_kex_packet_follows */
	/*协议中unit32的保留长度*/
	buffer_put_int(b, 0);			/* uint32 reserved */
}

/* parse buffer and return algorithm proposal */
static char **
kex_buf2prop(Buffer *raw, int *first_kex_follows)
{
	Buffer b;
	u_int i;
	char **proposal;

	proposal = xcalloc(PROPOSAL_MAX, sizeof(char *));

	buffer_init(&b);
	buffer_append(&b, buffer_ptr(raw), buffer_len(raw));
	/* skip cookie */
	for (i = 0; i < KEX_COOKIE_LEN; i++)
		buffer_get_char(&b);
	/* extract kex init proposal strings */
	for (i = 0; i < PROPOSAL_MAX; i++) {
		proposal[i] = buffer_get_string(&b,NULL);
		zlog_debug(VD_SSH, "kex_parse_kexinit: %s", proposal[i]);
	}
	/* first kex follows / reserved */
	i = buffer_get_char(&b);
	if (first_kex_follows != NULL)
		*first_kex_follows = i;
	zlog_debug(VD_SSH, "kex_parse_kexinit: first_kex_follows %d ", i);
	i = buffer_get_int(&b);
	zlog_debug(VD_SSH, "kex_parse_kexinit: reserved %u ", i);
	ssh_buffer_free(&b);
	return proposal;
}

static void
kex_prop_free(char **proposal)
{
	u_int i;

	for (i = 0; i < PROPOSAL_MAX; i++)
		xfree(proposal[i]);
	xfree(proposal);
}

/* ARGSUSED */
static void
kex_protocol_error(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	zlog_err("Hm, kex protocol error: type %d seq %u", type, seq);
}

static void
kex_reset_dispatch(struct ssh_packet *ssh_packet)
{

	/* 初始化TRANS层协议的消息回调函数为默认kex_protocol_error */
	dispatch_range(SSH2_MSG_TRANSPORT_MIN,
	    SSH2_MSG_TRANSPORT_MAX, &kex_protocol_error, ssh_packet);
	/* 设置算法协商消息的回调函数 */
	dispatch_set(SSH2_MSG_KEXINIT, &kex_input_kexinit, ssh_packet);
}

void
kex_finish(Kex *kex, struct ssh_packet *ssh_packet)
{
	zlog_debug(VD_SSH, "kex_finish\n");

	kex_reset_dispatch(ssh_packet);

	packet_start(SSH2_MSG_NEWKEYS, ssh_packet);
	packet_start_ssh(SSH2_MSG_NEWKEYS, ssh_packet);
	packet_send(ssh_packet);
	/* packet_write_wait(); */
	zlog_debug(VD_SSH, "SSH2_MSG_NEWKEYS sent");

	packet_read_expect(SSH2_MSG_NEWKEYS, ssh_packet);
	packet_check_eom(ssh_packet);
	zlog_debug(VD_SSH, "SSH2_MSG_NEWKEYS received");

	kex->done = 1;
	buffer_clear(&kex->peer);
	/* buffer_clear(&kex->my); */
	kex->flags &= ~KEX_INIT_SENT;
	xfree(kex->name);
	kex->name = NULL;
}

void
kex_send_kexinit(Kex *kex, struct ssh_packet *ssh_packet)
{
	u_int32_t rnd = 0;
	u_char *cookie;
	u_int i;
//	unsigned char buf_printf[1024];

	zlog_debug(VD_SSH, "kex_send_kexinit\n");
	if (kex == NULL) {
		zlog_err("kex_send_kexinit: no kex, cannot rekey");
		return;
	}
	/* 此时kex->flags还没置位 */
	if (kex->flags & KEX_INIT_SENT) {
		zlog_debug(VD_SSH, "KEX_INIT_SENT");
		return;
	}
	kex->done = 0;

	/* generate a random cookie */
	/*在这里覆盖了预留的cookie位置*/
	if (buffer_len(&kex->my) < KEX_COOKIE_LEN)
		fatal("kex_send_kexinit: kex proposal too short");
	cookie = buffer_ptr(&kex->my);
	for (i = 0; i < KEX_COOKIE_LEN; i++) {
		if (i % 4 == 0)
			rnd = arc4random();
		cookie[i] = rnd;
		rnd >>= 8;
	}

	/*从这里开始将数据封装到临时缓存outgoing_packet中*/
	/*封装消息类型*/
	packet_start(SSH2_MSG_KEXINIT, ssh_packet);
	packet_start_ssh(SSH2_MSG_KEXINIT, ssh_packet);
	/*封装算法列表数据*/
	packet_put_raw(buffer_ptr(&kex->my), buffer_len(&kex->my), ssh_packet);
//	buffer_dump_ipc(ssh_packet);
	/*算法协商时还没有协商出kex，依然是明文传送*/
	packet_send(ssh_packet);
	zlog_debug(VD_SSH, "SSH2_MSG_KEXINIT sent");
	kex->flags |= KEX_INIT_SENT;
}

/* ARGSUSED */
/*算法协商的dispach回调函数。
 *参数--type:SSH2_MSG_KEXINIT; seq:消息序列号; ctxt:kex*/
void
kex_input_kexinit(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	char *ptr;
	u_int i, dlen;
	Kex *kex = (Kex *)ctxt;

zlog_debug(VD_SSH, "kex_input_kexinit\n");
	if (kex == NULL)
		fatal("kex_input_kexinit: no kex, cannot rekey");

	/*获取dlen:incoming_packet的长度。 ptr:incoming_packet指针*/
	ptr = packet_get_raw(&dlen, ssh_packet);
	/*数据拷贝到kex->peer中。kex->my存放服务器的算法列表，kex->peer存放客户端的算法列表*/
	buffer_append(&kex->peer, ptr, dlen);

	/* discard packet */
	for (i = 0; i < KEX_COOKIE_LEN; i++)
	{
		packet_get_char(ssh_packet);
	}
	for (i = 0; i < PROPOSAL_MAX; i++)
	{
		xfree(packet_get_string(NULL, ssh_packet));
	}
	(void) packet_get_char(ssh_packet);
	(void) packet_get_int(ssh_packet);
	/*宏定义 don't allow remaining bytes after the end of the message */
	packet_check_eom(ssh_packet);

	kex_kexinit_finish(kex, ssh_packet);
}

Kex *
kex_setup(char *proposal[PROPOSAL_MAX], struct ssh_packet *ssh_packet)
{
	Kex *kex;

	kex = xcalloc(1, sizeof(*kex));
	buffer_init(&kex->peer);
	buffer_init(&kex->my);
	/*在这里初始化和封装算法字符串表到kex->my中*/
	kex_prop2buf(&kex->my, proposal);
	kex->done = 0;

	/*kex算法协商,封装SSH_MSG_KEXINIT消息并发送*/
	kex_send_kexinit(kex, ssh_packet);					/* we start */
	/*在这里初始化TRANS所有协议的dispath回调函数，并注册算法协商的回调函数*/
	kex_reset_dispatch(ssh_packet);

	return kex;
}

/*处理协商算法*/
static void
kex_kexinit_finish(Kex *kex, struct ssh_packet *ssh_packet)
{
	zlog_debug(VD_SSH, "kex_kexinit_finish\n");

	if (!(kex->flags & KEX_INIT_SENT))
		{
		kex_send_kexinit(kex, ssh_packet);
		}

	/*获取到对端的算法列表后，开始协商算法*/
	kex_choose_conf(kex, ssh_packet);

	if (kex->kex_type >= 0 && kex->kex_type < KEX_MAX &&
	    kex->kex[kex->kex_type] != NULL) {
	    zlog_debug(VD_SSH, "call kex[%d] func,Algorithm:%s\n",kex->kex_type,kex->name);
		(kex->kex[kex->kex_type])(kex, ssh_packet);
	} else {
		fatal("Unsupported key exchange %d", kex->kex_type);
	}
}

static void
choose_enc(Enc *enc, char *client, char *server)
{
	char *name = match_list(client, server, NULL);
	if (name == NULL)
		fatal("no matching cipher found: client %s server %s",
		    client, server);
	if ((enc->cipher = cipher_by_name(name)) == NULL)
		fatal("matching cipher is not supported: %s", name);
	enc->name = name;
	enc->enabled = 0;
	enc->iv = NULL;
	enc->key = NULL;
	enc->key_len = cipher_keylen(enc->cipher);
	enc->block_size = cipher_blocksize(enc->cipher);
}

static void
choose_mac(Mac *mac, char *client, char *server)
{
	char *name = match_list(client, server, NULL);
	if (name == NULL)
		fatal("no matching mac found: client %s server %s",
		    client, server);
	if (mac_setup(mac, name) < 0)
		fatal("unsupported mac %s", name);

	mac->name = name;
	mac->key = NULL;
	mac->enabled = 0;
}

static void
choose_comp(Comp *comp, char *client, char *server)
{
	char *name = match_list(client, server, NULL);
	if (name == NULL)
		fatal("no matching comp found: client %s server %s", client, server);
	if (strcmp(name, "zlib@openssh.com") == 0) {
		comp->type = COMP_DELAYED;
	} else if (strcmp(name, "zlib") == 0) {
		comp->type = COMP_ZLIB;
	} else if (strcmp(name, "none") == 0) {
		comp->type = COMP_NONE;
	} else {
		fatal("unsupported comp %s", name);
	}
	comp->name = name;
}

static void
choose_kex(Kex *k, char *client, char *server)
{
	k->name = match_list(client, server, NULL);
	if (k->name == NULL)
		fatal("Unable to negotiate a key exchange method");
	if (strcmp(k->name, KEX_DH1) == 0) {
		k->kex_type = KEX_DH_GRP1_SHA1;
		k->evp_md = EVP_sha1();
	} else if (strcmp(k->name, KEX_DH14) == 0) {
		k->kex_type = KEX_DH_GRP14_SHA1;
		k->evp_md = EVP_sha1();
	} else if (strcmp(k->name, KEX_DHGEX_SHA1) == 0) {
		k->kex_type = KEX_DH_GEX_SHA1;
		k->evp_md = EVP_sha1();
#if OPENSSL_VERSION_NUMBER >= 0x00907000L
	} else if (strcmp(k->name, KEX_DHGEX_SHA256) == 0) {
		k->kex_type = KEX_DH_GEX_SHA256;
		k->evp_md = evp_ssh_sha256();
#endif
	} else
		fatal("bad kex alg %s", k->name);
}

static void
choose_hostkeyalg(Kex *k, char *client, char *server)
{
	char *hostkeyalg = match_list(client, server, NULL);
	if (hostkeyalg == NULL)
		fatal("no hostkey alg");
	k->hostkey_type = key_type_from_name(hostkeyalg);
	if (k->hostkey_type == KEY_UNSPEC)
		fatal("bad hostkey alg '%s'", hostkeyalg);
	xfree(hostkeyalg);
}

static int
proposals_match(char *my[PROPOSAL_MAX], char *peer[PROPOSAL_MAX])
{
	static int check[] = {
		PROPOSAL_KEX_ALGS, PROPOSAL_SERVER_HOST_KEY_ALGS, -1
	};
	int *idx;
	char *p;

	for (idx = &check[0]; *idx != -1; idx++) {
		if ((p = strchr(my[*idx], ',')) != NULL)
			*p = '\0';
		if ((p = strchr(peer[*idx], ',')) != NULL)
			*p = '\0';
		if (strcmp(my[*idx], peer[*idx]) != 0) {
			zlog_debug(VD_SSH, "proposal mismatch: my %s peer %s",
			    my[*idx], peer[*idx]);
			return (0);
		}
	}
	zlog_debug(VD_SSH, "proposals match");
	return (1);
}

/*协商算法*/
static void
kex_choose_conf(Kex *kex, struct ssh_packet *ssh_packet)
{
	Newkeys *newkeys;
	char **my, **peer;
	char **cprop, **sprop;
	int nenc, nmac, ncomp;
	u_int mode, ctos, need;
	int first_kex_follows, type;

	my   = kex_buf2prop(&kex->my, NULL);
	peer = kex_buf2prop(&kex->peer, &first_kex_follows);

	if (kex->server) {
		cprop=peer;
		sprop=my;
	} else {
		cprop=my;
		sprop=peer;
	}

	/* Algorithm Negotiation */
	for (mode = 0; mode < MODE_MAX; mode++) {
		newkeys = xcalloc(1, sizeof(*newkeys));
		kex->newkeys[mode] = newkeys;
		ctos = (!kex->server && mode == MODE_OUT) ||
		    (kex->server && mode == MODE_IN);
		nenc  = ctos ? PROPOSAL_ENC_ALGS_CTOS  : PROPOSAL_ENC_ALGS_STOC;
		nmac  = ctos ? PROPOSAL_MAC_ALGS_CTOS  : PROPOSAL_MAC_ALGS_STOC;
		ncomp = ctos ? PROPOSAL_COMP_ALGS_CTOS : PROPOSAL_COMP_ALGS_STOC;
		choose_enc (&newkeys->enc,  cprop[nenc],  sprop[nenc]);  //加密算法
		choose_mac (&newkeys->mac,  cprop[nmac],  sprop[nmac]);   //mac校验算法
		choose_comp(&newkeys->comp, cprop[ncomp], sprop[ncomp]);  //压缩算法
		zlog_debug(VD_SSH, "kex: %s %s %s %s",
		    ctos ? "client->server" : "server->client",
		    newkeys->enc.name,
		    newkeys->mac.name,
		    newkeys->comp.name);
	}
	choose_kex(kex, cprop[PROPOSAL_KEX_ALGS], sprop[PROPOSAL_KEX_ALGS]); //秘钥加密算法
	choose_hostkeyalg(kex, cprop[PROPOSAL_SERVER_HOST_KEY_ALGS],
	    sprop[PROPOSAL_SERVER_HOST_KEY_ALGS]); //服务器主机秘钥算法
	need = 0;
	/*取出最长的key_len? */
	for (mode = 0; mode < MODE_MAX; mode++) {
		newkeys = kex->newkeys[mode];
		if (need < newkeys->enc.key_len)
			need = newkeys->enc.key_len;
		if (need < newkeys->enc.block_size)
			need = newkeys->enc.block_size;
		if (need < newkeys->mac.key_len)
			need = newkeys->mac.key_len;
	}
	/* XXX need runden? */
	kex->we_need = need;

	/* ignore the next message if the proposals do not match */
	if (first_kex_follows && !proposals_match(my, peer)) {
		type = packet_read(ssh_packet);
		zlog_debug(VD_SSH, "skipping next packet (type %u)", type);
	}

	kex_prop_free(my);
	kex_prop_free(peer);
}

static u_char *
derive_key(Kex *kex, int id, u_int need, u_char *hash, u_int hashlen,
    BIGNUM *shared_secret)
{
	Buffer b;
	EVP_MD_CTX md;
	char c = id;
	u_int have;
	int mdsz;
	u_char *digest;

	if ((mdsz = EVP_MD_size(kex->evp_md)) <= 0)
		fatal("bad kex md size %d", mdsz);
	digest = xmalloc(roundup(need, mdsz));

	buffer_init(&b);
	buffer_put_bignum2(&b, shared_secret);

	/* K1 = HASH(K || H || "A" || session_id) */
	EVP_DigestInit(&md, kex->evp_md);
//	if (!(datafellows & SSH_BUG_DERIVEKEY))
		EVP_DigestUpdate(&md, buffer_ptr(&b), buffer_len(&b));
	EVP_DigestUpdate(&md, hash, hashlen);
	EVP_DigestUpdate(&md, &c, 1);
	EVP_DigestUpdate(&md, kex->session_id, kex->session_id_len);
	EVP_DigestFinal(&md, digest, NULL);

	/*
	 * expand key:
	 * Kn = HASH(K || H || K1 || K2 || ... || Kn-1)
	 * Key = K1 || K2 || ... || Kn
	 */
	for (have = mdsz; need > have; have += mdsz) {
		EVP_DigestInit(&md, kex->evp_md);
//		if (!(datafellows & SSH_BUG_DERIVEKEY))
			EVP_DigestUpdate(&md, buffer_ptr(&b), buffer_len(&b));
		EVP_DigestUpdate(&md, hash, hashlen);
		EVP_DigestUpdate(&md, digest, have);
		EVP_DigestFinal(&md, digest + have, NULL);
	}
	ssh_buffer_free(&b);
#ifdef DEBUG_KEX
	fprintf(stderr, "key '%c'== ", c);
	dump_digest("key", digest, need);
#endif
	return digest;
}

Newkeys *current_keys[MODE_MAX];

#define NKEYS	6
void
kex_derive_keys(Kex *kex, u_char *hash, u_int hashlen, BIGNUM *shared_secret, struct ssh_packet *ssh_packet)
{
	u_char *keys[NKEYS];
	u_int i, mode, ctos;

	for (i = 0; i < NKEYS; i++) {
		keys[i] = derive_key(kex, 'A'+i, kex->we_need, hash, hashlen,
		    shared_secret);
	}
#if 0
printf("kex_derive_keys11\n");
for (mode = 0; mode < MODE_MAX; mode++) {
	current_keys[mode] = kex->newkeys[mode];
	kex->newkeys[mode] = NULL;
	ctos = (!kex->server && mode == MODE_OUT) ||
			(kex->server && mode == MODE_IN);
	current_keys[mode]->enc.iv	= keys[ctos ? 0 : 1];
	current_keys[mode]->enc.key = keys[ctos ? 2 : 3];
	current_keys[mode]->mac.key = keys[ctos ? 4 : 5];
}
#endif 
#if 1
	for (mode = 0; mode < MODE_MAX; mode++) {
		ssh_packet->current_keys[mode] = kex->newkeys[mode];
		kex->newkeys[mode] = NULL;
		ctos = (!kex->server && mode == MODE_OUT) ||
		    (kex->server && mode == MODE_IN);
		ssh_packet->current_keys[mode]->enc.iv  = keys[ctos ? 0 : 1];
		ssh_packet->current_keys[mode]->enc.key = keys[ctos ? 2 : 3];
		ssh_packet->current_keys[mode]->mac.key = keys[ctos ? 4 : 5];
	}
#endif 
}

Newkeys *
kex_get_newkeys(int mode, struct ssh_packet *ssh_packet)
{
	Newkeys *ret;

	ret = ssh_packet->current_keys[mode];
	ssh_packet->current_keys[mode] = NULL;
	return ret;
}

void
derive_ssh1_session_id(BIGNUM *host_modulus, BIGNUM *server_modulus,
    u_int8_t cookie[8], u_int8_t id[16])
{
	const EVP_MD *evp_md = EVP_md5();
	EVP_MD_CTX md;
	u_int8_t nbuf[2048], obuf[EVP_MAX_MD_SIZE];
	int len;

	EVP_DigestInit(&md, evp_md);

	len = BN_num_bytes(host_modulus);
	if (len < (512 / 8) || (u_int)len > sizeof(nbuf))
		fatal("%s: bad host modulus (len %d)", __func__, len);
	BN_bn2bin(host_modulus, nbuf);
	EVP_DigestUpdate(&md, nbuf, len);

	len = BN_num_bytes(server_modulus);
	if (len < (512 / 8) || (u_int)len > sizeof(nbuf))
		fatal("%s: bad server modulus (len %d)", __func__, len);
	BN_bn2bin(server_modulus, nbuf);
	EVP_DigestUpdate(&md, nbuf, len);

	EVP_DigestUpdate(&md, cookie, 8);

	EVP_DigestFinal(&md, obuf, NULL);
	memcpy(id, obuf, 16);

	memset(nbuf, 0, sizeof(nbuf));
	memset(obuf, 0, sizeof(obuf));
	memset(&md, 0, sizeof(md));
}

void kex_delete(Kex *kex)
{
	int i;
	if(kex->session_id)
	{
		free(kex->session_id);
		kex->session_id = NULL;
		
	}
	
	for(i = 0; i < MODE_MAX; i++)
		if(kex->newkeys[i])
		{
			free(kex->newkeys[i]);
			kex->newkeys[i] = NULL;
		}
}

#if defined(DEBUG_KEX) || defined(DEBUG_KEXDH)
void
dump_digest(char *msg, u_char *digest, int len)
{
	u_int i;

	fprintf(stderr, "%s\n", msg);
	for (i = 0; i < len; i++) {
		fprintf(stderr, "%02x", digest[i]);
		if (i%32 == 31)
			fprintf(stderr, "\n");
		else if (i%8 == 7)
			fprintf(stderr, " ");
	}
	fprintf(stderr, "\n");
}
#endif
