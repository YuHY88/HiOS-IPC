## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libftm.a
sbin_PROGRAMS = ftm

libftm_a_SOURCES = \
	pkt_ip.c pkt_ipv6.c pkt_icmp.c ftm_debug.c ftm_nhp.c ftm_ilm.c ftm_ldp.c ftm_nhlfe.c ftm_lsp.c ftm_msg.c \
	ftm_fib.c ftm_arp.c pkt_arp.c  ftm_vlan.c ftm_pw.c ftm_ifm.c proto_reg.c ftm_pkt.c pkt_eth.c  pkt_tcp.c pkt_udp.c ftm_vpls.c \
	ftm_tunnel.c pkt_mpls.c pkt_netif.c ftm_ndp.c pkt_ndp.c

noinst_HEADERS = \
	pkt_ip.h pkt_ipv6.h pkt_icmp.h ftm_debug.h ftm_nhp.h ftm.h ftm_ilm.h ftm_ldp.h ftm_nhlfe.h ftm_lsp.h \
	ftm_fib.h ftm_arp.h pkt_arp.h ftm_vlan.h ftm_pw.h ftm_ifm.h proto_reg.h \
	ftm_pkt.h pkt_eth.h  pkt_tcp.h pkt_udp.h ftm_vpls.h \
	ftm_tunnel.h pkt_mpls.h pkt_netif.h ftm_ndp.h pkt_ndp.h

ftm_SOURCES = ftm_main.c $(libftm_a_SOURCES)
ftm_LDADD = ../lib/libzebra.la @LIBCAP@


examplesdir = $(exampledir)
dist_examples_DATA = ftm.conf.sample 

EXTRA_DIST = FTM-MIB.txt

