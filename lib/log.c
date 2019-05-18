/*
 * Logging of zebra
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


#include <zebra.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "log.h"
#include "memory.h"
#include "command.h"
#include "thread.h"
#include "module_id.h"
#include "msg_ipc.h"
#include "msg_ipc_n.h"
#include "memshare.h"

#include "vty.h"
#include "network.h"
#include "buffer.h"

#ifndef SUNOS_5
#include <sys/un.h>
#endif
/* for printstack on solaris */
#ifdef HAVE_UCONTEXT_H
#include <ucontext.h>
#endif

#define ZLOG_LOG_MAX_LENTH  900

extern volatile vector vtyvec;


static int logfile_fd = -1; /* Used in signal handler. */

struct zlog *zlog_default = NULL;

const char *zlog_proto_names[] =
{
    "NONE",
    "DEFAULT",
    "ZEBRA",
    "RIB",
    "RIP",
    "BGP",
    "OSPF",
    "RIPNG",
    "BABEL",
    "OSPF6",
    "ISIS",
    "PIM",
    "IFM",
    "QOS",
    "FTM",
    "ROUTE",
    "MPLS",
    "LDP",
    "ARP",
    "VRRP",
    "DHCP",
    "HAL",
    "HSL",
    "SYSLOG",
    "DEVM",
    "L2",
    "MASC",
    "NTP",
    "FILEM",
    "CES",
    "AAA",
	"STATIS",
    "PINGD",
    "SYSTEM",
    "CFM",
    "SNMPD",
    "ALARM",	
    "VTYSH",
	"WEB",
    "OpenFlow",
    "IPMC",
	"CLOCK",
    "DHCPV6",
	"RMOND",
    "SDHMGT",
    "SDHVCG",
    NULL,
};

const char *zlog_priority[] =
{
    "emergencies",
    "alerts",
    "critical",
    "errors",
    "warnings",
    "notifications",
    "informational",
    "debugging",
    NULL,
};

static int  syslog_stat = 0;        /* status bits, set by zlog_open() */
static const char *syslog_tag = " ";    /* string to tag the entry with */
static int  syslog_facility = LOG_USER; /* default facility code */

int syslog_debug = 0;/* zlog_debug 的开关，= 0 时不发 ipc，直接丢弃 */

/* 打开系统日志 */
static void zlog_open(const char *ident, int logstat, int logfac)
{
    if (ident != NULL)
    {
        syslog_tag = ident;
    }

    syslog_stat = logstat;

#ifdef ALLOW_KERNEL_LOGGING

    if ((logfac & ~ LOG_FACMASK) == 0)
#else
    if (logfac != 0 && (logfac & ~ LOG_FACMASK) == 0)
#endif
    {
        syslog_facility = logfac;
    }

    return ;
}


/* syslogd 发送 IPC 消息 */
static void zlog_send(int pri, const char *fmt, va_list ap)
{
    register int cnt;
//    register char *p;
    time_t now;
    int saved_errno;
    char *tbuf = NULL;
	struct ipc_mesg_n *pSndMsg =  NULL;
    char fmt_cpy[1024];//, *stdp = (char *) 0;
    int ret = 0;
	int len = 0;

    //debug 和 informational 级别的log都要限制
    if ((syslog_debug == 0) && (LOG_PRI(pri) == LOG_DEBUG))
    {
        return ;
    }
	
	saved_errno = errno;

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + ZLOG_LOG_MAX_LENTH, MODULE_ID_SYSLOG);
	if(pSndMsg == NULL)
	{
		printf("shared memory malloc fail \n");
		return ;
	}
	memset(pSndMsg, 0, sizeof(struct ipc_msghdr_n) + ZLOG_LOG_MAX_LENTH);
	
	tbuf = (char *)(pSndMsg->msg_data);

	/* build log message: <PRI>time program[pid]: loginformation */
    time(&now);
    len = sprintf(tbuf,  "<%d>%.15s ", pri, ctime(&now) + 4);

    tbuf += len;

    if (syslog_tag)
    {
		len = sprintf(tbuf, "%s", syslog_tag);

        tbuf += len;
    }

    if (syslog_stat & LOG_PID)
    {
        len = sprintf(tbuf, "[%d]", getpid());

        tbuf += len;
    }

    if (syslog_tag)
    {
        *tbuf++ = ':';
        *tbuf++ = ' ';
    }

    /* substitute error message for %m */
    {
        register char ch, *t1, *t2;
        char *strerror();

        for (t1 = fmt_cpy; (ch = *fmt) != '\0' && t1 < fmt_cpy + sizeof(fmt_cpy); ++fmt)
        {
            if (ch == '%' && fmt[1] == 'm')
            {
                ++fmt;

                for (t2 = strerror(saved_errno); (*t1 = *t2++); ++t1);
            }
            else
            {
                *t1++ = ch;
            }

            *t1 = '\0';
        }
    }

    vsnprintf(tbuf, ZLOG_LOG_MAX_LENTH - strlen(tbuf), fmt_cpy, ap);
    cnt = strlen(tbuf) + 1;

	/* build and send IPC message */
	pSndMsg->msghdr.data_len	= cnt;
	pSndMsg->msghdr.module_id	= MODULE_ID_SYSLOG;
	pSndMsg->msghdr.sender_id	= MODULE_ID_SYSLOG;
	pSndMsg->msghdr.msg_type	= IPC_TYPE_SYSLOG;
	pSndMsg->msghdr.msg_subtype = 0;
	pSndMsg->msghdr.msg_index	= 0;
	pSndMsg->msghdr.data_num	= 1;
	pSndMsg->msghdr.opcode		= IPC_OPCODE_EVENT;

	ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_msghdr_n) + cnt);

    if(ret < 0)
    {
    	mem_share_free(pSndMsg, MODULE_ID_SYSLOG);
    }
	
	errno = saved_errno;

    return ;
}


/* For time string format. */
size_t quagga_timestamp(int timestamp_precision, char *buf, size_t buflen)
{
    static struct
    {
        time_t last;
        size_t len;
        char buf[28];
    } cache;
    struct timeval clock;

    /* would it be sufficient to use global 'recent_time' here?  I fear not... */
    gettimeofday(&clock, NULL);

    /* first, we update the cache if the time has changed */
    if (cache.last != clock.tv_sec)
    {
        struct tm *tm;
        cache.last = clock.tv_sec;
        tm = localtime(&cache.last);
        cache.len = strftime(cache.buf, sizeof(cache.buf),
                             "%Y/%m/%d %H:%M:%S", tm);
    }

    /* note: it's not worth caching the subsecond part, because
       chances are that back-to-back calls are not sufficiently close together
       for the clock not to have ticked forward */

    if (buflen > cache.len)
    {
        memcpy(buf, cache.buf, cache.len);

        if ((timestamp_precision > 0) &&
                (buflen > cache.len + 1 + timestamp_precision))
        {
            /* should we worry about locale issues? */
            static const int divisor[] = {0, 100000, 10000, 1000, 100, 10, 1};
            int prec;
            char *p = buf + cache.len + 1 + (prec = timestamp_precision);
            *p-- = '\0';

            while (prec > 6)
                /* this is unlikely to happen, but protect anyway */
            {
                *p-- = '0';
                prec--;
            }

            clock.tv_usec /= divisor[prec];

            do
            {
                *p-- = '0' + (clock.tv_usec % 10);
                clock.tv_usec /= 10;
            }
            while (--prec > 0);

            *p = '.';
            return cache.len + 1 + timestamp_precision;
        }

        buf[cache.len] = '\0';
        return cache.len;
    }

    if (buflen > 0)
    {
        buf[0] = '\0';
    }

    return 0;
}

/* Utility routine for current time printing. */
static void time_print(FILE *fp, struct timestamp_control *ctl)
{
    if (!ctl->already_rendered)
    {
        ctl->len = quagga_timestamp(ctl->precision, ctl->buf, sizeof(ctl->buf));
        ctl->already_rendered = 1;
    }

    fprintf(fp, "%s ", ctl->buf);
}

/* va_list version of zlog. */
static void vzlog(struct zlog *zl, int priority, const char *format, va_list args)
{
    int original_errno = errno;
    va_list ac;
    struct timestamp_control tsctl;
    tsctl.already_rendered = 0;

    /* If zlog is not specified, use default one. */
    if (zl == NULL)
    {
        zl = zlog_default;
    }

    /* When zlog_default is also NULL, use stderr for logging. */
    if (zl == NULL)
    {
        tsctl.precision = 0;
        time_print(stderr, &tsctl);
        fprintf(stderr, "%s: ", "unknown");
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
        fflush(stderr);

        /* In this case we return at here. */
        errno = original_errno;
        return;
    }

    tsctl.precision = zl->timestamp_precision;

    /* Syslog output */
    va_copy(ac, args);
    zlog_send(priority | zlog_default->facility, format, ac);
    va_end(ac);

    errno = original_errno;
}

static char *str_append(char *dst, int len, const char *src)
{
    while ((len-- > 0) && *src)
    {
        *dst++ = *src++;
    }

    return dst;
}

static char *num_append(char *s, int len, u_long x)
{
    char buf[30];
    char *t;

    if (!x)
    {
        return str_append(s, len, "0");
    }

    *(t = &buf[sizeof(buf) - 1]) = '\0';

    while (x && (t > buf))
    {
        *--t = '0' + (x % 10);
        x /= 10;
    }

    return str_append(s, len, t);
}

#if defined(SA_SIGINFO) || defined(HAVE_STACK_TRACE)
static char *hex_append(char *s, int len, u_long x)
{
    char buf[30];
    char *t;

    if (!x)
    {
        return str_append(s, len, "0");
    }

    *(t = &buf[sizeof(buf) - 1]) = '\0';

    while (x && (t > buf))
    {
        u_int cc = (x % 16);
        *--t = ((cc < 10) ? ('0' + cc) : ('a' + cc - 10));
        x /= 16;
    }

    return str_append(s, len, t);
}
#endif


static void zlog_sigsafe(int priority, const char *msg, size_t msglen)
{
//    static int syslog_fd = -1;
    char buf[sizeof("<1234567890>ripngd[1234567890]: ") + msglen + 50];
    char *s;

#define LOC s,buf+sizeof(buf)-s
    s = buf;
    s = str_append(LOC, "<");
    s = num_append(LOC, priority);
    s = str_append(LOC, ">");
    /* forget about the timestamp, too difficult in a signal handler */
    s = str_append(LOC, zlog_default->ident);

    if (zlog_default->syslog_options & LOG_PID)
    {
        s = str_append(LOC, "[");
        s = num_append(LOC, getpid());
        s = str_append(LOC, "]");
    }

    s = str_append(LOC, ": ");
    s = str_append(LOC, msg);

	ipc_send_msg_n2(buf, s - buf,  1, MODULE_ID_SYSLOG, 
		MODULE_ID_SYSLOG, IPC_TYPE_SYSLOG, 0, IPC_OPCODE_EVENT, 0);
#undef LOC
}


/* Note: the goal here is to use only async-signal-safe functions. */
void zlog_signal(int signo, const char *action
#ifdef SA_SIGINFO
            , siginfo_t *siginfo, void *program_counter
#endif
           )
{
    time_t now;
    char buf[sizeof("DEFAULT: Received signal S at T (si_addr 0xP, PC 0xP); aborting...") + 100];
    char *s = buf;
    char *msgstart = buf;
	
#define LOC s,buf+sizeof(buf)-s

    time(&now);

    if (zlog_default)
    {
        s = str_append(LOC, zlog_proto_names[zlog_default->protocol]);
        *s++ = ':';
        *s++ = ' ';
        msgstart = s;
    }

    s = str_append(LOC, "Received signal ");
    s = num_append(LOC, signo);
    s = str_append(LOC, " at ");
    s = num_append(LOC, now);
#ifdef SA_SIGINFO
    s = str_append(LOC, " (si_addr 0x");
    s = hex_append(LOC, (u_long)(siginfo->si_addr));

    if (program_counter)
    {
        s = str_append(LOC, ", PC 0x");
        s = hex_append(LOC, (u_long)program_counter);
    }

    s = str_append(LOC, "); ");
#else /* SA_SIGINFO */
    s = str_append(LOC, "; ");
#endif /* SA_SIGINFO */
    s = str_append(LOC, action);

    if (s < buf + sizeof(buf))
    {
        *s++ = '\n';
    }

    /* N.B. implicit priority is most severe */
    zlog_sigsafe(LOG_CRIT | zlog_default->facility, msgstart, s - msgstart);

    zlog_backtrace_sigsafe(LOG_CRIT,
#ifdef SA_SIGINFO
                           program_counter
#else
                           NULL
#endif
                          );

    s = buf;

    if (!thread_current)
    {
        s = str_append(LOC, "no thread information available\n");
    }
    else
    {
        s = str_append(LOC, "in thread ");
        s = str_append(LOC, thread_current->funcname);
        s = str_append(LOC, " scheduled from ");
        s = str_append(LOC, thread_current->schedfrom);
        s = str_append(LOC, ":");
        s = num_append(LOC, thread_current->schedfrom_line);
        s = str_append(LOC, "\n");
    }

    /* If no file logging configured, try to write to fallback log file. */
    zlog_sigsafe(LOG_CRIT | zlog_default->facility, msgstart, s - msgstart);

#undef LOC
}

/* Log a backtrace using only async-signal-safe functions.
   Needs to be enhanced to support syslog logging. */
void zlog_backtrace_sigsafe(int priority, void *program_counter)
{
#ifdef HAVE_STACK_TRACE
    static const char pclabel[] = "Program counter: ";
    void *array[64];
    int size;
    char buf[100];
    char *s, **bt = NULL;
#define LOC s,buf+sizeof(buf)-s

#ifdef HAVE_GLIBC_BACKTRACE
    size = backtrace(array, array_size(array));

    if (size <= 0 || (size_t)size > array_size(array))
    {
        return;
    }

#define DUMP(FD) { \
  if (program_counter) \
    { \
      write(FD, pclabel, sizeof(pclabel)-1); \
      backtrace_symbols_fd(&program_counter, 1, FD); \
    } \
  write(FD, buf, s-buf);    \
  backtrace_symbols_fd(array, size, FD); \
}
#elif defined(HAVE_PRINTSTACK)
#define DUMP(FD) { \
  if (program_counter) \
    write((FD), pclabel, sizeof(pclabel)-1); \
  write((FD), buf, s-buf); \
  printstack((FD)); \
}
#endif /* HAVE_GLIBC_BACKTRACE, HAVE_PRINTSTACK */

    s = buf;
    s = str_append(LOC, "Backtrace for ");
    s = num_append(LOC, size);
    s = str_append(LOC, " stack frames:\n");

    if (!zlog_default)
        DUMP(STDERR_FILENO)
    else
    {
        int i;   
		zlog_sigsafe(priority | zlog_default->facility, buf, s - buf);
#ifdef HAVE_GLIBC_BACKTRACE
        bt = backtrace_symbols(array, size);
#endif

        /* Just print the function addresses. */
        for (i = 0; i < size; i++)
        {
            s = buf;

            if (bt)
            {
                s = str_append(LOC, bt[i]);
            }
            else
            {
                s = str_append(LOC, "[bt ");
                s = num_append(LOC, i);
                s = str_append(LOC, "] 0x");
                s = hex_append(LOC, (u_long)(array[i]));
            }

            *s = '\0';

            zlog_sigsafe(priority | zlog_default->facility, buf, s - buf);
        }

        if (bt)
        {
            free(bt);
        }
    }
    

#undef DUMP
#undef LOC
#endif /* HAVE_STRACK_TRACE */
}

void zlog_backtrace(int priority)
{
#ifndef HAVE_GLIBC_BACKTRACE
    zlog(NULL, priority, "No backtrace available on this platform.");
#else
    void *array[20];
    int size, i;
    char **strings;

    size = backtrace(array, array_size(array));

    if (size <= 0 || (size_t)size > array_size(array))
    {
        zlog_err("Cannot get backtrace, returned invalid # of frames %d "
                 "(valid range is between 1 and %lu)",
                 size, (unsigned long)(array_size(array)));
        return;
    }

    zlog(NULL, priority, "Backtrace for %d stack frames:", size);

    if (!(strings = backtrace_symbols(array, size)))
    {
        zlog_err("Cannot get backtrace symbols (out of memory?)");

        for (i = 0; i < size; i++)
        {
            zlog(NULL, priority, "[bt %d] %p", i, array[i]);
        }
    }
    else
    {
        for (i = 0; i < size; i++)
        {
            zlog(NULL, priority, "[bt %d] %s", i, strings[i]);
        }

        free(strings);
    }

#endif /* HAVE_GLIBC_BACKTRACE */
}


void zlog(struct zlog *zl, int priority, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vzlog(zl, priority, format, args);
    va_end(args);
}

int zlog_out (struct vty *vty, const char *level, const char *proto_str,
               const char *format, struct timestamp_control *ctl, va_list va)
{
    int ret;
    int len;
    char buf[1024];

    if (!ctl->already_rendered)
    {
        ctl->len = quagga_timestamp(ctl->precision, ctl->buf, sizeof(ctl->buf));
        ctl->already_rendered = 1;
    }
    if (ctl->len + 1 >= sizeof(buf))
    {
        return -1;
    }
    memcpy(buf, ctl->buf, len = ctl->len);
    buf[len++] = ' ';
    buf[len] = '\0';

    if (level)
    {
        ret = snprintf(buf + len, sizeof(buf) - len, "%s: %s: ", level, proto_str);
    }
    else
    {
        ret = snprintf(buf + len, sizeof(buf) - len, "%s: ", proto_str);
    }
    if ((ret < 0) || ((size_t)(len += ret) >= sizeof(buf)))
    {
        return -1;
    }

    if (((ret = vsnprintf(buf + len, sizeof(buf) - len, format, va)) < 0) ||
            ((size_t)((len += ret) + 2) > sizeof(buf)))
    {
        return -1;
    }

    buf[len++] = '\r';
    buf[len++] = '\n';

    if (write(vty->wfd, buf, len) < 0)
    {
        if (ERRNO_IO_RETRY(errno))
            /* Kernel buffer is full, probably too much debugging output, so just
               drop the data and ignore. */
        {
            return -1;
        }
        /* Fatal I/O error. */
        vty->monitor = 0; /* disable monitoring to avoid infinite recursion */
        zlog_warn("%s: write failed to vty client fd %d, closing: %s",
                  __func__, vty->fd, safe_strerror(errno));
        if(vty->obuf) buffer_reset(vty->obuf);   /* temp solution, ????*/
        /* cannot call vty_close, because a parent routine may still try
           to access the vty struct */
        vty->status = VTY_CLOSE;
        shutdown(vty->fd, SHUT_RDWR);
        return -1;
    }
    return 0;
}


void zlog_debug(unsigned int logtype, const char *format, ...)
{
	unsigned int i;
	struct vty *vty;
	struct timestamp_control tsctl;
    tsctl.already_rendered = 0;

	if (!vtyvec)
	{
		return;
	}

	for(i = 0; i < vector_active(vtyvec); i++)
	{
		if((vty = vector_slot(vtyvec, i)) != NULL)
		{
			if(vty->monitor & logtype)
			{
				va_list args;;
				va_start(args, format);
				zlog_out (vty, "debug", zlog_proto_names[zlog_default->protocol], format, &tsctl, args);
				va_end(args);
			}
		}
	}
}


/*
	函数名称: zlog_debug_set
	参     数:
		vty:     当前活动的vty
		type:    每个进程自定义的log类型
		enable:  使能状态，非0：使能该类型的log输出，0：去使能该类型的log输出
*/

void zlog_debug_set(struct vty *vty, unsigned int type, int enable)
{
     if(enable)
     {
         vty->monitor |= type;
	 }
	 else
	 {
	     vty->monitor &= (~type);
	 }
}

void zlog_debug_clear(struct vty *vty, unsigned int type)
{
     if(!vtyvec) return;
        
     if(vty)
     {
         vty->monitor &= (~type);
     }
     else
     {
         unsigned int i;
         for(i = 0; i < vector_active(vtyvec); i++)
         {
             if((vty = vector_slot(vtyvec, i)) != NULL)
             {
                 vty->monitor &= (~type);
             }
         }
     }
}

struct vty* zlog_debug_nextvty(unsigned int *idx)
{
     unsigned int i;

     if((!vtyvec) || (!idx)) return(NULL);
        
   
     for(i = *idx; i < vector_active(vtyvec); i++)
     {
          if(vector_slot(vtyvec, i) != NULL) break;
     }

     if(i >= vector_active(vtyvec))
     {
          return(NULL);
     }
     else 
     {
         *idx = i + 1;
          
          return(vector_slot(vtyvec, i));
     }
     
}


#define ZLOG_FUNC(FUNCNAME,PRIORITY) \
void \
FUNCNAME(const char *format, ...) \
{ \
  va_list args; \
  va_start(args, format); \
  vzlog (NULL, PRIORITY, format, args); \
  va_end(args); \
}

ZLOG_FUNC(zlog_err, LOG_ERR)

ZLOG_FUNC(zlog_warn, LOG_WARNING)

ZLOG_FUNC(zlog_info, LOG_INFO)

ZLOG_FUNC(zlog_notice, LOG_NOTICE)

//ZLOG_FUNC(zlog_debug, LOG_DEBUG)

#undef ZLOG_FUNC

#define PLOG_FUNC(FUNCNAME,PRIORITY) \
void \
FUNCNAME(struct zlog *zl, const char *format, ...) \
{ \
  va_list args; \
  va_start(args, format); \
  vzlog (zl, PRIORITY, format, args); \
  va_end(args); \
}

PLOG_FUNC(plog_err, LOG_ERR)

PLOG_FUNC(plog_warn, LOG_WARNING)

PLOG_FUNC(plog_info, LOG_INFO)

PLOG_FUNC(plog_notice, LOG_NOTICE)

PLOG_FUNC(plog_debug, LOG_DEBUG)

#undef PLOG_FUNC

void zlog_thread_info(int log_level)
{
    if (thread_current)
        zlog(NULL, log_level, "Current thread function %s, scheduled from "
             "file %s, line %u", thread_current->funcname,
             thread_current->schedfrom, thread_current->schedfrom_line);
    else
    {
        zlog(NULL, log_level, "Current thread not known/applicable");
    }
}

void _zlog_assert_failed(const char *assertion, const char *file,
                    unsigned int line, const char *function)
{
    /* Force fallback file logging? */
    zlog(NULL, LOG_CRIT, "Assertion `%s' failed in file %s, line %u, function %s",
         assertion, file, line, (function ? function : "?"));
    zlog_backtrace(LOG_CRIT);
    zlog_thread_info(LOG_CRIT);
    abort();
}

/* Open log stream */
struct zlog *openzlog(const char *progname, zlog_proto_t protocol,
         int syslog_flags, int syslog_facility)
{
    struct zlog *zl;
    u_int i;

    zl = XCALLOC(MTYPE_ZLOG, sizeof(struct zlog));

    zl->ident = progname;
    zl->protocol = protocol;
    zl->facility = syslog_facility;
    zl->syslog_options = syslog_flags;

    /* Set default logging levels. */
    for (i = 0; i < array_size(zl->maxlvl); i++)
    {
        zl->maxlvl[i] = ZLOG_DISABLED;
    }

    /*disable dest monitor, enable syslog*/
    /*zl->maxlvl[ZLOG_DEST_MONITOR] = LOG_DEBUG; */

    zl->maxlvl[ZLOG_DEST_SYSLOG] = LOG_DEBUG;
    zl->default_lvl = LOG_DEBUG;

    zlog_open(progname, syslog_flags, zl->facility);
// openlog (progname, syslog_flags, zl->facility);

    return zl;
}

void closezlog(struct zlog *zl)
{
    if (zl->fp != NULL)
    {
        fclose(zl->fp);
    }

    if (zl->filename != NULL)
    {
        free(zl->filename);
    }

    XFREE(MTYPE_ZLOG, zl);
}

/* Called from command.c. */
void zlog_set_level(struct zlog *zl, zlog_dest_t dest, int log_level)
{
    if (zl == NULL)
    {
        zl = zlog_default;
    }

    zl->maxlvl[dest] = log_level;
}

int zlog_set_file(struct zlog *zl, const char *filename, int log_level)
{
    FILE *fp;
    mode_t oldumask;

    /* There is opend file.  */
    zlog_reset_file(zl);

    /* Set default zl. */
    if (zl == NULL)
    {
        zl = zlog_default;
    }

    /* Open file. */
    oldumask = umask(0777 & ~LOGFILE_MASK);
    fp = fopen(filename, "a");
    umask(oldumask);

    if (fp == NULL)
    {
        return 0;
    }

    /* Set flags. */
    zl->filename = strdup(filename);
    zl->maxlvl[ZLOG_DEST_FILE] = log_level;
    zl->fp = fp;
    logfile_fd = fileno(fp);

    return 1;
}

/* Reset opend file. */
int zlog_reset_file(struct zlog *zl)
{
    if (zl == NULL)
    {
        zl = zlog_default;
    }

    if (zl->fp)
    {
        fclose(zl->fp);
    }

    zl->fp = NULL;
    logfile_fd = -1;
    zl->maxlvl[ZLOG_DEST_FILE] = ZLOG_DISABLED;

    if (zl->filename)
    {
        free(zl->filename);
    }

    zl->filename = NULL;

    return 1;
}

/* Reopen log file. */
int zlog_rotate(struct zlog *zl)
{
#if 0
    int level;

    if (zl == NULL)
    {
        zl = zlog_default;
    }

    if (zl->fp)
    {
        fclose(zl->fp);
    }

    zl->fp = NULL;
    logfile_fd = -1;
    level = zl->maxlvl[ZLOG_DEST_FILE];
    zl->maxlvl[ZLOG_DEST_FILE] = ZLOG_DISABLED;

    if (zl->filename)
    {
        mode_t oldumask;
        int save_errno;

        oldumask = umask(0777 & ~LOGFILE_MASK);
        zl->fp = fopen(zl->filename, "a");
        save_errno = errno;
        umask(oldumask);

        if (zl->fp == NULL)
        {
            zlog_err("Log rotate failed: cannot open file %s for append: %s",
                     zl->filename, safe_strerror(save_errno));
            return -1;
        }

        logfile_fd = fileno(zl->fp);
        zl->maxlvl[ZLOG_DEST_FILE] = level;
    }
#endif
    return 1;
}

/* Message lookup function. */
const char *lookup(const struct message *mes, int key)
{
    const struct message *pnt;

    for (pnt = mes; pnt->key != 0; pnt++)
        if (pnt->key == key)
        {
            return pnt->str;
        }

    return "";
}

/* Older/faster version of message lookup function, but requires caller to pass
 * in the array size (instead of relying on a 0 key to terminate the search).
 *
 * The return value is the message string if found, or the 'none' pointer
 * provided otherwise.
 */
const char *mes_lookup(const struct message *meslist, int max, int index,
           const char *none, const char *mesname)
{
    int pos = index - meslist[0].key;

    /* first check for best case: index is in range and matches the key
     * value in that slot.
     * NB: key numbering might be offset from 0. E.g. protocol constants
     * often start at 1.
     */
    if ((pos >= 0) && (pos < max)
            && (meslist[pos].key == index))
    {
        return meslist[pos].str;
    }

    /* fall back to linear search */
    {
        int i;

        for (i = 0; i < max; i++, meslist++)
        {
            if (meslist->key == index)
            {
                const char *str = (meslist->str ? meslist->str : none);

                zlog_debug(ZLOG_LIB_DBG_LIB, "message index %d [%s] found in %s at position %d (max is %d)",
                           index, str, mesname, i, max);
                return str;
            }
        }
    }
    zlog_err("message index %d not found in %s (max is %d)", index, mesname, max);
    assert(none);
    return none;
}

/* Wrapper around strerror to handle case where it returns NULL. */
const char *safe_strerror(int errnum)
{
    const char *s = strerror(errnum);
    return (s != NULL) ? s : "Unknown error";
}


void zlog_hexdump(void *mem, unsigned int len)
{
    unsigned long i = 0;
    unsigned int j = 0;
    unsigned int columns = 8;
    char buf[(len * 4) + ((len / 4) * 20) + 30];
    char *s = buf;

    for (i = 0; i < len + ((len % columns) ? (columns - len % columns) : 0); i++)
    {
        /* print offset */
        if (i % columns == 0)
        {
            s += sprintf(s, "0x%016lx: ", (unsigned long)mem + i);
        }

        /* print hex data */
        if (i < len)
        {
            s += sprintf(s, "%02x ", 0xFF & ((char *)mem)[i]);
        }

        /* end of block, just aligning for ASCII dump */
        else
        {
            s += sprintf(s, "   ");
        }

        /* print ASCII dump */
        if (i % columns == (columns - 1))
        {
            for (j = i - (columns - 1); j <= i; j++)
            {
                if (j >= len) /* end of block, not really printing */
                {
                    s += sprintf(s, " ");
                }

                else if (isprint((int)((char *)mem)[j])) /* printable char */
                {
                    s += sprintf(s, "%c", 0xFF & ((char *)mem)[j]);
                }

                else /* other char */
                {
                    s += sprintf(s, ".");
                }
            }

            s += sprintf(s, "\n");
        }
    }

    zlog_debug(ZLOG_LIB_DBG_LIB, "\n%s", buf);
}


