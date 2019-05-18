## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libroute.a
sbin_PROGRAMS = route

libroute_a_SOURCES = \
	route_static.c route_cmd.c route.c route_if.c route_rib.c route_nhp.c route_msg.c route_register.c arp_static.c arp_cmd.c ndp_cmd.c ndp_static.c h3croute_cmd.c h3croute_if.c

noinst_HEADERS = \
	route_static.h route_cmd.h route.h route_if.h route_rib.h route_nhp.h route_msg.h route_register.h route_main.h arp.h arp_static.h arp_cmd.h ndp.h ndp_cmd.h ndp_static.h h3croute_cmd.h h3croute_if.h

route_SOURCES = \
	route_main.c $(libroute_a_SOURCES)

route_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = route.conf.sample

EXTRA_DIST = ROUTEv2-MIB.txt

