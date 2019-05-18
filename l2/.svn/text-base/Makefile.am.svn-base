## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libl2.a
sbin_PROGRAMS = l2

am_libl2_a_OBJECTS = vlan.$(OBJEXT) vlan_cmd.$(OBJEXT) l2_if.$(OBJEXT) \
        l2if_cmd.$(OBJEXT) qinq.$(OBJEXT) qinq_cmd.$(OBJEXT) \
        mac_cmd.$(OBJEXT) mac_static.$(OBJEXT) l2_msg.$(OBJEXT) \
        lldp/lldp_api.$(OBJEXT) lldp/lldp_cli.$(OBJEXT) lldp/lldp_if.$(OBJEXT) \
        lldp/lldp_init.$(OBJEXT) lldp/lldp_msg.$(OBJEXT) \
        lldp/lldp_rx_sm.$(OBJEXT) lldp/lldp_show.$(OBJEXT) \
        lldp/lldp_tx_sm.$(OBJEXT)  l2cp/l2cp_cli.$(OBJEXT) l2cp/l2cp.$(OBJEXT)\
		trunk.$(OBJEXT) trunk_cmd.$(OBJEXT) lacp/lacp.$(OBJEXT) \
		loopd/loop_detect.$(OBJEXT) loopd/loop_detect_cmd.$(OBJEXT)\
		cfm/cfm.$(OBJEXT) cfm/cfm_session.$(OBJEXT) cfm/cfm_lb.$(OBJEXT)  cfm/cfm_lt.$(OBJEXT)  cfm/cfm_test.$(OBJEXT) cfm/cfm_cli.$(OBJEXT)\
		aps/elps.$(OBJEXT) aps/elps_cmd.$(OBJEXT) aps/elps_pkt.$(OBJEXT) aps/elps_fsm.$(OBJEXT) aps/elps_timer.$(OBJEXT)\
	mstp/mstp_api.$(OBJEXT) mstp/mstp_base_procedure.$(OBJEXT) mstp/mstp_cli.$(OBJEXT) \
	mstp/mstp_init.$(OBJEXT) mstp/mstp_msg.$(OBJEXT) mstp/mstp_port.$(OBJEXT) \
	mstp/mstp_show.$(OBJEXT) mstp/mstp_sm.$(OBJEXT) mstp/mstp_sm_procedure.$(OBJEXT) \
	mstp/mstp_timer.$(OBJEXT) mstp/mstp_cfg_digest.$(OBJEXT)\
        efm/efm.$(OBJEXT)  efm/efm_cmd.$(OBJEXT) efm/efm_state.$(OBJEXT) efm/efm_link_monitor.$(OBJEXT) efm/efm_pkt.$(OBJEXT) efm/efm_agent.$(OBJEXT) efm/efm_agent_snmp.$(OBJEXT) efm/efm_snmp.$(OBJEXT) \
	raps/erps.$(OBJEXT) raps/erps_cmd.$(OBJEXT) raps/erps_fsm.$(OBJEXT) raps/erps_fsm_action.$(OBJEXT) raps/erps_pkt.$(OBJEXT) raps/erps_timer.$(OBJEXT)\
	port_isolate.$(OBJEXT)
am__objects_1 = vlan.$(OBJEXT) vlan_cmd.$(OBJEXT) l2_if.$(OBJEXT) \
        l2if_cmd.$(OBJEXT) qinq.$(OBJEXT) qinq_cmd.$(OBJEXT) \
        mac_cmd.$(OBJEXT) mac_static.$(OBJEXT) l2_msg.$(OBJEXT)\
        lldp/lldp_api.$(OBJEXT) lldp/lldp_cli.$(OBJEXT) lldp/lldp_if.$(OBJEXT) \
        lldp/lldp_init.$(OBJEXT) lldp/lldp_msg.$(OBJEXT) \
        lldp/lldp_rx_sm.$(OBJEXT) lldp/lldp_show.$(OBJEXT) \
        lldp/lldp_tx_sm.$(OBJEXT)  l2cp/l2cp_cli.$(OBJEXT) l2cp/l2cp.$(OBJEXT)\
		trunk.$(OBJEXT) trunk_cmd.$(OBJEXT) lacp/lacp.$(OBJEXT) \
		loopd/loop_detect.$(OBJEXT) loopd/loop_detect_cmd.$(OBJEXT)\
		cfm/cfm.$(OBJEXT) cfm/cfm_session.$(OBJEXT) cfm/cfm_lb.$(OBJEXT) cfm/cfm_lt.$(OBJEXT) cfm/cfm_test.$(OBJEXT) cfm/cfm_cli.$(OBJEXT)\
		aps/elps.$(OBJEXT) aps/elps_cmd.$(OBJEXT) aps/elps_pkt.$(OBJEXT) aps/elps_fsm.$(OBJEXT) aps/elps_timer.$(OBJEXT)\
	mstp/mstp_api.$(OBJEXT) mstp/mstp_base_procedure.$(OBJEXT) \
	mstp/mstp_cli.$(OBJEXT) mstp/mstp_init.$(OBJEXT) \
	mstp/mstp_msg.$(OBJEXT) mstp/mstp_port.$(OBJEXT) mstp/mstp_show.$(OBJEXT) \
	mstp/mstp_sm.$(OBJEXT) mstp/mstp_sm_procedure.$(OBJEXT) mstp/mstp_timer.$(OBJEXT) mstp/mstp_cfg_digest.$(OBJEXT)\
        efm/efm.$(OBJEXT)  -lpthread efm/efm_cmd.$(OBJEXT) efm/efm_state.$(OBJEXT) efm/efm_link_monitor.$(OBJEXT) efm/efm_pkt.$(OBJEXT) efm/efm_agent.$(OBJEXT) efm/efm_agent_snmp.$(OBJEXT) efm/efm_snmp.$(OBJEXT) \
	raps/erps.$(OBJEXT) raps/erps_cmd.$(OBJEXT) raps/erps_fsm.$(OBJEXT) raps/erps_fsm_action.$(OBJEXT) raps/erps_pkt.$(OBJEXT) raps/erps_timer.$(OBJEXT)\
	port_isolate.$(OBJEXT)

libl2_a_SOURCES = \
	vlan.c vlan_cmd.c \
	l2_if.c l2if_cmd.c\
	qinq.c qinq_cmd.c \
	mac_cmd.c mac_static.c l2_msg.c\
	lldp/lldp_api.c lldp/lldp_cli.c lldp/lldp_if.c lldp/lldp_init.c  \
	lldp/lldp_msg.c lldp/lldp_rx_sm.c lldp/lldp_show.c lldp/lldp_tx_sm.c \
	trunk.c trunk_cmd.c lacp/lacp.c loopd/loop_detect.c loopd/loop_detect_cmd.c\
	cfm/cfm.c cfm/cfm_session.c cfm/cfm_lb.c cfm/cfm_lt.c cfm/cfm_test.c cfm/cfm_cli.c\
	aps/elps.c aps/elps_cmd.c aps/elps_pkt.c aps/elps_fsm.c aps/elps_timer.c\
	mstp/mstp_api.c mstp/mstp_base_procedure.c mstp/mstp_cli.c \
        mstp/mstp_init.c mstp/mstp_msg.c mstp/mstp_port.c \
        mstp/mstp_show.c mstp/mstp_sm.c mstp/mstp_sm_procedure.c \
        mstp/mstp_timer.c mstp/mstp_cfg_digest.c l2cp/l2cp_cli.c l2cp/l2cp.c\
		efm/efm.c efm/efm_cmd.c efm/efm_state.c efm/efm_link_monitor.c efm/efm_pkt.c efm/efm_snmp.c efm/efm_agent.c efm/efm_agent_snmp.c\
		raps/erps.c raps/erps_cmd.c raps/erps_fsm.c raps/erps_fsm_action.c raps/erps_pkt.c raps/erps_timer.c\
	port_isolate.c

noinst_HEADERS = \
	vlan.h l2_if.h qinq.h mac_static.h l2_msg.h l2_snmp.h\
	lldp/lldp_cli.h lldp/lldp.h lldp/lldp_init.h lldp/lldp_msg.h \
	lldp/lldp_rx_sm.h lldp/lldp_show.h lldp/lldp_tlv.h lldp/lldp_tx_sm.h \ 
	trunk.h lacp/lacp.h loopd/loop_detect.h l2cp/l2cp.h\
	cfm/cfm.h cfm/cfm_session.h cfm/cfm_lb.h cfm/cfm_lt.h cfm/cfm_test.h cfm/cfm_cli.h\
	aps/elps.h aps/elps_cmd.h aps/elps_pkt.h aps/elps_fsm.h aps/elps_timer.h\
	mstp/mstp_api.h mstp/mstp_base_procedure.h mstp/mstp_bpdu.h \
        mstp/mstp_cli.h mstp/mstp.h mstp/mstp_init.h \
        mstp/mstp_msg.h mstp/mstp_port.h mstp/mstp_show.h \
        mstp/mstp_sm.h mstp/mstp_sm_procedure.h mstp/mstp_timer.h mstp/mstp_cfg_digest.h\
		efm/efm.h efm/efm_cmd.h efm/efm_state.h efm/efm_link_monitor.h efm/efm_pkt.h efm/efm_snmp.h efm/efm_agent.h efm/efm_agent_snmp.h\   
		raps/erps.h raps/erps_cmd.h raps/erps_fsm.h raps/erps_fsm_action.h raps/erps_pkt.h raps/erps_timer.h\
	port_isolate.h

@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/vlan.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/vlan_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2_if.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2if_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2_msg.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mac_static.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2if_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_api.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_cli.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_if.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_init.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_msg.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_rx_sm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_show.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lldp/lldp_tx_sm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/trunk.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/trunk_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/lacp/lacp.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/loopd/loop_detect.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/loopd/loop_detect_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/qinq.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/qinq_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2_main.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/cfm/cfm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/cfm/cfm_session.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/cfm/cfm_lb.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/cfm/cfm_lt.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/cfm/cfm_test.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/cfm/cfm_cli.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2cp/l2cp_cli.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/l2cp/l2cp.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aps/elps.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aps/elps_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aps/elps_pkt.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aps/elps_fsm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aps/elps_timer.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_api.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_base_procedure.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_cli.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_init.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_msg.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_port.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_show.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_sm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_sm_procedure.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_timer.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mstp/mstp_cfg_digest.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/raps/erps.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/raps/erps_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/raps/erps_fsm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/raps/erps_fsm_action.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/raps/erps_pkt.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/raps/erps_timer.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/efm/efm.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/efm/efm_cmd.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/efm/efm_state.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/efm/efm_link_monitor.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/efm/efm_pkt.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/efm/efm_snmp.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/port_isolate.Po@am__quote@

l2_SOURCES = l2_main.c $(libl2_a_SOURCES)
l2_LDADD = ../lib/libzebra.la @LIBCAP@


mostlyclean-libtool:
	-rm -f *.lo
	-rm -f lldp/*.lo
	-rm -f cfm/*.lo
	-rm -f aps/*.lo	
	-rm -f raps/*.lo	
	-rm -f mstp/*.lo
	-rm -f efm/*.lo
	-rm -f lacp/*.lo	
	-rm -f loopd/*.lo
	-rm -f l2cp/*.lo
	
mostlyclean-compile:
	-rm -f *.$(OBJEXT)
	-rm -f lldp/*.$(OBJEXT)
	-rm -f cfm/*.$(OBJEXT)
	-rm -f aps/*.$(OBJEXT)	
	-rm -f raps/*.$(OBJEXT)	
	-rm -f mstp/*.$(OBJEXT)
	-rm -f efm/*.$(OBJEXT)	
	-rm -f lacp/*.$(OBJEXT)	
	-rm -f loopd/*.$(OBJEXT)
	-rm -f l2cp/*.$(OBJEXT)	

examplesdir = $(exampledir)
dist_examples_DATA = l2.conf.sample 

EXTRA_DIST = L2-MIB.txt

