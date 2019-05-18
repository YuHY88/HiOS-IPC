## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libclock.a
sbin_PROGRAMS = clock

libclock_a_SOURCES = \
	clock_msg.c  clock_src.c clock_if.c clock_cmd.c clock_alarm.c synce/synce.c synce/synce_cmd.c synce/synce_ssm.c

noinst_HEADERS = \
	clock_msg.h clock_src.h clock_if.h clock_cmd.h clock_alarm.c synce/synce.h synce/synce_cmd.h synce/synce_ssm.h

clock_SOURCES = \
	clock_main.c $(libclock_a_SOURCES)

clock_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = clock.conf.sample

EXTRA_DIST = CLOCKv2-MIB.txt

