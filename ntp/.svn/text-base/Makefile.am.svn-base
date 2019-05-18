## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libntp.a
sbin_PROGRAMS = ntp

libntp_a_SOURCES = \
	ntp_cli.c ntp_ntpclient.c

noinst_HEADERS = \
	ntp.h ntp_cli.h ntp_ntpclient.h

ntp_SOURCES = \
	ntp_main.c $(libntp_a_SOURCES)

ntp_LDADD = ../lib/libzebra.la  @LIBCAP@ @LIBM@

examplesdir = $(exampledir)
dist_examples_DATA = ntp.conf.sample

