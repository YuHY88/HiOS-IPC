## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libipmc.a
sbin_PROGRAMS = ipmc

libipmc_a_SOURCES = \
	ipmc_group.c ipmc_if.c ipmc_ip.c ipmc_mac.c ipmc_msg.c pim/pim.c pim/pim_cmd.c pim/pim_neighbor.c pim/pim_register.c pim/pim_pkt.c \
	pim/pim_mroute.c pim/pim_ssm.c pim/pim_join_prune.c pim/pim_join_prune_state_machine.c\
	igmp/igmp.c igmp/igmp_cmd.c igmp/igmp_pkt.c igmp/igmpv3.c igmp/igmp_util.c igmp/igmp_time.c

noinst_HEADERS = \
	ipmc.h ipmc_group.h ipmc_if.h ipmc_ip.h ipmc_mac.h ipmc_public.h pim/pim.h pim/pim_cmd.h pim/pim_neighbor.h pim/pim_register.h pim/pim_pkt.h \
	pim/pim_mroute.h  pim/pim_join_prune.h \
	igmp/igmp.h igmp/igmp_cmd.h igmp/igmp_pkt.h igmp/igmpv3.h igmp/igmp_util.h igmp/igmp_time.h

ipmc_SOURCES = \
	ipmc_main.c $(libipmc_a_SOURCES)

ipmc_LDADD = ../lib/libzebra.la @LIBCAP@

examplesdir = $(exampledir)
dist_examples_DATA = ipmc.conf.sample

EXTRA_DIST = IPMCv2-MIB.txt

