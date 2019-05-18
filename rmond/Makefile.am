## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

LIBS = @LIBS@ @CURSES@ @LIBPAM@ -pthread

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = librmond.a
sbin_PROGRAMS = rmond

librmond_a_SOURCES = 

noinst_HEADERS = rmond_main.h rmon_cli.h rmon_api.h rmon_timer.h rmon_config.h


rmond_SOURCES = rmond_main.c rmon_cli.c rmon_api.c rmon_timer.c $(librmond_a_SOURCES)
rmond_LDADD = ../lib/libzebra.la @LIBCAP@


examplesdir = $(exampledir)
dist_examples_DATA = rmond.conf.sample 


