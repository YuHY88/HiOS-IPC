## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libsyslog.a
sbin_PROGRAMS = syslog

libsyslog_a_SOURCES = \
	syslog_cmd.c

noinst_HEADERS = \
	syslogd.h syslog_cmd.h

syslog_SOURCES = \
	syslogd.c $(libsyslog_a_SOURCES)

syslog_LDADD = ../lib/libzebra.la  @LIBCAP@ @LIBM@

examplesdir = $(exampledir)
dist_examples_DATA = syslog.conf.sample

