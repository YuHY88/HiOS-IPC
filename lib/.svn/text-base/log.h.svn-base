/*
 * Zebra logging funcions.
 * Copyright (C) 1997, 1998, 1999 Kunihiro Ishiguro
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

#ifndef _ZEBRA_LOG_H
#define _ZEBRA_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "syslog.h"



/*
 * Option flags for syslog_open.
 *
 * LOG_ODELAY no longer does anything.
 * LOG_NDELAY is the inverse of what it used to be.
 */
#define ZLOG_NOLOG              0x00
#define LOG_PID     0x01    /* log the pid with each message */
#define LOG_CONS    0x02    /* log on the console if errors in sending */
#define LOG_ODELAY  0x04    /* delay open until first syslog() (default) */
#define LOG_NDELAY  0x08    /* don't delay open */
#define LOG_NOWAIT  0x10    /* don't wait for console forks: DEPRECATED */
#define LOG_PERROR  0x20    /* log to stderr as well */

/* If maxlvl is set to ZLOG_DISABLED, then no messages will be sent
   to that logging destination. */
#define ZLOG_DISABLED       (LOG_EMERG-1)
#define ZLOG_NUM_DESTS      (ZLOG_DEST_FILE+1)


/*need define zlog_proto_names !!*/
typedef enum
{
    ZLOG_NONE,
    ZLOG_DEFAULT,
    ZLOG_ZEBRA,
    ZLOG_RIB,
    ZLOG_RIP,
    ZLOG_BGP,
    ZLOG_OSPF,
    ZLOG_RIPNG,
    ZLOG_BABEL,
    ZLOG_OSPF6,
    ZLOG_ISIS,
    ZLOG_PIM,
    ZLOG_IFM,
    ZLOG_QOS,
    ZLOG_FTM,
    ZLOG_ROUTE,
    ZLOG_MPLS,
    ZLOG_LDP,
    ZLOG_ARP,
    ZLOG_VRRP,
    ZLOG_DHCP,
    ZLOG_HAL,
    ZLOG_HSL,
    ZLOG_SYSLOG,
    ZLOG_DEVM,
    ZLOG_L2,
    ZLOG_MASC,
    ZLOG_NTP,
    ZLOG_FILEM,
    ZLOG_CES,
    ZLOG_AAA,
	ZLOG_STATIS,
    ZLOG_PING,
    ZLOG_SYSTEM,
    ZLOG_CFM,
    ZLOG_SNMPD,
    ZLOG_ALARM,
    ZLOG_VTYSH,
    ZLOG_WEB,
    ZLOG_OPENFLOW,
    ZLOG_IPMC,
    ZLOG_CLOCK,
    ZLOG_DHCP6C,
    ZLOG_RMOND,
    ZLOG_SDHMGT,
    ZLOG_VCG,
} zlog_proto_t;


typedef enum
{
    ZLOG_DEST_SYSLOG = 0,
    ZLOG_DEST_STDOUT,
    ZLOG_DEST_MONITOR,
    ZLOG_DEST_FILE
} zlog_dest_t;



struct zlog
{
    const char *ident;            /* daemon name (first arg to syslog_open) */
    zlog_proto_t protocol;
    int maxlvl[ZLOG_NUM_DESTS];   /* maximum priority to send to associated logging destination */
    int default_lvl;              /* maxlvl to use if none is specified */
    FILE *fp;
    char *filename;
    int facility;         /* as per syslog facility */
    int record_priority;  /* should messages logged through stdio include the priority of the message? */
    int syslog_options;   /* 2nd arg to syslog_open */
    int timestamp_precision;  /* # of digits of subsecond precision */
};


/* Message structure. */
struct message
{
    int key;
    const char *str;
};


/* LIB库的debug类型统一定义如下，占高8位 */
#define ZLOG_LIB_DBG_LIB    (1 << 24)
#define ZLOG_LIB_DBG_PKG    (1 << 25)
#define ZLOG_LIB_DBG_DEVM   (1 << 26)
#define ZLOG_LIB_DBG_IO     (1 << 27)
#define ZLOG_LIB_DBG_ALL    ZLOG_LIB_DBG_LIB | ZLOG_LIB_DBG_PKG | ZLOG_LIB_DBG_DEVM | ZLOG_LIB_DBG_IO

#define ZLOG_LIB_DBG_LIB_NAME   "lib"  /* lib库中除以下debug类型外的，其他debug */
#define ZLOG_LIB_DBG_PKG_NAME   "pkg"  /* lib库中收发包的debug类型 */
#define ZLOG_LIB_DBG_DEVM_NAME  "devm" /* lib库中devm 的debug类型 */
#define ZLOG_LIB_DBG_IO_NAME    "io"   /* lib库中I/O操作相关的debug类型 */

/*=============== end ===============*/

#define LOG_EMERG   0   /* system is unusable */
#define LOG_ALERT   1   /* action must be taken immediately */
#define LOG_CRIT    2   /* critical conditions */
#define LOG_ERR     3   /* error conditions */
#define LOG_WARNING 4   /* warning conditions */
#define LOG_NOTICE  5   /* normal but significant condition */
#define LOG_INFO    6   /* informational */
#define LOG_DEBUG   7   /* debug-level messages */

#define LOG_PRIMASK 0x07    /* mask to extract priority part (internal) */
/* extract priority */
#define LOG_PRI(p)  ((p) & LOG_PRIMASK)
#define LOG_MAKEPRI(fac, pri)   (((fac) << 3) | (pri))


#define INTERNAL_NOPRI  0x10    /* the "no priority" priority */
/* mark "facility" */
#define INTERNAL_MARK   LOG_MAKEPRI(LOG_NFACILITIES, 0)
typedef struct _code
{
    const char  *c_name;
    int c_val;
} CODE;



/* facility codes */
#define LOG_KERN    (0<<3)  /* kernel messages */
#define LOG_USER    (1<<3)  /* random user-level messages */
#define LOG_MAIL    (2<<3)  /* mail system */
#define LOG_DAEMON  (3<<3)  /* system daemons */
#define LOG_AUTH    (4<<3)  /* security/authorization messages */
#define LOG_SYSLOG  (5<<3)  /* messages generated internally by syslogd */
#define LOG_LPR     (6<<3)  /* line printer subsystem */
#define LOG_NEWS    (7<<3)  /* network news subsystem */
#define LOG_UUCP    (8<<3)  /* UUCP subsystem */
#define LOG_CRON    (9<<3)  /* clock daemon */
#define LOG_AUTHPRIV    (10<<3) /* security/authorization messages (private) */
#define LOG_FTP     (11<<3) /* ftp daemon */

/* other codes through 15 reserved for system use */
#define LOG_LOCAL0  (16<<3) /* reserved for local use */
#define LOG_LOCAL1  (17<<3) /* reserved for local use */
#define LOG_LOCAL2  (18<<3) /* reserved for local use */
#define LOG_LOCAL3  (19<<3) /* reserved for local use */
#define LOG_LOCAL4  (20<<3) /* reserved for local use */
#define LOG_LOCAL5  (21<<3) /* reserved for local use */
#define LOG_LOCAL6  (22<<3) /* reserved for local use */
#define LOG_LOCAL7  (23<<3) /* reserved for local use */

#define LOG_NFACILITIES 24  /* current number of facilities */
#define LOG_FACMASK 0x03f8  /* mask to extract facility part */
/* facility of pri */
#define LOG_FAC(p)  (((p) & LOG_FACMASK) >> 3)



/*
 * arguments to setlogmask.
 */
#define LOG_MASK(pri)   (1 << (pri))        /* mask for one priority */
#define LOG_UPTO(pri)   ((1 << ((pri)+1)) - 1)  /* all priorities through pri */

/*
 * Option flags for syslog_open.
 *
 * LOG_ODELAY no longer does anything.
 * LOG_NDELAY is the inverse of what it used to be.
 */
#define LOG_PID     0x01    /* log the pid with each message */
#define LOG_CONS    0x02    /* log on the console if errors in sending */
#define LOG_ODELAY  0x04    /* delay open until first syslog() (default) */
#define LOG_NDELAY  0x08    /* don't delay open */
#define LOG_NOWAIT  0x10    /* don't wait for console forks: DEPRECATED */
#define LOG_PERROR  0x20    /* log to stderr as well */





extern int syslog_debug;  /* zlog_debug �Ŀ��أ�= 0 ʱ���� ipc��ֱ�Ӷ��� */


/* Default logging strucutre. */
extern struct zlog *zlog_default;

/* Open zlog function */
extern struct zlog *openzlog(const char *progname, zlog_proto_t protocol,
                             int syslog_options, int syslog_facility);

/* Close zlog function. */
extern void closezlog(struct zlog *zl);
//extern void log_cmd_init(void);

//extern void syslog_zlog_debug(struct vty *vty, char *is_enable);



/* GCC have printf type attribute check.  */
#ifdef __GNUC__
#define PRINTF_ATTRIBUTE(a,b) __attribute__ ((__format__ (__printf__, a, b)))
#else
#define PRINTF_ATTRIBUTE(a,b)
#endif /* __GNUC__ */

/* Generic function for zlog. */
extern void zlog(struct zlog *zl, int priority, const char *format, ...)
PRINTF_ATTRIBUTE(3, 4);

/* Handy zlog functions. */
extern void zlog_err(const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_warn(const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_info(const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_notice(const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
//extern void zlog_debug(const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_debug(unsigned int logtype, const char *format, ...);
//extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);
//extern void zlog_debug_clear(struct vty *vty, unsigned int type);
//extern struct vty* zlog_debug_nextvty(unsigned int *idx);

/* For bgpd's peer oriented log. */
extern void plog_err(struct zlog *, const char *format, ...)
PRINTF_ATTRIBUTE(2, 3);
extern void plog_warn(struct zlog *, const char *format, ...)
PRINTF_ATTRIBUTE(2, 3);
extern void plog_info(struct zlog *, const char *format, ...)
PRINTF_ATTRIBUTE(2, 3);
extern void plog_notice(struct zlog *, const char *format, ...)
PRINTF_ATTRIBUTE(2, 3);
extern void plog_debug(struct zlog *, const char *format, ...)
PRINTF_ATTRIBUTE(2, 3);

extern void zlog_thread_info(int log_level);

/* Set logging level for the given destination.  If the log_level
   argument is ZLOG_DISABLED, then the destination is disabled.
   This function should not be used for file logging (use zlog_set_file
   or zlog_reset_file instead). */
extern void zlog_set_level(struct zlog *zl, zlog_dest_t, int log_level);

/* Set logging to the given filename at the specified level. */
extern int zlog_set_file(struct zlog *zl, const char *filename, int log_level);
/* Disable file logging. */
extern int zlog_reset_file(struct zlog *zl);

/* Rotate log. */
extern int zlog_rotate(struct zlog *);

/* For hackey message lookup and check */
#define LOOKUP_DEF(x, y, def) mes_lookup(x, x ## _max, y, def, #x)
#define LOOKUP(x, y) LOOKUP_DEF(x, y, "(no item found)")

extern const char *lookup(const struct message *, int);
extern const char *mes_lookup(const struct message *meslist,
                              int max, int index,
                              const char *no_item, const char *mesname);

extern const char *zlog_priority[];
extern const char *zlog_proto_names[];

/* Safe version of strerror -- never returns NULL. */
extern const char *safe_strerror(int errnum);

/* To be called when a fatal signal is caught. */
extern void zlog_signal(int signo, const char *action
#ifdef SA_SIGINFO
                        , siginfo_t *siginfo, void *program_counter
#endif
                       );

/* Log a backtrace. */
extern void zlog_backtrace(int priority);

/* Log a backtrace, but in an async-signal-safe way.  Should not be
   called unless the program is about to exit or abort, since it messes
   up the state of zlog file pointers.  If program_counter is non-NULL,
   that is logged in addition to the current backtrace. */
extern void zlog_backtrace_sigsafe(int priority, void *program_counter);

/* Puts a current timestamp in buf and returns the number of characters
   written (not including the terminating NUL).  The purpose of
   this function is to avoid calls to localtime appearing all over the code.
   It caches the most recent localtime result and can therefore
   avoid multiple calls within the same second.  If buflen is too small,
   *buf will be set to '\0', and 0 will be returned. */
extern size_t quagga_timestamp(int timestamp_precision /* # subsecond digits */,
                               char *buf, size_t buflen);

extern void zlog_hexdump(void *mem, unsigned int len);

extern void syslog_open(const char *ident, int logstat, int logfac);

//extern int syslog_send_ipc_msg(void *pdata, int data_len, int sender_id, uint8_t subtype, enum IPC_OPCODE opcode);

extern void syslog_print(int pri, const char *fmt, va_list ap);

/* structure useful for avoiding repeated rendering of the same timestamp */
struct timestamp_control
{
    size_t len;      /* length of rendered timestamp */
    int precision;   /* configuration parameter */
    int already_rendered; /* should be initialized to 0 */
    char buf[40];    /* will contain the rendered timestamp */
};

/* Defines for use in command construction: */

#define LOG_LEVELS "(emergencies|alerts|critical|errors|warnings|notifications|informational|debugging)"

#define LOG_LEVEL_DESC \
  "System is unusable\n" \
  "Immediate action needed\n" \
  "Critical conditions\n" \
  "Error conditions\n" \
  "Warning conditions\n" \
  "Normal but significant conditions\n" \
  "Informational messages\n" \
  "Debugging messages\n"

#define LOG_FACILITIES "(kern|user|mail|daemon|auth|syslog|lpr|news|uucp|cron|local0|local1|local2|local3|local4|local5|local6|local7)"

#define LOG_FACILITY_DESC \
       "Kernel\n" \
       "User process\n" \
       "Mail system\n" \
       "System daemons\n" \
       "Authorization system\n" \
       "Syslog itself\n" \
       "Line printer system\n" \
       "USENET news\n" \
       "Unix-to-Unix copy system\n" \
       "Cron/at facility\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n"

#endif /* _ZEBRA_LOG_H */
