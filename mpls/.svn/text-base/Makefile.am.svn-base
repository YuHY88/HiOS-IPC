## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libmpls.a
sbin_PROGRAMS = mpls

am_libmpls_a_OBJECTS = lspm.$(OBJEXT) lsp_cmd.$(OBJEXT) \
	lsp_static.$(OBJEXT) pw.$(OBJEXT) pw_cmd.$(OBJEXT) \
	mpls_if.$(OBJEXT) lsp_fec.$(OBJEXT) mpls.$(OBJEXT) \
	labelm.$(OBJEXT) vpls.$(OBJEXT) \
	vpls_cmd.$(OBJEXT) tunnel.$(OBJEXT) tunnel_cmd.$(OBJEXT) \
	mpls_msg.$(OBJEXT) bfd/bfd_session.$(OBJEXT) bfd/bfd_packet.$(OBJEXT) \
	bfd/bfd_cmd.$(OBJEXT) mpls_ping.$(OBJEXT) mpls_ping_cmd.$(OBJEXT) \
	mpls_oam/mpls_oam.$(OBJEXT) mpls_oam/mpls_oam_cmd.$(OBJEXT) mpls_oam/mpls_oam_pkt.$(OBJEXT) mpls_oam/mpls_oam_ais.$(OBJEXT) mpls_oam/mpls_oam_lck.$(OBJEXT) mpls_oam/mpls_oam_csf.$(OBJEXT)\
	ldp/ldp.$(OBJEXT) ldp/ldp_packet.$(OBJEXT) ldp/ldp_session.$(OBJEXT) \
	ldp/ldp_cmd.$(OBJEXT) ldp/ldp_label.$(OBJEXT) ldp/ldp_adjance.$(OBJEXT) \
	ldp/ldp_fec.$(OBJEXT) ldp/ldp_pw.$(OBJEXT) ldp/ldp_fsm.$(OBJEXT) ldp/ldp_notify.$(OBJEXT) ldp/ldp_address.$(OBJEXT) ldp/ldp_lsptrigger.$(OBJEXT)\
	l3vpn.$(OBJEXT) l3vpn_cmd.$(OBJEXT) mpls_register.$(OBJEXT) \
	mpls_aps/mpls_aps.$(OBJEXT) mpls_aps/mpls_aps_cmd.$(OBJEXT) mpls_aps/mpls_aps_fsm.$(OBJEXT) \
	mpls_alarm.$(OBJEXT) lsp_cmd_h3c.$(OBJEXT) l2vpn_cmd_h3c.$(OBJEXT) l2vpn_h3c.$(OBJEXT)

am__objects_1 = lspm.$(OBJEXT) lsp_cmd.$(OBJEXT) lsp_static.$(OBJEXT) \
	pw.$(OBJEXT) pw_cmd.$(OBJEXT) mpls_if.$(OBJEXT) \
	lsp_fec.$(OBJEXT) mpls.$(OBJEXT) labelm.$(OBJEXT) \
	vpls.$(OBJEXT) vpls_cmd.$(OBJEXT) tunnel.$(OBJEXT) \
	tunnel_cmd.$(OBJEXT) mpls_msg.$(OBJEXT) bfd/bfd_session.$(OBJEXT) bfd/bfd_packet.$(OBJEXT) bfd/bfd_cmd.$(OBJEXT) \
	mpls_ping.$(OBJEXT) mpls_ping_cmd.$(OBJEXT) mpls_oam/mpls_oam.$(OBJEXT) mpls_oam/mpls_oam_ais.$(OBJEXT) mpls_oam/mpls_oam_lck.$(OBJEXT) mpls_oam/mpls_oam_csf.$(OBJEXT)\
	mpls_oam/mpls_oam_cmd.$(OBJEXT) mpls_oam/mpls_oam_pkt.$(OBJEXT) \
	ldp/ldp.$(OBJEXT) ldp/ldp_packet.$(OBJEXT) ldp/ldp_session.$(OBJEXT) \
	ldp/ldp_cmd.$(OBJEXT) ldp/ldp_label.$(OBJEXT) ldp/ldp_adjance.$(OBJEXT) \
	ldp/ldp_fec.$(OBJEXT) ldp/ldp_pw.$(OBJEXT) ldp/ldp_fsm.$(OBJEXT) ldp/ldp_notify.$(OBJEXT) ldp/ldp_address.$(OBJEXT) ldp/ldp_lsptrigger.$(OBJEXT)\
	l3vpn.$(OBJEXT) l3vpn_cmd.$(OBJEXT) mpls_register.$(OBJEXT) \
	mpls_aps/mpls_aps.$(OBJEXT) mpls_aps/mpls_aps_cmd.$(OBJEXT) mpls_aps/mpls_aps_fsm.$(OBJEXT) \
	mpls_alarm.$(OBJEXT) lsp_cmd_h3c.$(OBJEXT) l2vpn_cmd_h3c.$(OBJEXT) l2vpn_h3c.$(OBJEXT)

libmpls_a_SOURCES = \
	lspm.c lsp_cmd.c lsp_static.c pw.c pw_cmd.c mpls_if.c lsp_fec.c mpls.c labelm.c vpls.c vpls_cmd.c \
	tunnel.c tunnel_cmd.c mpls_msg.c bfd/bfd_session.c bfd/bfd_packet.c bfd/bfd_cmd.c mpls_ping.c mpls_ping_cmd.c \
	mpls_oam/mpls_oam.c mpls_oam/mpls_oam_cmd.c mpls_oam/mpls_oam_pkt.c mpls_oam/mpls_oam_ais.c mpls_oam/mpls_oam_lck.c mpls_oam/mpls_oam_csf.c ldp/ldp.c ldp/ldp_packet.c ldp/ldp_session.c \
	ldp/ldp_cmd.c ldp/ldp_adjance.c ldp/ldp_fec.c ldp/ldp_pw.c ldp/ldp_fsm.c ldp/ldp_notify.c ldp/ldp_address.c ldp/ldp_lsptrigger.c l3vpn.c l3vpn_cmd.c mpls_register.c \
	mpls_aps/mpls_aps.c mpls_aps/mpls_aps_cmd.c mpls_aps/mpls_aps_fsm.c mpls_alarm.c lsp_cmd_h3c.c l2vpn_cmd_h3c.c l2vpn_h3c.c

noinst_HEADERS = \
	lspm.h lsp_cmd.h lsp_static.h pw.h pw_cmd.h mpls_if.h lsp_fec.h mpls.h labelm.h vpls.h vpls_cmd.h \
	tunnel.h mpls_msg.h bfd/bfd.h bfd/bfd_session.h bfd/bfd_packet.h mpls_ping.h mpls_ping_cmd.h mpls_oam/mpls_oam.h mpls_oam/mpls_oam_cmd.h \
	mpls_oam/mpls_oam_pkt.h mpls_oam/mpls_oam_ais.h mpls_oam/mpls_oam_lck.h mpls_oam/mpls_oam_csf.h ldp/ldp.h ldp/ldp_packet.h ldp/ldp_session.h ldp/ldp_label.h ldp/ldp_adjance.h \
	l3vpn.h l3vpn_cmd.h mpls_register.h ldp/ldp_fec.h ldp/ldp_pw.h ldp/ldp_fsm.h ldp/ldp_notify.h ldp/ldp_address.h ldp/ldp_lsptrigger.h\
	mpls_aps/mpls_aps.h mpls_aps/mpls_aps_cmd.h mpls_aps/mpls_aps_fsm.h mpls_alarm.h lsp_cmd_h3c.h l2vpn_cmd_h3c.h l2vpn_h3c.h


@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/labelm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lsp_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lsp_fec.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lsp_static.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lspm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_if.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_main.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_oam/mpls_oam.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_oam/mpls_oam_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_oam/mpls_oam_ais.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_oam/mpls_oam_lck.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_oam/mpls_oam_csf.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_ping.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_ping_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/pw.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/pw_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/tunnel.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/tunnel_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/vpls.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/vpls_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/bfd/bfd_session.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/bfd/bfd_packet.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/bfd/bfd_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_ping.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_ping_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_session.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_packet.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_label.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_adjance.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_fec.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_pw.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_fsm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_notify.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_address.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ldp/ldp_lsptrigger.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_aps/mpls_aps.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_aps/mpls_aps_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_aps/mpls_aps_fsm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l3vpn.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l3vpn_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_register.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lsp_cmd_h3c.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2vpn_cmd_h3c.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2vpn_h3c.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mpls_alarm.Po@am__quote@

mpls_SOURCES = \
	mpls_main.c $(libmpls_a_SOURCES)

mpls_LDADD = ../lib/libzebra.la @LIBCAP@

mostlyclean-libtool:
	-rm -f *.lo
	-rm -f bfd/*.lo
	-rm -f ldp/*.lo
	-rm -f mpls_oam/*.lo
	-rm -f mpls_aps/*.lo
mostlyclean-compile:
	-rm -f *.$(OBJEXT)
	-rm -f bfd/*.$(OBJEXT)
	-rm -f ldp/*.$(OBJEXT)
	-rm -f mpls_oam/*.$(OBJEXT)
	-rm -f mpls_aps/*.$(OBJEXT)
examplesdir = $(exampledir)
dist_examples_DATA = mpls.conf.sample

EXTRA_DIST = MPLSv2-MIB.txt

