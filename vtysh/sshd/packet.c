/* $OpenBSD: packet.c,v 1.160 2009/02/13 11:50:21 markus Exp $ */
/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains code implementing the packet protocol and communication
 * with the other side.  This same code is used both on client and server side.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 *
 * SSH2 packet format added by Markus Friedl.
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

//#include "includes.h"
 
#include <sys/types.h>
#include "sys-queue.h"
//#include <sys/queue.h>
#include <sys/param.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

//#include <netinet/in.h>
//#include <netinet/ip.h>
#include <lib/log.h>

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "xmalloc.h"
#include "buffer_ssh.h"
#include "packet.h"
#include "compress.h"
#include "ssh1.h"
#include "ssh2.h"
#include "cipher.h"
#include "key.h"
#include "kex.h"
#include "mac.h"
#include "misc.h"
#include "ssh_version.h"
#include "vtysh_sshd.h"
#include "buffer.h"
#include "vtysh_session.h"
#include <ftm/pkt_tcp.h>
#include "vtysh.h"

#ifdef PACKET_DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif

#define PACKET_MAX_SIZE (256 * 1024)

/*
 * This variable contains the file descriptors used for communicating with
 * the other side.  connection_in is used for reading; connection_out for
 * writing.  These can be the same descriptor, in which case it is assumed to
 * be a socket.
 */
static int connection_in = -1;
static int connection_out = -1;

/* Protocol flags for the remote side. */
static u_int remote_protocol_flags = 0;

/* Encryption context for receiving data.  This is only used for decryption. */
/*用于接收数据的加密上下文。 这只用于解密。*/
//static CipherContext receive_context;

/* Encryption context for sending data.  This is only used for encryption. */
/*用于发送数据的加密上下文。 这只用于加密。*/
//static CipherContext send_context;

/* Buffer for raw input data from the socket. */
/*接收的原始报文*/
//Buffer input;

/* Buffer for raw output data going to the socket. */
/*output是最终要发送的原始buffer(可能是加密后)*/
//Buffer output;

/* Buffer for the partial outgoing packet being constructed. */
/*在packet_send2（）和其后续函数内的临时缓存*/
//static Buffer outgoing_packet;

/* Buffer for the incoming packet currently being processed. */
/*解密、处理后的报文*/
//static Buffer incoming_packet;

/* Scratch buffer for packet compression/decompression. */
//static Buffer compression_buffer;
//static int compression_buffer_ready = 0;

/* Flag indicating whether packet compression/decompression is enabled. */
//static int packet_compression = 0;

/* default maximum packet size */
u_int max_packet_size = 32768;

/* Flag indicating whether this module has been initialized. */
//static int initialized = 0;

/* Set to true if the connection is interactive. */
static int interactive_mode = 0;

/* Set to true if we are the server side. */
static int server_side = 0;

/* Set to true if we are authenticated. */
static int after_authentication = 0;

int keep_alive_timeouts = 0;

/* Set to the maximum time that we will wait to send or receive a packet */
/* 收发包的超时时间 */
static int packet_timeout_ms = -1;

/* Session key information for Encryption and MAC */
/* 加密和MAC的会话密钥信息 */
Newkeys *newkeys[MODE_MAX];

/*p_read/p_send是对收发数据包的计数，包括序号、总字节等*/
static struct packet_state  p_read, p_send;

static u_int64_t max_blocks_in, max_blocks_out;
static u_int32_t rekey_limit;

/* Session key for protocol v1 */
static u_char ssh1_key[SSH_SESSION_KEY_LENGTH];
static u_int ssh1_keylen;

/* roundup current message to extra_pad bytes */
static u_char extra_pad = 0;

/* XXX discard incoming data after MAC error */
//static u_int packet_discard = 0;
//static Mac *packet_discard_mac = NULL;

struct packet {
	TAILQ_ENTRY(packet) next;
	u_char type;
	Buffer payload;
};
TAILQ_HEAD(, packet) outgoing;

/*
 * Sets the descriptors used for communication.  Disables encryption until
 * packet_set_encryption_key is called.
 * 设置用于通信的描述符。 禁用加密，直到调用packet_set_encryption_key。
 */
void
packet_set_connection(struct ssh_packet *ssh_packet)
{
	Cipher *none = cipher_by_name("none");

	if (none == NULL)
		fatal("packet_set_connection: cannot load cipher 'none'");
	cipher_init(&(ssh_packet->send_context), none, (const u_char *)"",
	    0, NULL, 0, CIPHER_ENCRYPT);
	cipher_init(&(ssh_packet->receive_context), none, (const u_char *)"",
	    0, NULL, 0, CIPHER_DECRYPT);
	ssh_packet->newkeys[MODE_IN] = ssh_packet->newkeys[MODE_OUT] = NULL;
	if (!ssh_packet->initialized) {
		ssh_packet->initialized = 1;
		/*收发的原始数据包*/
		buffer_init(&ssh_packet->input);
		buffer_init(&ssh_packet->output);
		/*outgoing_packet是在packet_send2（）和其后续函数内的临时缓存*/
		buffer_init(&ssh_packet->outgoing_packet);
		buffer_init(&ssh_packet->incoming_packet);
		TAILQ_INIT(&outgoing);
		ssh_packet->p_send.packets = ssh_packet->p_read.packets = 0;
	}
}

void
packet_set_timeout(int timeout, int count)
{
	if (timeout == 0 || count == 0) {
		packet_timeout_ms = -1;
		return;
	}
	if ((INT_MAX / 1000) / count < timeout)
		packet_timeout_ms = INT_MAX;
	else
		packet_timeout_ms = timeout * count * 1000;
}
#if 0
static void
packet_stop_discard(struct ssh_packet *ssh_packet)
{
	if (packet_discard_mac) {
		char buf[1024];
		
		memset(buf, 'a', sizeof(buf));
		while (buffer_len(&incoming_packet) < PACKET_MAX_SIZE)
			buffer_append(&incoming_packet, buf, sizeof(buf));
		(void) mac_compute(packet_discard_mac,
		    p_read.seqnr,
		    buffer_ptr(&incoming_packet),
		    PACKET_MAX_SIZE);
	}
//	zlog_debug(VD_SSH, "Finished discarding for %.200s", get_remote_ipaddr());
	cleanup_exit(255);
}
#endif
static void
packet_start_discard(Enc *enc, Mac *mac, u_int packet_length, u_int discard)
{
#if 0
	if (enc == NULL || !cipher_is_cbc(enc->cipher))
		packet_disconnect("Packet corrupt");
	if (packet_length != PACKET_MAX_SIZE && mac && mac->enabled)
		packet_discard_mac = mac;
	if (buffer_len(&input) >= discard)
		packet_stop_discard();
	packet_discard = discard - buffer_len(&input);
	#endif
}

/* Returns 1 if remote host is connected via socket, 0 if not. */

int
packet_connection_is_on_socket(void)
{
	struct sockaddr_storage from, to;
	socklen_t fromlen, tolen;

	/* filedescriptors in and out are the same, so it's a socket */
	if (connection_in == connection_out)
		return 1;
	fromlen = sizeof(from);
	memset(&from, 0, sizeof(from));
	if (getpeername(connection_in, (struct sockaddr *)&from, &fromlen) < 0)
		return 0;
	tolen = sizeof(to);
	memset(&to, 0, sizeof(to));
	if (getpeername(connection_out, (struct sockaddr *)&to, &tolen) < 0)
		return 0;
	if (fromlen != tolen || memcmp(&from, &to, fromlen) != 0)
		return 0;
	if (from.ss_family != AF_INET && from.ss_family != AF_INET6)
		return 0;
	return 1;
}

/*
 * Exports an IV from the CipherContext required to export the key
 * state back from the unprivileged child to the privileged parent
 * process.
 */
#if 0
void
packet_get_keyiv(int mode, u_char *iv, u_int len)
{
	CipherContext *cc;

	if (mode == MODE_OUT)
		cc = &send_context;
	else
		cc = &receive_context;

	cipher_get_keyiv(cc, iv, len);
}
#endif
#if 0
int
packet_get_keycontext(int mode, u_char *dat)
{
	CipherContext *cc;

	if (mode == MODE_OUT)
		cc = &send_context;
	else
		cc = &receive_context;

	return (cipher_get_keycontext(cc, dat));
}

void
packet_set_keycontext(int mode, u_char *dat)
{
	CipherContext *cc;

	if (mode == MODE_OUT)
		cc = &send_context;
	else
		cc = &receive_context;

	cipher_set_keycontext(cc, dat);
}



int
packet_get_keyiv_len(int mode)
{
	CipherContext *cc;

	if (mode == MODE_OUT)
		cc = &send_context;
	else
		cc = &receive_context;

	return (cipher_get_keyiv_len(cc));
}


void
packet_set_iv(int mode, u_char *dat)
{
	CipherContext *cc;

	if (mode == MODE_OUT)
		cc = &send_context;
	else
		cc = &receive_context;

	cipher_set_keyiv(cc, dat);
}

int
packet_get_ssh1_cipher(void)
{
	return (cipher_get_number(receive_context.cipher));
}
#endif

void
packet_get_state(int mode, u_int32_t *seqnr, u_int64_t *blocks, u_int32_t *packets,
    u_int64_t *bytes)
{
	struct packet_state *state;

	state = (mode == MODE_IN) ? &p_read : &p_send;
	if (seqnr)
		*seqnr = state->seqnr;
	if (blocks)
		*blocks = state->blocks;
	if (packets)
		*packets = state->packets;
	if (bytes)
		*bytes = state->bytes;
}

void
packet_set_state(int mode, u_int32_t seqnr, u_int64_t blocks, u_int32_t packets,
    u_int64_t bytes)
{
	struct packet_state *state;

	state = (mode == MODE_IN) ? &p_read : &p_send;
	state->seqnr = seqnr;
	state->blocks = blocks;
	state->packets = packets;
	state->bytes = bytes;
}

/* returns 1 if connection is via ipv4 */

int
packet_connection_is_ipv4(void)
{
	struct sockaddr_storage to;
	socklen_t tolen = sizeof(to);

	memset(&to, 0, sizeof(to));
	if (getsockname(connection_out, (struct sockaddr *)&to, &tolen) < 0)
		return 0;
	if (to.ss_family == AF_INET)
		return 1;
#ifdef IPV4_IN_IPV6
	if (to.ss_family == AF_INET6 &&
	    IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6 *)&to)->sin6_addr))
		return 1;
#endif
	return 0;
}

/* Sets the connection into non-blocking mode. */

void
packet_set_nonblocking(void)
{
	/* Set the socket into non-blocking mode. */
	set_nonblock(connection_in);

	if (connection_out != connection_in)
		set_nonblock(connection_out);
}

/* Returns the socket used for reading. */

int
packet_get_connection_in(void)
{
	return connection_in;
}

/* Returns the descriptor used for writing. */

int
packet_get_connection_out(void)
{
	return connection_out;
}

/* Closes the connection and clears and frees internal data structures. */

void
packet_close(struct ssh_packet *ssh_packet)
{
	if (!ssh_packet->initialized)
		return;
	ssh_packet->initialized = 0;

	ssh_buffer_free(&ssh_packet->input);
	ssh_buffer_free(&ssh_packet->output);
	ssh_buffer_free(&ssh_packet->outgoing_packet);
	ssh_buffer_free(&ssh_packet->incoming_packet);
	if (ssh_packet->compression_buffer_ready) {
		ssh_buffer_free(&ssh_packet->compression_buffer);
		buffer_compress_uninit(ssh_packet);
	}
	cipher_cleanup(&ssh_packet->send_context);
	cipher_cleanup(&ssh_packet->receive_context);
}

/* Sets remote side protocol flags. */

void
packet_set_protocol_flags(u_int protocol_flags)
{
	remote_protocol_flags = protocol_flags;
}

/* Returns the remote protocol flags set earlier by the above function. */

u_int
packet_get_protocol_flags(void)
{
	return remote_protocol_flags;
}

/*
 * Starts packet compression from the next packet on in both directions.
 * Level is compression level 1 (fastest) - 9 (slow, best) as in gzip.
 */

static void
packet_init_compression(struct ssh_packet *ssh_packet)
{
	if (ssh_packet->compression_buffer_ready == 1)
		return;
	ssh_packet->compression_buffer_ready = 1;
	buffer_init(&ssh_packet->compression_buffer);
}

#if 0
void
packet_start_compression(int level)
{
	if (packet_compression && !compat20)
		fatal("Compression already enabled.");
	packet_compression = 1;
	packet_init_compression();
	buffer_compress_init_send(level, ssh_packet);
	buffer_compress_init_recv();
}
#endif

/*
 * Causes any further packets to be encrypted using the given key.  The same
 * key is used for both sending and reception.  However, both directions are
 * encrypted independently of each other.
 */

void
packet_set_encryption_key(const u_char *key, u_int keylen,
    int number)
{
#if 0
	Cipher *cipher = cipher_by_number(number);

	if (cipher == NULL)
		fatal("packet_set_encryption_key: unknown cipher number %d", number);
	if (keylen < 20)
		fatal("packet_set_encryption_key: keylen too small: %d", keylen);
	if (keylen > SSH_SESSION_KEY_LENGTH)
		fatal("packet_set_encryption_key: keylen too big: %d", keylen);
	memcpy(ssh1_key, key, keylen);
	ssh1_keylen = keylen;
	cipher_init(&send_context, cipher, key, keylen, NULL, 0, CIPHER_ENCRYPT);
	cipher_init(&receive_context, cipher, key, keylen, NULL, 0, CIPHER_DECRYPT);
#endif
}

u_int
packet_get_encryption_key(u_char *key)
{
	if (key == NULL)
		return (ssh1_keylen);
	memcpy(key, ssh1_key, ssh1_keylen);
	return (ssh1_keylen);
}

/* Start constructing a packet to send. */
void
packet_start(u_char type, struct ssh_packet *ssh_packet)
{
	u_char buf[9];
	int len;

//	DBG(zlog_debug(VD_SSH, "packet_start[%d]", type));
//	len = compat20 ? 6 : 9;
	len = 6;
	memset(buf, 0, len - 1);
	buf[len - 1] = type;
	buffer_clear(&ssh_packet->outgoing_packet);
	buffer_append(&ssh_packet->outgoing_packet, buf, len);
//	buffer_dump(&ssh_packet->outgoing_packet);
}

/* Start constructing a packet to send. */
void
packet_start_ssh(u_char type, struct ssh_packet *ssh_packet)
{
	u_char buf[9];
	int len;
	Buffer *outgoing_packet;


	outgoing_packet = &(ssh_packet->outgoing_packet);
//		((struct ssh_packet *)(session->ssh_packet))->outgoing_packet;
//	DBG(zlog_debug(VD_SSH, "packet_start[%d]", type));
//	len = compat20 ? 6 : 9;
	len = 6;
	memset(buf, 0, len - 1);
	buf[len - 1] = type;
	buffer_clear(outgoing_packet);
	buffer_append(outgoing_packet, buf, len);
//		buffer_dump(outgoing_packet);

}


/* Append payload. */
void
packet_put_char(int value, struct ssh_packet *ssh_packet)
{
	char ch = value;

	buffer_append(&ssh_packet->outgoing_packet, &ch, 1);
}

void
packet_put_int(u_int value, struct ssh_packet *ssh_packet)
{
	buffer_put_int(&ssh_packet->outgoing_packet, value);
}

void
packet_put_string(const void *buf, u_int len, struct ssh_packet *ssh_packet)
{
	buffer_put_string(&ssh_packet->outgoing_packet, buf, len);
}

void
packet_put_cstring(const char *str, struct ssh_packet *ssh_packet)
{
	buffer_put_cstring(&ssh_packet->outgoing_packet, str);
}

void
packet_put_raw(const void *buf, u_int len, struct ssh_packet *ssh_packet)
{
	buffer_append(&(ssh_packet->outgoing_packet), buf, len);

	
//	buffer_dump(&ssh_packet->outgoing_packet);
}

void
packet_put_bignum(BIGNUM * value)
{

//	buffer_put_bignum(&outgoing_packet, value);
}

void
packet_put_bignum2(BIGNUM * value, struct ssh_packet *ssh_packet)
{
	buffer_put_bignum2(&ssh_packet->outgoing_packet, value);
}

/*
 * Finalizes and sends the packet.  If the encryption key has been set,
 * encrypts the packet before sending.
 */
#if 0
static void
packet_send1(void)
{
	u_char buf[8], *cp;
	int i, padding, len;
	u_int checksum;
	u_int32_t rnd = 0;

	/*
	 * If using packet compression, compress the payload of the outgoing
	 * packet.
	 */
	if (packet_compression) {
		buffer_clear(&compression_buffer);
		/* Skip padding. */
		buffer_consume(&outgoing_packet, 8);
		/* padding */
		buffer_append(&compression_buffer, "\0\0\0\0\0\0\0\0", 8);
		buffer_compress(&outgoing_packet, &compression_buffer);
		buffer_clear(&outgoing_packet);
		buffer_append(&outgoing_packet, buffer_ptr(&compression_buffer),
		    buffer_len(&compression_buffer));
	}
	/* Compute packet length without padding (add checksum, remove padding). */
	len = buffer_len(&outgoing_packet) + 4 - 8;

	/* Insert padding. Initialized to zero in packet_start1() */
	padding = 8 - len % 8;
	if (!send_context.plaintext) {
		cp = buffer_ptr(&outgoing_packet);
		for (i = 0; i < padding; i++) {
			if (i % 4 == 0)
				rnd = arc4random();
			cp[7 - i] = rnd & 0xff;
			rnd >>= 8;
		}
	}
	buffer_consume(&outgoing_packet, 8 - padding);

	/* Add check bytes. */
	checksum = ssh_crc32(buffer_ptr(&outgoing_packet),
	    buffer_len(&outgoing_packet));
	put_u32(buf, checksum);
	buffer_append(&outgoing_packet, buf, 4);

#ifdef PACKET_DEBUG
	fprintf(stderr, "packet_send plain: ");
	buffer_dump(&outgoing_packet);
#endif

	/* Append to output. */
	put_u32(buf, len);
	buffer_append(&output, buf, 4);
	cp = buffer_append_space(&output, buffer_len(&outgoing_packet));
	cipher_crypt(&send_context, cp, buffer_ptr(&outgoing_packet),
	    buffer_len(&outgoing_packet));

#ifdef PACKET_DEBUG
	fprintf(stderr, "encrypted: ");
	buffer_dump(&output);
#endif
	p_send.packets++;
	p_send.bytes += len + buffer_len(&outgoing_packet);
	buffer_clear(&outgoing_packet);

	/*
	 * Note that the packet is now only buffered in output.  It won't be
	 * actually sent until packet_write_wait or packet_write_poll is
	 * called.
	 */
}
#endif

void
set_newkeys(int mode, struct ssh_packet *ssh_packet)
{
	Enc *enc;
	Mac *mac;
	Comp *comp;
	CipherContext *cc;
	u_int64_t *max_blocks;
	int crypt_type;

	if (mode == MODE_OUT) {
		cc = &ssh_packet->send_context;
		crypt_type = CIPHER_ENCRYPT;
		ssh_packet->p_send.packets = ssh_packet->p_send.blocks = 0;
		max_blocks = &ssh_packet->max_blocks_out;
	} else {
		cc = &ssh_packet->receive_context;
		crypt_type = CIPHER_DECRYPT;
		ssh_packet->p_read.packets = ssh_packet->p_read.blocks = 0;
		max_blocks = &ssh_packet->max_blocks_in;
	}
	if (ssh_packet->newkeys[mode] != NULL) {
		cipher_cleanup(cc);
		enc  = &ssh_packet->newkeys[mode]->enc;
		mac  = &ssh_packet->newkeys[mode]->mac;
		comp = &ssh_packet->newkeys[mode]->comp;
		mac_clear(mac);
		xfree(enc->name);
		xfree(enc->iv);
		xfree(enc->key);
		xfree(mac->name);
		xfree(mac->key);
		xfree(comp->name);
		xfree(ssh_packet->newkeys[mode]);
	}
	ssh_packet->newkeys[mode] = kex_get_newkeys(mode, ssh_packet);
	if (ssh_packet->newkeys[mode] == NULL)
		fatal("newkeys: no keys for mode %d", mode);
	enc  = &ssh_packet->newkeys[mode]->enc;
	mac  = &ssh_packet->newkeys[mode]->mac;
	comp = &ssh_packet->newkeys[mode]->comp;
	if (mac_init(mac) == 0)
		mac->enabled = 1;
	DBG(zlog_debug(VD_SSH, "cipher_init_context: %d", mode));
	cipher_init(cc, enc->cipher, enc->key, enc->key_len,
	    enc->iv, enc->block_size, crypt_type);
	/* Deleting the keys does not gain extra security */
	/* memset(enc->iv,  0, enc->block_size);
	   memset(enc->key, 0, enc->key_len);
	   memset(mac->key, 0, mac->key_len); */
	if ((comp->type == COMP_ZLIB ||
	    (comp->type == COMP_DELAYED && ssh_packet->after_authentication)) &&
	    comp->enabled == 0) {
		packet_init_compression(ssh_packet);
		if (mode == MODE_OUT)
			buffer_compress_init_send(6, ssh_packet);
		else
			buffer_compress_init_recv(ssh_packet);
		comp->enabled = 1;
	}
	/*
	 * The 2^(blocksize*2) limit is too expensive for 3DES,
	 * blowfish, etc, so enforce a 1GB limit for small blocksizes.
	 */
	if (enc->block_size >= 16)
		*max_blocks = (u_int64_t)1 << (enc->block_size*2);
	else
		*max_blocks = ((u_int64_t)1 << 30) / enc->block_size;
//	if (rekey_limit)
//		*max_blocks = MIN(*max_blocks, rekey_limit / enc->block_size);
}

/*
 * Delayed compression for SSH2 is enabled after authentication:
 * This happens on the server side after a SSH2_MSG_USERAUTH_SUCCESS is sent,
 * and on the client side after a SSH2_MSG_USERAUTH_SUCCESS is received.
 * SSH2的延迟压缩在认证后启用：服务器端发送SSH2_MSG_USERAUTH_SUCCESS之后，并且在客户端收到
 * SSH2_MSG_USERAUTH_SUCCESS之后在。
 */
static void
packet_enable_delayed_compress(struct ssh_packet *ssh_packet)
{
	Comp *comp = NULL;
	int mode;

	/*
	 * Remember that we are past the authentication step, so rekeying
	 * with COMP_DELAYED will turn on compression immediately.
	 */
	ssh_packet->after_authentication = 1;
	for (mode = 0; mode < MODE_MAX; mode++) {
		/* protocol error: USERAUTH_SUCCESS received before NEWKEYS */
		if (ssh_packet->newkeys[mode] == NULL)
			continue;
		comp = &ssh_packet->newkeys[mode]->comp;
		if (comp && !comp->enabled && comp->type == COMP_DELAYED) {
			packet_init_compression(ssh_packet);
			if (mode == MODE_OUT)
				buffer_compress_init_send(6, ssh_packet);
			else
				buffer_compress_init_recv(ssh_packet);
			comp->enabled = 1;
		}
	}
}

/*
 * Finalize packet in SSH2 format (compress, mac, encrypt, enqueue)
 * 以SSH2要求格式完成完整数据包
 */
static void
packet_send2_wrapped(struct ssh_packet *ssh_packet)
{
	u_char type, *cp, *macbuf = NULL;
	u_char padlen, pad;
	u_int packet_length = 0;
	u_int i, len;
	u_int32_t rnd = 0;
	Enc *enc   = NULL;
	Mac *mac   = NULL;
	Comp *comp = NULL;
	int block_size;
	Buffer *outgoing_packet;
	Buffer *compression_buffer;
	CipherContext *send_context;
	struct packet_state *p_send;
	Buffer *output;
	
	outgoing_packet = &(ssh_packet->outgoing_packet);
	compression_buffer = &(ssh_packet->compression_buffer);
	send_context = &(ssh_packet->send_context);
	p_send = &(ssh_packet->p_send);
	output = &(ssh_packet->output);

	/*检查session key是否被初始化,在算法协商阶段，尚未协商出key*/
	if (ssh_packet->newkeys[MODE_OUT] != NULL) {
		enc  = &ssh_packet->newkeys[MODE_OUT]->enc;
		mac  = &ssh_packet->newkeys[MODE_OUT]->mac;
		comp = &ssh_packet->newkeys[MODE_OUT]->comp;
	}

	block_size = enc ? enc->block_size : 8;

	cp = buffer_ptr(outgoing_packet);
	type = cp[5];

#ifdef PACKET_DEBUG
	fprintf(stderr, "plain:     ");
	buffer_dump(outgoing_packet);
#endif

	/* 如果使能压缩算法，压缩payload */
	if (comp && comp->enabled) {
		len = buffer_len(outgoing_packet);
		/* skip header, compress only payload */
		buffer_consume(outgoing_packet, 5);
		buffer_clear(compression_buffer);
		buffer_compress(outgoing_packet, compression_buffer);
		buffer_clear(outgoing_packet);
		buffer_append(outgoing_packet, "\0\0\0\0\0", 5);
		buffer_append(outgoing_packet, buffer_ptr(compression_buffer),
		    buffer_len(compression_buffer));
		DBG(zlog_debug(VD_SSH, "compression: raw %d compressed %d", len,
		    buffer_len(outgoing_packet)));
	}

	/* sizeof (packet_len + pad_len + payload) */
	len = buffer_len(outgoing_packet);

	/*
	 * calc size of padding, alloc space, get random data,
	 * minimum padding is 4 bytes
	 */
	/* uint 32		 packet_length
	 * byte       padding_length
	 * byte[n1]   payload; n1 = packet_length - padding_length -1
	 * byte[n2]   radom padding; n2 = padding_length
	 * byte[m]    mac  验证
	 */
	 /*block_size:8,确保长度是8的倍数*/
	padlen = block_size - (len % block_size);
	if (padlen < 4)
	{
		padlen += block_size;
	}
	if (ssh_packet->extra_pad) {
		/* will wrap if extra_pad+padlen > 255 */
		ssh_packet->extra_pad  = roundup(ssh_packet->extra_pad, block_size);
		if(ssh_packet->extra_pad == 0)
		{
            return;
		}
		pad = ssh_packet->extra_pad - ((len + padlen) % ssh_packet->extra_pad);
//		zlog_debug(VD_SSH, "packet_send2: adding %d (len %d padlen %d extra_pad %d)",
//		    pad, len, padlen, ssh_packet->extra_pad);
		padlen += pad;
		ssh_packet->extra_pad = 0;
	}
	cp = buffer_append_space(outgoing_packet, padlen);
	if (enc && !send_context->plaintext) {
		/* random padding */
		for (i = 0; i < padlen; i++) {
			if (i % 4 == 0)
				rnd = arc4random();
			cp[i] = rnd & 0xff;
			rnd >>= 8;
		}
	} else {
		/* clear padding */
		memset(cp, 0, padlen);
	}
	/* packet_length includes payload, padding and padding length field */
	packet_length = buffer_len(outgoing_packet) - 4;
	cp = buffer_ptr(outgoing_packet);
	put_u32(cp, packet_length);
	cp[4] = padlen;
	DBG(zlog_debug(VD_SSH, "send: len %d (includes padlen %d)", packet_length+4, padlen));

	/* compute MAC over seqnr and packet(length fields, payload, padding) */
	/* mac从秘钥、数据包序列号、和数据包内容计算出来*/
	if (mac && mac->enabled) {
		macbuf = mac_compute(mac, p_send->seqnr,
		    buffer_ptr(outgoing_packet),
		    buffer_len(outgoing_packet));
		DBG(zlog_debug(VD_SSH, "done calc MAC out #%d", p_send->seqnr));
	}

//	buffer_dump(outgoing_packet);
	/* encrypt packet and append to output buffer. */
	cp = buffer_append_space(output, buffer_len(outgoing_packet));
	cipher_crypt(send_context, cp, buffer_ptr(outgoing_packet),buffer_len(outgoing_packet));

	/* append unencrypted MAC */
	if (mac && mac->enabled)
		buffer_append(output, macbuf, mac->mac_len);

#ifdef PACKET_DEBUG
	fprintf(stderr, "encrypted: ");
	buffer_dump(output);
#endif
	/* increment sequence number for outgoing packets */
	if (++p_send->seqnr == 0)
		zlog_debug(VD_SSH, "outgoing seqnr wraps around");
	if (++p_send->packets == 0)
			fatal("XXX too many packets with same key");
	p_send->blocks += (packet_length + 4) / block_size;
	p_send->bytes += packet_length + 4;
	
	buffer_clear(outgoing_packet);

	if (type == SSH2_MSG_NEWKEYS)
		set_newkeys(MODE_OUT, ssh_packet);
	else if (type == SSH2_MSG_USERAUTH_SUCCESS)
		packet_enable_delayed_compress(ssh_packet);

}

static void
packet_send2(struct ssh_packet *ssh_packet)
{	
//	static int rekeying = 0;
	struct packet *p;
	u_char type, *cp;
	Buffer *outgoing_packet;

	outgoing_packet = &(ssh_packet->outgoing_packet);
	cp = buffer_ptr(outgoing_packet);
	type = cp[5];

	/* during rekeying we can only send key exchange messages */
	if (ssh_packet->rekeying) {
		zlog_debug(VD_SSH, "rekeying\n");
		if (!((type >= SSH2_MSG_TRANSPORT_MIN) &&
		    (type <= SSH2_MSG_TRANSPORT_MAX))) {
			zlog_debug(VD_SSH, "enqueue packet: %u", type);
			p = xmalloc(sizeof(*p));
			p->type = type;
			memcpy(&p->payload, outgoing_packet, sizeof(Buffer));
			buffer_init(outgoing_packet);
			TAILQ_INSERT_TAIL(&outgoing, p, next);
			return;
		}
	}

	/* rekeying starts with sending KEXINIT */
	if (type == SSH2_MSG_KEXINIT)
	{
		/*秘钥交换第一个包为SSH2_MSG_KEXINIT，后面的秘钥交换都会处理rekey状态*/
		ssh_packet->rekeying = 1;
	}

	/*主要的封包函数*/
	packet_send2_wrapped(ssh_packet);

	/* after a NEWKEYS message we can send the complete queue */
	/*每一方发送一个SSH2_MSG_NEWKEYS消息后结束密钥交换*/
	if (type == SSH2_MSG_NEWKEYS) {
		ssh_packet->rekeying = 0;
		zlog_debug(VD_SSH, "SSH2_MSG_NEWKEYS, rekeying up\n");
		while ((p = TAILQ_FIRST(&outgoing))) {
			type = p->type;
			zlog_debug(VD_SSH, "dequeue packet: %u", type);
			ssh_buffer_free(outgoing_packet);
			memcpy(outgoing_packet, &p->payload,
			    sizeof(Buffer));
			TAILQ_REMOVE(&outgoing, p, next);
			xfree(p);
			packet_send2_wrapped(ssh_packet);
		}
	}
}

void
packet_send(struct ssh_packet *ssh_packet)
{
//	if (compat20)
		packet_send2(ssh_packet);
//	else
//		packet_send1();
//	DBG(zlog_debug(VD_SSH, "packet_send done"));
}


int
packet_read_seqnr_ipc(u_int32_t *seqnr_p, struct ssh_packet *ssh_packet)
{
	int type;
	struct login_session *session;
	char buf[8192];
	int len;
//	struct timeval timeout, start, *timeoutp = NULL;

	/* Since we are blocking, ensure that all written packets have been sent. */
	/* 首先在这里发送output中的所有分组 */
	packet_write_poll_ipc(ssh_packet);
//	packet_read_poll2(NULL, ssh_packet);


	session = ssh_packet->session;

	for(;;)
	{
		if(session->session_status == SESSION_CLOSE)
		{
			channel_close_all(ssh_packet);
			pthread_exit(NULL);
		}
		type = packet_read_poll_seqnr(seqnr_p, ssh_packet);
		/* If we got a packet, return it. */
		/*在下面读，读完后在这里return ret*/
		if (type != SSH_MSG_NONE) 
		{
			return type;
		}
		memset(buf, 0, sizeof(buf));
		len = sshd_ipc_read(session,buf, sizeof(buf));
		if(len > 0)
		{
			buffer_append(&ssh_packet->input, buf, len);
		}
		/* Try to read a packet from the buffer. */
	usleep(500);
	}
#if 0
	if (!compat20 && (
	    type == SSH_SMSG_SUCCESS
	    || type == SSH_SMSG_FAILURE
	    || type == SSH_CMSG_EOF
	    || type == SSH_CMSG_EXIT_CONFIRMATION))
		packet_check_eom();
#endif
	/* If we got a packet, return it. */
	if (type != SSH_MSG_NONE) {
		return type;
			}
}

/*
 * Waits until a packet has been received, and returns its type.  Note that
 * no other data is processed until this returns, so this function should not
 * be used during the interactive session.
 * 等待直到接收到分组，并返回其类型。 请注意，在返回之前不会处理其他数据，因此
 * 在交互式会话期间不应使用此函数。
 */
int
packet_read_seqnr(u_int32_t *seqnr_p, struct ssh_packet *ssh_packet)
{
	int type, len, ret, ms_remain;
	fd_set *setp;
	char buf[8192];
	struct timeval timeout, start, *timeoutp = NULL;
	struct login_session *session_tmp;
	session_tmp = ssh_packet->session;


	DBG(zlog_debug(VD_SSH, "packet_read()"));

	setp = (fd_set *)xcalloc(howmany(connection_in+1, NFDBITS),
	    sizeof(fd_mask));

	/* Since we are blocking, ensure that all written packets have been sent. */
	/* 首先在这里发送output中的所有分组 */
	packet_write_poll();

	/* Stay in the loop until we have received a complete packet. */
	for (;;) {
		/* Try to read a packet from the buffer. */
		type = packet_read_poll_seqnr(seqnr_p, ssh_packet);
#if 0
		if (!compat20 && (
		    type == SSH_SMSG_SUCCESS
		    || type == SSH_SMSG_FAILURE
		    || type == SSH_CMSG_EOF
		    || type == SSH_CMSG_EXIT_CONFIRMATION))
			packet_check_eom();
#endif
		/* If we got a packet, return it. */
		/*在下面读，读完后在这里return ret*/
		if (type != SSH_MSG_NONE) {
			xfree(setp);
			return type;
		}
		/*
		 * Otherwise, wait for some data to arrive, add it to the
		 * buffer, and try again.
		 */
		memset(setp, 0, howmany(connection_in + 1, NFDBITS) *
		    sizeof(fd_mask));
		FD_SET(connection_in, setp);

		if (packet_timeout_ms > 0) {
			ms_remain = packet_timeout_ms;
			timeoutp = &timeout;
		}
		/* Wait for some data to arrive. */
		for (;;) {
			if (packet_timeout_ms != -1) {
				ms_to_timeval(&timeout, ms_remain);
				gettimeofday(&start, NULL);
			}
			if ((ret = select(connection_in + 1, setp, NULL,
			    NULL, timeoutp)) >= 0)
				break;
		   	if (errno != EAGAIN && errno != EINTR &&
			    errno != EWOULDBLOCK)
				break;
			if (packet_timeout_ms == -1)
				continue;
			ms_subtract_diff(&start, &ms_remain);
			if (ms_remain <= 0) {
				ret = 0;
				break;
			}
		}
		if (ret == 0) {
//			zlog_debug(VD_SSH, "Connection to %.200s timed out while "
//			    "waiting to read", get_remote_ipaddr());
			cleanup_exit(255);
		}
		/* Read data from the socket. */
		len = read(connection_in, buf, sizeof(buf));
		if (len == 0) {
//			zlog_debug(VD_SSH, "Connection closed by %.200s", get_remote_ipaddr());
			cleanup_exit(255);
		}
		if (len < 0)
			fatal("Read from socket failed: %.100s", strerror(errno));
		/* Append it to the buffer. */
		/* 将收到的数据放到input全局buf中，在大循环刚开始的函数packet_read_poll_seqnr中
		 * 读取input buf并处理解密到incoming_packet中*/
		packet_process_incoming(buf, len);
	}
	/* NOTREACHED */
}

int
packet_read(struct ssh_packet *ssh_packet)
{
	return packet_read_seqnr_ipc(NULL, ssh_packet);
}

/*
 * Waits until a packet has been received, verifies that its type matches
 * that given, and gives a fatal error and exits if there is a mismatch.
 */

void
packet_read_expect(int expected_type, struct ssh_packet *ssh_packet)
{
	int type;
	type = packet_read(ssh_packet);
	if (type != expected_type)
	{
		packet_disconnect(ssh_packet, "Protocol error: expected packet type %d, got %d\n",
		    expected_type, type);
		}
}

/* Checks if a full packet is available in the data received so far via
 * packet_process_incoming.  If so, reads the packet; otherwise returns
 * SSH_MSG_NONE.  This does not wait for data from the connection.
 *
 * SSH_MSG_DISCONNECT is handled specially here.  Also,
 * SSH_MSG_IGNORE messages are skipped by this function and are never returned
 * to higher levels.
 */
#if 0
static int
packet_read_poll1(void)
{
	u_int len, padded_len;
	u_char *cp, type;
	u_int checksum, stored_checksum;

	/* Check if input size is less than minimum packet size. */
	if (buffer_len(&input) < 4 + 8)
		return SSH_MSG_NONE;
	/* Get length of incoming packet. */
	cp = buffer_ptr(&input);
	len = get_u32(cp);
	if (len < 1 + 2 + 2 || len > 256 * 1024)
		packet_disconnect("Bad packet length %u.", len);
	padded_len = (len + 8) & ~7;

	/* Check if the packet has been entirely received. */
	if (buffer_len(&input) < 4 + padded_len)
		return SSH_MSG_NONE;

	/* The entire packet is in buffer. */

	/* Consume packet length. */
	buffer_consume(&input, 4);

	/*
	 * Cryptographic attack detector for ssh
	 * (C)1998 CORE-SDI, Buenos Aires Argentina
	 * Ariel Futoransky(futo@core-sdi.com)
	 */
	if (!receive_context.plaintext) {
		switch (detect_attack(buffer_ptr(&input), padded_len)) {
		case DEATTACK_DETECTED:
			packet_disconnect("crc32 compensation attack: "
			    "network attack detected");
		case DEATTACK_DOS_DETECTED:
			packet_disconnect("deattack denial of "
			    "service detected");
		}
	}

	/* Decrypt data to incoming_packet. */
	buffer_clear(&incoming_packet);
	cp = buffer_append_space(&incoming_packet, padded_len);
	cipher_crypt(&receive_context, cp, buffer_ptr(&input), padded_len);

	buffer_consume(&input, padded_len);

#ifdef PACKET_DEBUG
	fprintf(stderr, "read_poll plain: ");
	buffer_dump(&incoming_packet);
#endif

	/* Compute packet checksum. */
	checksum = ssh_crc32(buffer_ptr(&incoming_packet),
	    buffer_len(&incoming_packet) - 4);

	/* Skip padding. */
	buffer_consume(&incoming_packet, 8 - len % 8);

	/* Test check bytes. */
	if (len != buffer_len(&incoming_packet))
		packet_disconnect("packet_read_poll1: len %d != buffer_len %d.",
		    len, buffer_len(&incoming_packet));

	cp = (u_char *)buffer_ptr(&incoming_packet) + len - 4;
	stored_checksum = get_u32(cp);
	if (checksum != stored_checksum)
		packet_disconnect("Corrupted check bytes on input.");
	buffer_consume_end(&incoming_packet, 4);

	if (packet_compression) {
		buffer_clear(&compression_buffer);
		buffer_uncompress(&incoming_packet, &compression_buffer);
		buffer_clear(&incoming_packet);
		buffer_append(&incoming_packet, buffer_ptr(&compression_buffer),
		    buffer_len(&compression_buffer));
	}
	p_read.packets++;
	p_read.bytes += padded_len + 4;
	type = buffer_get_char(&incoming_packet);
	if (type < SSH_MSG_MIN || type > SSH_MSG_MAX)
		packet_disconnect("Invalid ssh1 packet type: %d", type);
	return type;
}
#endif

int
packet_read_poll2(u_int32_t *seqnr_p, struct ssh_packet *ssh_packet)
{
	struct login_session *session_tmp = NULL;
	u_int padlen, need;
	u_char *macbuf, *cp, type;
	u_int maclen, block_size;
	Enc *enc   = NULL;
	Mac *mac   = NULL;
	Comp *comp = NULL;

	if (ssh_packet->newkeys[MODE_IN] != NULL) {
		enc  = &ssh_packet->newkeys[MODE_IN]->enc;
		mac  = &ssh_packet->newkeys[MODE_IN]->mac;
		comp = &ssh_packet->newkeys[MODE_IN]->comp;
	}
	maclen = mac && mac->enabled ? mac->mac_len : 0;
	/*包长(除去mac的部分)应为加密分块或8的整数倍*/
	block_size = enc ? enc->block_size : 8;

	if (ssh_packet->packet_length == 0) {
		/*
		 * check if input size is less than the cipher block size,
		 * decrypt first block and extract length of incoming packet
		 */
		if (buffer_len(&ssh_packet->input) < block_size)
			{
			return SSH_MSG_NONE;
			}
		session_tmp = ssh_packet->session;
		buffer_clear(&ssh_packet->incoming_packet);
		/*增加空间，返回指针*/
		cp = buffer_append_space(&ssh_packet->incoming_packet, block_size);
		/*将input中的报文解密到incoming_packet中*/
		cipher_crypt(&ssh_packet->receive_context, cp, buffer_ptr(&ssh_packet->input),block_size);
		/*更新指针到包头，即第一个字节所处的位置*/
		cp = buffer_ptr(&ssh_packet->incoming_packet);
		/*第一部分uint32长度的数据表示packet_length，获取packet长度*/
		/* uint32 		packet_length //不包括mac和自身
		 * byte 	 		padding_length
		 * byte[n1]	payload
		 * byte[n2]	random padding
		 * byte[m]		mac
		 */
		ssh_packet->packet_length = get_u32(cp);
		if (ssh_packet->packet_length < 1 + 4 || ssh_packet->packet_length > PACKET_MAX_SIZE) {
			
#ifdef PACKET_DEBUG
			buffer_dump(&ssh_packet->incoming_packet);
#endif
			printf("Bad packet length %u.", ssh_packet->packet_length);
			packet_start_discard(enc, mac, ssh_packet->packet_length,
			    PACKET_MAX_SIZE);
			return SSH_MSG_NONE;
		}
		DBG(zlog_debug(VD_SSH, "input: packet len %u", packet_length1+4));
		/*更新input指针，后移block_size长度*/
		buffer_consume(&ssh_packet->input, block_size);
	}

	/* we have a partial packet of block_size bytes */
	/* 读完block_size长度后，剩下除mac外的所有长度 */
	need = 4 + ssh_packet->packet_length - block_size;
	if (need % block_size != 0) {
		zlog_debug(VD_SSH, "padding error: need %d block %d mod %d",
		    need, block_size, need % block_size);
		packet_start_discard(enc, mac, ssh_packet->packet_length,
		    PACKET_MAX_SIZE - block_size);
		return SSH_MSG_NONE;
	}

	/*
	 * check if the entire packet has been received and
	 * decrypt into incoming_packet
	 */
	if (buffer_len(&ssh_packet->input) < need + maclen)
		{
		return SSH_MSG_NONE;
		}
#ifdef PACKET_DEBUG
	fprintf(stderr, "read_poll enc/full: ");
	buffer_dump(&ssh_packet->input);
#endif
	cp = buffer_append_space(&ssh_packet->incoming_packet, need);
	cipher_crypt(&ssh_packet->receive_context, cp, buffer_ptr(&ssh_packet->input), need);
	buffer_consume(&ssh_packet->input, need);
	
	/*
	 * compute MAC over seqnr and packet,
	 * increment sequence number for incoming packet
	 */
	if (mac && mac->enabled) {
		macbuf = mac_compute(mac, ssh_packet->p_read.seqnr,
		    buffer_ptr(&ssh_packet->incoming_packet),
		    buffer_len(&ssh_packet->incoming_packet));
		if (memcmp(macbuf, buffer_ptr(&ssh_packet->input), mac->mac_len) != 0) {
			zlog_debug(VD_SSH, "Corrupted MAC on input.");
			if (need > PACKET_MAX_SIZE)
				fatal("internal error need %d", need);
			packet_start_discard(enc, mac, ssh_packet->packet_length,
			    PACKET_MAX_SIZE - need);
			return SSH_MSG_NONE;
		}
				
		DBG(zlog_debug(VD_SSH, "MAC #%d ok", ssh_packet->p_read.seqnr));
		buffer_consume(&ssh_packet->input, mac->mac_len);
	}
	/* XXX now it's safe to use fatal/packet_disconnect */
	if (seqnr_p != NULL)
		*seqnr_p = ssh_packet->p_read.seqnr;
	if (++ssh_packet->p_read.seqnr == 0)
		zlog_debug(VD_SSH, "incoming seqnr wraps around");
	if (++ssh_packet->p_read.packets == 0)
			fatal("XXX too many packets with same key");
	ssh_packet->p_read.blocks += (ssh_packet->packet_length + 4) / block_size;
	ssh_packet->p_read.bytes += ssh_packet->packet_length + 4;

	/* get padlen */
	cp = buffer_ptr(&ssh_packet->incoming_packet);
	padlen = cp[4];
	DBG(zlog_debug(VD_SSH, "input: padlen %d", padlen));
	if (padlen < 4)
		{
		
		packet_disconnect(ssh_packet, "Corrupted padlen %d on input.", padlen);
		}

	/* skip packet size + padlen, discard padding */
	buffer_consume(&ssh_packet->incoming_packet, 4 + 1);
	buffer_consume_end(&ssh_packet->incoming_packet, padlen);

	zlog_debug(VD_SSH, "input: len before de-compress %d", buffer_len(&ssh_packet->incoming_packet));
	if (comp && comp->enabled) {
		buffer_clear(&ssh_packet->compression_buffer);
		buffer_uncompress(&ssh_packet->incoming_packet, &ssh_packet->compression_buffer, ssh_packet);
		buffer_clear(&ssh_packet->incoming_packet);
		buffer_append(&ssh_packet->incoming_packet, buffer_ptr(&ssh_packet->compression_buffer),
		    buffer_len(&ssh_packet->compression_buffer));
		zlog_debug(VD_SSH, "input: len after de-compress %d",
		    buffer_len(&ssh_packet->incoming_packet));
	}
	/*
	 * get packet type, implies consume.
	 * return length of payload (without type field)
	 */
	type = buffer_get_char(&ssh_packet->incoming_packet);
	if (type < SSH2_MSG_MIN || type >= SSH2_MSG_LOCAL_MIN)
	{
		packet_disconnect(ssh_packet, "Invalid ssh2 packet type: %d", type);
	}
	if (type == SSH2_MSG_NEWKEYS)
	{
		set_newkeys(MODE_IN, ssh_packet);
	}
//	else if (type == SSH2_MSG_USERAUTH_SUCCESS && !server_side)
//		packet_enable_delayed_compress(ssh_packet);
#ifdef PACKET_DEBUG
	fprintf(stderr, "read/plain[%d]:\r\n", type);
	buffer_dump(&ssh_packet->incoming_packet);
#endif
	/* reset for next packet */
	ssh_packet->packet_length = 0;
	return type;
}

int
packet_read_poll_seqnr(u_int32_t *seqnr_p, struct ssh_packet *ssh_packet)
{
	u_int reason, seqnr;
	u_char type;
	char *msg;
	struct login_session *session;

	for (;;) {
			type = packet_read_poll2(seqnr_p, ssh_packet);
			if (type) {
				ssh_packet->keep_alive_timeouts = 0;
				DBG(zlog_debug(VD_SSH, "received packet type %d", type));
			}
			switch (type) {
			case SSH2_MSG_IGNORE:
				zlog_debug(VD_SSH, "Received SSH2_MSG_IGNORE");
				break;
			case SSH2_MSG_DEBUG:
				packet_get_char(ssh_packet);
				msg = packet_get_string(NULL, ssh_packet);
				zlog_debug(VD_SSH, "Remote: %.900s", msg);
				xfree(msg);
				msg = packet_get_string(NULL, ssh_packet);
				xfree(msg);
				break;
			case SSH2_MSG_DISCONNECT:
				session = ssh_packet->session;
				reason = packet_get_int(ssh_packet);
				msg = packet_get_string(NULL, ssh_packet);
				zlog_debug(VD_SSH, "Received disconnect %u: %.400s",reason, msg);
				xfree(msg);

				#if 0
					/* close all channels. */
				channel_close_all(ssh_packet);
				/* clear kex */
				kex_delete(ssh_packet->xxx_kex);
				/* clear bufs. */
				packet_close(ssh_packet);
				vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);				
				free(ssh_packet);
				#endif    // xiawl 
				session->session_status = SESSION_CLOSE;
				channel_close_all(ssh_packet); 
//				pthread_exit(NULL);
			case SSH2_MSG_UNIMPLEMENTED:
				seqnr = packet_get_int(ssh_packet);
				zlog_debug(VD_SSH, "Received SSH2_MSG_UNIMPLEMENTED for %u",
				    seqnr);
				break;
			default:
			{
				return type;
			}
			}
	}
}

/*
 * Buffers the given amount of input characters.  This is intended to be used
 * together with packet_read_poll.
 */

void
packet_process_incoming(const char *buf, u_int len)
{
#if 0
	if (packet_discard) {
		keep_alive_timeouts = 0; /* ?? */
		if (len >= packet_discard)
			packet_stop_discard();
		packet_discard -= len;
		return;
	}
	buffer_append(&input, buf, len);
#endif
}

void
packet_process_incoming_ipc(const char *buf, u_int len, struct ssh_packet *ssh_packet)
{
#if 0
	if (ssh_packet->packet_discard) {
		ssh_packet->keep_alive_timeouts = 0; /* ?? */
		if (len >= ssh_packet->packet_discard)
			packet_stop_discard();
		packet_discard -= len;
		return;
	}
#endif
	buffer_append(&ssh_packet->input, buf, len);
}

/* Returns a character from the packet. */

u_int
packet_get_char(struct ssh_packet *ssh_packet)
{
	char ch;

	buffer_get(&ssh_packet->incoming_packet, &ch, 1);
	return (u_char) ch;
}

/* Returns an integer from the packet data. */

u_int
packet_get_int(struct ssh_packet *ssh_packet)
{
	return buffer_get_int(&ssh_packet->incoming_packet);
}

/*
 * Returns an arbitrary precision integer from the packet data.  The integer
 * must have been initialized before this call.
 */

void
packet_get_bignum(BIGNUM * value)
{
#if 0
	buffer_get_bignum(&incoming_packet, value);
#endif
}

void
packet_get_bignum2(BIGNUM * value, struct ssh_packet *ssh_packet)
{
	buffer_get_bignum2(&ssh_packet->incoming_packet, value);
}

void *
packet_get_raw(u_int *length_ptr, struct ssh_packet *ssh_packet)
{
	u_int bytes = buffer_len(&ssh_packet->incoming_packet);

	if (length_ptr != NULL)
		*length_ptr = bytes;
	return buffer_ptr(&ssh_packet->incoming_packet);
}

int
packet_remaining(struct ssh_packet *ssh_packet)
{
	return buffer_len(&ssh_packet->incoming_packet);
}

/*
 * Returns a string from the packet data.  The string is allocated using
 * xmalloc; it is the responsibility of the calling program to free it when
 * no longer needed.  The length_ptr argument may be NULL, or point to an
 * integer into which the length of the string is stored.
 */

void *
packet_get_string(u_int *length_ptr, struct ssh_packet *ssh_packet)
{
	return buffer_get_string(&(ssh_packet->incoming_packet), length_ptr);
}

void *
packet_get_string_ptr(u_int *length_ptr, struct ssh_packet *ssh_packet)
{
	return buffer_get_string_ptr(&ssh_packet->incoming_packet, length_ptr);
}

/*
 * Sends a diagnostic message from the server to the client.  This message
 * can be sent at any time (but not while constructing another message). The
 * message is printed immediately, but only if the client is being executed
 * in verbose mode.  These messages are primarily intended to ease debugging
 * authentication problems.   The length of the formatted message must not
 * exceed 1024 bytes.  This will automatically call packet_write_wait.
 */
#if 0
void
packet_send_debug(const char *fmt,...)
{
	char buf[1024];
	va_list args;

//	if (compat20 && (datafellows & SSH_BUG_DEBUG))
//		return;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (compat20) {
		packet_start(SSH2_MSG_DEBUG);
		packet_put_char(0);	/* bool: always display */
		packet_put_cstring(buf);
		packet_put_cstring("");
	} else {
		packet_start(SSH_MSG_DEBUG);
		packet_put_cstring(buf);
	}
	packet_send();
	packet_write_wait();
}
#endif
/*
 * Logs the error plus constructs and sends a disconnect packet, closes the
 * connection, and exits.  This function never returns. The error message
 * should not contain a newline.  The length of the formatted message must
 * not exceed 1024 bytes.
 */
#if 0
	void
	packet_disconnect(const char *fmt,...)
	{
	printf("packet_disconnect\n");
	exit(0);
}
#endif

void
packet_disconnect(struct ssh_packet *ssh_packet, const char *fmt,...)
{
	char buf[1024];
	struct login_session *session = ssh_packet->session;
//	uint32_t dip = session->client_ip;
//	uint16_t dport = session->client_port;
	va_list args;

	/*
	 * Format the message.  Note that the caller must make sure the
	 * message is of limited size.
	 */
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	/* Display the error locally */
	printf("Disconnecting: %.100s", buf);

	/* Send the disconnect message to the other side, and wait for it to get sent. */
	packet_start(SSH2_MSG_DISCONNECT, ssh_packet);
	packet_put_int(SSH2_DISCONNECT_PROTOCOL_ERROR, ssh_packet);
	packet_put_cstring(buf, ssh_packet);
	packet_put_cstring("", ssh_packet);
		
	packet_send(ssh_packet);
	packet_write_wait_ipc(ssh_packet);

	/* close all channels. */
	channel_close_all(ssh_packet);

	/* clear kex */
	kex_delete(ssh_packet->xxx_kex);

	/* clear bufs. */
	packet_close(ssh_packet);

	vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);
	
	free(ssh_packet);

    session->tid_ssh_exit = 1;
	session->session_status = SESSION_CLOSE;
	pthread_exit(NULL);
}

/* Checks if there is any buffered output, and tries to write some of the output. */

void
packet_write_poll(void)
{
//int len = buffer_len(&output);
	int len = 0;

	if (len > 0) {
//		len = write(connection_out, buffer_ptr(&output), len);
		if (len == -1) {
			if (errno == EINTR || errno == EAGAIN ||
			    errno == EWOULDBLOCK)
				return;
			fatal("Write failed: %.100s", strerror(errno));
		}
		if (len == 0)
			fatal("Write connection closed");
//		buffer_consume(&output, len);
	}
}

int
packet_write_poll_ipc(struct ssh_packet *ssh_packet)
{
	Buffer *output = &(ssh_packet->output);
	int len = buffer_len(output);
//	int i;
	unsigned char *buf_printf;
	buf_printf = buffer_ptr(output);

	if(len > 0)
	{
		if(sshd_ipc_write(ssh_packet->session, buf_printf, len) == -1)
		{
			return -1;
		}
		buffer_consume(output, len);
		return len;
	}
	return 0;
}

int packet_read_poll_ipc(struct ssh_packet *ssh_packet)
{
	int  cnt,total;
	char buf[1024];
//	Buffer *input = &(ssh_packet->input);

	cnt = 0;
	total = 0;
	do
	{
		memset(buf, 0 ,sizeof(buf));
		cnt = sshd_ipc_read(ssh_packet->session, buf, sizeof(buf));
		buffer_append(&ssh_packet->input, buf, cnt);
		total += cnt;
	}while(cnt != 0);

	return total;
	
}


#if 0
void
packet_write_wait_ipc(struct ssh_packet *ssh_packet)
{
	int ret, ms_remain;
	struct timeval start, timeout, *timeoutp = NULL;

	/*真正的write函数*/
	packet_write_poll_ipc();
}
#endif


void
packet_write_wait_ipc(struct ssh_packet *ssh_packet)
{
	packet_write_poll_ipc(ssh_packet);
}

/*
 * Calls packet_write_poll repeatedly until all pending output data has been
 * written.
 */

void
packet_write_wait(void)
{
	fd_set *setp;
	int ret, ms_remain;
	struct timeval start, timeout, *timeoutp = NULL;

	setp = (fd_set *)xcalloc(howmany(connection_out + 1, NFDBITS),
	    sizeof(fd_mask));
	/*真正的write函数*/
	packet_write_poll();
	/*轮询output中是否仍有数据需要发送*/
	while (packet_have_data_to_write()) {
		memset(setp, 0, howmany(connection_out + 1, NFDBITS) *
		    sizeof(fd_mask));
		FD_SET(connection_out, setp);

		if (packet_timeout_ms > 0) {
			ms_remain = packet_timeout_ms;
			timeoutp = &timeout;
		}
		for (;;) {
			if (packet_timeout_ms != -1) {
				ms_to_timeval(&timeout, ms_remain);
				gettimeofday(&start, NULL);
			}
			/*侦听准备好写的socket*/
			if ((ret = select(connection_out + 1, NULL, setp,
			    NULL, timeoutp)) >= 0)
				break;
		   	if (errno != EAGAIN && errno != EINTR &&
			    errno != EWOULDBLOCK)
				break;
			if (packet_timeout_ms == -1)
				continue;
			ms_subtract_diff(&start, &ms_remain);
			if (ms_remain <= 0) {
				ret = 0;
				break;
			}
		}
		if (ret == 0) {
//			zlog_debug(VD_SSH, "Connection to %.200s timed out while "
//			    "waiting to write", get_remote_ipaddr());
			cleanup_exit(255);
		}
		packet_write_poll();
	}
	xfree(setp);
}

/* Returns true if there is buffered data to write to the connection. */

int
packet_have_data_to_write()
{
#if 0
	return buffer_len(&output) != 0;
#endif
return 0;
}

/* Returns true if there is not too much data to write to the connection. */

int
packet_not_very_much_data_to_write(void)
{
#if 0
	if (interactive_mode)
		return buffer_len(&output) < 16384;
	else
		return buffer_len(&output) < 128 * 1024;
#endif
return 0;
}

#if 0
static void
packet_set_tos(int interactive)
{
#if defined(IP_TOS) && !defined(IP_TOS_IS_BROKEN)
	int tos = interactive ? IPTOS_LOWDELAY : IPTOS_THROUGHPUT;

	if (!packet_connection_is_on_socket() ||
	    !packet_connection_is_ipv4())
		return;
	if (setsockopt(connection_in, IPPROTO_IP, IP_TOS, &tos,
	    sizeof(tos)) < 0)
		error("setsockopt IP_TOS %d: %.100s:",
		    tos, strerror(errno));
#endif
}
#endif
/* Informs that the current session is interactive.  Sets IP flags for that. */

void
packet_set_interactive(int interactive)
{
	static int called = 0;

	if (called)
		return;
	called = 1;

	/* Record that we are in interactive mode. */
	interactive_mode = interactive;

	/* Only set socket options if using a socket.  */
	if (!packet_connection_is_on_socket())
		return;
	set_nodelay(connection_in);
//	packet_set_tos(interactive);
}

/* Returns true if the current connection is interactive. */

int
packet_is_interactive(void)
{
	return interactive_mode;
}

int
packet_set_maxsize(u_int s)
{
	static int called = 0;

	if (called) {
		zlog_debug(VD_SSH, "packet_set_maxsize: called twice: old %d new %d",
		    max_packet_size, s);
		return -1;
	}
	if (s < 4 * 1024 || s > 1024 * 1024) {
		zlog_debug(VD_SSH, "packet_set_maxsize: bad size %d", s);
		return -1;
	}
	called = 1;
	zlog_debug(VD_SSH, "packet_set_maxsize: setting to %d", s);
	max_packet_size = s;
	return s;
}

/* roundup current message to pad bytes */
void
packet_add_padding(u_char pad)
{
	extra_pad = pad;
}

#if 0
/*
 * 9.2.  Ignored Data Message
 *
 *   byte      SSH_MSG_IGNORE
 *   string    data
 *
 * All implementations MUST understand (and ignore) this message at any
 * time (after receiving the protocol version). No implementation is
 * required to send them. This message can be used as an additional
 * protection measure against advanced traffic analysis techniques.
 */
void
packet_send_ignore(int nbytes)
{
	u_int32_t rnd = 0;
	int i;

	packet_start(compat20 ? SSH2_MSG_IGNORE : SSH_MSG_IGNORE);
	packet_put_int(nbytes);
	for (i = 0; i < nbytes; i++) {
		if (i % 4 == 0)
			rnd = arc4random();
		packet_put_char((u_char)rnd & 0xff);
		rnd >>= 8;
	}
}
#endif

#define MAX_PACKETS	(1U<<31)
int
packet_need_rekeying(void)
{
//	if (datafellows & SSH_BUG_NOREKEY)
//		return 0;
	return
	    (p_send.packets > MAX_PACKETS) ||
	    (p_read.packets > MAX_PACKETS) ||
	    (max_blocks_out && (p_send.blocks > max_blocks_out)) ||
	    (max_blocks_in  && (p_read.blocks > max_blocks_in));
}

void
packet_set_rekey_limit(u_int32_t bytes)
{
	rekey_limit = bytes;
}

void
packet_set_server(void)
{
	server_side = 1;
}

void
packet_set_authenticated(void)
{
	after_authentication = 1;
}

void packet_check_eom(struct ssh_packet *ssh_packet)
{
	int len = packet_remaining(ssh_packet); 
	if (len > 0) { 
		printf("Packet integrity error (%d bytes remaining) at %s:%d\n", 
		    len ,__FILE__, __LINE__); 
		packet_disconnect(ssh_packet, "Packet integrity error."); 
		}

}

