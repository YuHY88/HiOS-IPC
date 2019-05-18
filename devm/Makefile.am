## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libdevm.a
sbin_PROGRAMS = devm

libdevm_a_SOURCES = \
	devm.c devm_ha.c  devm_msg.c devm_cmd.c devm_register.c devm_timer.c

noinst_HEADERS = \
	devm.h devm_ha.h devm_timer.h

devm_SOURCES = \
	devm_main.c $(libdevm_a_SOURCES)

devm_LDADD = ../lib/libzebra.la @LIBCAP@ 

examplesdir = $(exampledir)
dist_examples_DATA = devm.conf.sample

EXTRA_DIST = DEVMv2-MIB.txt

