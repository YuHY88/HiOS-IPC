/* $OpenBSD: channels.c,v 1.295 2009/02/12 03:00:56 djm Exp $ */
/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains functions for generic socket connection forwarding.
 * There is also code for initiating connection forwarding for X11 connections,
 * arbitrary tcp/ip connections, and the authentication agent connection.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 support added by Markus Friedl.
 * Copyright (c) 1999, 2000, 2001, 2002 Markus Friedl.  All rights reserved.
 * Copyright (c) 1999 Dug Song.  All rights reserved.
 * Copyright (c) 1999 Theo de Raadt.  All rights reserved.
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
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#include <netinet/in.h>

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include "xmalloc.h"
#include "ssh1.h"
#include "ssh2.h"
#include "packet.h"
#include "log.h"
#include "misc.h"
#include "vtysh_sshd.h"
#include "channels.h"
#include "key.h"
#include "pathnames.h"
#include "memtypes.h"
#include "memory.h"
#include <pkt_buffer.h>
#include "vtysh_session.h"
#include <ftm/pkt_tcp.h>
#include "lib/thread.h"
#include "vtysh.h"
#include "kex.h"
#include "vtysh_telnet.h"


/* -- channel core */

/*
 * Pointer to an array containing all allocated channels.  The array is
 * dynamically extended as needed.
 */
//static Channel **channels = NULL;

/*
 * Maximum file descriptor value used in any of the channels.  This is
 * updated in channel_new.
 */
//static int channel_max_fd = 0;


/* -- tcp forwarding */

/*
 * Data structure for storing which hosts are permitted for forward requests.
 * The local sides of any remote forwards are stored in this array to prevent
 * a corrupt remote server from accessing arbitrary TCP/IP ports on our local
 * network (which might be behind a firewall).
 */
typedef struct {
	char *host_to_connect;		/* Connect to 'host'. */
	u_short port_to_connect;	/* Connect to 'port'. */
	u_short listen_port;		/* Remote side should listen port number. */
} ForwardPermission;

/* List of all permitted host/port pairs to connect by the user. */
//static ForwardPermission permitted_opens[SSH_MAX_FORWARDS_PER_DIRECTION];

/* List of all permitted host/port pairs to connect by the admin. */
//static ForwardPermission permitted_adm_opens[SSH_MAX_FORWARDS_PER_DIRECTION];

/* Number of permitted host/port pairs in the array permitted by the user. */
//static int num_permitted_opens = 0;

/* Number of permitted host/port pair in the array permitted by the admin. */
//static int num_adm_permitted_opens = 0;

/*
 * If this is true, all opens are permitted.  This is the case on the server
 * on which we have to trust the client anyway, and the user could do
 * anything after logging in anyway.
 */
//static int all_opens_permitted = 0;


/* -- X11 forwarding */

/* Maximum number of fake X11 displays to try. */
#define MAX_DISPLAYS  1000

/* Saved X11 local (client) display. */
//static char *x11_saved_display = NULL;

/* Saved X11 authentication protocol name. */
//static char *x11_saved_proto = NULL;

/* Saved X11 authentication data.  This is the real data. */
//static char *x11_saved_data = NULL;
//static u_int x11_saved_data_len = 0;

/*
 * Fake X11 authentication data.  This is what the server will be sending us;
 * we should replace any occurrences of this by the real data.
 */
//static u_char *x11_fake_data = NULL;
//static u_int x11_fake_data_len;


/* -- agent forwarding */

#define	NUM_SOCKS	10

/* AF_UNSPEC or AF_INET or AF_INET6 */
//static int IPv4or6 = AF_UNSPEC;

/* helper */
//static void port_open_helper(Channel *c, char *rtype);

/* non-blocking connect helpers */
//static int connect_next(struct channel_connect *);
//static void channel_connect_ctx_free(struct channel_connect *);

/* -- channel core */

Channel *
channel_by_id(int id, struct ssh_packet *ssh_packet)
{
	Channel *c;

	if ((u_int)id < 0 || (u_int)id >= ssh_packet->channels_alloc) {
		return NULL;
	}
	c = ssh_packet->channels[id];
	if (c == NULL) {
		return NULL;
	}
	return c;
}

/*
 * Returns the channel if it is allowed to receive protocol messages.
 * Private channels, like listening sockets, may not receive messages.
 */
Channel *
channel_lookup(int id, struct ssh_packet *ssh_packet)
{
	Channel *c;

	if ((c = channel_by_id(id, ssh_packet)) == NULL)
		return (NULL);

	switch (c->state) {
	case SSH_CHANNEL_LARVAL:
	case SSH_CHANNEL_CONNECTING:
	case SSH_CHANNEL_DYNAMIC:
	case SSH_CHANNEL_OPENING:
	case SSH_CHANNEL_OPEN:
	case SSH_CHANNEL_INPUT_DRAINING:
	case SSH_CHANNEL_OUTPUT_DRAINING:
		return (c);
	}
	printf("Non-public channel %d, type %d.", id, c->state);
	return (NULL);
}

/*
 * Allocate a new channel object and set its type and socket. This will cause
 * remote_name to be freed.
 */
Channel *
channel_new(char *ctype, int type, int rfd, int wfd, int efd,
    u_int window, u_int maxpack, int extusage, char *remote_name, int nonblock, struct ssh_packet *ssh_packet)
{
	int found;
	int i;
	Channel *c;
//	ssh_packet->channels_alloc;

	/*only one channel*/
	if(ssh_packet->channels_alloc != 0)
		return NULL;
	
	/* Do initial allocation if this is the first call. */
	if (ssh_packet->channels_alloc == 0) {
		ssh_packet->channels_alloc = 1;
		ssh_packet->channels = xcalloc(ssh_packet->channels_alloc, sizeof(Channel *));
		for (i = 0; i < ssh_packet->channels_alloc; i++)
			ssh_packet->channels[i] = NULL;
	}
	/* Try to find a free slot where to put the new channel. */
	for (found = -1, i = 0; i < ssh_packet->channels_alloc; i++)
		if (ssh_packet->channels[i] == NULL) {
			/* Found a free slot. */
			found = (int)i;
			break;
		}
//		printf("found:%d\n",found);
	if (found < 0) {
		/* There are no free slots.  Take last+1 slot and expand the array.  */
		found = ssh_packet->channels_alloc;
		if (ssh_packet->channels_alloc > 10000)
			zlog_debug(VD_SSH, "channel_new: internal error: channels_alloc %d "
			    "too big.", ssh_packet->channels_alloc);
		ssh_packet->channels = ssh_xrealloc((void *)ssh_packet->channels, ssh_packet->channels_alloc + 10,
		    sizeof(Channel *));
		ssh_packet->channels_alloc += 10;
		for (i = found; i < ssh_packet->channels_alloc; i++)
			ssh_packet->channels[i] = NULL;
	}
	/* Initialize and return new channel. */
	c = ssh_packet->channels[found] = xcalloc(1, sizeof(Channel));
	c->type = CHANNEL_UNKNOW;
	c->ostate = CHAN_OUTPUT_OPEN;
	c->istate = CHAN_INPUT_OPEN;
	c->flags = 0;
	c->self = found;
	c->state = type;
	c->ctype = ctype;
	c->local_window = window;
	c->local_window_max = window;
	c->local_consumed = 0;
	c->local_maxpacket = maxpack;
	c->remote_id = -1;
	c->remote_name = ssh_xstrdup(remote_name);
	c->remote_window = 0;
	c->remote_maxpacket = 0;
	c->force_drain = 0;
	c->single_connection = 0;
	c->buf_ipc_recv = buffer_new(PKT_BUFFER_SIZE2);
	c->read_bufdata = NULL;
	c->recv_bufdata = NULL;
	c->channel_output_data = buffer_new(PKT_BUFFER_SIZE2);
	c->session = ssh_packet->session;
	if(pthread_mutex_init(&c->bufdata_delete_lock, NULL) != 0)
	{
		perror("obuf_lock init failed\n");
		exit(1);
	}
	if(pthread_mutex_init(&c->netconf_data_lock, NULL) != 0)
	{
		perror("obuf_lock init failed\n");
		exit(1);
	}

//	TAILQ_INIT(&c->status_confirms);
	zlog_debug(VD_SSH, "channel %d: new [%s]\n", found, remote_name);
	return c;
}

/* Close channel vty. */
static void
channel_close_vty(Channel *c)
{
	struct vty *vty = c->vty;

#if 0
	if(c->type == CHANNEL_VTY)
	{
		vty_term_close(vty);
		c->vty = NULL;
	}
#endif

	xfree(c);
}

/* Free the channel and close its fd/socket. */
void
channel_free(Channel *c)
{
	if(c->buf_ipc_recv)
	{
		buffer_free(c->buf_ipc_recv);
		c->buf_ipc_recv = NULL;
	}
	c->read_bufdata = NULL;
	c->recv_bufdata = NULL;

	if (c->remote_name) 
	{
		xfree(c->remote_name);
		c->remote_name = NULL;
	}
	c->ctype = NULL;

}

void channel_close_all(struct ssh_packet *ssh_packet)
{
	int i;
	Channel *c;

	for (i = 0; i < ssh_packet->channels_alloc; i++)
	{
		if ((c = ssh_packet->channels[i]) != NULL)
		{
			
			c->state = SSH_CHANNEL_CLOSED;
		}
	}
	
	for (i = 0; i < ssh_packet->channels_alloc; i++)
	{
		if ((c = ssh_packet->channels[i]) != NULL)
		{
			/* wait untile channel thread exit */
			while(!c->channel_exit_flag)
				usleep(100);

			if(c->type == CHANNEL_VTY)
				channel_close_vty(ssh_packet->channels[i]);

			if(c->buf_ipc_recv)
				buffer_free(c->channel_output_data);

#if 0
			pthread_mutex_lock(&c->bufdata_delete_lock);
			if(c->buf_ipc_recv)
				buffer_free(c->buf_ipc_recv);

			if(c->channel_output_data)
				buffer_free(c->channel_output_data);

			pthread_mutex_unlock(&c->bufdata_delete_lock);

			pthread_mutex_destroy(&c->bufdata_delete_lock);
#endif

			channel_free(c);
			ssh_packet->channels[c->self] = NULL;
		}
	}
}


/* -- protocol input */

/* put data into channel buffer */
void
channel_input_data(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	int id;
	char *data;
	u_int data_len;
	Channel *c;
	struct vty *vty;

	/* Get the channel number and verify it. */
	id = packet_get_int(ssh_packet);
	c = channel_lookup(id, ssh_packet);
	if (c == NULL)
		packet_disconnect(ssh_packet, "Received data for nonexistent channel %d.", id);

	/* Ignore any data for non-open channels (might happen on close) */
	if (c->state != SSH_CHANNEL_OPEN)
		{
		fprintf(stderr, "ignore nonopen ctype\n");
		return;
		}

//	int i = 0;

	extern struct thread_master *vtysh_master;
	/* Get the data. */
	data = packet_get_string_ptr(&data_len, ssh_packet);

	vty = c->vty;
	if (vty != NULL && vty->v_timeout)
	{
#if 0		
		pthread_mutex_lock (&session_timer_lock);
		if (vty->t_timeout)
		{
			thread_cancel (vty->t_timeout);
			vty->t_timeout = NULL;
		}
		vty->t_timeout = 
			thread_add_timer (vtysh_master, vty_timeout, vty, vty->v_timeout);		
		pthread_mutex_unlock (&session_timer_lock);
#endif		
	}
	
	if(vty != NULL && vty->telnet_recv_tmp == 1)
	{
		if(VTY_BUFSIZ - strlen(vty->buf_tmp) > data_len)
		{
			/*some block place need specific input*/
			memcpy(vty->buf_tmp,data, data_len);
		}
	}
	else
	{
		/* wait here if read thread lock bufdata_delete_lock */
		pthread_mutex_lock (&c->bufdata_delete_lock);
		if(c->bufdata_cnt < BUF_DATA_IPC_MAX)
		{
			c->recv_copy_busy = 1;
			char *buf_tmp = data;
			size_t buf_len_tmp = data_len;
			/*whatever read is using the buf, alloc a new bufdata*/
			/*alloc bufdata until we put all the buf_len data*/
			if(c->read_bufdata == c->recv_bufdata)
			{
	//			printf("##Channel:read_bufdata == recv_bufdata. alloc a bufdata\n");
				while(buf_len_tmp)
				{
					usleep(100);
	//				printf("##Channel:buf_len:%d\n",buf_len_tmp);
					
					c->recv_bufdata =	\
						buffer_put_new_bufdata(c->buf_ipc_recv, buf_tmp, &buf_len_tmp);
					if(buf_len_tmp)
					{
						buf_tmp = buf_tmp + (data_len - buf_len_tmp);							
					}
					if(!c->read_bufdata)
					{
						c->read_bufdata = c->recv_bufdata;
					}
					
					c->bufdata_cnt++;
	//				printf("##Channel:after buffer_put_new_bufdata bufdata_cnt:%d\n", c->bufdata_cnt);
				}
			}
			else
			{
	//			printf("##Channel:put in current bufdata\n");
				/*we put*/
				buffer_put_current_bufdata(c->buf_ipc_recv, data, &buf_len_tmp);
				
				while(buf_len_tmp)
				{
					usleep(100);

					buf_tmp = buf_tmp + (data_len - buf_len_tmp);	
	//								printf("##RECV:bufdata full, char left buf_len_tmp:%d\n",buf_len_tmp);
					c->recv_bufdata =	\
						buffer_put_new_bufdata(c->buf_ipc_recv, buf_tmp, &buf_len_tmp);
	//				printf("##Channel:alloc a new bufferdata\n");

					if(!c->read_bufdata)
					{
						c->read_bufdata = c->recv_bufdata;
	//								printf("##RECV:point read_bufdata to recv_bufdata\n");
					}
					c->bufdata_cnt++;
	//				printf("##Channel:bufdata_cnt:%d\n",c->bufdata_cnt);
				}
			}
			
			c->recv_copy_busy = 0;
		//				session->read_buf_len = buf_len;
		}
		pthread_mutex_unlock (&c->bufdata_delete_lock);
	}
	packet_check_eom(ssh_packet);
}

/*get channel output data from vty->obuf*/
void
channel_output_data(struct ssh_packet *ssh_packet)
{
	int i;
	Channel *c;
	struct vty *vty;
	struct buffer_data *data;
	struct buffer *b;
	unsigned int data_len;

	for(i = 0; i < ssh_packet->channels_alloc; i++)
	{
		c = channel_lookup(i, ssh_packet);
		if(c == NULL)
			continue;

		if(c->state == SSH_CHANNEL_LARVAL)
		{
//			printf("SSH_CHANNEL_LARVAL return from channel_output_data\n");
			return;
		}
		if(c->type == CHANNEL_SUB_NETCONF)
		{
			b = c->channel_output_data;
		}
		else if(c->type == CHANNEL_VTY)
		{
			vty = c->vty;
			b = vty->obuf;
		}
		else
		{
			zlog_err("channel_output_data wrong channel type:%d\n", c->type);
			break;
		}

		if(c->state == SSH_CHANNEL_OPEN && !buffer_empty(b))
		{
//			fprintf(stderr, "send SSH2_MSG_CHANNEL_DATA\n");
			for(data = b->head; data != NULL && data->cp != 0; data = data->next)
			{		
				packet_start(SSH2_MSG_CHANNEL_DATA, ssh_packet);
				packet_put_int(c->remote_id, ssh_packet);
//				int offset = ssh_packet->outgoing_packet.end;

				/*window use finish, need another window adjust*/
				if(c->remote_window == 0)
				{
					return;
				}
				
				data_len = data->cp - data->sp;
				if(data_len > c->remote_window)
				{
					data_len = c->remote_window;
				}
							
				if(data_len > 0)
				{
					packet_put_string(data->data + data->sp, data_len, ssh_packet);

					packet_send(ssh_packet);

					packet_write_poll_ipc(ssh_packet);
					c->remote_window -= data_len;
					data->sp += data_len;
					usleep(1000);
				}
			}
			
			/*lock before delete*/
			if(c->type == CHANNEL_SUB_NETCONF)
				pthread_mutex_lock (&c->netconf_data_lock);	

			/* Free printed buffer data. */
			while(b->head && (b->head->sp == b->head->cp))
			{				
				struct buffer_data *del;
				if (!(b->head = (del = b->head)->next))
					b->tail = NULL;
				XFREE(MTYPE_BUFFER, del);
			}		
  
			if(c->type == CHANNEL_SUB_NETCONF)
				pthread_mutex_unlock (&c->netconf_data_lock);
		}			
	}
}

void
channel_input_oclose(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	int id = packet_get_int(ssh_packet);
	Channel *c = channel_lookup(id, ssh_packet);
	packet_check_eom(ssh_packet);
	if (c == NULL)
		packet_disconnect(ssh_packet, "Received oclose for nonexistent channel %d.", id);

	channel_close(ssh_packet, c);
}

void channel_close(struct ssh_packet *ssh_packet, Channel *c)
{

	/*send SSH2_MSG_CHANNEL_CLOSE to client*/
	packet_start(SSH2_MSG_CHANNEL_CLOSE, ssh_packet);
	packet_put_int(c->remote_id, ssh_packet);
	packet_send(ssh_packet);

	packet_write_wait_ipc(ssh_packet);

	c->state = SSH_CHANNEL_CLOSED;
}

void channel_collect_close(struct ssh_packet *ssh_packet)
{
	int i;
	u_int channel_alive_count = 0;
	Channel *c;
	struct login_session *session = ssh_packet->session;

	if(ssh_packet->channels_alloc == 0)
		return;
	
	for (i = 0; i < ssh_packet->channels_alloc; i++)
	{
		/*find SSH_CHANNEL_CLOSED channel, and close it*/
		if ((c = ssh_packet->channels[i]) != NULL)
		{
			if(c->state == SSH_CHANNEL_CLOSED)
			{
				/* wait untile channel thread exit */
				while(!c->channel_exit_flag)
					usleep(10000);

				if(c->type == CHANNEL_VTY)
					channel_close_vty(ssh_packet->channels[i]);
#if 0
				if(c->buf_ipc_recv)
				{
					buffer_free(c->channel_output_data);
					c->buf_ipc_recv = NULL;
				}
#endif
				channel_free(c);
				ssh_packet->channels[c->self] = NULL;			
			}
			else
				channel_alive_count++;
		}
	}

	/*no channel exist, close tcp*/
	if(channel_alive_count == 0)
	{
		/* clear kex */
		kex_delete(ssh_packet->xxx_kex);
	
		/* clear bufs. */
		packet_close(ssh_packet);
	
//		vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);
		
		free(ssh_packet);
	
//        printf("%s: FREE SSH PACKE , tid_ssh_exit = 1 \n",__FUNCTION__);	
		session->tid_ssh_exit = 1;
//		session->session_status = SESSION_CLOSE;
		pthread_exit(NULL);		
	}

}

/* ARGSUSED */
void channel_input_window_adjust(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	Channel *c;
	int id;
	u_int adjust;

	/* Get the channel number and verify it. */
	id = packet_get_int(ssh_packet);
	c = channel_lookup(id, ssh_packet);

	if (c == NULL) {
		zlog_debug(VD_SSH, "Received window adjust for non-open channel %d.", id);
		return;
	}
	adjust = packet_get_int(ssh_packet);
	packet_check_eom(ssh_packet);
	zlog_debug(VD_SSH, "channel %d: rcvd adjust %u", id, adjust);
	c->remote_window += adjust;
}

int channel_out(Channel *c, const char *format, ...)
{
  va_list args;
  int len = 0;
  int size = 1024;
  char buf[1024];
  char *p = NULL;

	/* Try to write to initial buffer.  */
	va_start (args, format);
	len = vsnprintf (buf, sizeof(buf), format, args);
	va_end (args);

	/* Initial buffer is not enough.  */
	if (len < 0 || len >= size)
	{
		while (1)
		{
	 	 if (len > -1)
			size = len + 1;
	 	 else
			size = size * 2;

	 	 p = XREALLOC (MTYPE_VTY, p, size);
	 	 if (! p)
		return -1;

	 	 va_start (args, format);
	 	 len = vsnprintf (p, size, format, args);
	 	 va_end (args);

		  if (len > -1 && len < size)
			break;
		}
	}

	/* When initial buffer is enough to store all output.  */
	if (! p)
		p = buf;

//	pthread_mutex_lock (&vty->obuf_lock);
	/* Pointer p must point out buffer. */
	buffer_put (c->channel_output_data, (u_char *) p, len);
//	pthread_mutex_unlock (&vty->obuf_lock);

	/* If p is not different with buf, it is allocated buffer.  */
	if (p != buf)
	XFREE (MTYPE_VTY, p);

	return len; 
}


