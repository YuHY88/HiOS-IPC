## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I.. -I$(top_srcdir) -I$(top_srcdir)/lib -I$(top_builddir)/lib \
	-I$(top_builddir)/lib/gpnDevInfo -I$(top_builddir)/lib/gpnFtFileType -I$(top_builddir)/lib/gpnLog \
        -I$(top_builddir)/lib/gpnSignal -I$(top_builddir)/lib/gpnSocket -I$(top_builddir)/lib/gpnSupport \
        -I$(top_builddir)/lib/gpnSyn
DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"
INSTALL_SDATA=@INSTALL@ -m 600

LIBS = @LIBS@ @CURSES@ @LIBPAM@ -pthread

AM_CFLAGS = $(WERROR)

noinst_LIBRARIES = libstatis.a
sbin_PROGRAMS = statis

libstatis_a_SOURCES = gpnMemUserSup.c gpnStatCollectProc.c gpnStatDataStruct.c \
	gpnStatDebug.c gpnStatFuncApi.c gpnStatIpc.c gpnStatProcMain.c \
	gpnStatSnmpApi.c gpnStatTimeProc.c gpnStatType.c gpnStatUnifyPath.c \
	gpnStatSnmp.c gpnStatCmd.c gpnStatCmdShow.c 

noinst_HEADERS = gpnMemUserSup.h gpnStatCollectProc.h gpnStatDataStruct.h \
	gpnStatDebug.h gpnStatFuncApi.h gpnStatMain.h gpnStatProcMain.h \
	gpnStatSnmpApi.h gpnStatTimeProc.h gpnStatUnifyPath.h \
	snmp_handle.h snmp_subagent.h stat_debug.h type_def.h \
	gpnStatIpc.h gpnStatSnmp.h gpnStatCmd.h gpnStatCmdShow.h


statis_SOURCES = gpnStatMain.c $(libstatis_a_SOURCES)
statis_LDADD = ../lib/libzebra.la @LIBCAP@


examplesdir = $(exampledir)
dist_examples_DATA = statis.conf.sample 


