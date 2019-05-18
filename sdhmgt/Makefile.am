## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = sdhmgt.a
sbin_PROGRAMS = sdhmgt

libsdhmgt_a_SOURCES = sdhmgt_msg.c sdhmgt_cmd.c sdhmgt_api.c sdhmgt.c sdhmgt_socket.c sdhmgt_snmp.c

noinst_HEADERS = sdhmgt_msg.h sdhmgt_cmd.h sdhmgt_api.h sdhmgt.h sdhmgt_socket.h sdhmgt_snmp.h sdhmgt_main.h

sdhmgt_SOURCES = sdhmgt_main.c $(libsdhmgt_a_SOURCES)
sdhmgt_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = sdhmgt.conf.sample 


