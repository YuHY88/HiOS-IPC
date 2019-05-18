## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libsdhvcg.a
sbin_PROGRAMS = sdhvcg

libsdhvcg_a_SOURCES = \
	sdhvcg_if.c  sdhvcg_cmd.c sdhvcg_msg.c

noinst_HEADERS = \
	sdhvcg_if.h sdhvcg_cmd.h sdhvcg_msg.h

sdhvcg_SOURCES = \
	sdhvcg_main.c $(libsdhvcg_a_SOURCES)

sdhvcg_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = sdhvcg.conf.sample

EXTRA_DIST = SDHVCGv2-MIB.txt

