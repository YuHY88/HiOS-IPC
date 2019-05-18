## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

LIBS = @LIBS@ @CURSES@ @LIBPAM@ -pthread

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libaaa.a
sbin_PROGRAMS = aaa

libaaa_a_SOURCES = aaa_msg_io.c aaa_cmd.c aaa_packet.c aaa_radius.c aaa_radius_auth.c aaa_radius_acct.c\
    aaa_tac_plus.c magic.c xalloc.c tac_plus_crypt.c tac_plus_header.c \
    tac_plus_authen.c tac_plus_attrib.c tac_plus_author.c tac_plus_acct.c aaa_snmp.c \
    aaa_msg_handle.c aaa_local_auth.c aaa_none_auth.c aaa_user_manager.c aaa_config.c aaa_dot1x.c

noinst_HEADERS = aaa_cmd.h aaa_packet.h aaa_radius.h aaa_radius_auth.h aaa_radius_acct.h\
    aaa_tac_plus.h xalloc.h tacplus.h radiusclient.h aaa_snmp.h \
    aaa_msg_io.h aaa_msg_handle.h aaa_local_auth.h aaa_none_auth.h aaa_user_manager.h \
    aaa_radius_common.h aaa_tac_plus_common.h aaa_config.h aaa_dot1x.h aaa.h


aaa_SOURCES = aaa_main.c $(libaaa_a_SOURCES)
aaa_LDADD = ../lib/libzebra.la @LIBCAP@


examplesdir = $(exampledir)
dist_examples_DATA = aaa.conf.sample 


