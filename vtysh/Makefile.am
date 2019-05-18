## Process this file with Automake to create Makefile.in

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib -I$(top_builddir)/vtysh/sshd
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"

LIBS = @LIBS@ @CURSES@ @LIBPAM@

AM_CFLAGS = $(WERROR)

bin_PROGRAMS = vtysh

vtysh_SOURCES = vtysh_main.c vtysh.c vtysh_user.c vtysh_config.c vtysh_cmd.c vtysh_mpls_cmd.c vtysh_route_cmd.c  \
		vtysh_arp_cmd.c vtysh_ndp_cmd.c vtysh_ospf_cmd.c vtysh_ospf6_cmd.c vtysh_isis_cmd.c vtysh_ripd_cmd.c vtysh_ripng_cmd.c vtysh_dhcp_cmd.c vtysh_qos_cmd.c \
		vtysh_ifm_cmd.c vtysh_syslog_cmd.c vtysh_devm_cmd.c vtysh_filem_cmd.c vtysh_ftm_cmd.c vtysh_ntp_cmd.c \
		vtysh_l2_cmd.c vtysh_lldp_cmd.c vtysh_mstp_cmd.c vtysh_efm_cmd.c vtysh_hal_cmd.c vtysh_ping_cmd.c vtysh_ces_cmd.c vtysh_aaa_cmd.c vtysh_web_cmd.c \
		vtysh_system_cmd.c vtysh_bgp_cmd.c vtysh_synce_cmd.c vtysh_clock_cmd.c vtysh_msg.c vtysh_alarm_cmd.c \
		vtysh_snmpd_cmd.c vtysh_sdhmgt_cmd.c vtysh_sdhvcg_cmd.c vtysh_session.c vtysh_telnet.c vtysh_ipmc_cmd.c vtysh_sshd.c vtysh_telnet_client.c vtysh_telnet_client_cmd.c\
		vtysh_sync.c vtysh_sync_slave.c vtysh_rmond_cmd.c vtysh_statis_cmd.c vtysh_mpls_cmd_h3c.c\
		sshd/servconf.c sshd/authfile.c sshd/cipher.c sshd/buffer_ssh.c sshd/rsa.c sshd/misc.c sshd/atomicio.c \
		sshd/xmalloc.c sshd/fatal.c sshd/bufaux.c sshd/bufbn.c sshd/key.c sshd/uuencode.c sshd/cipher-ctr.c \
		sshd/cipher-3des1.c sshd/ssh-rsa.c sshd/ssh-dss.c sshd/bsd-arc4random.c sshd/kex.c sshd/packet.c sshd/mac.c \
		sshd/dispatch.c sshd/match.c sshd/md-sha256.c sshd/umac.c sshd/compress.c sshd/kexgexs.c sshd/kexdhs.c sshd/dh.c \
		sshd/sha2.c sshd/strtonum.c sshd/kexgex.c sshd/kexdh.c sshd/auth2.c sshd/auth2-none.c sshd/auth2-pubkey.c \
		sshd/auth2-passwd.c sshd/channels.c sshd/serverloop2.c
#nodist_vtysh_SOURCES = vtysh_cmd.c 
CLEANFILES = 
noinst_HEADERS = vtysh.h vtysh_user.h
vtysh_LDADD = ../lib/libzebra.la @LIBCAP@ @LIBREADLINE@

examplesdir = $(exampledir)
dist_examples_DATA = vtysh.conf.sample

EXTRA_DIST = extract.pl

vtysh_cmd_FILES = $(top_srcdir)/bgpd/*.c $(top_srcdir)/isisd/*.c \
		  $(top_srcdir)/ospfd/*.c $(top_srcdir)/ospf6d/*.c \
		  $(top_srcdir)/ripd/*.c $(top_srcdir)/ripd/ripngd/*.c \
		  $(top_srcdir)/pimd/pim_cmd.c \
		  $(top_srcdir)/lib/keychain.c $(top_srcdir)/lib/routemap.c \
		  $(top_srcdir)/lib/filter.c $(top_srcdir)/lib/plist.c \
		  $(top_srcdir)/lib/distribute.c $(top_srcdir)/lib/if_rmap.c \
		  $(top_srcdir)/lib/vrf.c \
		  $(top_srcdir)/lib/vty.c $(top_srcdir)/zebra/debug.c \
		  $(top_srcdir)/zebra/interface.c \
		  $(top_srcdir)/zebra/irdp_interface.c \
		  $(top_srcdir)/zebra/rtadv.c $(top_srcdir)/zebra/zebra_vty.c \
		  $(top_srcdir)/zebra/zserv.c $(top_srcdir)/zebra/router-id.c \
		  $(top_srcdir)/zebra/zebra_routemap.c \
	      $(top_srcdir)/zebra/zebra_fpm.c \
	      $(top_srcdir)/ribd/rib_vty.c \
	      $(top_srcdir)/arp/*.c\
	      $(top_srcdir)/qos/acl_cli.c \
	      $(top_srcdir)/ifm/*.c

#vtysh_cmd.c: $(vtysh_cmd_FILES)
#	./$(EXTRA_DIST) $(vtysh_cmd_FILES) > vtysh_cmd.c

