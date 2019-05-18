## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libping.a
sbin_PROGRAMS = pingd

libping_a_SOURCES = \
	ping.c ping6.c ping_cmd.c

noinst_HEADERS = \
	ping.h ping6.h ping_cmd.h

pingd_SOURCES = \
	ping_main.c $(libping_a_SOURCES)

pingd_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = ping.conf.sample


