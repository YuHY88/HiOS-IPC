## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libqos.a
sbin_PROGRAMS = qos

libqos_a_SOURCES = \
	acl.c acl_cli.c qos_if.c qos_policy.c qos_mirror.c \
	qos_mapping.c qos_mapping_cmd.c qos_car.c qos_msg.c \
	qos_cpcar.c sla/sla_cmd.c sla/sla_session.c \
	hqos.c

noinst_HEADERS = \
	acl.h acl_cli.h qos_if.h qos_policy.h qos_mirror.h \
	qos_mapping.h qos_mapping_cmd.h qos_car.h qos_msg.h \
	qos_cpcar.h  sla/sla_session.h \
	hqos.h

qos_SOURCES = \
	qos_main.c $(libqos_a_SOURCES)

qos_LDADD = ../lib/libzebra.la  @LIBCAP@ @LIBM@

examplesdir = $(exampledir)
dist_examples_DATA = qos.conf.sample

