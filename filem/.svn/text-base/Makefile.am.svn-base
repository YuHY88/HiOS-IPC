## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libfilem.a
sbin_PROGRAMS = filem

libfilem_a_SOURCES = \
	filem_init.c filem_oper.c filem_show.c filem_cli.c filem_md5.c filem_tftp.c filem_ftp.c filem_cmd.c filem_img.c filem_fpga.c filem_tran.c filem_sync.c filem_slot.c

noinst_HEADERS = filem.h 
#version.h vtysh_debug.h vtysh_log.h

filem_SOURCES = \
	filem_main.c $(libfilem_a_SOURCES)

#filem_LDADD = ../lib/libzebra.la  @LIBCAP@  libspidrv.so
filem_LDADD = ../lib/libzebra.la 

examplesdir = $(exampledir)
dist_examples_DATA = filem.conf.sample

