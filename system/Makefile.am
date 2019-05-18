## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libsystem.a
sbin_PROGRAMS = system

libsystem_a_SOURCES = \
	system.c  system_cmd.c system_usage.c system_alarm.c

noinst_HEADERS = \
	system.h system_cmd.h  system_usage.h system_alarm.h

system_SOURCES = \
	system_main.c $(libsystem_a_SOURCES)

system_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = system.conf.sample

EXTRA_DIST = CESv2-MIB.txt

