## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\" -DLOCALDBDIR=\"$(localdbdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libdhcpv6.a
sbin_PROGRAMS = dhcpv6

libdhcpv6a_SOURCES = \
	 common.c dhcpv6_config.c dhcpv6_msg.c prefixconf.c \
     dhcp6c_ia.c dhcpv6_timer.c dhcp6c_script.c \
     base64.c auth.c dhcp6_ctl.c addrconf.c lease.c \
	 dhcp6c_cmd.c cfparse.c cftoken.c dhcpv6_packet.c dhcpv6_options.c dhcpv6_if.c

noinst_HEADERS = \
	 common.h dhcpv6_msg.h dhcpv6_config.h prefixconf.h \
	 dhcp6c_ia.h dhcpv6_timer.h   base64.h auth.h \
	 dhcpv6_ctl.h addrconf.h lease.h	 dhcpv6_packet.h dhcpv6_options.h dhcpv6_if.h

dhcpv6_SOURCES = dhcp6c_main.c $(libdhcpv6_a_SOURCES)
dhcp6c_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = dhcp6c.conf.sample

EXTRA_DIST = DHCP-MIB.txt

