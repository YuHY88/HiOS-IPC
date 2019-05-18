## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib \
	      -I./gpnDevInfo -I./gpnDevInfo/devCoreInfo/ -I./gpnSignal/ -I./gpnSignal/gpnSignal/\
	      -I./gpnSocket/ -I./gpnSocket/socketComm/ -I./gpnSupport -I./gpnSupport/gpnSupport/\
	      -I./gpnLog -I./gpnLog/gpnLog -I$(top_builddir)/snmp/include -I$(top_builddir)/snmp
AM_CFLAGS = $(WERROR)
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"

lib_LTLIBRARIES = libzebra.la
libzebra_la_LDFLAGS = -version-info 0:0:0 

libzebra_la_SOURCES = \
	network.c pid_output.c getopt.c getopt1.c daemon.c \
	checksum.c vector.c linklist.c vty.c command.c \
	sockunion.c prefix.c thread.c if.c memory.c buffer.c table.c hash.c \
	filter.c distribute.c stream.c str.c log.c plist.c \
	sockopt.c smux.c agentx.c snmp.c md5.c if_rmap.c keychain.c privs.c \
	sigevent.c pqueue.c jhash.c memtypes.c workqueue.c vrf.c ifm_common.c ether.c hash1.c \
	index.c inet_ip.c msg_ipc.c msg_ipc_n.c hptimer.c pkt_buffer.c ptree.c radix_tree.c \
	routemap.c devm_com.c route_com.c errcode.c timer.c qos_common.c alarm_common.c \
	mpls_common.c filem_common.c sys_ha.c mempool.c msg_ipc_r.c memshare.c snmp_common.c ospf_common.c\
	msdh_common.c crc8.c crc32.c sdh_public_common.c oam_common.c\
	gpnDevInfo/gpnDevCoreInfo.c gpnDevInfo/gpnDevCoreInfoFunApi.c gpnDevInfo/gpnGlobalDevDriver.c gpnDevInfo/gpnGlobalDevTypeDef.c\
	gpnLog/gpnLog.c gpnLog/gpnLogFuncApi.c gpnLog/gpnLogUnifyPath.c \
	gpnSignal/gpnSignalPowDown.c gpnSignal/gpnSignalProc.c gpnSignal/gpnSignalReboot.c gpnSignal/gpnSignalSdkPain.c \
	gpnSocket/gpnCommList.c gpnSocket/gpnSelectOpt.c gpnSocket/gpnSockCommFuncApi.c gpnSocket/gpnSockCoprocMsgDef.c \
	gpnSocket/gpnSockImiMsgDef.c gpnSocket/gpnSockProtocolMsgDef.c gpnSocket/gpnSockSysMgtMsgDef.c gpnSocket/gpnTimerService.c \
	gpnSocket/gpnDebug.c gpnSocket/gpnSockAliveMan.c gpnSocket/gpnSockCommModuDef.c gpnSocket/gpnSocketMan.c gpnSocket/gpnSockInitdMsgDef.c \
	gpnSocket/gpnSockSdmMsgDef.c gpnSocket/gpnSockTransferMsgDef.c gpnSocket/gpnUnifyPathDef.c gpnSocket/gpnDebugFuncApi.c\
	gpnSocket/gpnSockAlmMsgDef.c gpnSocket/gpnSockCommRoleMan.c gpnSocket/gpnSockFTMsgDef.c gpnSocket/gpnSockMsgDef.c gpnSocket/gpnSockStatMsgDef.c \
	gpnSocket/gpnSockXloadMsgDef.c gpnSocket/gpnVariableLenList.c gpnSocket/gpnGlobalPortIndexDef.c gpnSocket/gpnSockCfgMgtMsgDef.c \
	gpnSocket/gpnSockCommUnifyPath.c gpnSocket/gpnSockIfmMsgDef.c gpnSocket/gpnSockNmSynMsgDef.c gpnSocket/gpnSockSynceMsgDef.c gpnSocket/gpnSpecialWarrper.c \
	gpnSupport/gpnMemoryApi.c  gpnSupport/gpnMemorySmpMgt.c  gpnSupport/gpnMemoryUP.c

BUILT_SOURCES = memtypes.h gitversion.h

libzebra_la_DEPENDENCIES = @LIB_REGEX@

libzebra_la_LIBADD = @LIB_REGEX@ @LIBCAP@

pkginclude_HEADERS = \
	buffer.h checksum.h command.h filter.h getopt.h hash.h \
	if.h linklist.h log.h syslog.h \
	memory.h network.h prefix.h  distribute.h sockunion.h \
	str.h stream.h table.h thread.h vector.h version.h vty.h zebra.h \
	plist.h sockopt.h smux.h md5.h if_rmap.h keychain.h \
	privs.h sigevent.h pqueue.h jhash.h zassert.h memtypes.h \
	workqueue.h route_types.h libospf.h vrf.h fifo.h ifm_common.h ether.h \
	hash1.h index.h inet_ip.h module_id.h msg_ipc.h msg_ipc_n.h  hptimer.h\
	pkt_buffer.h pkt_type.h ptree.h radix_tree.h sem.h types.h \
	routemap.h devm_com.h route_com.h aaa_common.h errcode.h timer.h qos_common.h ospf_common.h\
	mpls_common.h rmon_common.h mempool.h memshare.h snmp_common.h alarm.h \
	msdh_common.h crc8.h crc32.h sdh_public_common.h sdh_vcg_common.h  oam_common.h\
	gpnDevInfo/devCoreInfo/gpnDevCoreInfoFunApi.h  gpnDevInfo/devCoreInfo/gpnDevCoreInfo.h \
	gpnDevInfo/devCoreInfo/gpnDevStatus.h  gpnDevInfo/devCoreInfo/gpnGlobalDevDriver.h  \
	gpnDevInfo/devCoreInfo/gpnGlobalDevTypeDef.h\
	gpnLog/gpnLog/gpnLogFuncApi.h  gpnLog/gpnLog/gpnLog.h  gpnLog/gpnLog/gpnLogUnifyPath.h\
	gpnSignal/gpnSignal/gpnSignalPowDown.h  gpnSignal/gpnSignal/gpnSignalProc.h  gpnSignal/gpnSignal/gpnSignalReboot.h \
	gpnSignal/gpnSignal/gpnSignalSdkPain.h  gpnSignal/gpnSignal/gpnSignalType.h \
	gpnSocket/socketComm/gpnAlmTypeDef.h gpnSocket/socketComm/gpnDevInfoProc.h gpnSocket/socketComm/gpnSockCfgMgtMsgDef.h\
	gpnSocket/socketComm/gpnSockCoprocMsgDef.h gpnSocket/socketComm/gpnSockInclude.h \
	gpnSocket/socketComm/gpnSockSdmMsgDef.h gpnSocket/socketComm/gpnSockTypeDef.h gpnSocket/socketComm/gpnUnifyPathDef.h\
	gpnSocket/socketComm/gpnCommList.h gpnSocket/socketComm/gpnGlobalPortIndexDef.h\
	gpnSocket/socketComm/gpnSockCommFuncApi.h gpnSocket/socketComm/gpnSocketMan.h gpnSocket/socketComm/gpnSockInitdMsgDef.h\
	gpnSocket/socketComm/gpnSockStatMsgDef.h gpnSocket/socketComm/gpnSockXloadMsgDef.h \
	gpnSocket/socketComm/gpnVariableLenList.h gpnSocket/socketComm/gpnDebugFuncApi.h gpnSocket/socketComm/gpnSelectOpt.h \
	gpnSocket/socketComm/gpnSockCommModuDef.h  gpnSocket/socketComm/gpnSockFTMsgDef.h \
	gpnSocket/socketComm/gpnSockMsgDef.h gpnSocket/socketComm/gpnSockSynceMsgDef.h gpnSocket/socketComm/gpnSpecialWarrper.h \
	gpnSocket/socketComm/gpnDebug.h gpnSocket/socketComm/gpnSockAliveMan.h gpnSocket/socketComm/gpnSockCommRoleMan.h \
	gpnSocket/socketComm/gpnSockIfmMsgDef.h gpnSocket/socketComm/gpnSockNmSynMsgDef.h gpnSocket/socketComm/gpnSockSysMgtMsgDef.h\
	gpnSocket/socketComm/gpnStatTypeDef.h gpnSocket/socketComm/gpnDebugUser.h gpnSocket/socketComm/gpnSockAlmMsgDef.h\
	gpnSocket/socketComm/gpnSockCommUnifyPath.h gpnSocket/socketComm/gpnSockImiMsgDef.h gpnSocket/socketComm/gpnSockProtocolMsgDef.h \
	gpnSocket/socketComm/gpnSockTransferMsgDef.h gpnSocket/socketComm/gpnTimerService.h \
	gpnSupport/gpnSupport/gpnMemoryApi.h  gpnSupport/gpnSupport/gpnMemorySmpMgt.h  gpnSupport/gpnSupport/gpnMemoryUP.h

noinst_HEADERS = \
	plist_int.h

EXTRA_DIST = \
	regex.c regex-gnu.h \
	queue.h \
	memtypes.awk \
	gitversion.pl

#route_types.h: $(srcdir)/route_types.txt $(srcdir)/route_types.pl
#	@PERL@ $(srcdir)/route_types.pl < $(srcdir)/route_types.txt > $@

if GIT_VERSION

# bit of a trick here to always have up-to-date git stamps without triggering
# unneccessary rebuilds.  .PHONY causes the .tmp file to be rebuilt always,
# but if we use that on gitversion.h it'll ripple through the .c file deps.
# (even if gitversion.h's file timestamp doesn't change, make will think it
# did, because of .PHONY...)

.PHONY: gitversion.h.tmp
.SILENT: gitversion.h gitversion.h.tmp
GITH=gitversion.h
gitversion.h.tmp: $(srcdir)/../.git
	@PERL@ $(srcdir)/gitversion.pl $(srcdir) > ${GITH}.tmp
gitversion.h: gitversion.h.tmp
	{ test -f ${GITH} && diff -s -q ${GITH}.tmp ${GITH}; } || cp -v ${GITH}.tmp ${GITH}

else
.PHONY: gitversion.h
gitversion.h:
	true
endif
