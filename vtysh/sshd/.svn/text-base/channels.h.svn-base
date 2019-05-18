/* $OpenBSD: channels.h,v 1.98 2009/02/12 03:00:56 djm Exp $ */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */
/*
 * Copyright (c) 1999, 2000, 2001, 2002 Markus Friedl.  All rights reserved.
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

#ifndef CHANNEL_H
#define CHANNEL_H

#include "sys-queue.h"
//#include "buffer_ssh.h"
//#include "vtysh_sshd.h"

struct ssh_packet;


/* Definitions for channel types. */
#define SSH_CHANNEL_OPENING			1	/* waiting for confirmation */
#define SSH_CHANNEL_OPEN			2	/* normal open two-way channel */
#define SSH_CHANNEL_CLOSED			3	/* waiting for close confirmation */
#define SSH_CHANNEL_INPUT_DRAINING	4	/* sending remaining data to conn */
#define SSH_CHANNEL_OUTPUT_DRAINING	5	/* sending remaining data to app */
#define SSH_CHANNEL_LARVAL			6	/* larval session */
#define SSH_CHANNEL_CONNECTING		7
#define SSH_CHANNEL_DYNAMIC			8
#define SSH_CHANNEL_ZOMBIE			9	/* Almost dead. */
#define SSH_CHANNEL_MAX_TYPE		10

struct Channel;
typedef struct Channel Channel;

typedef void channel_callback_fn(int, void *);
typedef int channel_infilter_fn(struct Channel *, char *, int);
typedef void channel_filter_cleanup_fn(int, void *);
typedef u_char *channel_outfilter_fn(struct Channel *, u_char **, u_int *);

/* Channel success/failure callbacks */
typedef void channel_confirm_cb(int, struct Channel *, void *);
typedef void channel_confirm_abandon_cb(struct Channel *, void *);
struct channel_confirm {
	TAILQ_ENTRY(channel_confirm) entry;
	channel_confirm_cb *cb;
	channel_confirm_abandon_cb *abandon_cb;
	void *ctx;
};
TAILQ_HEAD(channel_confirms, channel_confirm);

/* Context for non-blocking connects */
struct channel_connect {
	char *host;
	int port;
	struct addrinfo *ai, *aitop;
};

enum SUB_SYS
{
	SUB_SYS_NONE = 0,
	SUB_SYS_NETCONF,
};

enum CHANNEL_TYPE
{
	CHANNEL_UNKNOW,
	CHANNEL_VTY,
	CHANNEL_SUB_NETCONF,	
};

struct Channel {
	int     type;		/* channel type */
	int		state;		/* channel state*/
	int     self;		/* my own channel identifier */
	int     remote_id;	/* channel identifier for remote peer */
	u_int   istate;		/* input from channel (state of receive half) */
	u_int   ostate;		/* output to channel  (state of transmit half) */
	int     flags;		/* close sent/rcvd */
	int     force_drain;	/* force close on iEOF */
	int     delayed;		/* fdset hack */

	int     host_port;	/* remote port to connect for forwards */
	char   *remote_name;	/* remote hostname */

	u_int	remote_window;
	u_int	remote_maxpacket;
	u_int	local_window;
	u_int	local_window_max;
	u_int	local_consumed;
	u_int	local_maxpacket;
	int     extended_usage;
	int	single_connection;

	char   *ctype;		/* type */

	/* keep boundaries */
	int     		datagram;

	int bufdata_cnt;
	
	int recv_copy_busy;
	/* point to the buffer_data we copy to */
	struct buffer_data *recv_bufdata;	
	/* point to the buffer_data we read from */
	struct buffer_data *read_bufdata;
	/* lock the buffer while free a buffer_buf or the buffer*/
	pthread_mutex_t bufdata_delete_lock;
	struct buffer *buf_ipc_recv;
	
	struct vty *vty;
	void *session;
	
	pthread_mutex_t netconf_data_lock;
	struct buffer *channel_output_data;

	int subsystem;
	int netconf_ipc_id;

	int channel_exit_flag;
};

#define CHAN_EXTENDED_IGNORE		0
#define CHAN_EXTENDED_READ		1
#define CHAN_EXTENDED_WRITE		2

/* default window/packet sizes for tcp/x11-fwd-channel */
#define CHAN_SES_PACKET_DEFAULT	(32*1024)
#define CHAN_SES_WINDOW_DEFAULT	(64*CHAN_SES_PACKET_DEFAULT)
#define CHAN_TCP_PACKET_DEFAULT	(32*1024)
#define CHAN_TCP_WINDOW_DEFAULT	(64*CHAN_TCP_PACKET_DEFAULT)
#define CHAN_X11_PACKET_DEFAULT	(16*1024)
#define CHAN_X11_WINDOW_DEFAULT	(4*CHAN_X11_PACKET_DEFAULT)

/* possible input states */
#define CHAN_INPUT_OPEN			0
#define CHAN_INPUT_WAIT_DRAIN		1
#define CHAN_INPUT_WAIT_OCLOSE		2
#define CHAN_INPUT_CLOSED		3

/* possible output states */
#define CHAN_OUTPUT_OPEN		0
#define CHAN_OUTPUT_WAIT_DRAIN		1
#define CHAN_OUTPUT_WAIT_IEOF		2
#define CHAN_OUTPUT_CLOSED		3

#define CHAN_CLOSE_SENT			0x01
#define CHAN_CLOSE_RCVD			0x02
#define CHAN_EOF_SENT			0x04
#define CHAN_EOF_RCVD			0x08

#define CHAN_RBUF	16*1024

/* check whether 'efd' is still in use */
#define CHANNEL_EFD_INPUT_ACTIVE(c) \
	(compat20 && c->extended_usage == CHAN_EXTENDED_READ && \
	(c->efd != -1 || \
	buffer_len(&c->extended) > 0))
#define CHANNEL_EFD_OUTPUT_ACTIVE(c) \
	(compat20 && c->extended_usage == CHAN_EXTENDED_WRITE && \
	c->efd != -1 && (!(c->flags & (CHAN_EOF_RCVD|CHAN_CLOSE_RCVD)) || \
	buffer_len(&c->extended) > 0))

/* channel management */

Channel	*channel_by_id(int, struct ssh_packet *);
Channel	*channel_lookup(int, struct ssh_packet *);
Channel *channel_new(char *, int, int, int, int, u_int, u_int, int, char *, int, struct ssh_packet *);
void	 channel_set_fds(int, int, int, int, int, int, int, u_int);
void	 channel_free(Channel *);
void	 channel_free_all(void);
void	 channel_stop_listening(void);

void	 channel_send_open(int);
void	 channel_request_start(int, char *, int);
void	 channel_register_cleanup(int, channel_callback_fn *, int);
void	 channel_register_open_confirm(int, channel_callback_fn *, void *);
void	 channel_register_filter(int, channel_infilter_fn *,
    channel_outfilter_fn *, channel_filter_cleanup_fn *, void *);
void	 channel_register_status_confirm(int, channel_confirm_cb *,
    channel_confirm_abandon_cb *, void *);
void	 channel_cancel_cleanup(int);
int	 channel_close_fd(int *);
void	 channel_send_window_changes(void);

/* protocol handler */

void	 channel_input_close(int, u_int32_t, void *);
void	 channel_input_close_confirmation(int, u_int32_t, void *);
void	 channel_input_data(int, u_int32_t, void *, struct ssh_packet *);
void	 channel_input_extended_data(int, u_int32_t, void *);
void	 channel_input_ieof(int, u_int32_t, void *);
void	 channel_input_oclose(int, u_int32_t, void *, struct ssh_packet *ssh_packet);
void	 channel_input_open_confirmation(int, u_int32_t, void *);
void	 channel_input_open_failure(int, u_int32_t, void *);
void	 channel_input_port_open(int, u_int32_t, void *);
void	 channel_input_window_adjust(int, u_int32_t, void *, struct ssh_packet *);
void	 channel_input_status_confirm(int, u_int32_t, void *);

/* file descriptor handling (read/write) */

void	 channel_prepare_select(fd_set **, fd_set **, int *, u_int*, int);
void     channel_after_select(fd_set *, fd_set *);
void     channel_output_poll(void);

int      channel_not_very_much_buffered_data(void);
void     channel_close_all(struct ssh_packet *ssh_packet);
int      channel_still_open(void);
char	*channel_open_message(void);
int	 channel_find_open(void);

/* tcp forwarding */
void	 channel_set_af(int af);
void     channel_permit_all_opens(void);
void	 channel_add_permitted_opens(char *, int);
int	 channel_add_adm_permitted_opens(char *, int);
void	 channel_clear_permitted_opens(void);
void	 channel_clear_adm_permitted_opens(void);
void 	 channel_print_adm_permitted_opens(void);
int      channel_input_port_forward_request(int, int);
Channel	*channel_connect_to(const char *, u_short, char *, char *);
Channel	*channel_connect_by_listen_address(u_short, char *, char *);
int	 channel_request_remote_forwarding(const char *, u_short,
	     const char *, u_short);
int	 channel_setup_local_fwd_listener(const char *, u_short,
	     const char *, u_short, int);
void	 channel_request_rforward_cancel(const char *host, u_short port);
int	 channel_setup_remote_fwd_listener(const char *, u_short, int *, int);
int	 channel_cancel_rport_listener(const char *, u_short);

/* x11 forwarding */

int	 x11_connect_display(void);
int	 x11_create_display_inet(int, int, int, u_int *, int **);
void     x11_input_open(int, u_int32_t, void *);
void	 x11_request_forwarding_with_spoofing(int, const char *, const char *,
	     const char *);
void	 deny_input_open(int, u_int32_t, void *);

/* agent forwarding */

void	 auth_request_forwarding(void);

/* channel close */

int	 chan_is_dead(Channel *, int);
void	 chan_mark_dead(Channel *);

/* channel events */

void	 chan_rcvd_oclose(Channel *);
void	 chan_rcvd_eow(Channel *);	/* SSH2-only */
void	 chan_read_failed(Channel *);
void	 chan_ibuf_empty(Channel *);

void	 chan_rcvd_ieof(Channel *);
void	 chan_write_failed(Channel *);
void	 chan_obuf_empty(Channel *);
extern void channel_output_data(struct ssh_packet *ssh_packet);
extern int	session_input_channel_req(Channel *c, const char *rtype, struct ssh_packet *ssh_packet);
extern void channel_collect_close(struct ssh_packet *ssh_packet);
extern void channel_close(struct ssh_packet *ssh_packet, Channel *c);
extern int channel_out(Channel *c, const char *format, ...);


#endif
