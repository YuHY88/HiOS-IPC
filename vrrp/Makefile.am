## Process this file with automake to produce Makefile.in.

INCLUDES =-g  @INCLUDES@ -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I../../common 

DEFS = @DEFS@ \
	-D__BSD_SOURCE \
	-DRAW_INPUT_IS_RAW \
	-DRAW_OUTPUT_IS_RAW \
	-DIOCTL_OK_ON_RAW_SOCKET \
	-DLinux\
	-DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

noinst_LIBRARIES = libvrrp.a
sbin_PROGRAMS = vrrpd

libvrrp_a_SOURCES = \
	ipaddr.c   ipsecah.c     ll_map.c  vrrpd.c  utils.c   \
	iproute.c  libnetlink.c  md5.c  vrrp_sub.c  vrrp_if.c  vrrp_zclient.c

noinst_HEADERS = \
	ipaddr.h  iproute.h  ipsecah.h  libnetlink.h  ll_map.h  md5.h  proto.h  vrrpd.h  utils.h

vrrpd_SOURCES = \
	  vrrp_main.c  $(libvrrp_a_SOURCES)

vrrpd_LDADD = ../lib/libzebra.a @VMALLOCA@

sysconf_DATA = vrrpd.conf

EXTRA_DIST = #$(sysconf_DATA)

install-sysconfDATA: $(sysconf_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	@list='$(sysconf_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_SDATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_SDATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_SDATA) $$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_SDATA) $$p $(DESTDIR)$(sysconfdir)/$$p; \
	  fi; fi; \
	done

ramdisk: all
	install -D -c -m 755 vrrpd $(DISKMOUNT)/usr/bin/vrrpd
	$(STRIP) $(DISKMOUNT)/usr/bin/vrrpd
#	touch $(DISKMOUNT)/etc/vrrpd.conf
