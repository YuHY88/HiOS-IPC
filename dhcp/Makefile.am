## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libdhcp.a
sbin_PROGRAMS = dhcp

libdhcp_a_SOURCES = \
	 clientpacket.c      dhcp_api.c    options.c  packet.c  \
	 files.c   dhcp_server.c serverpacket.c \
	 dhcp_if.c pool.c pool_cli.c dhcp_msg.c dhcp_relay.c dhcp_client.c pool_address.c pool_static_address.c

noinst_HEADERS = \
	 clientpacket.h     dhcp.h      dhcp_snmp.h       options.h  \
	 dhcp_api.h  dhcp_server.h         packet.h \
	 files.h   serverpacket.h \
	 dhcp_if.h pool.h pool_cli.h dhcp_msg.h dhcp_relay.h dhcp_client.h pool_address.h pool_static_address.h

dhcp_SOURCES = dhcp_main.c $(libdhcp_a_SOURCES)
dhcp_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = dhcp.conf.sample 

EXTRA_DIST = DHCP-MIB.txt

