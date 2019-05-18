## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libifm.a
sbin_PROGRAMS = ifm

libifm_a_SOURCES = \
	ifm.c ifm_cli.c ifm_message.c ifm_init.c ifm_register.c ifm_errno.c ifm_qos.c ifm_alarm.c

noinst_HEADERS = \
	ifm.h ifm_cli.h ifm_message.h ifm_init.h ifm_register.h ifm_errno.h ifm_qos.h ifm_alarm.h

ifm_SOURCES = \
	ifm_main.c $(libifm_a_SOURCES)

ifm_LDADD = ../lib/libzebra.la @LIBCAP@ @LIBM@

examplesdir = $(exampledir)
dist_examples_DATA = ifm.conf.sample

