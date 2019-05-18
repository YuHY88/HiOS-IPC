## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libces.a
sbin_PROGRAMS = ces

libces_a_SOURCES = \
	ces_if.c  ces_cmd.c ces_msg.c ces_alarm.c

noinst_HEADERS = \
	ces_if.h ces_cmd.h ces_msg.h ces_alarm.h

ces_SOURCES = \
	ces_main.c $(libces_a_SOURCES)

ces_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = ces.conf.sample

EXTRA_DIST = CESv2-MIB.txt

