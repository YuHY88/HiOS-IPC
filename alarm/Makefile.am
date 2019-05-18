## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib \
	-I$(top_builddir)/lib/gpnDevInfo -I$(top_builddir)/lib/gpnFtFileType -I$(top_builddir)/lib/gpnLog \
	-I$(top_builddir)/lib/gpnSignal -I$(top_builddir)/lib/gpnSocket -I$(top_builddir)/lib/gpnSupport \
	-I$(top_builddir)/lib/gpnSyn
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libalarm.a
sbin_PROGRAMS = alarm

libalarm_a_SOURCES = gpnAlmApi.c gpnAlmDebug.c gpnAlmScan.c gpnAlmShow.c \
	gpnAlmTimeProc.c gpnAlmTmpRestrain.c gpnAlmType.c gpnAlmUnifyPath.c gpnMemUserSup.c \
	gpnAlmMsgIO.c gpnAlmSnmp.c gpnAlmCmd.c gpnAlmCmdShow.c gpnAlmAlmNotify.c gpnAlmCfg.c \
	gpnAlmDataStructApi.c gpnAlmMsgProc.c gpnAlmOptDriver.c gpnAlmPortNotify.c \
	gpnAlmPowDown.c gpnAlmSnmpApi.c gpnAlmDataSheet.c testDataStructure.c gpnAlmTrap.c \
	gpnAlmTrapNotify.c gpnAlmMasterSlaveSync.c

noinst_HEADERS = alarm_debug.h gpnAlmApi.h gpnAlmDebug.h gpnAlmMain.h gpnAlmScan.h \
	gpnAlmShow.h gpnAlmTimeProc.h gpnAlmTmpRestrain.h gpnAlmTrapNotify.h gpnAlmTypeStruct.h \
	gpnAlmUnifyPath.h gpnMemUserSup.h type_def.h gpnAlmMsgIO.h gpnAlmSnmp.h gpnAlmCmd.h \
	gpnAlmCmdShow.h gpnAlmAlmNotify.h gpnAlmCfg.h \
    	gpnAlmDataStructApi.h gpnAlmMsgProc.h gpnAlmOptDriver.h gpnAlmPortNotify.h \
    	gpnAlmPowDown.h gpnAlmSnmpApi.h gpnAlmDataSheet.h testDataStructure.h gpnAlmTrap.h \
	gpnAlmMasterSlaveSync.h

alarm_SOURCES = gpnAlmMain.c $(libalarm_a_SOURCES)

alarm_LDADD = ../lib/libzebra.la  @LIBCAP@  

examplesdir = $(exampledir)
dist_examples_DATA = alarm.conf.sample

