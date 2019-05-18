
/*
 * syslogd.h - jhb add for externd some functions in syslogd.c
 */

#ifndef _SYSLOGD_H
#define _SYSLOGD_H

#include "lib/log.h"
#include "syslog_cmd.h"


extern void syslog_logerror(const char *fmt, ...);
extern int syslog_decode (char *name, CODE *codetab);
extern void syslog_enforce_log_item (struct log_item *ptr);
extern void syslog_del_all_conf (void);


#endif
