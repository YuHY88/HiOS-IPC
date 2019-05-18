/* Virtual terminal [aka TeletYpe] interface routine
   Copyright (C) 1997 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _ZEBRA_VTY_H
#define _ZEBRA_VTY_H

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <lib/thread.h>
#include <lib/sockunion.h>
//#include <lib/inet_ip.h>
//#include <lib/prefix.h>


#define DAEMON_MAX 50


/* VTY port number.  */
#define VTYSH_PORT                      23

#define VTY_BUFSIZ 512
#define VTY_CMD_BUFSIZ 256
#define VTY_MAXHIST 100
//#define USER_LOGIN_MAX 9
#define USER_LOGIN_MAX 32


/*flush 60 lines each time*/
#define FLUSH_LINE 60
#define MAX_USERS_COUNT 10


#define VTYSH_HOST_NAME_LEN         64


/*bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
 *bit0 CONFIG_READ_FLAG
 *bit1 SIG_INT_FLAG
 *bit2 
 *bit3 CMD_REPLAY
 *bit4-bit7 unused
 */
#define CONFIG_READ_FLAG_BIT 0
#define CONFIG_READ_FLAG 0x01<<CONFIG_READ_FLAG_BIT
#define SIG_INT_FLAG_BIT     1
#define SIG_INT_FLAG     0x01<<SIG_INT_FLAG_BIT
#define VTY_CMD_REPLAY_BIT		 2
#define VTY_CMD_REPLAY(replay) (replay) << VTY_CMD_REPLAY_BIT

#define BUF_DATA_IPC_MAX 50

/*loop for 60 lines*/
#define FLUSH_LINE_CHECK(line, line_num)\
         if ((line) == ((line_num)+60)) {(line)=0; return CMD_CONTINUE;}\
         if ((((line))<((line_num)+60)) && ((++(line))>line_num))

/* vty 类型 */
enum VTY_TYPE_E
{
    VTY_TERM = 0, 
	VTY_FILE, 
	VTY_SHELL, 
	VTY_SHELL_SERV, 
	VTY_SNMP,
	VTY_OFP,
	VTY_SSH,
	VTY_OSPF,
	VTY_SYNC,
};


/* VTY 状态 */
enum VTY_STATUS_E
{
	VTY_NORMAL = 0, 
	VTY_CLOSE, 
	VTY_MORE, 
	VTY_MORELINE,
};

enum VTY_REPLEY
{
	VTY_REPLAY_NONE = 0, 
	VTY_REPLAY_YES, 
	VTY_REPLAY_NO, 
};


/* VTY struct. */
struct vty 
{
  /* File descripter of this vty. */
  int fd;

  char ttyname[SU_ADDRSTRLEN];

  uint32_t 		 		 client_ip;
  uint32_t 		 		 server_ip;
  uint16_t 				 client_port;
  uint16_t				 server_port;
  char hostname[VTYSH_HOST_NAME_LEN + 1];

  /* output FD, to support stdin/stdout combination */
  int wfd;

  /* Is this vty connect to file or not */
  enum VTY_TYPE_E type;

/*for sync type, on slave slot, this tell us the kind of vty map to main slot*/
  enum VTY_TYPE_E sub_type;

  /* Node status of this vty */
  int node;

  int pre_node;

	/*catch buf*/
  struct list *catch_buf;
	pthread_mutex_t catch_buf_lock;

  /* Failure count */
  int fail;

  struct buffer *obuf;  /* Output buffer. */
	
	pthread_mutex_t obuf_lock;

  uint32_t flush_cp;

	uint32_t flush_local;

  /* Command input buffer */
  char *buf;

  /* Command cursor point */
  int cp;

  /* Command length */
  int length;

  /* Command max length. */
  int max;
  
  char *hist[VTY_MAXHIST]; /* Histry of command */
  int hp;       /* History lookup current point */
  int hindex;   /* History insert end point */
  
  void *index;     /* For user data */  
  void *index_sub; /* For user data */

  /* For escape character. */
  unsigned char escape;

  /* Current vty status. */
  enum VTY_STATUS_E status;

  /* IAC handling: was the last character received the
     IAC (interpret-as-command) escape character (and therefore the next
     character will be the command code)?  Refer to Telnet RFC 854. */
  unsigned char iac;

  /* IAC SB (option subnegotiation) handling */
  unsigned char iac_sb_in_progress;
  /* At the moment, we care only about the NAWS (window size) negotiation,
     and that requires just a 5-character buffer (RFC 1073):
       <NAWS char> <16-bit width> <16-bit height> */
#define TELNET_NAWS_SB_LEN 5
  unsigned char sb_buf[TELNET_NAWS_SB_LEN];
  /* How many subnegotiation characters have we received?  We just drop
     those that do not fit in the buffer. */
  size_t sb_len;

  /* Window width/height. */
  int width;
  int height;

  /* Configure lines. */
  int lines;

  /* Terminal monitor. */
  int monitor;

  /* In configure mode. */
  int config;

  /* Read and write thread. */
  struct thread *t_read;
  struct thread *t_write;

  /* Timeout seconds and thread. */
  unsigned long v_timeout;
  unsigned long t_timeout; // is TIMERID      

	/*vtysh socket fd connect to daemons from for each vty*/
	int connect_fd[DAEMON_MAX];

	fd_set read_app_fd;

	int cmd_process_lock;

	int host_auth;

	int config_read_flag;
	
	/*Prompt character need change at  at vty interface*/
  char change_prompt[VTY_BUFSIZ];	

	/*session of this vty*/
	void *session;

	/*append buf of telnet user recv*/
	char *buf_tmp;

	int telnet_recv_tmp;

	/*used in VTY_MORE state*/
	int continue_flash;

	/*receive sig-int from terminal*/
	int sig_int_recv;

	int cmd_block;

	enum VTY_REPLEY cmd_replay;

	void *channle;

	int privilege;
	int sync_flag;
};


/* Host configuration variable */
struct host
{
	char *device_name;

	char *name;

	/*how many users have been loged in?*/
	int login_cnt;

	/* User max idle time to exit */
	time_t idle_max;

	/* Enable password */
	char *enable;
	char *enable_encrypt;

	/* System wide terminal lines. */
	int lines;

	/* Log filename. */
	char *logfile;

	/* config file name of this host */
  char *config;

	/*banner*/
	char *banner;
};

struct host_users
{
	char *name;
	char *password;
	char *password_encrypt;
	char *enable_password;
	char *enable_password_encrypt;
	int encrypt;
	int privilege;
	
	int login_cnt;  /*whether current user have login*/	
	pthread_mutex_t login_cnt_lock;
};


struct cmd_type
{
	char cmd_flag;
	uint32_t flush_cp;

	char ttyname[SU_ADDRSTRLEN];	//lipf add
	
	uint32_t client_ip;
	uint32_t server_ip;
	uint16_t client_port;
	uint16_t server_port;
	
	char lines[512]; 
};

typedef enum
{
	VTY_MSG_COMMON,
	VTY_MSG_CMD = 1,

}VTY_MSG_SUB_TYPE;

/* Small macro to determine newline is newline only or linefeed needed. */
#define VTY_NEWLINE  ((vty->type == VTY_TERM || vty->type == VTY_SHELL_SERV || vty->type == VTY_SSH) ? "\r\n" : "\n")

/* Default time out value */
#define VTY_TIMEOUT_DEFAULT 1800

/* Vty read buffer size. */
#define VTY_READ_BUFSIZ 512

/* Directory separator. */
#ifndef DIRECTORY_SEP
#define DIRECTORY_SEP '/'
#endif /* DIRECTORY_SEP */

#ifndef IS_DIRECTORY_SEP
#define IS_DIRECTORY_SEP(c) ((c) == DIRECTORY_SEP)
#endif

/* GCC have printf type attribute check.  */
#ifdef __GNUC__
#define PRINTF_ATTRIBUTE(a,b) __attribute__ ((__format__ (__printf__, a, b)))
#else
#define PRINTF_ATTRIBUTE(a,b)
#endif /* __GNUC__ */

/* Utility macros to convert VTY argument to unsigned long */
#define VTY_GET_ULONG(NAME,V,STR) \
do { \
  char *endptr = NULL; \
  errno = 0; \
  (V) = strtoul ((STR), &endptr, 10); \
  if (*(STR) == '-' || *endptr != '\0' || errno) \
    { \
      vty_out (vty, "%% Invalid %s value%s", NAME, VTY_NEWLINE); \
      return CMD_WARNING; \
    } \
} while (0)

/*
 * The logic below ((TMPL) <= ((MIN) && (TMPL) != (MIN)) is
 * done to circumvent the compiler complaining about
 * comparing unsigned numbers against zero, if MIN is zero.
 * NB: The compiler isn't smart enough to supress the warning
 * if you write (MIN) != 0 && tmpl < (MIN).
 */
#define VTY_GET_INTEGER_RANGE_HEART(NAME,TMPL,STR,MIN,MAX)      \
do {                                                            \
  VTY_GET_ULONG(NAME, (TMPL), STR);                             \
  if ( ((TMPL) <= (MIN) && (TMPL) != (MIN)) || (TMPL) > (MAX) ) \
    {                                                           \
      vty_out (vty, "%% Invalid %s value%s", NAME, VTY_NEWLINE);\
      return CMD_WARNING;                                       \
    }                                                           \
} while (0)

#define VTY_GET_INTEGER_RANGE(NAME,V,STR,MIN,MAX)               \
do {                                                            \
  unsigned long tmpl;                                           \
  VTY_GET_INTEGER_RANGE_HEART(NAME,tmpl,STR,MIN,MAX);           \
  (V) = tmpl;                                                   \
} while (0)

#define VTY_CHECK_INTEGER_RANGE(NAME,STR,MIN,MAX)               \
do {                                                            \
  unsigned long tmpl;                                           \
  VTY_GET_INTEGER_RANGE_HEART(NAME,tmpl,STR,MIN,MAX);           \
} while (0)

#define VTY_GET_INTEGER(NAME,V,STR)                             \
    VTY_GET_INTEGER_RANGE(NAME,V,STR,0U,UINT32_MAX)

#define VTY_GET_IPV4_ADDRESS(NAME,V,STR)                                      \
do {                                                                             \
  int retv;                                                                   \
  retv = inet_aton ((STR), &(V));                                             \
  if (!retv)                                                                  \
    {                                                                         \
      vty_out (vty, "%% Invalid %s value%s", NAME, VTY_NEWLINE);              \
      return CMD_WARNING;                                                     \
    }                                                                         \
} while (0)

#define VTY_GET_IPV4_PREFIX(NAME,V,STR)                                       \
do {                                                                             \
  int retv;                                                                   \
  retv = str2prefix_ipv4 ((STR), &(V));                                       \
  if (retv <= 0)                                                              \
    {                                                                         \
      vty_out (vty, "%% Invalid %s value%s", NAME, VTY_NEWLINE);              \
      return CMD_WARNING;                                                     \
    }                                                                         \
} while (0)


/* Prototypes. */
extern void vty_init (struct thread_master *);
extern void vty_terminate (void);
extern void vty_reset (void);
extern struct vty *vty_new (void);
extern struct vty *vty_stdio (void (*atclose)(void));
extern int vty_out (struct vty *, const char *, ...) PRINTF_ATTRIBUTE(2, 3);
extern void vty_time_print (struct vty *, int);
void vty_serv_sock (const char *);
extern void vty_close (struct vty *);
extern char *vty_get_cwd (void);
extern int vty_config_lock (struct vty *);
extern int vty_config_unlock (struct vty *);
extern int vty_shell (struct vty *);
extern int vty_shell_serv (struct vty *);
extern void vty_hello (struct vty *);
extern int vty_flush (struct thread *);
extern int vtysh_flush(struct vty *vty);
extern int is_vty_timeout_set(char **line);
extern int vty_timeout (struct thread *thread);
extern void vty_ensure (struct vty *vty, int length);
extern int vty_telnet_execute ( struct vty *vty);
extern void vty_stdio_reset (void);
extern struct vty *vty_new_init (void);
extern void vty_clear_buf (struct vty *vty);
extern void vty_init_vtysh ( void );
extern void vty_vtysh_init(struct thread_master *master_thread);
extern int time_cmp_sec( time_t start, time_t stop, uint32_t sec_val );
extern void vtysh_return (struct vty *vty, int cmd_flag);
extern int vty_error_out(struct vty *vty, const char *format, ...);
extern int vty_warning_out(struct vty *vty, const char *format, ...);
extern int vty_info_out(struct vty *vty, const char *format, ...);
//extern char *vty_buf_out(const char *format, va_list args);

/* Send a fixed-size message to all vty terminal monitors; this should be
   an async-signal-safe function. */
extern void vty_log_fixed (char *buf, size_t len);
/*以下三个声明,放在LOG.H中头文件冲突*/
extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);
extern void zlog_debug_clear(struct vty *vty, unsigned int type);
extern struct vty* zlog_debug_nextvty(unsigned int *idx);


extern struct host host;

extern unsigned long vty_timeout_val;


#endif /* _ZEBRA_VTY_H */
