/* $OpenBSD: packet.h,v 1.49 2008/07/10 18:08:11 markus Exp $ */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Interface for the packet protocol functions.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef PACKET_H
#define PACKET_H

#include <termios.h>

#include <openssl/bn.h>

struct ssh_packet;

struct packet_state {
	u_int32_t seqnr;
	u_int32_t packets;
	u_int64_t blocks;
	u_int64_t bytes;
};

void     packet_set_connection(struct ssh_packet *);
void     packet_set_timeout(int, int);
void     packet_set_nonblocking(void);
int      packet_get_connection_in(void);
int      packet_get_connection_out(void);
void     packet_close(struct ssh_packet *);
void	 packet_set_encryption_key(const u_char *, u_int, int);
u_int	 packet_get_encryption_key(u_char *);
void     packet_set_protocol_flags(u_int);
u_int	 packet_get_protocol_flags(void);
void     packet_start_compression(int);
void     packet_set_interactive(int);
int      packet_is_interactive(void);
void     packet_set_server(void);
void     packet_set_authenticated(void);

void     packet_start(u_char, struct ssh_packet *);
void     packet_put_char(int ch, struct ssh_packet *);
void     packet_put_int(u_int value, struct ssh_packet *);
void     packet_put_bignum(BIGNUM * value);
void     packet_put_bignum2(BIGNUM * value, struct ssh_packet *);
void     packet_put_string(const void *buf, u_int len, struct ssh_packet *);
void     packet_put_cstring(const char *str, struct ssh_packet *);
void     packet_put_raw(const void *buf, u_int len, struct ssh_packet *);
void     packet_send(struct ssh_packet *);

int      packet_read(struct ssh_packet *);
void     packet_read_expect(int type, struct ssh_packet *);
int      packet_read_poll(void);
void     packet_process_incoming(const char *buf, u_int len);
int      packet_read_seqnr(u_int32_t *seqnr_p, struct ssh_packet *);
int      packet_read_poll_seqnr(u_int32_t *seqnr_p, struct ssh_packet *);

u_int	 packet_get_char(struct ssh_packet *);
u_int	 packet_get_int(struct ssh_packet *);
void     packet_get_bignum(BIGNUM * value);
void     packet_get_bignum2(BIGNUM * value, struct ssh_packet *);
void	*packet_get_raw(u_int *length_ptr, struct ssh_packet *);
void	*packet_get_string(u_int *length_ptr, struct ssh_packet *);
void	*packet_get_string_ptr(u_int *length_ptr, struct ssh_packet *);
void     packet_disconnect(struct ssh_packet *, const char *fmt,...) __attribute__((format(printf, 2, 3)));
void     packet_send_debug(const char *fmt,...) __attribute__((format(printf, 1, 2)));

void	 set_newkeys(int mode, struct ssh_packet *);
int	 packet_get_keyiv_len(int);
void	 packet_get_keyiv(int, u_char *, u_int);
int	 packet_get_keycontext(int, u_char *);
void	 packet_set_keycontext(int, u_char *);
extern void	 packet_get_state(int, u_int32_t *, u_int64_t *, u_int32_t *, u_int64_t *);
extern void	 packet_set_state(int, u_int32_t, u_int64_t, u_int32_t, u_int64_t);
int	 packet_get_ssh1_cipher(void);
void	 packet_set_iv(int, u_char *);

void     packet_write_poll(void);
void     packet_write_wait(void);
int      packet_have_data_to_write(void);
int      packet_not_very_much_data_to_write(void);

int	 packet_connection_is_on_socket(void);
int	 packet_connection_is_ipv4(void);
int	 packet_remaining(struct ssh_packet *ssh_packet);
void	 packet_send_ignore(int);
void	 packet_add_padding(u_char);

void	 tty_make_modes(int, struct termios *);
void	 tty_parse_modes(int, int *);
extern void packet_check_eom(struct ssh_packet *ssh_packet);
extern int packet_write_poll_ipc(struct ssh_packet *ssh_packet);
extern void packet_write_wait_ipc(struct ssh_packet *ssh_packet);
extern int packet_read_seqnr_ipc(u_int32_t *seqnr_p, struct ssh_packet *ssh_packet);
extern int packet_read_poll_ipc(struct ssh_packet *ssh_packet);
extern int packet_read_poll2(u_int32_t *seqnr_p, struct ssh_packet *ssh_packet);
extern void packet_process_incoming_ipc(const char *buf, u_int len, struct ssh_packet *ssh_packet);
extern void packet_start_ssh(u_char type, struct ssh_packet *ssh_packet);



extern u_int max_packet_size;
extern int keep_alive_timeouts;
int	 packet_set_maxsize(u_int);
#define  packet_get_maxsize() max_packet_size

#if 0
/* don't allow remaining bytes after the end of the message */
#define packet_check_eom() \
do { \
	int _len = packet_remaining(); \
	if (_len > 0) { \
		printf("Packet integrity error (%d bytes remaining) at %s:%d\n", \
		    _len ,__FILE__, __LINE__); \
		packet_disconnect("Packet integrity error."); \
	} \
} while (0)
#endif

int	 packet_need_rekeying(void);
void	 packet_set_rekey_limit(u_int32_t);

#endif				/* PACKET_H */
