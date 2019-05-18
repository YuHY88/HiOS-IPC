/*
 * Virtual terminal [aka TeletYpe] interface routine.
 * Copyright (C) 1997, 98F Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>
#include <lib/types.h>
#include <arpa/telnet.h>
#include <termios.h>
#include <lib/version.h>
#include <lib/linklist.h>
#include <lib/thread.h>
#include "buffer.h"
#include "command.h"
#include "sockunion.h"
#include <lib/memory.h>
#include <lib/memtypes.h>
#include "str.h"
#include "log.h"
#include <lib/prefix.h>
#include "filter.h"
#include "privs.h"
#include "network.h"
#include "inet_ip.h"
#include "aaa_common.h"
#include "msg_ipc.h"
#include <time.h>
#include "pkt_type.h"
#include <lib/pkt_buffer.h>
#include <lib/module_id.h>
#include <lib/errcode.h>
#include <lib/vector.h>
#include <lib/log.h>
#include <pthread.h>
#include "vty.h"



/* Vty events */
enum event
{
  VTY_SERV,
  VTY_READ,
  VTY_WRITE,
  VTY_TIMEOUT_RESET,
#ifdef VTYSH
  VTYSH_SERV,
  VTYSH_READ,
  VTYSH_WRITE
#endif /* VTYSH */
};

static void vty_event (enum event, int, struct vty *);

struct host host;

/* Extern host structure from command.c */
//extern struct host host;

/* Vector which store each vty structure. */
volatile vector vtyvec;

/* Vector which store each slave sync vty structure. */
volatile vector vtyvec_sync;
/* Vty timeout value. */
unsigned long vty_timeout_val = VTY_TIMEOUT_DEFAULT;

/* Vty access-class command */
static char *vty_accesslist_name = NULL;

/* VTY server thread. */
static vector Vvty_serv_thread;

/* Current directory. */
char *vty_cwd = NULL;

/* Restrict unauthenticated logins? */
static const u_char restricted_mode_default = 0;

vector telvec;

#if 0
/*WARINING: Remmber to free memory after this function*/
static char *vty_buf_out(const char *format, va_list args)
{
	int len;
	int size = 1024;
	char buf[1024];
	char *p = NULL;	
	len = vsnprintf (buf, sizeof(buf), format, args);

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
				return NULL;

			len = vsnprintf (p, size, format, args);

			if (len > -1 && len < size)
				break;
		}
	}

	/* When initial buffer is enough to store all output.	*/
	if (! p)
		p = buf;

	return p;
}

int vty_error_out(struct vty *vty, const char *format, ...)
{
	char *buf = NULL;
	int len = 0;
	va_list args;

	vty_out(vty, "ERROR: ");
	
	va_start (args, format);
	buf = vty_buf_out(format, args);
	va_end (args);
	
	len = vty_out(vty, buf);
	
	if (strlen(buf) > 1024)
		XFREE (MTYPE_VTY, buf);
	return len;
}

int vty_warning_out(struct vty *vty, const char *format, ...)
{
	char *buf = NULL;
	int len = 0;
	va_list args;

	vty_out(vty, "WARNING: ");
	
	va_start (args, format);
	buf = vty_buf_out(format, args);
	va_end (args);
	
	len = vty_out(vty, buf);
	
	if (strlen(buf) > 1024)
		XFREE (MTYPE_VTY, buf);
	return len;
}

int vty_info_out(struct vty *vty, const char *format, ...)
{
	char *buf = NULL;
	int len = 0;
	va_list args;

	vty_out(vty, "INFO: ");
	
	va_start (args, format);
	buf = vty_buf_out(format, args);
	va_end (args);
	
	len = vty_out(vty, buf);
	
	if (strlen(buf) > 1024)
		XFREE (MTYPE_VTY, buf);
	return len;
}
#endif

int vty_warning_out(struct vty *vty, const char *format, ...)
{	
	va_list args;
	int len = 0;
	int size = 1024;
	char buf[1024];
	char *p = NULL;
	
	if(vty->type == VTY_SNMP || vty->type == VTY_OFP || vty->type == VTY_OSPF|| vty->type == VTY_SYNC)
		  return 0;

	  vty_out(vty, "WARNING: ");
	  
	if (vty_shell (vty))
	  {
		va_start (args, format);
		vprintf (format, args);
		va_end (args);
	  }
	else
	  {
		/* Try to write to initial buffer.	*/
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
	
			  pthread_mutex_lock (&vty->obuf_lock);
		/* Pointer p must point out buffer. */
		buffer_put (vty->obuf, (u_char *) p, len);
			  pthread_mutex_unlock (&vty->obuf_lock);
	
		/* If p is not different with buf, it is allocated buffer.	*/
		if (p != buf)
	  XFREE (MTYPE_VTY, p);
	  }
	
	  if (vty->type == VTY_SHELL_SERV && vty->t_write == NULL)
		  vty_event (VTYSH_WRITE, vty->fd, vty);
	
	return len;

}

int vty_info_out(struct vty *vty, const char *format, ...)
{	
	va_list args;
	int len = 0;
	int size = 1024;
	char buf[1024];
	char *p = NULL;
	
	if(vty->type == VTY_SNMP || vty->type == VTY_OFP || vty->type == VTY_OSPF|| vty->type == VTY_SYNC)
		  return 0;

	  vty_out(vty, "INFO: ");
	  
	if (vty_shell (vty))
	  {
		va_start (args, format);
		vprintf (format, args);
		va_end (args);
	  }
	else
	  {
		/* Try to write to initial buffer.	*/
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
	
			  pthread_mutex_lock (&vty->obuf_lock);
		/* Pointer p must point out buffer. */
		buffer_put (vty->obuf, (u_char *) p, len);
			  pthread_mutex_unlock (&vty->obuf_lock);
	
		/* If p is not different with buf, it is allocated buffer.	*/
		if (p != buf)
	  XFREE (MTYPE_VTY, p);
	  }
	
	  if (vty->type == VTY_SHELL_SERV && vty->t_write == NULL)
		  vty_event (VTYSH_WRITE, vty->fd, vty);
	
	return len;

}

int vty_error_out(struct vty *vty, const char *format, ...)
{	
	va_list args;
	int len = 0;
	int size = 1024;
	char buf[1024];
	char *p = NULL;
	
	if(vty->type == VTY_SNMP || vty->type == VTY_OFP || vty->type == VTY_OSPF|| vty->type == VTY_SYNC)
		  return 0;

	  vty_out(vty, "ERROR: ");
	  
	if (vty_shell (vty))
	  {
		va_start (args, format);
		vprintf (format, args);
		va_end (args);
	  }
	else
	  {
		/* Try to write to initial buffer.	*/
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
	
			  pthread_mutex_lock (&vty->obuf_lock);
		/* Pointer p must point out buffer. */
		buffer_put (vty->obuf, (u_char *) p, len);
			  pthread_mutex_unlock (&vty->obuf_lock);
	
		/* If p is not different with buf, it is allocated buffer.	*/
		if (p != buf)
	  XFREE (MTYPE_VTY, p);
	  }
	
	  if (vty->type == VTY_SHELL_SERV && vty->t_write == NULL)
		  vty_event (VTYSH_WRITE, vty->fd, vty);
	
	return len;

}

/* VTY standard output function. */
int
vty_out (struct vty *vty, const char *format, ...)
{
  va_list args;
  int len = 0;
  int size = 1024;
  char buf[1024];
  char *p = NULL;

	if(vty->type == VTY_SNMP || vty->type == VTY_OFP || vty->type == VTY_OSPF|| vty->type == VTY_SYNC)
		return 0;

  if (vty_shell (vty))
    {
      va_start (args, format);
      vprintf (format, args);
      va_end (args);
    }
  else
    {
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

			pthread_mutex_lock (&vty->obuf_lock);
      /* Pointer p must point out buffer. */
      buffer_put (vty->obuf, (u_char *) p, len);
			pthread_mutex_unlock (&vty->obuf_lock);

      /* If p is not different with buf, it is allocated buffer.  */
      if (p != buf)
	XFREE (MTYPE_VTY, p);
    }

	if (vty->type == VTY_SHELL_SERV && vty->t_write == NULL)
		vty_event (VTYSH_WRITE, vty->fd, vty);

  return len;
}

/* Output current time to the vty. */
void
vty_time_print (struct vty *vty, int cr)
{
  char buf [25];

  if (quagga_timestamp(0, buf, sizeof(buf)) == 0)
    {
      zlog (NULL, LOG_INFO, "quagga_timestamp error");
      return;
    }
  if (cr)
    vty_out (vty, "%s\n", buf);
  else
    vty_out (vty, "%s ", buf);

  return;
}

/* Say hello to vty interface. */
void
vty_hello (struct vty *vty)
{
	vty_out (vty, "%s", host.banner);
}

/* Allocate new vty struct. */
struct vty *
vty_new ()
{
  struct vty *new = XCALLOC (MTYPE_VTY, sizeof (struct vty));

  new->obuf = buffer_new(0);	/* Use default buffer size. */
  new->buf = XCALLOC (MTYPE_VTY, VTY_BUFSIZ);
  new->max = VTY_BUFSIZ;
	new->buf_tmp = XCALLOC (MTYPE_VTY, VTY_BUFSIZ);

  return new;
}


int time_cmp_sec( time_t start, time_t stop, uint32_t sec_val )
{
	uint   sec;
	if( stop < start )
		return 1;
	sec	=  stop - start;
	return (sec > sec_val);
}


/* Command execution over the vty interface. */
static int
vty_command (struct vty *vty, char *buf)
{
  int ret;
  vector vline;
  const char *protocolname;
  char *cp;

  /*
   * Log non empty command lines
   */
  cp = buf;
  if (cp != NULL)
    {
      /* Skip white spaces. */
      while (isspace ((int) *cp) && *cp != '\0')
        cp++;
    }

  if (cp != NULL && *cp != '\0')
    {
      unsigned i;
      char	vty_str[VTY_BUFSIZ];
      char        prompt_str[VTY_BUFSIZ];

      /* format the base vty info */
      snprintf(vty_str, sizeof(vty_str), "vty[??]");
      if (vty)
        for (i = 0; i < vector_active (vtyvec); i++)
          if ((vty == vector_slot (vtyvec, i)))
            {
              snprintf(vty_str, sizeof(vty_str), "vty[%d]",
                                                 i);
              break;
            }

      /* format the prompt */
	  if(cmd_prompt (vty->node) == NULL)
	  {
		zlog_err ( "%s[%d] fd %d command [%s] under node [%d] is unknown, return\n", __FUNCTION__, __LINE__,
			vty->fd,
			buf,
			vty->node);
		/* now log the command */
		zlog(NULL, LOG_INFO, "%s%s", vty_str, buf);
		return CMD_SUCCESS;
	  }
      else
      {
      	snprintf(prompt_str, sizeof(prompt_str), cmd_prompt (vty->node), vty_str);
		/* now log the command */
		zlog(NULL, LOG_INFO, "%s%s", prompt_str, buf);
      }

    }
  /* Split readline string up into the vector */
  vline = cmd_make_strvec (buf);

  if (vline == NULL)
    return CMD_SUCCESS;
#if 0
#ifndef CONSUMED_TIME_CHECK
  {
    RUSAGE_T before;
    RUSAGE_T after;
    unsigned long realtime, cputime;

    GETRUSAGE(&before);
#endif /* CONSUMED_TIME_CHECK */
#endif
  ret = cmd_execute_command (vline, vty, NULL, 0);

  /* Get the name of the protocol if any */
  if (zlog_default)
      protocolname = zlog_proto_names[zlog_default->protocol];
  else
      protocolname = zlog_proto_names[ZLOG_NONE];
#if 0
#ifndef CONSUMED_TIME_CHECK
    GETRUSAGE(&after);
    if ((realtime = thread_consumed_time(&after, &before, &cputime)) >
    	CONSUMED_TIME_CHECK)
      /* Warn about CPU hog that must be fixed. */
      zlog_warn("SLOW COMMAND: command took %lums (cpu time %lums): %s",
      		realtime/1000, cputime/1000, buf);
  }
#endif /* CONSUMED_TIME_CHECK */
#endif

  if (ret != CMD_SUCCESS)
    switch (ret)
      {
      case CMD_WARNING:
	if (vty->type == VTY_FILE)
	  vty_out (vty, "Warning...%s", VTY_NEWLINE);
	break;
      case CMD_ERR_AMBIGUOUS:
	vty_warning_out (vty, "[%s] Ambiguous command: %s, node %d %s", protocolname,buf, vty->node, VTY_NEWLINE);
	break;
      case CMD_ERR_NO_MATCH:
	vty_warning_out (vty, "[%s] Unknown command: %s, node %d %s", protocolname, buf, vty->node, VTY_NEWLINE);
	break;
      case CMD_ERR_INCOMPLETE:
	vty_warning_out (vty, "[%s] Command incomplete: %s node %d %s", protocolname, buf, vty->node, VTY_NEWLINE);
	break;
      }
  cmd_free_strvec (vline);

  return ret;
}

void
vty_clear_buf (struct vty *vty)
{
  memset (vty->buf, 0, vty->max);
}

/* Execute current command line. */
static int
vty_execute_apps ( struct vty *vty)
{
  int ret;

  ret = CMD_SUCCESS;

	ret = vty_command (vty, vty->buf);

	vty->cp = vty->length = 0;
  /* Clear command line buffer. */
  vty_clear_buf (vty);

  return ret;
}

/* create vty for stdio */
static struct termios stdio_orig_termios;
static struct vty *stdio_vty = NULL;
static void (*stdio_vty_atclose)(void);

void
vty_stdio_reset (void)
{
  if (stdio_vty)
    {
      tcsetattr (0, TCSANOW, &stdio_orig_termios);
      stdio_vty = NULL;

      if (stdio_vty_atclose)
        stdio_vty_atclose ();
      stdio_vty_atclose = NULL;
    }
}

/* Ensure length of input buffer.  Is buffer is short, double it. */
void
vty_ensure (struct vty *vty, int length)
{
  if (vty->max <= length)
    {
      vty->max *= 2;
      vty->buf = XREALLOC (MTYPE_VTY, vty->buf, vty->max);
    }
}

#ifdef VTYSH
/* For sockaddr_un. */
#include <sys/un.h>

/* VTY shell UNIX domain socket. */
static void
vty_serv_un (const char *path)
{
  int ret;
  int sock, len;
  struct sockaddr_un serv;
  mode_t old_mask;
  struct zprivs_ids_t ids;

  /* First of all, unlink existing socket */
  unlink (path);

  /* Set umask */
  old_mask = umask (0007);

  /* Make UNIX domain socket. */
  sock = socket (AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0)
    {
      zlog_err("Cannot create unix stream socket: %s", safe_strerror(errno));
      return;
    }

  /* Make server socket. */
  memset (&serv, 0, sizeof (struct sockaddr_un));
  serv.sun_family = AF_UNIX;
  strncpy (serv.sun_path, path, strlen (path));
#ifdef HAVE_STRUCT_SOCKADDR_UN_SUN_LEN
  len = serv.sun_len = SUN_LEN(&serv);
#else
  len = sizeof (serv.sun_family) + strlen (serv.sun_path);
#endif /* HAVE_STRUCT_SOCKADDR_UN_SUN_LEN */

  ret = bind (sock, (struct sockaddr *) &serv, len);
  if (ret < 0)
    {
      zlog_err("Cannot bind path %s: %s", path, safe_strerror(errno));
      close (sock);	/* Avoid sd leak. */
      return;
    }

  ret = listen (sock, 5);
  if (ret < 0)
    {
      zlog_err("listen(fd %d) failed: %s", sock, safe_strerror(errno));
      close (sock);	/* Avoid sd leak. */
      return;
    }

  umask (old_mask);

  zprivs_get_ids(&ids);

  if (ids.gid_vty > 0)
    {
      /* set group of socket */
      if ( chown (path, -1, ids.gid_vty) )
        {
          zlog_err ("vty_serv_un: could chown socket, %s",
                     safe_strerror (errno) );
        }
    }

  vty_event (VTYSH_SERV, sock, NULL);
}

/* #define VTYSH_DEBUG 1 */

static int
vtysh_accept (struct thread *thread)
{
  int accept_sock;
  int sock;
  int client_len;
  struct sockaddr_un client;
  struct vty *vty;

  accept_sock = THREAD_FD (thread);

  vty_event (VTYSH_SERV, accept_sock, NULL);

  memset (&client, 0, sizeof (struct sockaddr_un));
  client_len = sizeof (struct sockaddr_un);

  sock = accept (accept_sock, (struct sockaddr *) &client,
		 (socklen_t *) &client_len);

  if (sock < 0)
    {
      zlog_warn ("can't accept vty socket : %s", safe_strerror (errno));
      return -1;
    }

  if (set_nonblocking(sock) < 0)
    {
      zlog_warn ("vtysh_accept: could not set vty socket %d to non-blocking,"
                 " %s, closing", sock, safe_strerror (errno));
      close (sock);
      return -1;
    }

#ifdef VTYSH_DEBUG
  printf ("VTY shell accept\n");
#endif /* VTYSH_DEBUG */

  vty = vty_new ();
	vector_set(vtyvec, vty);
  vty->fd = sock;
  vty->wfd = sock;
  vty->type = VTY_SHELL_SERV;
  vty->node = CONFIG_NODE;

  vty_event (VTYSH_READ, sock, vty);
  vty_event (VTYSH_WRITE, sock, vty);

  return 0;
}

int vtysh_flush(struct vty *vty)
{
  switch (buffer_flush_available(vty->obuf, vty->wfd))
    {
    case BUFFER_PENDING:
      vty_event(VTYSH_WRITE, vty->wfd, vty);
      break;
    case BUFFER_ERROR:
      vty->monitor = 0; /* disable monitoring to avoid infinite recursion */
      zlog_warn("%s: write error to fd %d, closing", __func__, vty->fd);
      buffer_reset(vty->obuf);
      vty_close(vty);
      return -1;
      break;
    case BUFFER_EMPTY:
      break;
    }
  return 0;
}
/* Master of the threads. */
static struct thread_master *vty_master;


static int
vtysh_read (struct thread *thread)
{
  int ret;
  int sock;
  int nbytes;
  struct vty *vty;
  char *p;
  u_char header[4] = {0, 0, 0, 0};
	struct cmd_type recv_line;
	char replay;

  sock = THREAD_FD (thread);
  vty = THREAD_ARG (thread);
  vty->t_read = NULL;

  if ((nbytes = read (sock, &recv_line, sizeof(struct cmd_type))) <= 0)
  {
 //   printf("read cmd:%s\n",buf);
    if (nbytes < 0)
		{
		  if (ERRNO_IO_RETRY(errno))
		    {
		      vty_event (VTYSH_READ, sock, vty);
		      return 0;
		    }
		  vty->monitor = 0; /* disable monitoring to avoid infinite recursion */
		  zlog_warn("%s: read failed on vtysh client fd %d, closing: %s",
			    __func__, sock, safe_strerror(errno));
		}
    buffer_reset(vty->obuf);
    vty_close (vty);
#ifdef VTYSH_DEBUG
    printf ("close vtysh\n");
#endif /* VTYSH_DEBUG */
    return 0;
  }

#ifdef VTYSH_DEBUG
  printf ("line: %.*s\n", nbytes, buf);
#endif /* VTYSH_DEBUG */

	/*vtysh has recv a sig-int, record it*/
	if(recv_line.cmd_flag & SIG_INT_FLAG)
	{
		vty->sig_int_recv = 1;
		/*ret 0:CMD_SUCESS*/
//		header[3] = 0;
//		buffer_put(vty->obuf, header, 4);
//	  if (!vty->t_write && (vtysh_flush(vty) < 0))
//	  {
		  /* Try to flush results; exit if a write error occurs. */
//	 		return 0;
//	  }
		vty_event (VTYSH_READ, sock, vty);
		return 0;
	}
	else if(vty->sig_int_recv == 1)
	{
		vty->sig_int_recv = 0;
	}

	/*check whether we are reading config file*/
	if(recv_line.cmd_flag & CONFIG_READ_FLAG)
	{
		vty->config_read_flag = 1;
	}
	else if(vty->config_read_flag == 1)
	{
		vty->config_read_flag = 0;
	}

	if(recv_line.flush_cp != 0)
		vty->flush_cp = recv_line.flush_cp;

	if((replay = recv_line.cmd_flag & 0x03<<VTY_CMD_REPLAY_BIT))
	{
		vty->cmd_replay = (int)(replay >> VTY_CMD_REPLAY_BIT);
	}
	else
	{
		vty->cmd_replay = 0;
	}

	memcpy(vty->ttyname, recv_line.ttyname, SU_ADDRSTRLEN);
	vty->client_ip = recv_line.client_ip;
	vty->server_ip = recv_line.server_ip;
	vty->client_port = recv_line.client_port;
	vty->server_port = recv_line.server_port;

	

  for (p = recv_line.lines; p < recv_line.lines+strlen(recv_line.lines)+1; p++)
    {
      vty_ensure(vty, vty->length+1);
      vty->buf[vty->length++] = *p;
      if (*p == '\0')
		{
			/*config finish? call callback*/
			if(vty->config_read_flag == 1 \
				&& thread->config_finish != NULL \
				&& !strcmp(vty->buf, "end"))
			{
				thread_add_event (vty_master, (int (*)(struct thread *))(thread->config_finish), NULL, 0);
			}
		  /* Pass this line to parser. */
		  ret = vty_execute_apps (vty);

				if(ret != CMD_CONTINUE && vty->flush_cp != 0)
					vty->flush_cp = 0;
			  /* Note that vty_execute clears the command buffer and resets
			     vty->length to 0. */

			  /* Return result. */
#ifdef VTYSH_DEBUG
			  printf ("result: %d\n", ret);
			  printf ("vtysh node: %d\n", vty->node);
#endif /* VTYSH_DEBUG */

            header[3] = (u_char)ret;
			  buffer_put(vty->obuf, header, 4);

			  if (!vty->t_write && (vtysh_flush(vty) < 0))
			    /* Try to flush results; exit if a write error occurs. */
			    return 0;
			}
    }

  vty_event (VTYSH_READ, sock, vty);

	if (vty->type == VTY_SHELL_SERV && vty->t_write == NULL)
		vty_event (VTYSH_WRITE, vty->fd, vty);


  return 0;
}

static int
vtysh_write (struct thread *thread)
{
  struct vty *vty = THREAD_ARG (thread);

  vty->t_write = NULL;
  vtysh_flush(vty);

  return 0;
}

void
vtysh_return (struct vty *vty, int cmd_flag)
{
	u_char header[4] = {0, 0, 0, 0};

	header[3] = (u_char)cmd_flag;
	buffer_put(vty->obuf, header, 4);
	
	vtysh_flush(vty);

}

#endif /* VTYSH */

/* Determine address family to bind. */
void
vty_serv_sock (const char *path)
{
#ifdef VTYSH
  vty_serv_un (path);
#endif /* VTYSH */
}

/* Close vty interface.  Warning: call this only from functions that
   will be careful not to access the vty afterwards (since it has
   now been freed).  This is safest from top-level functions (called
   directly by the thread dispatcher). */
void
vty_close (struct vty *vty)
{
  int i;

  /* Cancel threads.*/
  if (vty->t_read)
  {
    thread_cancel (vty->t_read);
	vty->t_read = NULL;
  }
  if (vty->t_write)
  {
    thread_cancel (vty->t_write);
	vty->t_write = NULL;
  }
  if (vty->t_timeout)
  {
    //thread_cancel (vty->t_timeout);
	high_pre_timer_delete(vty->t_timeout);
	vty->t_timeout = 0;
  }

  /* Flush buffer. */
  buffer_flush_all (vty->obuf, vty->wfd);

  /* Free input buffer. */
  buffer_free (vty->obuf);

  /* Free command history. */
  for (i = 0; i < VTY_MAXHIST; i++)
    if (vty->hist[i])
      XFREE (MTYPE_VTY_HIST, vty->hist[i]);

  /* Unset vector. */
//  vector_unset (vtyvec, vty->fd);
    vector_obj_unset(vtyvec, vty);
    
	for(int j = 0; j < 50; j++)
	{
		if(vty->connect_fd[j] > 0)
		{
			close (vty->connect_fd[j]);
		}
	}


  /* Close socket. */
  if (vty->fd > 0)
    close (vty->fd);
  else
    vty_stdio_reset ();

  if (vty->buf)
    XFREE (MTYPE_VTY, vty->buf);

  /* OK free vty. */
  XFREE (MTYPE_VTY, vty);
}

/* Async-signal-safe version of vty_log for fixed strings. */
void
vty_log_fixed (char *buf, size_t len)
{
  unsigned int i;
  struct iovec iov[2];

  /* vty may not have been initialised */
  if (!vtyvec)
    return;

  iov[0].iov_base = buf;
  iov[0].iov_len = len;
  iov[1].iov_base = (void *)"\r\n";
  iov[1].iov_len = 2;

  for (i = 0; i < vector_active (vtyvec); i++)
    {
      struct vty *vty;
      if ((vty = vector_slot (vtyvec, i)) != NULL)
	/* N.B. We don't care about the return code, since process is
	   most likely just about to die anyway. */
	writev(vty->wfd, iov, 2);
    }
}


static void
vty_event (enum event event, int sock, struct vty *vty)
{
  struct thread *vty_serv_thread;

  switch (event)
    {
#ifdef VTYSH
    case VTYSH_SERV:
      vty_serv_thread = thread_add_read (vty_master, vtysh_accept, vty, sock);
      vector_set_index (Vvty_serv_thread, sock, vty_serv_thread);
      break;
    case VTYSH_READ:
      vty->t_read = thread_add_read (vty_master, vtysh_read, vty, sock);
      break;
    case VTYSH_WRITE:
      vty->t_write = thread_add_write (vty_master, vtysh_write, vty, sock);
      break;
#endif /* VTYSH */
#if 0
    case VTY_TIMEOUT_RESET:
      if (vty->t_timeout)
			{
			  thread_cancel (vty->t_timeout);
			  vty->t_timeout = NULL;
			}
      if (vty->v_timeout)
			{
			  vty->t_timeout =
			   // thread_add_timer (vty_master, vty_timeout, vty, vty->v_timeout);
			}
      break;
#endif
    default:
	  break;
    }
}

/* Reset all VTY status. */
void
vty_reset ()
{
  unsigned int i;
  struct vty *vty;
  struct thread *vty_serv_thread;

  for (i = 0; i < vector_active (vtyvec); i++)
    if ((vty = vector_slot (vtyvec, i)) != NULL)
    {
		if(vty->obuf)
			buffer_reset (vty->obuf);
			vty->status = VTY_CLOSE;
//	vty_close (vty);
    }

  for (i = 0; i < vector_active (Vvty_serv_thread); i++)
    if ((vty_serv_thread = vector_slot (Vvty_serv_thread, i)) != NULL)
      {
	thread_cancel (vty_serv_thread);
	vty_serv_thread = NULL;
	vector_slot (Vvty_serv_thread, i) = NULL;
        close (i);
      }

  vty_timeout_val = VTY_TIMEOUT_DEFAULT;

  if (vty_accesslist_name)
    {
      XFREE(MTYPE_VTY, vty_accesslist_name);
      vty_accesslist_name = NULL;
    }
}

static void
vty_save_cwd (void)
{
  char cwd[MAXPATHLEN];
  char *c;

  c = getcwd (cwd, MAXPATHLEN);

  if (!c)
    {
      chdir (SYSCONFDIR);
      getcwd (cwd, MAXPATHLEN);
    }

  vty_cwd = XMALLOC (MTYPE_TMP, strlen (cwd) + 1);
  strcpy (vty_cwd, cwd);
}

char *
vty_get_cwd ()
{
  return vty_cwd;
}

int
vty_shell (struct vty *vty)
{
  return vty->type == VTY_SHELL ? 1 : 0;
}

int
vty_shell_serv (struct vty *vty)
{
  return vty->type == VTY_SHELL_SERV ? 1 : 0;
}

void
vty_init_vtysh ()
{
  vtyvec = vector_init (VECTOR_MIN_SIZE);
}

void
vty_vtysh_init(struct thread_master *master_thread)
{
	vty_master = master_thread;
	Vvty_serv_thread = vector_init (VECTOR_MIN_SIZE);
}

/* Install vty's own commands like `who' command. */
void
vty_init (struct thread_master *master_thread)
{
  /* For further configuration read, preserve current directory. */
  vty_save_cwd ();

  vtyvec = vector_init (VECTOR_MIN_SIZE);

  vty_master = master_thread;

  atexit (vty_stdio_reset);

  /* Initilize server thread vector. */
  Vvty_serv_thread = vector_init (VECTOR_MIN_SIZE);
}

void
vty_terminate (void)
{
  if (vty_cwd)
    XFREE (MTYPE_TMP, vty_cwd);

  if (vtyvec && Vvty_serv_thread)
    {
      vty_reset ();
      vector_free (vtyvec);
      vector_free (Vvty_serv_thread);
    }
}

