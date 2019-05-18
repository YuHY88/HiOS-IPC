/*
 * snmpd.c
 */
/** @defgroup agent The Net-SNMP agent
 * The snmp agent responds to SNMP queries from management stations
 */
/* Portions of this file are subject to the following copyrights.  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Copyright 1988, 1989 by Carnegie Mellon University
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of CMU not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * *****************************************************************
 */
/*
 * Copyright ? 2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */
#define _GNU_SOURCE
#include <sched.h>
#include <assert.h>
#include "net-snmp-config.h"

#include <lib/vty.h>
#include <lib/thread.h>
#include <lib/msg_ipc.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/alarm.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/msg_ipc.h>
#include <lib/snmp_common.h>
#include "snmp_config_table.h"
#include <lib/ospf_common.h>
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"

#include "ipran_snmp_data_cache.h"


#if HAVE_IO_H
#include <io.h>
#endif
#include <stdio.h>
#include <errno.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#elif HAVE_WINSOCK_H
#include <winsock.h>
#endif
#if HAVE_NET_IF_H
#include <net/if.h>
#endif
#if HAVE_INET_MIB2_H
#include <inet/mib2.h>
#endif
#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#include <signal.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_PROCESS_H              /* Win32-getpid */
#include <process.h>
#endif
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif
#if HAVE_GRP_H
#include <grp.h>
#endif

#ifndef PATH_MAX
# ifdef _POSIX_PATH_MAX
#  define PATH_MAX _POSIX_PATH_MAX
# else
#  define PATH_MAX 255
# endif
#endif

#ifndef FD_SET
typedef long    fd_mask;
#define NFDBITS (sizeof(fd_mask) * NBBY)        /* bits per mask */
#define FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      memset((p), 0, sizeof(*(p)))
#endif

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "fd_event_manager.h"

#include "m2m.h"
#include "mib_module_config.h"

#include "struct.h"
#include "mib_modules.h"

#include "util_funcs.h"

#include "agent_trap.h"

#include "system.h"

#include "table.h"
#include "table_iterator.h"

#include "snmpd.h"

#include "lib/rmon_common.h"
#include "dhcp/dhcp_msg.h"
#include "l2/l2_snmp.h"
#include "l2/lldp/lldp.h"
#include "l2/efm/efm_agent.h"
#include "ospfd/ospf_msg.h"
#include "snmp_config_table.h"
#include "alarm/gpnAlmSnmp.h"
#include "alarm/gpnAlmTypeStruct.h"
#include "statis/gpnStatDataStruct.h"

#include <lib/alarm.h>
#include <sys/prctl.h>

/*
 * Include winservice.h to support Windows Service
 */
#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <winservice.h>

#define WIN32SERVICE

#endif

/*
 * Globals.
 */
#ifdef USE_LIBWRAP
#include <tcpd.h>
#endif                          /* USE_LIBWRAP */

#define TIMETICK         500000L

#define POWERDOWN_ALARM
#ifdef POWERDOWN_ALARM
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#ifndef NETLINK_MASK
#define NETLINK_MASK 3
#endif
#define MAX_MSGSIZE 1024
#endif

u_char str_value[64];

int             snmp_dump_packet;
int             reconfig = 0;
int             Facility = LOG_DAEMON;

#ifdef WIN32SERVICE
/*
 * SNMP Agent Status
 */
#define AGENT_RUNNING 1
#define AGENT_STOPPED 0
int             agent_status = AGENT_STOPPED;
/* app_name_long used for Event Log (syslog), SCM, registry etc */
LPTSTR          app_name_long = _T("Net-SNMP Agent");     /* Application Name */
#endif

const char     *app_name = "snmpd";

extern int      netsnmp_running;
extern char   **argvrestartp;
extern char    *argvrestart;
extern char    *argvrestartname;

#define SNMPD_DEFAULT_CONFIG   "snmpd.conf"
#define PATH_SNMPD_PID         "/var/run/snmpd.pid"
struct thread   pthread;
struct thread_master  *snmp_cli_master = NULL;
char           *vty_addr = NULL;
char           *pid_file = PATH_SNMPD_PID;  /* defined in config.h */
char           *config_file = NULL;
//int             vty_port = SNMPD_VTY_PORT;  /* defined in vty.h */
char            config_current[] = SNMPD_DEFAULT_CONFIG;
char            config_default[] = SNMPD_DEFAULT_CONFIG;

extern void     init_snmp(const char *type);
extern int snmp_check_input_ip_valid(int ip); //to check ip valid
extern SNMP_VERSION snmp_version;
#define NUM_SOCKETS 32

/*
 * Prototypes.
 */
int             snmp_read_packet(int);
int             snmp_input(int, netsnmp_session *, int, netsnmp_pdu *,
                           void *);
static void     usage(char *);
static void     SnmpTrapNodeDown(void);
static int      receive(void);
#ifdef WIN32SERVICE
void            StopSnmpAgent(void);
int             SnmpDaemonMain(int argc, TCHAR *argv[]);
int __cdecl     _tmain(int argc, TCHAR *argv[]);
#else
int             main(int, char **);
#endif

/*
 * These definitions handle 4.2 systems without additional syslog facilities.
 */
#ifndef LOG_CONS
#define LOG_CONS    0       /* Don't bother if not defined... */
#endif
#ifndef LOG_PID
#define LOG_PID     0       /* Don't bother if not defined... */
#endif
#ifndef LOG_LOCAL0
#define LOG_LOCAL0  0
#endif
#ifndef LOG_LOCAL1
#define LOG_LOCAL1  0
#endif
#ifndef LOG_LOCAL2
#define LOG_LOCAL2  0
#endif
#ifndef LOG_LOCAL3
#define LOG_LOCAL3  0
#endif
#ifndef LOG_LOCAL4
#define LOG_LOCAL4  0
#endif
#ifndef LOG_LOCAL5
#define LOG_LOCAL5  0
#endif
#ifndef LOG_LOCAL6
#define LOG_LOCAL6  0
#endif
#ifndef LOG_LOCAL7
#define LOG_LOCAL7  0
#endif
#ifndef LOG_DAEMON
#define LOG_DAEMON  0
#endif

/*For trap cache*/
int trap_cache_enable = ENABLE;
int trap_cache_num = 0;
struct list *trap_cache_list  = NULL;
int send_warm_start_enable = ENABLE;

/*alarm trap oid and define*/
#define SNMP_TRAP_POLL_INTVAL           10*1000

static oid SNMP_TRAP_OID[]              = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

static oid ALM_TRAP_PRODUCT_OID[]           = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 1};
static oid ALM_TRAP_DISAPPEAR_OID[]         = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 2};
static oid ALM_TRAP_EVENT_REPORT_OID[]      = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 3};
static oid ALM_TRAP_REVERSE_AUTO_REC_OID[]  = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 4};

static oid DHCP_TRAP_EVENT_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 1, 11};

static oid LLDP_REMOTE_INSERT_TRAP_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 10, 3, 1};
static oid LLDP_REMOTE_DELETE_TRAP_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 10, 3, 2};
static oid LLDP_REMOTE_AGEOUT_TRAP_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 10, 3, 3};

static oid LLDP_LOCAL_PORT_DESC_OID[]           = {1, 0, 8802, 1, 1, 2, 1, 3, 7, 1, 4};
static oid LLDP_REMOTE_CHASSIS_ID_SUBTYPE_OID[] = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 4};
static oid LLDP_REMOTE_CHASSIS_ID_OID[]         = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 5};
static oid LLDP_REMOTE_PORT_ID_SUBTYPE_OID[]    = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 6};
static oid LLDP_REMOTE_PORT_ID_OID[]            = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 7};
static oid LLDP_REMOTE_PORT_DESC_OID[]          = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 8};
static oid LLDP_REMOTE_SYS_NAME_OID[]           = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 9};
static oid LLDP_REMOTE_SYS_DESC_OID[]           = {1, 0, 8802, 1, 1, 2, 1, 4, 1, 1, 10};
static oid EFM_AGENT_TRAP_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 2, 100, 1, 2, 12};
static oid EFM_AGENT_REMOTE_OLT_ETH_OID[][EFM_AGENT_ALM_MIBNODE_LEN]   =
	{{1, 3, 6, 1, 4, 1, 9966, 2, 100, 1, 1, 11, 2, 2, 2, 1, 3, 0, 1},
           {1, 3, 6, 1, 4, 1, 9966, 2, 100, 1, 1, 11, 2, 2, 4, 1, 3, 0, 1}};

/*OSPFDCN trap*/
static oid OSPFDCN_TRAP_NE_ONLINE_OID[]         = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 3, 1};
static oid OSPFDCN_TRAP_NE_OFFLINE_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 3, 2};
static oid OSPFDCN_TRAP_NE_ID_OID[]             = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 2, 1, 1};
static oid OSPFDCN_TRAP_NE_IP_OID[]             = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 2, 1, 2};
static oid OSPFDCN_TRAP_NE_METRIC_OID[]         = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 2, 1, 3};
static oid OSPFDCN_TRAP_NE_DEVICE_TYPE_OID[]    = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 2, 1, 4};
static oid OSPFDCN_TRAP_NE_MAC_OID[]            = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 2, 1, 5};
static oid OSPFDCN_TRAP_NE_VENDOR_OID[]         = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 3, 8, 2, 1, 6};
static oid MSDH_CMD_TRAP_EVENT_OID[]            = {1, 3, 6, 1, 4, 1, 9966, 5, 37, 1, 4};
static oid MSDH_CMD_TRAP_EVENT_MSG_OID[]        = {1, 3, 6, 1, 4, 1, 9966, 5, 37, 1, 3, 0};

static oid RMON_TRAP_RAISE_OID[]                = {1, 3, 6, 1, 2, 1, 16, 0, 1};
static oid RMON_TRAP_FALL_OID[]                 = {1, 3, 6, 1, 2, 1, 16, 0, 2};
static oid RMON_TRAP_ALARM_ID_OID[]             = {1, 3, 6, 1, 2, 1, 16, 3, 1, 1, 1, 0};
static oid RMON_TRAP_ALARM_VAR_OID[]            = {1, 3, 6, 1, 2, 1, 16, 3, 1, 1, 3, 0};
static oid RMON_TRAP_ALARM_SAMPLE_TYPE_OID[]    = {1, 3, 6, 1, 2, 1, 16, 3, 1, 1, 4, 0};
static oid RMON_TRAP_ALARM_VALUE_OID[]          = {1, 3, 6, 1, 2, 1, 16, 3, 1, 1, 5, 0};
static oid RMON_TRAP_ALARM_RIASE_OID[]          = {1, 3, 6, 1, 2, 1, 16, 3, 1, 1, 7, 0};
static oid RMON_TRAP_ALARM_FALL_OID[]           = {1, 3, 6, 1, 2, 1, 16, 3, 1, 1, 8, 0};

/* statis trap, add by lipf, 2018/3/24 */
static oid STATIS_TRAP_PRODUCT_OID[]            = {1, 3, 6, 1, 4, 1, 9966, 5, 15, 24, 1};

static oid IF_LINK_IFINDEX_OID[]                = {1, 3, 6, 1, 2, 1, 2, 2, 1, 1, 0};
static oid IF_LINK_ADMIN_STATUS_OID[]           = {1, 3, 6, 1, 2, 1, 2, 2, 1, 7, 0};
static oid IF_LINK_OPER_STATUS_OID[]            = {1, 3, 6, 1, 2, 1, 2, 2, 1, 8, 0};
static oid IF_LINK_DESC_OID[]                   = {1, 3, 6, 1, 2, 1, 2, 2, 1, 2, 0};
static oid IPRAN_LINK_UP_OID[]                  = {1, 3, 6, 1, 6, 3, 1, 1, 5, 3};
static oid IPRAN_LINK_DOWN_OID[]                = {1, 3, 6, 1, 6, 3, 1, 1, 5, 4};

static struct counter64 uint64_value;

unsigned int gDevTypeID ;

static void
usage(char *prog)
{
#ifdef WIN32SERVICE
    printf("\nUsage:  %s [-register] [-quiet] [OPTIONS] [LISTENING ADDRESSES]",
           prog);
    printf("\n        %s [-unregister] [-quiet]", prog);
#else
    printf("\nUsage:  %s [OPTIONS] [LISTENING ADDRESSES]", prog);
#endif
    printf("\n");
    printf("\n\tVersion:  %s\n", netsnmp_get_version());
    printf("\tWeb:      http://www.net-snmp.org/\n");
    printf("\tEmail:    net-snmp-coders@lists.sourceforge.net\n");
    printf("\n  -a\t\t\tlog addresses\n");
    printf("  -A\t\t\tappend to the logfile rather than truncating it\n");
    printf("  -c FILE[,...]\t\tread FILE(s) as configuration file(s)\n");
    printf("  -C\t\t\tdo not read the default configuration files\n");
    //printf("  -d\t\t\tdump sent and received SNMP packets\n");
    printf("  -d\t\t\tdeamon mode\n");
    printf("  -D TOKEN[,...]\tturn on debugging output for the given TOKEN(s)\n"
           "\t\t\t  (try ALL for extremely verbose output)\n");
    printf("  -f\t\t\tdo not fork from the shell\n");
#if HAVE_UNISTD_H
    printf("  -g GID\t\tchange to this numeric gid after opening\n"
           "\t\t\t  transport endpoints\n");
#endif
    printf("  -h, --help\t\tdisplay this usage message\n");
    printf("  -H\t\t\tdisplay configuration file directives understood\n");
    printf("  -I [-]INITLIST\tlist of mib modules to initialize (or not)\n");
    printf("\t\t\t  (run snmpd with -Dmib_init for a list)\n");
    printf("  -L <LOGOPTS>\t\ttoggle options controlling where to log to\n");
    snmp_log_options_usage("\t", stdout);
    printf("  -m MIBLIST\t\tuse MIBLIST instead of the default MIB list\n");
    printf("  -M DIRLIST\t\tuse DIRLIST as the list of locations\n\t\t\t  to look for MIBs\n");
    printf("  -p FILE\t\tstore process id in FILE\n");
    printf("  -q\t\t\tprint information in a more parsable format\n");
    printf("  -r\t\t\tdo not exit if files only accessible to root\n"
           "\t\t\t  cannot be opened\n");
#ifdef WIN32SERVICE
    printf("  -register\t\tregister as a Windows service\n");
    printf("  \t\t\t  (followed by -quiet to prevent message popups)\n");
    printf("  \t\t\t  (followed by the startup parameter list)\n");
    printf("  \t\t\t  Note that some parameters are not relevant when running as a service\n");
#endif
#if HAVE_UNISTD_H
    printf("  -u UID\t\tchange to this uid (numeric or textual) after\n"
           "\t\t\t  opening transport endpoints\n");
#endif
#ifdef WIN32SERVICE
    printf("  -unregister\t\tunregister as a Windows service\n");
    printf("  \t\t\t  (followed -quiet to prevent message popups)\n");
#endif
    printf("  -v, --version\t\tdisplay version information\n");
    printf("  -V\t\t\tverbose display\n");
#if defined(USING_AGENTX_SUBAGENT_MODULE)|| defined(USING_AGENTX_MASTER_MODULE)
    printf("  -x ADDRESS\t\tuse ADDRESS as AgentX address\n");
#endif
#ifdef USING_AGENTX_SUBAGENT_MODULE
    printf("  -X\t\t\trun as an AgentX subagent rather than as an\n"
           "\t\t\t  SNMP master agent\n");
#endif

    printf("\nDeprecated options:\n");
    printf("  -l FILE\t\tuse -Lf <FILE> instead\n");
    printf("  -P\t\t\tuse -p instead\n");
    printf("  -s\t\t\tuse -Lsd instead\n");
    printf("  -S d|i|0-7\t\tuse -Ls <facility> instead\n");

    printf("\n");
    exit(1);
}

static void
version(void)
{
    printf("\nNET-SNMP version:  %s\n", netsnmp_get_version());
    printf("Web:               http://www.net-snmp.org/\n");
    printf("Email:             net-snmp-coders@lists.sourceforge.net\n\n");
    exit(0);
}



/*---------------------------------- ä¿®æ”¹å…±äº«å†…å­˜æ—¶ä¿®æ”? lipf, 2018/8/23 --------------------*/
static void __attribute__ ((noreturn)) snmp_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(snmp_cli_master) 
    {
        thread_master_free (snmp_cli_master);
    }

    if(zlog_default)  
    {
        closezlog(zlog_default);
    }
    exit(status);
}


/* SIGHUP handler. */
static void sighup(void)
{
    zlog_info("SIGHUP received");
}

/* SIGINT handler. */
static void sigint(void)
{
    zlog_notice("Terminating on signal SIGINT");
    exit(0);
}

/* SIGTERM handler. */
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    snmp_exit(0);
}

struct quagga_signal_t
{
  int signal;                     /* signal number    */
  void (*handler) (void);         /* handler to call  */

  volatile sig_atomic_t caught;   /* private member   */
};


struct quagga_signal_t snmp_signals[] =
{
    {
        .signal  = SIGHUP,
        .handler = &sighup,
        .caught  = 0,
    },
    {
        .signal  = SIGINT,
        .handler = &sigint,
        .caught  = 0,
    },
    {
        .signal  = SIGTERM,
        .handler = &sigterm,
        .caught  = 0,
    },
    {
        .signal  = SIGALRM,
        .handler = &high_pre_timer_dotick,
        .caught  = 0,
    },
};


/*-------------------------------------- end ------------------------------------*/   





RETSIGTYPE
SnmpdShutDown(int a)
{
#ifdef WIN32SERVICE
    extern netsnmp_session *main_session;
#endif
    netsnmp_running = 0;
#ifdef WIN32SERVICE
    /*
     * In case of windows, select() in receive() function will not return
     * on signal. Thats why following function is called, which closes the
     * socket descriptors and causes the select() to return
     */
    snmp_close(main_session);
#endif

    if (NULL != zlog_default)
    {
        closezlog(zlog_default);
    }
}

#ifdef SIGHUP
RETSIGTYPE
SnmpdReconfig(int a)
{
    reconfig = 1;
    signal(SIGHUP, SnmpdReconfig);

    /* Reload config file */
    //vty_read_config ( config_file, config_default );

    /* Create VTY's socket */
    //vty_serv_sock ( vty_addr, vty_port, SNMPD_VTYSH_PATH );
    vty_serv_sock(SNMPD_VTYSH_PATH);
}
#endif

#ifdef SIGUSR1
extern void     dump_registry(void);
RETSIGTYPE
SnmpdDump(int a)
{
    dump_registry();
    signal(SIGUSR1, SnmpdDump);
}
#endif

RETSIGTYPE
SnmpdCatchRandomSignal(int a)
{
    /* Disable all logs and log the error via syslog */
    snmp_disable_log();
    snmp_enable_syslog();
    snmp_log(LOG_ERR, "Exiting on signal %d\n", a);
    snmp_disable_syslog();
    exit(1);
}

static void
SnmpTrapNodeDown(void)
{
    send_easy_trap(SNMP_TRAP_ENTERPRISESPECIFIC, 2);
    /*
     * XXX  2 - Node Down #define it as NODE_DOWN_TRAP
     */
}

struct tFaTrapInfo
{
    int                 iOid;
    unsigned char       type;
    unsigned char        *buf;
    size_t              len;
};

static int pkt_alarm_product_trap(stSnmpAlmCurrDSTableInfo *pRow)
{
    netsnmp_variable_list *var_list = NULL;
    oid alarmProductTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 11, 1, 1, 0, 0};

    oid *tmp_oid ;
    int len = 0;

    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)ALM_TRAP_PRODUCT_OID,
                              sizeof(oid)*OID_LENGTH(ALM_TRAP_PRODUCT_OID));

    const struct tFaTrapInfo    TrapInfo[]      =
    {
        { 1     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->index.iAlmCurrDSIndex)),        sizeof(pRow->index.iAlmCurrDSIndex) },
        { 2     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrAlmType)),            sizeof(pRow->data.iCurrAlmType) },
        { 3     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrDevIndex)),           sizeof(pRow->data.iCurrDevIndex) },
        { 4     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrPortIndex1)),         sizeof(pRow->data.iCurrPortIndex1) },
        { 5     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrPortIndex2)),         sizeof(pRow->data.iCurrPortIndex2) },
        { 6     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrPortIndex3)),         sizeof(pRow->data.iCurrPortIndex3) },
        { 7     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrPortIndex4)),         sizeof(pRow->data.iCurrPortIndex4) },
        { 8     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iCurrPortIndex5)),         sizeof(pRow->data.iCurrPortIndex5) },
        { 9     , ASN_INTEGER    , (unsigned char *)(&(pRow->data.iCurrAlmLevel)),           sizeof(pRow->data.iCurrAlmLevel) },
        { 10    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmCurrProductCnt)),      sizeof(pRow->data.iAlmCurrProductCnt) },
        { 11    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmCurrFirstProductTime)), sizeof(pRow->data.iAlmCurrFirstProductTime) },
        { 12    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmCurrThisProductTime)), sizeof(pRow->data.iAlmCurrThisProductTime) },
    };

    const int NodeNum = sizeof(TrapInfo) / sizeof(TrapInfo[0]);

    int i = 0;

    for (i = 0; i < NodeNum; i++)
    {
        // index
        len                                 = sizeof(alarmProductTrapRow_oid) / sizeof(alarmProductTrapRow_oid[0]);
        alarmProductTrapRow_oid[len - 2]    = TrapInfo[i].iOid;
        alarmProductTrapRow_oid[len - 1]    = pRow->index.iAlmCurrDSIndex;

        snmp_varlist_add_variable(&var_list,
                                  alarmProductTrapRow_oid, OID_LENGTH(alarmProductTrapRow_oid),
                                  TrapInfo[i].type,
                                  /* Set an appropriate value for the leaf */
                                  TrapInfo[i].buf,
                                  TrapInfo[i].len);
    }

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}

static int pkt_alarm_disappear_trap(stSnmpAlmHistDSTableInfo *pRow)
{
    netsnmp_variable_list *var_list = NULL;
    oid alarmDisappearTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 13, 1, 1, 0, 0};

    oid *tmp_oid ;
    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)ALM_TRAP_DISAPPEAR_OID,
                              sizeof(oid)*OID_LENGTH(ALM_TRAP_DISAPPEAR_OID));

    const struct tFaTrapInfo    TrapInfo[]      =
    {
        { 1     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->index.iAlmHistDSIndex)),          sizeof(pRow->index.iAlmHistDSIndex) },
        { 2     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistAlmType)),              sizeof(pRow->data.iHistAlmType) },
        { 3     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistDevIndex)),             sizeof(pRow->data.iHistDevIndex) },
        { 4     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistPortIndex1)),           sizeof(pRow->data.iHistPortIndex1) },
        { 5     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistPortIndex2)),           sizeof(pRow->data.iHistPortIndex2) },
        { 6     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistPortIndex3)),           sizeof(pRow->data.iHistPortIndex3) },
        { 7     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistPortIndex4)),           sizeof(pRow->data.iHistPortIndex4) },
        { 8     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iHistPortIndex5)),           sizeof(pRow->data.iHistPortIndex5) },
        { 9     , ASN_INTEGER    , (unsigned char *)(&(pRow->data.iHistAlmLevel)),             sizeof(pRow->data.iHistAlmLevel) },
        { 10    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmHistProductCnt)),        sizeof(pRow->data.iAlmHistProductCnt) },
        { 11    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmHistFirstProductTime)),  sizeof(pRow->data.iAlmHistFirstProductTime) },
        { 12    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmHistThisProductTime)),   sizeof(pRow->data.iAlmHistThisProductTime) },
        { 13    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iAlmHistThisDisappearTime)), sizeof(pRow->data.iAlmHistThisDisappearTime) },
    };

    const int NodeNum = sizeof(TrapInfo) / sizeof(TrapInfo[0]);
    int len = 0;
    int i = 0;

    for (i = 0; i < NodeNum; i++)
    {
        // index
        len                                 = sizeof(alarmDisappearTrapRow_oid) / sizeof(alarmDisappearTrapRow_oid[0]);
        alarmDisappearTrapRow_oid[len - 2]  = TrapInfo[i].iOid;
        alarmDisappearTrapRow_oid[len - 1]  = pRow->index.iAlmHistDSIndex;

        snmp_varlist_add_variable(&var_list,
                                  alarmDisappearTrapRow_oid, OID_LENGTH(alarmDisappearTrapRow_oid),
                                  TrapInfo[i].type,
                                  /* Set an appropriate value for the leaf */
                                  TrapInfo[i].buf,
                                  TrapInfo[i].len);
    }

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}

static int pkt_alarm_event_report_trap(stSnmpEventDSTableInfo *pRow)
{
    netsnmp_variable_list *var_list = NULL;

    oid *tmp_oid ;

    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)ALM_TRAP_EVENT_REPORT_OID,
                              sizeof(oid)*OID_LENGTH(ALM_TRAP_EVENT_REPORT_OID));

    oid alarmEventReportTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 15, 1, 1, 0, 0};
    int len = 0;

    const struct tFaTrapInfo    TrapInfo[]      =
    {
        { 1     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->index.iEventDSIndex)),      sizeof(pRow->index.iEventDSIndex) },
        { 2     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventAlmType)),       sizeof(pRow->data.iEventAlmType) },
        { 3     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventDevIndex)),      sizeof(pRow->data.iEventDevIndex) },
        { 4     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventPortIndex1)),    sizeof(pRow->data.iEventPortIndex1) },
        { 5     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventPortIndex2)),    sizeof(pRow->data.iEventPortIndex2) },
        { 6     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventPortIndex3)),    sizeof(pRow->data.iEventPortIndex3) },
        { 7     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventPortIndex4)),    sizeof(pRow->data.iEventPortIndex4) },
        { 8     , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventPortIndex5)),    sizeof(pRow->data.iEventPortIndex5) },
        { 9     , ASN_INTEGER    , (unsigned char *)(&(pRow->data.iEventAlmLevel)),      sizeof(pRow->data.iEventAlmLevel) },
        { 10    , ASN_INTEGER    , (unsigned char *)(&(pRow->data.iEventDetail)),        sizeof(pRow->data.iEventDetail) },
        { 11    , ASN_UNSIGNED   , (unsigned char *)(&(pRow->data.iEventTime)),          sizeof(pRow->data.iEventTime) },
    };
    const int NodeNum = sizeof(TrapInfo) / sizeof(TrapInfo[0]);
    int i = 0;

    for (i = 0; i < NodeNum; i++)
    {
        // index
        len                                 = sizeof(alarmEventReportTrapRow_oid) / sizeof(alarmEventReportTrapRow_oid[0]);
        alarmEventReportTrapRow_oid[len - 2]  = TrapInfo[i].iOid;
        alarmEventReportTrapRow_oid[len - 1]  = pRow->index.iEventDSIndex;

        snmp_varlist_add_variable(&var_list,
                                  alarmEventReportTrapRow_oid, OID_LENGTH(alarmEventReportTrapRow_oid),
                                  TrapInfo[i].type,
                                  /* Set an appropriate value for the leaf */
                                  TrapInfo[i].buf,
                                  TrapInfo[i].len);
    }

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}

static int pkt_alarm_reverse_auto_rec_trap(stSnmpAlmAttribCfgTableInfo *pRow)
{
    netsnmp_variable_list *var_list = NULL;

    oid *tmp_oid ;

    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)ALM_TRAP_EVENT_REPORT_OID,
                              sizeof(oid)*OID_LENGTH(ALM_TRAP_EVENT_REPORT_OID));

    oid alarmReverseAutoRecTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 9, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    int len = 0;

    const struct tFaTrapInfo    TrapInfo[]      =
    {
        { 1     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAlmSubType)),        sizeof(pRow->index.iAlmSubType) },
        { 2     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAttrDevIndex)),      sizeof(pRow->index.iAttrDevIndex) },
        { 3     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAttrPortIndex1)),    sizeof(pRow->index.iAttrPortIndex1) },
        { 4     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAttrPortIndex2)),    sizeof(pRow->index.iAttrPortIndex2) },
        { 5     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAttrPortIndex3)),    sizeof(pRow->index.iAttrPortIndex3) },
        { 6     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAttrPortIndex4)),    sizeof(pRow->index.iAttrPortIndex4) },
        { 7     , ASN_INTEGER   , (unsigned char *)(&(pRow->index.iAttrPortIndex5)),    sizeof(pRow->index.iAttrPortIndex5) },
    };
    const int NodeNum = sizeof(TrapInfo) / sizeof(TrapInfo[0]);
    int i = 0;

    for (i = 0; i < NodeNum; i++)
    {
        // index
        len = sizeof(alarmReverseAutoRecTrapRow_oid) / sizeof(alarmReverseAutoRecTrapRow_oid[0]);
        alarmReverseAutoRecTrapRow_oid[len - 8]  = TrapInfo[i].iOid;
        alarmReverseAutoRecTrapRow_oid[len - 7]  = pRow->index.iAlmSubType;
        alarmReverseAutoRecTrapRow_oid[len - 6]  = pRow->index.iAttrDevIndex;
        alarmReverseAutoRecTrapRow_oid[len - 5]  = pRow->index.iAttrPortIndex1;
        alarmReverseAutoRecTrapRow_oid[len - 4]  = pRow->index.iAttrPortIndex2;
        alarmReverseAutoRecTrapRow_oid[len - 3]  = pRow->index.iAttrPortIndex3;
        alarmReverseAutoRecTrapRow_oid[len - 2]  = pRow->index.iAttrPortIndex4;
        alarmReverseAutoRecTrapRow_oid[len - 1]  = pRow->index.iAttrPortIndex5;

        snmp_varlist_add_variable(&var_list,
                                  alarmReverseAutoRecTrapRow_oid, OID_LENGTH(alarmReverseAutoRecTrapRow_oid),
                                  TrapInfo[i].type,
                                  /* Set an appropriate value for the leaf */
                                  TrapInfo[i].buf,
                                  TrapInfo[i].len);
    }

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}

static int snmp_send_trap(struct ipc_mesg_n *pmsg)
{
    netsnmp_variable_list *var_list = NULL;

    struct snmp_trap_hdr *phdr = (struct snmp_trap_hdr *)pmsg->msg_data;
    struct snmp_trap_data *pdata = NULL;

    phdr->trap_oid = (oid *)(pmsg->msg_data + sizeof(struct snmp_trap_hdr));

    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)phdr->trap_oid,
                              sizeof(oid)*phdr->len_trap_oid);

    size_t len_hdr = SNMP_TRAP_HDR_LEN + sizeof(oid) * phdr->len_trap_oid;
    size_t len_data = 0;

    int i = 0;

    for (i = 0; i < phdr->cnt_data; i++)
    {
        pdata = (struct snmp_trap_data *)(pmsg->msg_data + len_hdr + len_data);

        pdata->data_oid = (oid *)(pmsg->msg_data + len_hdr + len_data + SNMP_TRAP_DATA_LEN);
        pdata->data = (pmsg->msg_data + len_hdr + len_data + SNMP_TRAP_DATA_LEN + sizeof(oid) * pdata->len_data_oid);

        snmp_varlist_add_variable(&var_list,
                                  pdata->data_oid,
                                  pdata->len_data_oid,
                                  pdata->type,
                                  /* Set an appropriate value for the leaf */
                                  pdata->data,
                                  pdata->len_data);


        len_data += SNMP_TRAP_DATA_LEN + sizeof(oid) * pdata->len_data_oid + pdata->len_data;

        if ((len_hdr + len_data) > phdr->len_msg)
        {
            zlog_err("%s[%d] : total length of trap msg error, length(%d)\n", __func__, __LINE__, phdr->len_msg);
            snmp_free_varbind(var_list);
            return (0);
        }
    }

    zlog_debug(SNMP_DBG_TRAP, "%s[%d] : recv trap msg success and start to send trap\n", __func__, __LINE__);
    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}


static int pkt_dhcp_trap(struct TRAP_INFO *pinfo)
{
    unsigned char str_value[64];
    uint32_t ip = htonl(pinfo->ip);
    uint32_t mask = htonl(pinfo->mask);
    uint32_t gateway = htonl(pinfo->gateway);
    memset(str_value, 0, 64 * sizeof(unsigned char));
    str_value[0] = pinfo->series_id;
    str_value[1] = pinfo->model_id;
    memcpy(&str_value[2], pinfo->mac, 6 * sizeof(uint8_t));
    str_value[8] = pinfo->reserved;

    str_value[9]  = ((ip >> 24) & 0xff);
    str_value[10] = ((ip >> 16) & 0xff);
    str_value[11] = ((ip >> 8) & 0xff);
    str_value[12] = (ip & 0xff);

    str_value[13] = ((mask >> 24) & 0xff);
    str_value[14] = ((mask >> 16) & 0xff);
    str_value[15] = ((mask >> 8) & 0xff);
    str_value[16] = (mask & 0xff);

    str_value[17] = ((gateway >> 24) & 0xff);
    str_value[18] = ((gateway >> 16) & 0xff);
    str_value[19] = ((gateway >> 8) & 0xff);
    str_value[20] = (gateway & 0xff);


    netsnmp_variable_list *var_list = NULL;

    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)DHCP_TRAP_EVENT_OID,
                              sizeof(oid)*OID_LENGTH(DHCP_TRAP_EVENT_OID));
    snmp_varlist_add_variable(&var_list,
                              DHCP_TRAP_EVENT_OID,
                              OID_LENGTH(DHCP_TRAP_EVENT_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)str_value,
                              64 * sizeof(unsigned char));

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}

//dhcp
static int
dhcpTrapAndSend(struct ipc_mesg_n *pmsg)
{
    struct DHCP_TRAP *pinfo_dhcp;
    TRAP_ITEM   trap_item;

    zlog_debug(SNMP_DBG_TRAP, "%s[%d] : in function '%s' -> receive DHCP trap info successed\n", \
               __FILE__, __LINE__, __func__);

    /* init trap_item */
    memset(trap_item.ip_addr.addr, 0, 4 * sizeof(uint32_t));
    memset(trap_item.name, '\0', TRAP_NAME_MAX_LEN);

    pinfo_dhcp = (struct DHCP_TRAP *)pmsg->msg_data;

    if (SNMP_ERROR == snmp_check_input_ip_valid(ntohl(pinfo_dhcp->nm_info.nm_ip)))
    {
        zlog_err("%s[%d] : in function '%s' -> Illegal ip from dhcp\n", \
                 __FILE__, __LINE__, __func__);
        return SNMP_ERR_BADVALUE;
    }
    else
    {
        trap_item.ip_addr.addr[0] = ntohl(pinfo_dhcp->nm_info.nm_ip);
    }

    trap_item.port = ntohs(pinfo_dhcp->nm_info.nm_port);

    if ((trap_item.port < 1) || (trap_item.port > 65535))
    {
        zlog_err("%s[%d] : in function '%s' -> Illegal port from dhcp\n", \
                 __FILE__, __LINE__, __func__);
        return SNMP_ERR_BADVALUE;
    }

    trap_item.ip_addr.type = IPv4;

    if (V1 == snmp_version)
    {
        trap_item.version = V1;
    }
    else
    {
        trap_item.version = SNMP_VERSION_2c;
    }

    trap_item.vpn = 0;
    strcpy(trap_item.name, "trap");

    if (NULL == snmp_config_item_get(SNMP_TRAP_ITEM, &trap_item))
    {
        snmp_config_item_add(SNMP_TRAP_ITEM, &trap_item);
        snmp_trap_item_config_new(&trap_item);

        zlog_debug(SNMP_DBG_TRAP, "%s[%d] : in function '%s' -> Config dhcp server successfully\n", \
                   __FILE__, __LINE__, __func__);
    }

    pkt_dhcp_trap(&pinfo_dhcp->trap_info);

    return SNMP_ERR_NOERROR;
}

static int pkt_msdh_trap(struct ipc_mesg_n *pmesg)
{
    unsigned char str_value[4094] = {'\0'};
    netsnmp_variable_list *var_list = NULL;

    memcpy(str_value, (unsigned char *)pmesg->msg_data, pmesg->msghdr.data_len);
	
    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)MSDH_CMD_TRAP_EVENT_OID,
                              sizeof(oid)*OID_LENGTH(MSDH_CMD_TRAP_EVENT_OID));

    snmp_varlist_add_variable(&var_list,
                              MSDH_CMD_TRAP_EVENT_MSG_OID,
                              OID_LENGTH(MSDH_CMD_TRAP_EVENT_MSG_OID),
                              ASN_OCTET_STR,
                              (unsigned char *)str_value,
                              (pmesg->msghdr.data_len)*sizeof(unsigned char));

    send_v2trap(var_list);
    snmp_free_varbind(var_list);

    return 0;
}

//device alarm
static int
convert2TrapAndSend(struct ipc_mesg_n *pmsg)
{
    switch (pmsg->msghdr.msg_subtype)
    {
        case ALM_PRODUCT_TRAP :
            zlog_debug(SNMP_DBG_TRAP, "%s[%d] : receive alarm product trap\n", __func__, __LINE__);
            pkt_alarm_product_trap((stSnmpAlmCurrDSTableInfo *)pmsg->msg_data);
            break;

        case ALM_DISAPPEAR_TRAP :
            zlog_debug(SNMP_DBG_TRAP, "%s[%d] : receive alarm disappear trap\n", __func__, __LINE__);
            pkt_alarm_disappear_trap((stSnmpAlmHistDSTableInfo *)pmsg->msg_data);
            break;

        case ALM_EVENT_REPORT_TRAP :
            zlog_debug(SNMP_DBG_TRAP, "%s[%d] : receive alarm event trap\n", __func__, __LINE__);
            pkt_alarm_event_report_trap((stSnmpEventDSTableInfo *)pmsg->msg_data);
            break;

        case ALM_REVERSE_AUTO_REC_TRAP :
            zlog_debug(SNMP_DBG_TRAP, "%s[%d] : receive alarm reverse trap\n", __func__, __LINE__);
            pkt_alarm_reverse_auto_rec_trap((stSnmpAlmAttribCfgTableInfo *)pmsg->msg_data);
            break;

        default:
            return (SNMP_ERR_GENERR);
    }

    return SNMP_ERR_NOERROR;
}

//ospfdcn trap
static int
OspfDcnTrapAndSend(struct ipc_mesg_n *pmsg)
{
    uint8_t opcode;
    struct ospf_dcn_ne_info *pdata;

    pdata = (struct ospf_dcn_ne_info *)pmsg->msg_data; //point to first ospf ne info
    opcode = pmsg->msghdr.msg_subtype;

    netsnmp_variable_list *var_list = NULL;

    switch (opcode)
    {
        case OSPF_DCN_NE_ONLINE:
            snmp_varlist_add_variable(&var_list,
                                      SNMP_TRAP_OID,
                                      OID_LENGTH(SNMP_TRAP_OID),
                                      ASN_OBJECT_ID,
                                      (unsigned char *)OSPFDCN_TRAP_NE_ONLINE_OID,
                                      sizeof(oid)*OID_LENGTH(OSPFDCN_TRAP_NE_ONLINE_OID));
            break;

        case OSPF_DCN_NE_OFFLINE:
            snmp_varlist_add_variable(&var_list,
                                      SNMP_TRAP_OID,
                                      OID_LENGTH(SNMP_TRAP_OID),
                                      ASN_OBJECT_ID,
                                      (unsigned char *)OSPFDCN_TRAP_NE_OFFLINE_OID,
                                      sizeof(oid)*OID_LENGTH(OSPFDCN_TRAP_NE_OFFLINE_OID));
            break;

        default:
            break;
    }

    snmp_varlist_add_variable(&var_list,
                              OSPFDCN_TRAP_NE_ID_OID,
                              OID_LENGTH(OSPFDCN_TRAP_NE_ID_OID),
                              ASN_IPADDRESS ,
                              (unsigned char *)&pdata->dcn_ne_id,
                              sizeof(pdata->dcn_ne_id));

    snmp_varlist_add_variable(&var_list,
                              OSPFDCN_TRAP_NE_IP_OID,
                              OID_LENGTH(OSPFDCN_TRAP_NE_IP_OID),
                              ASN_IPADDRESS ,
                              (unsigned char *)&pdata->dcn_ne_ip,
                              sizeof(pdata->dcn_ne_ip));

    snmp_varlist_add_variable(&var_list,
                              OSPFDCN_TRAP_NE_METRIC_OID,
                              OID_LENGTH(OSPFDCN_TRAP_NE_METRIC_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&pdata->dcn_ne_metric,
                              sizeof(pdata->dcn_ne_metric));
    snmp_varlist_add_variable(&var_list,
                              OSPFDCN_TRAP_NE_DEVICE_TYPE_OID,
                              OID_LENGTH(OSPFDCN_TRAP_NE_DEVICE_TYPE_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->dcn_ne_device_type,
                              sizeof(pdata->dcn_ne_device_type));
    snmp_varlist_add_variable(&var_list,
                              OSPFDCN_TRAP_NE_MAC_OID,
                              OID_LENGTH(OSPFDCN_TRAP_NE_MAC_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->mac,
                              sizeof(pdata->mac));
    snmp_varlist_add_variable(&var_list,
                              OSPFDCN_TRAP_NE_VENDOR_OID,
                              OID_LENGTH(OSPFDCN_TRAP_NE_VENDOR_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->dcn_ne_vendor,
                              strlen(pdata->dcn_ne_vendor));

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}

//lldp trap
static int
pkt_lldp_trap(struct lldp_msap_trap *pdata, int opcode)
{
    netsnmp_variable_list *var_list = NULL;

    switch (opcode)
    {
        case IPC_TYPE_ALARM_LLDP_INSERT_TRAP:
            snmp_varlist_add_variable(&var_list,
                                      SNMP_TRAP_OID,
                                      OID_LENGTH(SNMP_TRAP_OID),
                                      ASN_OBJECT_ID,
                                      (unsigned char *)LLDP_REMOTE_INSERT_TRAP_OID,
                                      sizeof(oid)*OID_LENGTH(LLDP_REMOTE_INSERT_TRAP_OID));
            break;

        case IPC_TYPE_ALARM_LLDP_DELETE_TRAP:

            snmp_varlist_add_variable(&var_list,
                                      SNMP_TRAP_OID,
                                      OID_LENGTH(SNMP_TRAP_OID),
                                      ASN_OBJECT_ID,
                                      (unsigned char *)LLDP_REMOTE_DELETE_TRAP_OID,
                                      sizeof(oid)*OID_LENGTH(LLDP_REMOTE_DELETE_TRAP_OID));
            break;

        case IPC_TYPE_ALARM_LLDP_AGEOUT_TRAP:

            snmp_varlist_add_variable(&var_list,
                                      SNMP_TRAP_OID,
                                      OID_LENGTH(SNMP_TRAP_OID),
                                      ASN_OBJECT_ID,
                                      (unsigned char *)LLDP_REMOTE_AGEOUT_TRAP_OID,
                                      sizeof(oid)*OID_LENGTH(LLDP_REMOTE_AGEOUT_TRAP_OID));
            break;

        default:
            break;
    }

    snmp_varlist_add_variable(&var_list,
                              LLDP_LOCAL_PORT_DESC_OID,
                              OID_LENGTH(LLDP_LOCAL_PORT_DESC_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->if_name,
                              strlen(pdata->if_name));

    snmp_varlist_add_variable(&var_list,
                              LLDP_REMOTE_CHASSIS_ID_SUBTYPE_OID,
                              OID_LENGTH(LLDP_REMOTE_CHASSIS_ID_SUBTYPE_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&pdata->chassis_id_subtype,
                              sizeof(pdata->chassis_id_subtype));

    if (4 == pdata->chassis_id_subtype)
        snmp_varlist_add_variable(&var_list,
                                  LLDP_REMOTE_CHASSIS_ID_OID,
                                  OID_LENGTH(LLDP_REMOTE_CHASSIS_ID_OID),
                                  ASN_OCTET_STR ,
                                  (unsigned char *)pdata->chassis_id_mac_addr,
                                  sizeof(pdata->chassis_id_mac_addr));
    else if (5 == pdata->chassis_id_subtype)
        snmp_varlist_add_variable(&var_list,
                                  LLDP_REMOTE_CHASSIS_ID_OID,
                                  OID_LENGTH(LLDP_REMOTE_CHASSIS_ID_OID),
                                  ASN_OCTET_STR ,
                                  (unsigned char *)pdata->chassis_id_net_addr,
                                  sizeof(pdata->chassis_id_net_addr));

    snmp_varlist_add_variable(&var_list,
                              LLDP_REMOTE_PORT_ID_SUBTYPE_OID,
                              OID_LENGTH(LLDP_REMOTE_PORT_ID_SUBTYPE_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&pdata->port_id_subtype,
                              sizeof(pdata->port_id_subtype));

    if (5 == pdata->port_id_subtype)
        snmp_varlist_add_variable(&var_list,
                                  LLDP_REMOTE_PORT_ID_OID,
                                  OID_LENGTH(LLDP_REMOTE_PORT_ID_OID),
                                  ASN_OCTET_STR ,
                                  (unsigned char *)pdata->port_if_name,
                                  strlen(pdata->port_if_name));
    else if (4 == pdata->port_id_subtype)
        snmp_varlist_add_variable(&var_list,
                                  LLDP_REMOTE_PORT_ID_OID,
                                  OID_LENGTH(LLDP_REMOTE_PORT_ID_OID),
                                  ASN_OCTET_STR ,
                                  (unsigned char *)pdata->network_addr,
                                  sizeof(pdata->network_addr));

    snmp_varlist_add_variable(&var_list,
                              LLDP_REMOTE_PORT_DESC_OID,
                              OID_LENGTH(LLDP_REMOTE_PORT_DESC_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->pt_descr,
                              strlen(pdata->pt_descr));

    snmp_varlist_add_variable(&var_list,
                              LLDP_REMOTE_SYS_NAME_OID,
                              OID_LENGTH(LLDP_REMOTE_SYS_NAME_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->sys_name,
                              strlen(pdata->sys_name));

    snmp_varlist_add_variable(&var_list,
                              LLDP_REMOTE_SYS_DESC_OID,
                              OID_LENGTH(LLDP_REMOTE_SYS_DESC_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)pdata->sys_descr,
                              strlen(pdata->sys_descr));

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}


static int
lldpTrapAndSend(struct ipc_mesg_n *pmsg)
{
    int ret = 0;

    struct lldp_msap_trap *pdata;
    pdata = (struct lldp_msap_trap *)pmsg->msg_data;   //point to first lldp struct

    int num_lldp;                   //num of lldp port received
    num_lldp = pdata->msap_num;     //get lldp info num

    zlog_debug(SNMP_DBG_TRAP, "%s[%d] : in function '%s' -> receive LLDP trap num (%d)\n", \
               __FILE__, __LINE__, __func__, num_lldp);

    for (ret = 0; ret < num_lldp; ret++)
    {
        pkt_lldp_trap(pdata, pmsg->msghdr.msg_subtype);
        pdata++;
    }
}
//efm agent trap
static int
pkt_efm_agent_trap(uint8_t trap_data, uint8_t trap_node,uint32_t ifindex)
{
    netsnmp_variable_list *var_list = NULL;
    snmp_varlist_add_variable(&var_list,
                                      SNMP_TRAP_OID,
                                      OID_LENGTH(SNMP_TRAP_OID),
                                      ASN_OBJECT_ID,
                                      (unsigned char *)EFM_AGENT_TRAP_OID,
                                      sizeof(oid)*OID_LENGTH(EFM_AGENT_TRAP_OID));
	if(trap_node != hpmcoltalm &&  trap_node != hpmcethalm)
	{
		zlog_err("%s[%d]#################efm trap node %d is err!must be hpmcoltalm or hpmcethalm",\
			__FUNCTION__,__LINE__,trap_node);
		return -1;
	}
	EFM_AGENT_REMOTE_OLT_ETH_OID[trap_node][EFM_AGENT_ALM_MIBNODE_LEN-2] = ifindex;
		
    snmp_varlist_add_variable(&var_list,
                               EFM_AGENT_REMOTE_OLT_ETH_OID[trap_node],
                              OID_LENGTH( EFM_AGENT_REMOTE_OLT_ETH_OID[trap_node]),
                              ASN_OCTET_STR ,
                              (unsigned char *)&trap_data,
                              sizeof(trap_data));
    send_v2trap(var_list);
    snmp_free_varbind(var_list);
	
    return (0);
}
static int
efmagentTrapAndSend(struct ipc_mesg_n *pmesg)
{
    int ret = 0;
    uint8_t trap_data = 0;
    uint8_t trap_node = 0;
    uint8_t i = 0;

    trap_node =pmesg->msg_data[i++];   //point to first lldp struct
    trap_data = pmesg->msg_data[i++]; 

   if(i > pmesg->msghdr.data_num)
   {
	zlog_err("%s[%d]snmp trap get data num> efm agent give data num",__FUNCTION__,__LINE__);
   }

	ret = pkt_efm_agent_trap(trap_data, trap_node,pmesg->msghdr.msg_index);
	if(ret < 0)
	{
		zlog_err("%s[%d]#########efm agent trap fail",__FUNCTION__,__LINE__);
		return ret;
	}
	return 0;
}

void trap_cache_node_free(struct ipc_mesg_n *node)
{
    XFREE(0, node);
}

int trap_cache_node_add(struct ipc_mesg_n *pmsg)
{
    struct ipc_mesg_n *pmsg_add = NULL;

	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	
    pmsg_add = XCALLOC(0, (sizeof(struct ipc_msghdr_n) + phdr->data_len));
	
    memcpy(pmsg_add, pmsg, (sizeof(struct ipc_msghdr_n) + phdr->data_len));
    listnode_add(trap_cache_list, pmsg_add);
	
    return 0;
}


/* statis trap send, add by lipf, 2018/3/24 */
static int statis_trap_send(struct ipc_mesg_n *pmsg)
{
    netsnmp_variable_list *var_list = NULL;
    int len = 0;

    oid         histDataTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 15, 22, 1, 0, 0, 0};

    stStatTrapDataTable *pRow;
    pRow = (stStatTrapDataTable *)pmsg->msg_data;

    snmp_varlist_add_variable(&var_list,
                              SNMP_TRAP_OID,
                              OID_LENGTH(SNMP_TRAP_OID),
                              ASN_OBJECT_ID,
                              (unsigned char *)STATIS_TRAP_PRODUCT_OID,
                              sizeof(oid)*OID_LENGTH(STATIS_TRAP_PRODUCT_OID));

    const struct tFaTrapInfo trapInfo[]     =
    {
        { 1     , ASN_INTEGER   , (unsigned char *)(&(pRow->trapIndex)),            sizeof(pRow->trapIndex) },
        { 2     , ASN_INTEGER   , (unsigned char *)(&(pRow->subType)),              sizeof(pRow->subType) },
        { 3     , ASN_INTEGER   , (unsigned char *)(&(pRow->stopTime)),             sizeof(pRow->stopTime) },
        { 4     , ASN_INTEGER   , (unsigned char *)(&(pRow->scanType)),             sizeof(pRow->scanType) },
        { 5     , ASN_INTEGER   , (unsigned char *)(&(pRow->cycClass)),             sizeof(pRow->cycClass) },
        { 6     , ASN_INTEGER   , (unsigned char *)(&(pRow->viewPort.portIndex)),   sizeof(pRow->viewPort.portIndex) },
        { 7     , ASN_INTEGER   , (unsigned char *)(&(pRow->viewPort.portIndex3)),  sizeof(pRow->viewPort.portIndex3) },
        { 8     , ASN_INTEGER   , (unsigned char *)(&(pRow->viewPort.portIndex4)),  sizeof(pRow->viewPort.portIndex4) },
        { 9     , ASN_INTEGER   , (unsigned char *)(&(pRow->viewPort.portIndex5)),  sizeof(pRow->viewPort.portIndex5) },
        { 10    , ASN_INTEGER   , (unsigned char *)(&(pRow->viewPort.portIndex6)),  sizeof(pRow->viewPort.portIndex6) },
        { 11    , ASN_INTEGER   , (unsigned char *)(&(pRow->viewPort.devIndex)),    sizeof(pRow->viewPort.devIndex) },
        { 12    , ASN_INTEGER   , (unsigned char *)(&(pRow->dataHigh32)),           sizeof(pRow->dataHigh32) },
        { 13    , ASN_INTEGER   , (unsigned char *)(&(pRow->dataLow32)),            sizeof(pRow->dataLow32) },
    };

    const int histNodeNum = sizeof(trapInfo) / sizeof(trapInfo[0]);

    int i = 0;

    for (i = 0; i < histNodeNum; i++)
    {
        // index
        len                             = sizeof(histDataTrapRow_oid) / sizeof(histDataTrapRow_oid[0]);
        histDataTrapRow_oid[len - 3]    = trapInfo[i].iOid;
        histDataTrapRow_oid[len - 2]    = pRow->trapIndex;
        histDataTrapRow_oid[len - 1]    = pRow->subType;

        snmp_varlist_add_variable(&var_list,
                                  histDataTrapRow_oid, OID_LENGTH(histDataTrapRow_oid),
                                  trapInfo[i].type,
                                  /* Set an appropriate value for the leaf */
                                  trapInfo[i].buf,
                                  trapInfo[i].len);
    }

    send_v2trap(var_list);
    snmp_free_varbind(var_list);
    return (0);
}



static int rmon_trap_send(struct ipc_mesg_n *pmsg)
{
    netsnmp_variable_list *var_list = NULL;
    oid *tmp_oid ;
    int oid_length = 0 ;

    struct rmon_AlarmGroup *alarm;
    alarm = (struct rmon_AlarmGroup *)pmsg->msg_data;

    if (RMON_RISE_ALARM == alarm->raise_or_fall)
    {
        snmp_varlist_add_variable(&var_list,   SNMP_TRAP_OID,
                                  OID_LENGTH(SNMP_TRAP_OID),
                                  ASN_OBJECT_ID,
                                  (unsigned char *)RMON_TRAP_RAISE_OID,
                                  sizeof(oid)*OID_LENGTH(RMON_TRAP_RAISE_OID));
    }
    else
    {
        snmp_varlist_add_variable(&var_list,   SNMP_TRAP_OID,
                                  OID_LENGTH(SNMP_TRAP_OID),
                                  ASN_OBJECT_ID,
                                  (unsigned char *)RMON_TRAP_FALL_OID,
                                  sizeof(oid)*OID_LENGTH(RMON_TRAP_FALL_OID));
    }

    oid_length = OID_LENGTH(RMON_TRAP_ALARM_ID_OID);
    RMON_TRAP_ALARM_ID_OID[oid_length - 1] = alarm->alarmIndex;
    snmp_varlist_add_variable(&var_list,       RMON_TRAP_ALARM_ID_OID ,
                              OID_LENGTH(RMON_TRAP_ALARM_ID_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&alarm->alarmIndex,
                              sizeof(int));

    oid_length = OID_LENGTH(RMON_TRAP_ALARM_VAR_OID);
    RMON_TRAP_ALARM_VAR_OID[oid_length - 1] = alarm->alarmIndex;
    snmp_varlist_add_variable(&var_list,       RMON_TRAP_ALARM_VAR_OID ,
                              OID_LENGTH(RMON_TRAP_ALARM_VAR_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&alarm->alarmVariable,
                              sizeof(oid) * (RMON_ALARM_VARIABLE_MAXSIZE - 1));


    oid_length = OID_LENGTH(RMON_TRAP_ALARM_SAMPLE_TYPE_OID) ;
    RMON_TRAP_ALARM_SAMPLE_TYPE_OID[oid_length - 1] = alarm->alarmIndex;
    snmp_varlist_add_variable(&var_list,       RMON_TRAP_ALARM_SAMPLE_TYPE_OID ,
                              OID_LENGTH(RMON_TRAP_ALARM_SAMPLE_TYPE_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&alarm->alarmSampleType,
                              sizeof(int));

    oid_length = OID_LENGTH(RMON_TRAP_ALARM_VALUE_OID) ;
    RMON_TRAP_ALARM_VALUE_OID[oid_length - 1] = alarm->alarmIndex;
    uint64_value.high = (alarm->alarmValue.l[0]);
    uint64_value.low  = (alarm->alarmValue.l[1]);

    snmp_varlist_add_variable(&var_list,       RMON_TRAP_ALARM_VALUE_OID ,
                              OID_LENGTH(RMON_TRAP_ALARM_VALUE_OID),
                              ASN_INTEGER ,
                              (unsigned char *)(&uint64_value.low),
                              sizeof(int));

    if (RMON_RISE_ALARM == alarm->raise_or_fall)
    {
        oid_length = OID_LENGTH(RMON_TRAP_ALARM_RIASE_OID) ;
        RMON_TRAP_ALARM_RIASE_OID[oid_length - 1] = alarm->alarmIndex;
        uint64_value.high = (alarm->alarmRisingThreshold.l[0]);
        uint64_value.low  = (alarm->alarmRisingThreshold.l[1]);

        snmp_varlist_add_variable(&var_list,   RMON_TRAP_ALARM_RIASE_OID ,
                                  OID_LENGTH(RMON_TRAP_ALARM_RIASE_OID),
                                  ASN_INTEGER ,
                                  (unsigned char *)(&uint64_value.low),
                                  sizeof(int));
    }
    else
    {
        oid_length = OID_LENGTH(RMON_TRAP_ALARM_FALL_OID) ;
        RMON_TRAP_ALARM_FALL_OID[oid_length - 1] = alarm->alarmIndex;
        uint64_value.high = (alarm->alarmFallingThreshold.l[0]);
        uint64_value.low  = (alarm->alarmFallingThreshold.l[1]);
        snmp_varlist_add_variable(&var_list,   RMON_TRAP_ALARM_FALL_OID ,
                                  OID_LENGTH(RMON_TRAP_ALARM_FALL_OID),
                                  ASN_INTEGER ,
                                  (unsigned char *)(&uint64_value.low),
                                  sizeof(int));
    }

    send_v2trap(var_list);
    snmp_free_varbind(var_list);

    return (0);
}

int dev_trap_send(struct ipc_mesg_n *pmsg)
{
    struct ipc_msghdr_n *phdr = &pmsg->msghdr;
    struct ifm_event pevent;

    if (IPC_TYPE_SNMP_TRAP == phdr->msg_type)
    {
        snmp_send_trap(pmsg);
        return 0;
    }

    /* dhcp trap msg*/
    if ((IPC_TYPE_SNMP == phdr->msg_type) && (MODULE_ID_DHCP == phdr->sender_id))
    {
        dhcpTrapAndSend(pmsg);
    }

	/* lldp or efm  trap msg */
	if ((IPC_TYPE_SNMP == phdr->msg_type) && (MODULE_ID_L2 == phdr->sender_id))
	{

		if(IPC_TYPE_ALARM_EFMAGENT_TRAP == phdr->msg_subtype)
		{
			zlog_debug(SNMP_DBG_TRAP, "%s[%d]################################\n",__FUNCTION__,__LINE__);
			efmagentTrapAndSend(pmsg);
		}
		else
		{
			zlog_debug(SNMP_DBG_TRAP, "%s[%d] : in function '%s' -> receive LLDP trap info\n", \
					__FILE__, __LINE__, __func__);
			lldpTrapAndSend(pmsg);

		}
	}

    /* rmon trap msg */
    if ((IPC_TYPE_RMOND == phdr->msg_type) && (MODULE_ID_RMON == phdr->sender_id))
    {
        rmon_trap_send(pmsg);
    }

    /* device alarm msg */
    if ((IPC_TYPE_SNMP == phdr->msg_type) && (MODULE_ID_ALARM == phdr->sender_id))
    {
        convert2TrapAndSend(pmsg);
    }

    /* statis trap msg */
    if ((IPC_TYPE_SNMP == phdr->msg_type) && (MODULE_ID_STAT == phdr->sender_id))
    {
        statis_trap_send(pmsg);
    }

    /* ospfdcn trap msg*/
    if ((IPC_TYPE_SNMP == phdr->msg_type) && (MODULE_ID_OSPF == phdr->sender_id))
    {
        OspfDcnTrapAndSend(pmsg);
    }

    if ((IPC_TYPE_IFM == phdr->msg_type) && (MODULE_ID_IFM == phdr->sender_id))
    {
        if (IPC_OPCODE_EVENT == phdr->opcode)
        {
            memcpy(&pevent, pmsg->msg_data, sizeof(struct ifm_event));

            if (IFNET_EVENT_UP == pevent.event || IFNET_EVENT_DOWN == pevent.event)
            {
                if (snmp_trap_link_enable_get() == 1)
                {
                    linkupTrapAndSend(pmsg);
                }
            }
        }
    }

	if ((IPC_TYPE_SDHMGT == phdr->msg_type) && (MODULE_ID_SDHMGT == phdr->sender_id))
	{
		pkt_msdh_trap(pmsg);
	}

    return 0;
}

int trap_cache_node_send(void)
{
    struct listnode  *node;
    struct ipc_mesg_n *data_find;

    if (NULL != trap_cache_list)
    {
        for (ALL_LIST_ELEMENTS_RO(trap_cache_list, node, data_find))
        {
			if(data_find)
			{
            	dev_trap_send(data_find);
			}
        }

        list_delete_all_node(trap_cache_list);
    }

    return 0;
}

static int poll_dev_trap(struct ipc_mesg_n *pmsg)
{
    int trap_enable;

    if (trap_cache_enable == ENABLE)
    {
        trap_cache_num++;
        trap_cache_node_add(pmsg);
    }
    else
    {
        trap_enable = snmp_trap_enable_get();

        if (0 == trap_enable)
        {
            return 0;
        }

        dev_trap_send(pmsg);
    }

    return 0;
}


/* receive IPC messages */
int snmp_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int retva = 0;
    int revln = 0;

	struct ipc_msghdr_n *phdr = &pmsg->msghdr;

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 
	
	if(revln > imlen)
	{
		printf("snmpd recv trap msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
		return 0;
	}

	/* 由于之前该进程只接收trap消息，ospf为普通消息，需要进行区分。
	   由于之前其他模块发送trap消息时，各字段填写混乱，此处无法进行统一区分，
	   只能先将OSPF的消息挑出来，其他消息按trap处理 */
	   
	if((MODULE_ID_OSPF == phdr->sender_id) && 
	   (IPC_TYPE_EFM == phdr->msg_type))
	{
		snmp_handle_ospf_msg(pmsg);
	}
	if((MODULE_ID_IFM == phdr->sender_id) && 
	   (IPC_TYPE_IFM == phdr->msg_type) &&
	   (IPC_OPCODE_EVENT == phdr->opcode) &&
	   ((IFNET_EVENT_IP_ADD == phdr->msg_subtype) || (IFNET_EVENT_IP_DELETE == phdr->msg_subtype)))
	{
		snmp_handle_route_msg(pmsg);
	}
	else
	{
		poll_dev_trap(pmsg);
	}

	if(pmsg)
	{
		mem_share_free(pmsg, MODULE_ID_SNMPD_TRAP);
	}

	//thread_add_timer_msec(snmp_cli_master, snmp_ipc_msg_recv, NULL, 100);
	return 0;
}


static int trap_cold_start(void)
{
	oid oid_cold_start[] = { SNMP_OID_SNMPMODULES , 1, 1, 5, 1};
    int oid_cold_start_len  = OID_LENGTH(oid_cold_start);

    netsnmp_send_traps(SNMP_TRAP_COLDSTART, 0,
       oid_cold_start, oid_cold_start_len, NULL, NULL, 0);

    return SNMP_OK;
}


#ifdef POWERDOWN_ALARM
void *poweroff_thread(void *arg);
#endif

void set_timer(void)
{
    int ret = 0;
    struct itimerval itv;
    itv.it_value.tv_sec = 300;/*To achieve a timing*//*5 minutes*/
    itv.it_value.tv_usec = 0;
    itv.it_interval.tv_sec = 0; /*After the first timed timeout, the system re-initializes it_value to it_interval*/
    itv.it_interval.tv_usec = 0;
    ret = setitimer(ITIMER_REAL, &itv, NULL);

    if (ret < 0)
    {
        zlog_err("%s[%d]:'%s':trap cache set timer fail!\n", __FILE__, __LINE__, __func__);
    }

    zlog_debug(SNMP_DBG_TRAP, "%s[%d]:'%s':trap cache set timer success!\n", __FILE__, __LINE__, __func__);

}

void signal_handler_trap_cache(int m)
{
#if 0
	switch(m)
	{
		case SIGALRM:
			printf("%s[%d] : --------------------------catch SIGALRM\n", __func__, __LINE__);
			break;
			
		default:
			break;
	}	

	printf("%s[%d] : trap_cache_enable = (%d : %d)\n", __func__, __LINE__, trap_cache_enable, ENABLE);
#endif
    if (trap_cache_enable == ENABLE)
    {
        trap_cache_enable = DISABLE;
        zlog_notice("%s[%d]:'%s':time out,trap cache send start,num=%d.\n", __FILE__, __LINE__, __func__, trap_cache_num);

		struct timeval tm;
	    gettimeofday(&tm, NULL);
		
		stSnmpAlmHistDSTableInfo infoHist;
	    memset(&infoHist, 0, sizeof(stSnmpAlmHistDSTableInfo));
	    infoHist.data.iHistAlmLevel = (int)GPN_ALM_RANK_EMERG;
	    infoHist.data.iHistAlmType =  GPN_ALM_TYPE_DEV_POW_FAIL;

	    infoHist.data.iAlmHistThisDisappearTime = tm.tv_sec;
	    pkt_alarm_disappear_trap(&infoHist);

		trap_cold_start();
		
        trap_cache_node_send();	

	    zlog_debug(SNMP_DBG_TRAP, "send trap sec:%d usec:%d\n", tm.tv_sec, tm.tv_usec);
		
        zlog_notice("%s[%d]:'%s':time out,trap cache send end.\n", __FILE__, __LINE__, __func__);
    }
}

static void
dev_trap_thread_init(void)
{
#ifdef POWERDOWN_ALARM
    /*powerdown alarm thread, add by zhangjiajie 2017-6-5*/
    pthread_t poid;
    pthread_create(&poid, NULL, (void *)poweroff_thread, NULL);
    prctl(PR_SET_NAME, "snmp_trap");
#endif

    trap_cache_list = list_new();
    trap_cache_list->del = (void (*)(void *))trap_cache_node_free;
    //signal(SIGALRM, signal_handler_trap_cache);
    //set_timer();

    //thread_add_event(snmp_cli_master, poll_dev_trap, NULL, 0);
}

static void
cli_ipc_pkg_rcv(void)
{
    struct sched_param param;

    cpu_set_t mask;

    CPU_ZERO(&mask);
    //CPU_SET ( 0, &mask ); /* 绑定cpu0*/
    CPU_SET(1, &mask);    /* 绑定cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);

    param.sched_priority = 60;
#ifndef HAVE_KERNEL_3_0

    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }

#endif

    while (1)
    {
        if (thread_fetch(snmp_cli_master, &pthread))
        {
            thread_call(&pthread);
        }
    }
}

static void
exec_cli_rcv_thread(void)
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, (void *)cli_ipc_pkg_rcv, NULL) ;
    prctl(PR_SET_NAME, "snmp_cli");
    pthread_detach(thread_id);
}

/*----------------lidingcheng added 20180416 for link up/down---------------------------*/
void init_linkupdown_register()
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_SNMPD_TRAP, IFNET_IFTYPE_PORT);
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_SNMPD_TRAP, IFNET_IFTYPE_PORT);
}

int  linkupTrapAndSend(struct ipc_mesg_n *pmsg)
{
    oid objid_enterprisetrap_link[] = { SNMP_OID_SNMPMODULES , 1, 1, 4, 3, 0 };
    int enterprisetrap_link_len  = OID_LENGTH(objid_enterprisetrap_link);

    netsnmp_variable_list *var_list = NULL;
    oid *tmp_oid ;
    int oid_length = 0 ;

    void *pdata = NULL;
    struct ifm_event pevent;
    struct ifm_info  ifInfo = {0} ;
    char ifName[STRING_LEN] = {'\0'};

    uint32_t ifindex = 0 ;
    int admin_status = 0;
    int oper_status = 0;
    pdata = pmsg->msg_data;

    //pevent = *((struct ifm_event *)pdata);
    memcpy(&pevent, pdata, sizeof(struct ifm_event));

    ifindex = pevent.ifindex ;

    if(ifm_get_all_info(ifindex , MODULE_ID_SNMPD_TRAP, &ifInfo) != 0) 
    {
        return -1 ;
    }

    memset(ifName, 0, STRING_LEN);
    ifm_get_name_by_ifindex(ifindex, ifName);

    admin_status = ifInfo.shutdown ;
    oper_status = ifInfo.status ;

    snmp_varlist_add_variable(&var_list,       IF_LINK_IFINDEX_OID ,
                              OID_LENGTH(IF_LINK_IFINDEX_OID),
                              ASN_UNSIGNED,
                              (unsigned char *)&ifindex,
                              sizeof(uint32_t));
    snmp_varlist_add_variable(&var_list,       IF_LINK_ADMIN_STATUS_OID ,
                              OID_LENGTH(IF_LINK_ADMIN_STATUS_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&admin_status,
                              sizeof(int));
    snmp_varlist_add_variable(&var_list,       IF_LINK_OPER_STATUS_OID ,
                              OID_LENGTH(IF_LINK_OPER_STATUS_OID),
                              ASN_INTEGER ,
                              (unsigned char *)&oper_status ,
                              sizeof(int));
    snmp_varlist_add_variable(&var_list,       IF_LINK_DESC_OID ,
                              OID_LENGTH(IF_LINK_DESC_OID),
                              ASN_OCTET_STR ,
                              (unsigned char *)ifName,
                              strlen(ifName));

    switch (pevent.event)
    {
        case IFNET_EVENT_UP :
            netsnmp_send_traps(SNMP_TRAP_LINKUP, 0,
                               objid_enterprisetrap_link, enterprisetrap_link_len,
                               var_list, NULL, 0);
            break ;

        case IFNET_EVENT_DOWN :
            netsnmp_send_traps(SNMP_TRAP_LINKDOWN, 0,
                               objid_enterprisetrap_link, enterprisetrap_link_len,
                               var_list, NULL, 0);
            break ;

        default :
            snmp_free_varbind(var_list);
            return -1;
    }

    snmp_free_varbind(var_list);

    return (0);
}


static void snmp_ipc_init (void)
{	
	//mem_share_attach();	/* éžç¬¬ä¸?ä¸ªå¯åŠ¨è¿›ç¨‹éœ€è°ƒç”¨attach */

	/* å®šæ—¶å™¨åˆå§‹åŒ–ï¼Œæ—¶é—´é—´éš”å•ä½ms */
    high_pre_timer_init(5000, MODULE_ID_SNMPD_TRAP);
	//high_pre_timer_init(5000, MODULE_ID_SNMPD_PACKET);

	/* æ¶ˆæ¯æŽ¥æ”¶åˆå§‹åŒ?*/
	if(ipc_recv_init(snmp_cli_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	/* å¯åŠ¨æŽ¥æ”¶çº¿ç¨‹ */
    if(ipc_recv_thread_start("SnmpTrapIpcRev", MODULE_ID_SNMPD_TRAP, SCHED_OTHER, -1, snmp_msg_rcv, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }

	/* å¯åŠ¨åŸºå‡†å®šæ—¶å™?*/
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }

	/* æ·»åŠ å®šæ—¶å™¨å›žè°ƒï¼Œms */
	//high_pre_timer_add("GpnAlm1sProcTimer",    LIB_TIMER_TYPE_LOOP, gpnAlm1SecondTimerPorc, NULL, 1000);
	high_pre_timer_add("Snmp1sProcTimer", LIB_TIMER_TYPE_NOLOOP, signal_handler_trap_cache, NULL, 5000*40);	//200s	
	high_pre_timer_add("SnmpMibTablesListCheckTimer", LIB_TIMER_TYPE_LOOP, ipran_mib_tables_list_check, NULL, 5000);//5s
	
	netsnmp_ipc_register(161);
}



/*----------------lidingcheng added end 20180416 ---------------------------*/

void send_warm_start_trap_4_vtysh()
{
    if (trap_cache_enable == ENABLE)
    {
        trap_cache_enable = DISABLE;

        zlog_debug(SNMP_DBG_TRAP, "%s[%d]:'%s', vty read the configuration, send trap cache num = %d.\n", __FILE__, __LINE__, __func__, trap_cache_num);

		struct timeval tm;
	    gettimeofday(&tm, NULL);
		
		stSnmpAlmHistDSTableInfo infoHist;
	    memset(&infoHist, 0, sizeof(stSnmpAlmHistDSTableInfo));
	    infoHist.data.iHistAlmLevel = (int)GPN_ALM_RANK_EMERG;
	    infoHist.data.iHistAlmType =  GPN_ALM_TYPE_DEV_POW_FAIL;

	    infoHist.data.iAlmHistThisProductTime = tm.tv_sec;
	    pkt_alarm_disappear_trap(&infoHist);

		trap_cold_start();
		
        trap_cache_node_send();
    }

    if (send_warm_start_enable == ENABLE)
    {
        send_warm_start_enable = DISABLE;
    }
}


/*******************************************************************-o-******
 * main - Non Windows
 * SnmpDaemonMain - Windows to support windows service
 *
 * Parameters:
 *   argc
 *  *argv[]
 *
 * Returns:
 *  0   Always succeeds.  (?)
 *
 *
 * Setup and start the agent daemon.
 *
 * Also successfully EXITs with zero for some options.
 */
int
#ifdef WIN32SERVICE
SnmpDaemonMain(int argc, TCHAR *argv[])
#else
main(int argc, char *argv[])
#endif
{
    char            options[128] = "aAc:CdD::fhHI:l:L:m:M:n:p:P:qrsS:UvV-:";
    int             arg, i, ret;
    int             dont_fork = 0, do_help = 0;
    int             log_set = 0;
    int             uid = 0, gid = 0;
    int             agent_mode = -1;
    char           *cptr, **argvptr;
    char           *pid_file = NULL;
    char            option_compatability[] = "-Le";
    //char          *progname;
#if HAVE_GETPID
    int fd;
    FILE           *PID;
#endif

    struct sched_param param;

    cpu_set_t mask;

    CPU_ZERO(&mask);
    //CPU_SET ( 0, &mask ); /* 绑定cpu0*/
    CPU_SET(1, &mask);    /* 绑定cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);

    param.sched_priority = 60;
#ifndef HAVE_KERNEL_3_0

    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }

#endif
#ifndef WIN32

    /*
     * close all non-standard file descriptors we may have
     * inherited from the shell.
     */
    for (i = getdtablesize() - 1; i > 2; --i)
    {
        (void) close(i);
    }

#endif /* #WIN32 */

    /*
     * register signals ASAP to prevent default action (usually core)
     * for signals during startup...
     */
#ifdef SIGTERM
    DEBUGMSGTL(("signal", "registering SIGTERM signal handler\n"));
    signal(SIGTERM, SnmpdShutDown);
#endif
#ifdef SIGINT
    DEBUGMSGTL(("signal", "registering SIGINT signal handler\n"));
    signal(SIGINT, SnmpdShutDown);
#endif
#ifdef SIGHUP
    signal(SIGHUP, SIG_IGN);   /* do not terminate on early SIGHUP */
#endif
#ifdef SIGUSR1
    DEBUGMSGTL(("signal", "registering SIGUSR1 signal handler\n"));
    signal(SIGUSR1, SnmpdDump);
#endif
#ifdef SIGPIPE
    DEBUGMSGTL(("signal", "registering SIGPIPE signal handler\n"));
    signal(SIGPIPE, SIG_IGN);   /* 'Inline' failure of wayward readers */
#endif
#ifdef SIGXFSZ
    signal(SIGXFSZ, SnmpdCatchRandomSignal);
#endif

#ifdef NO_ROOT_ACCESS
    /*
     * Default to no.
     */
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                           NETSNMP_DS_AGENT_NO_ROOT_ACCESS, 1);
#endif
    /*
     * Default to NOT running an AgentX master.
     */
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                           NETSNMP_DS_AGENT_AGENTX_MASTER, 0);
    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                       NETSNMP_DS_AGENT_AGENTX_TIMEOUT, -1);
    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                       NETSNMP_DS_AGENT_AGENTX_RETRIES, -1);

    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                       NETSNMP_DS_AGENT_CACHE_TIMEOUT, 5);
    /*
     * Add some options if they are available.
     */
#if HAVE_UNISTD_H
    strcat(options, "g:u:");
#endif
#if defined(USING_AGENTX_SUBAGENT_MODULE)|| defined(USING_AGENTX_MASTER_MODULE)
    strcat(options, "x:");
#endif
#ifdef USING_AGENTX_SUBAGENT_MODULE
    strcat(options, "X");
#endif

    /*
     * This is incredibly ugly, but it's probably the simplest way
     *  to handle the old '-L' option as well as the new '-Lx' style
     */
    for (i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-L"))
        {
            argv[i] = option_compatability;
        }
    }

#ifdef WIN32
    snmp_log_syslogname(app_name_long);
#else
    snmp_log_syslogname(app_name);
#endif
    netsnmp_ds_set_string(NETSNMP_DS_LIBRARY_ID,
                          NETSNMP_DS_LIB_APPTYPE, app_name);

    zlog_default = openzlog("snmpd", ZLOG_SNMPD,
                            LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);	

	mem_share_attach();

    /*
     * Now process options normally.
     */
    while ((arg = getopt(argc, argv, options)) != EOF)
    {
        switch (arg)
        {
            case '-':
                if (strcasecmp(optarg, "help") == 0)
                {
                    usage(argv[0]);
                }

                if (strcasecmp(optarg, "version") == 0)
                {
                    version();
                }

                handle_long_opt(optarg);
                break;

            case 'a':
                log_addresses++;
                break;

            case 'A':
                netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID,
                                       NETSNMP_DS_LIB_APPEND_LOGFILES, 1);
                break;

            case 'c':
                if (optarg != NULL)
                {
                    netsnmp_ds_set_string(NETSNMP_DS_LIBRARY_ID,
                                          NETSNMP_DS_LIB_OPTIONALCONFIG, optarg);
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'C':
                netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID,
                                       NETSNMP_DS_LIB_DONT_READ_CONFIGS, 1);
                break;

            case 'd':
                //snmp_set_dump_packet(++snmp_dump_packet);
                //netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                //             NETSNMP_DS_AGENT_VERBOSE, 1);
                break;

            case 'D':
                debug_register_tokens(optarg);
                snmp_set_do_debugging(1);
                break;

            case 'f':
                dont_fork = 1;
                break;

#if HAVE_UNISTD_H

            case 'g':
                if (optarg != NULL)
                {
                    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                                       NETSNMP_DS_AGENT_GROUPID, atoi(optarg));
                }
                else
                {
                    usage(argv[0]);
                }

                break;
#endif

            case 'h':
                usage(argv[0]);
                break;

            case 'H':
                do_help = 1;
                break;

            case 'I':
                if (optarg != NULL)
                {
                    add_to_init_list(optarg);
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'l':
                printf("Warning: -l option is deprecated, use -Lf <file> instead\n");

                if (optarg != NULL)
                {
                    if (strlen(optarg) > PATH_MAX)
                    {
                        fprintf(stderr,
                                "%s: logfile path too long (limit %d chars)\n",
                                argv[0], PATH_MAX);
                        exit(1);
                    }

                    snmp_enable_filelog(optarg,
                                        netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID,
                                                               NETSNMP_DS_LIB_APPEND_LOGFILES));
                    log_set = 1;
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'L':
                if (snmp_log_options(optarg, argc, argv) < 0)
                {
                    usage(argv[0]);
                }

                log_set = 1;
                break;

            case 'm':
                if (optarg != NULL)
                {
                    setenv("MIBS", optarg, 1);
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'M':
                if (optarg != NULL)
                {
                    setenv("MIBDIRS", optarg, 1);
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'n':
                if (optarg != NULL)
                {
                    app_name = optarg;
                    netsnmp_ds_set_string(NETSNMP_DS_LIBRARY_ID,
                                          NETSNMP_DS_LIB_APPTYPE, app_name);
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'P':
                printf("Warning: -P option is deprecated, use -p instead\n");

            case 'p':
                if (optarg != NULL)
                {
                    pid_file = optarg;
                }
                else
                {
                    usage(argv[0]);
                }

                break;

            case 'q':
                snmp_set_quick_print(1);
                break;

            case 'r':
                netsnmp_ds_toggle_boolean(NETSNMP_DS_APPLICATION_ID,
                                          NETSNMP_DS_AGENT_NO_ROOT_ACCESS);
                break;

            case 's':
                printf("Warning: -s option is deprecated, use -Lsd instead\n");
                snmp_enable_syslog();
                log_set = 1;
                break;

            case 'S':
                printf("Warning: -S option is deprecated, use -Ls <facility> instead\n");

                if (optarg != NULL)
                {
                    switch (*optarg)
                    {
                        case 'd':
                        case 'D':
                            Facility = LOG_DAEMON;
                            break;

                        case 'i':
                        case 'I':
                            Facility = LOG_INFO;
                            break;

                        case '0':
                            Facility = LOG_LOCAL0;
                            break;

                        case '1':
                            Facility = LOG_LOCAL1;
                            break;

                        case '2':
                            Facility = LOG_LOCAL2;
                            break;

                        case '3':
                            Facility = LOG_LOCAL3;
                            break;

                        case '4':
                            Facility = LOG_LOCAL4;
                            break;

                        case '5':
                            Facility = LOG_LOCAL5;
                            break;

                        case '6':
                            Facility = LOG_LOCAL6;
                            break;

                        case '7':
                            Facility = LOG_LOCAL7;
                            break;

                        default:
                            fprintf(stderr, "invalid syslog facility: -S%c\n", *optarg);
                            usage(argv[0]);
                    }

                    snmp_enable_syslog_ident(snmp_log_syslogname(NULL), Facility);
                    log_set = 1;
                }
                else
                {
                    fprintf(stderr, "no syslog facility specified\n");
                    usage(argv[0]);
                }

                break;

            case 'U':
                netsnmp_ds_toggle_boolean(NETSNMP_DS_APPLICATION_ID,
                                          NETSNMP_DS_AGENT_LEAVE_PIDFILE);
                break;

#if HAVE_UNISTD_H

            case 'u':
                if (optarg != NULL)
                {
                    char           *ecp;
                    int             uid;

                    uid = strtoul(optarg, &ecp, 10);

                    if (*ecp)
                    {
#if HAVE_GETPWNAM && HAVE_PWD_H
                        struct passwd  *info;
                        info = getpwnam(optarg);

                        if (info)
                        {
                            uid = info->pw_uid;
                        }
                        else
                        {
#endif
                            fprintf(stderr, "Bad user id: %s\n", optarg);
                            exit(1);
#if HAVE_GETPWNAM && HAVE_PWD_H
                        }

#endif
                    }

                    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                                       NETSNMP_DS_AGENT_USERID, uid);
                }
                else
                {
                    usage(argv[0]);
                }

                break;
#endif

            case 'v':
                version();

            case 'V':
                netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                                       NETSNMP_DS_AGENT_VERBOSE, 1);
                break;

#if defined(USING_AGENTX_SUBAGENT_MODULE)|| defined(USING_AGENTX_MASTER_MODULE)

            case 'x':
                if (optarg != NULL)
                {
                    netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                                          NETSNMP_DS_AGENT_X_SOCKET, optarg);
                }
                else
                {
                    usage(argv[0]);
                }

                netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                                       NETSNMP_DS_AGENT_AGENTX_MASTER, 1);
                break;
#endif

            case 'X':
#if defined(USING_AGENTX_SUBAGENT_MODULE)
                agent_mode = SUB_AGENT;
#else
                fprintf(stderr, "%s: Illegal argument -X:"
                        "AgentX support not compiled in.\n", argv[0]);
                usage(argv[0]);
                exit(1);
#endif
                break;

            default:
                usage(argv[0]);
                break;
        }
    }

	

    if (do_help)
    {
        netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_AGENT_NO_ROOT_ACCESS, 1);
        init_agent(app_name);        /* register our .conf handlers */
        init_mib_modules();
        init_snmp(app_name);
        fprintf(stderr, "Configuration directives understood:\n");
        read_config_print_usage("  ");
        exit(0);
    }

    if (optind < argc)
    {
        /*
         * There are optional transport addresses on the command line.
         */
        DEBUGMSGTL(("snmpd/main", "optind %d, argc %d\n", optind, argc));

        for (i = optind; i < argc; i++)
        {
            char *c, *astring;

            if ((c = netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                           NETSNMP_DS_AGENT_PORTS)))
            {
                astring = malloc(strlen(c) + 2 + strlen(argv[i]));

                if (astring == NULL)
                {
                    fprintf(stderr, "malloc failure processing argv[%d]\n", i);
                    exit(1);
                }

                sprintf(astring, "%s,%s", c, argv[i]);
                netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                                      NETSNMP_DS_AGENT_PORTS, astring);
                SNMP_FREE(astring);
            }
            else
            {
                netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                                      NETSNMP_DS_AGENT_PORTS, argv[i]);
            }
        }

        DEBUGMSGTL(("snmpd/main", "port spec: %s\n",
                    netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                          NETSNMP_DS_AGENT_PORTS)));
    }

#ifdef LOGFILE

    if (0 == log_set)
        snmp_enable_filelog(LOGFILE,
                            netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID,
                                                   NETSNMP_DS_LIB_APPEND_LOGFILES));

#endif

    /*
     * Initialize a argv set to the current for restarting the agent.
     */
    argvrestartp = (char **)malloc((argc + 2) * sizeof(char *));
    argvptr = argvrestartp;

    for (i = 0, ret = 1; i < argc; i++)
    {
        ret += strlen(argv[i]) + 1;
    }

    argvrestart = (char *) malloc(ret);
    argvrestartname = (char *) malloc(strlen(argv[0]) + 1);

    if (!argvrestartp || !argvrestart || !argvrestartname)
    {
        fprintf(stderr, "malloc failure processing argvrestart\n");
        exit(1);
    }

    strcpy(argvrestartname, argv[0]);

    if (agent_mode == -1)
    {
        if (strstr(argvrestartname, "agentxd") != NULL)
        {
            netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                                   NETSNMP_DS_AGENT_ROLE, SUB_AGENT);
        }
        else
        {
            netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                                   NETSNMP_DS_AGENT_ROLE, MASTER_AGENT);
        }
    }
    else
    {
        netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_AGENT_ROLE, agent_mode);
    }

    for (cptr = argvrestart, i = 0; i < argc; i++)
    {
        strcpy(cptr, argv[i]);
        *(argvptr++) = cptr;
        cptr += strlen(argv[i]) + 1;
    }

    *cptr = 0;
    *argvptr = NULL;

#ifdef BUFSIZ
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
#endif

    netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID,
                           NETSNMP_DS_LIB_DONT_READ_CONFIGS, 1);

    /*
     * Initialize the world.  Detach from the shell.  Create initial user.
     */
    if (!dont_fork)
    {
        int quit = ! netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                            NETSNMP_DS_AGENT_QUIT_IMMEDIATELY);
        ret = netsnmp_daemonize(quit, snmp_stderrlog_status());

        /*
         * xxx-rks: do we care if fork fails? I think we should...
         */
        if (ret != 0)
        {
            Exit(1);    /*  Exit logs exit val for us  */
        }
    }

	

    SOCK_STARTUP;
    init_agent(app_name);        /* do what we need to do first. */

    //devm_comm_get_id(1, 0, MODULE_ID_SNMPD, &gDevTypeID );
	//snmp_ipc_init();

    init_mib_modules();

    /*
     * start library
     */
    
    init_snmp(app_name);

    if ((ret = init_master_agent()) != 0)
    {
        /*
         * Some error opening one of the specified agent transports.
         */
        Exit(1);                /*  Exit logs exit val for us  */
    }

    /*
     * Store persistent data immediately in case we crash later.
     */
    snmp_store(app_name);

#ifdef SIGHUP
    DEBUGMSGTL(("signal", "registering SIGHUP signal handler\n"));
    signal(SIGHUP, SnmpdReconfig);
#endif

    /*
     * Send coldstart trap if possible.
     */
    send_easy_trap(0, 0);

#if HAVE_GETPID

    if (pid_file != NULL)
    {
        /*
         * unlink the pid_file, if it exists, prior to open.  Without
         * doing this the open will fail if the user specified pid_file
         * already exists.
         */
        unlink(pid_file);
        fd = open(pid_file, O_CREAT | O_EXCL | O_WRONLY, 0600);

        if (fd == -1)
        {
            snmp_log_perror(pid_file);

            if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                        NETSNMP_DS_AGENT_NO_ROOT_ACCESS))
            {
                exit(1);
            }
        }
        else
        {
            if ((PID = fdopen(fd, "w")) == NULL)
            {
                snmp_log_perror(pid_file);
                exit(1);
            }
            else
            {
                fprintf(PID, "%d\n", (int) getpid());
                fclose(PID);
            }

            close(fd);
        }
    }

#endif

#if HAVE_UNISTD_H
#ifdef HAVE_SETGID

    if ((gid = netsnmp_ds_get_int(NETSNMP_DS_APPLICATION_ID,
                                  NETSNMP_DS_AGENT_GROUPID)) != 0)
    {
        DEBUGMSGTL(("snmpd/main", "Changing gid to %d.\n", gid));

        if (setgid(gid) == -1
#ifdef HAVE_SETGROUPS
                || setgroups(1, (gid_t *)&gid) == -1
#endif
           )
        {
            snmp_log_perror("setgid failed");

            if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                        NETSNMP_DS_AGENT_NO_ROOT_ACCESS))
            {
                exit(1);
            }
        }
    }

#endif

#ifdef HAVE_SETUID

    if ((uid = netsnmp_ds_get_int(NETSNMP_DS_APPLICATION_ID,
                                  NETSNMP_DS_AGENT_USERID)) != 0)
    {
        DEBUGMSGTL(("snmpd/main", "Changing uid to %d.\n", uid));

        if (setuid(uid) == -1)
        {
            snmp_log_perror("setuid failed");

            if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                        NETSNMP_DS_AGENT_NO_ROOT_ACCESS))
            {
                exit(1);
            }
        }
    }

#endif
#endif

    /*
     * We're up, log our version number.
     */
    snmp_log(LOG_INFO, "NET-SNMP version %s\n", netsnmp_get_version());
#ifdef WIN32SERVICE
    agent_status = AGENT_RUNNING;
#endif
    netsnmp_addrcache_initialise();

    
	
	snmp_cli_master = thread_master_create();
    snmp_cli_master->config = send_warm_start_trap_4_vtysh ;

	signal_init(snmp_cli_master, array_size(snmp_signals), snmp_signals);
	
    cmd_init(1);
    vty_init(snmp_cli_master);
    //keychain_init ();
    memory_init();
    //vty_read_config ( config_file, config_default );
    snmp_cli_init();

    //vty_serv_sock ( vty_addr, vty_port, SNMPD_VTYSH_PATH );
    vty_serv_sock(SNMPD_VTYSH_PATH);

    dev_trap_thread_init();	
	
	snmp_ipc_init();

	devm_comm_get_id(1, 0, MODULE_ID_SNMPD, &gDevTypeID );
	
    exec_cli_rcv_thread();

    init_linkupdown_register();

    init_sys_grp_value();

	snmp_U0_hash_init();

    /*
     * Forever monitor the dest_port for incoming PDUs.
     */
    DEBUGMSGTL(("snmpd/main", "We're up.  Starting to process data.\n"));

    if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                NETSNMP_DS_AGENT_QUIT_IMMEDIATELY))
    {
        receive();
    }

    DEBUGMSGTL(("snmpd/main", "sending shutdown trap\n"));
    SnmpTrapNodeDown();
    DEBUGMSGTL(("snmpd/main", "Bye...\n"));
    snmp_shutdown(app_name);
#ifdef SHUTDOWN_AGENT_CLEANLY /* broken code */
    /* these attempt to free all known memory, but result in double frees */
    shutdown_master_agent();
    shutdown_agent();
#endif

    if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                NETSNMP_DS_AGENT_LEAVE_PIDFILE) &&
            (pid_file != NULL))
    {
        unlink(pid_file);
    }

#ifdef WIN32SERVICE
    agent_status = AGENT_STOPPED;
#endif

    SNMP_FREE(argvrestartname);
    SNMP_FREE(argvrestart);
    SNMP_FREE(argvrestartp);
    SOCK_CLEANUP;

	snmp_exit(0);
    return 0;
}                               /* End main() -- snmpd */

/*******************************************************************-o-******
 * receive
 *
 * Parameters:
 *
 * Returns:
 *  0   On success.
 *  -1  System error.
 *
 * Infinite while-loop which monitors incoming messges for the agent.
 * Invoke the established message handlers for incoming messages on a per
 * port basis.  Handle timeouts.
 */
static int
receive(void)
{
    int             numfds;
    fd_set          readfds, writefds, exceptfds;
    struct timeval  timeout, *tvp = &timeout;
    int             count, block, i;

    /*
     * ignore early sighup during startup
     */
    reconfig = 0;

    /*
     * Loop-forever: execute message handlers for sockets with data
     */
    while (netsnmp_running)
    {
        if (reconfig)
        {
#if HAVE_SIGHOLD
            sighold(SIGHUP);
#endif
            reconfig = 0;
            snmp_log(LOG_INFO, "Reconfiguring daemon\n");
            /*  Stop and restart logging.  This allows logfiles to be
            rotated etc.  */
            netsnmp_logging_restart();
            snmp_log(LOG_INFO, "NET-SNMP version %s restarted\n",
                     netsnmp_get_version());
            update_config();
            send_easy_trap(SNMP_TRAP_ENTERPRISESPECIFIC, 3);
#if HAVE_SIGHOLD
            sigrelse(SIGHUP);
#endif
        }

        for (i = 0; i < NUM_EXTERNAL_SIGS; i++)
        {
            if (external_signal_scheduled[i])
            {
                external_signal_scheduled[i]--;
                external_signal_handler[i](i);
            }
        }

        /*
         * default to sleeping for a really long time. INT_MAX
         * should be sufficient (eg we don't care if time_t is
         * a long that's bigger than an int).
         */
        tvp = &timeout;
        tvp->tv_sec = INT_MAX;
        tvp->tv_usec = 0;

        numfds = 0;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        block = 0;
        snmp_select_info(&numfds, &readfds, tvp, &block);
//        if (block == 1) {
//            tvp = NULL;         /* block without timeout */
//        }

        netsnmp_external_event_info(&numfds, &readfds, &writefds, &exceptfds);

reselect:
        tvp->tv_sec = 0;
        tvp->tv_usec = 1000;

        count = select(numfds, &readfds, &writefds, &exceptfds, tvp);
        DEBUGMSGTL(("snmpd/select", "returned, count = %d\n", count));
//        if (count > 0)
        {
            netsnmp_dispatch_external_events(&count, &readfds,
                                             &writefds, &exceptfds);
            /* If there are still events leftover, process them */
            snmp_read(&readfds);
        }
        /*
                else
                {
                    switch (count)
                    {
                    case 0:
                        snmp_timeout();
                        break;

                    default:
                        snmp_log(LOG_ERR, "select returned %d\n", count);
                        break;
                    }
                }
        */
        /*
         * run requested alarms
         */
        run_alarms();
        netsnmp_check_outstanding_agent_requests();
    }                           /* endwhile */

    snmp_log(LOG_INFO, "Received TERM or STOP signal...  shutting down...\n");

    return 0;

}                               /* end receive() */



/*******************************************************************-o-******
 * snmp_input
 *
 * Parameters:
 *   op
 *  *session
 *   requid
 *  *pdu
 *  *magic
 *
 * Returns:
 *  1       On success  -OR-
 *  Passes through  Return from alarmGetResponse() when
 *            USING_V2PARTY_ALARM_MODULE is defined.
 *
 * Call-back function to manage responses to traps (informs) and alarms.
 * Not used by the agent to process other Response PDUs.
 */
int
snmp_input(int op,
           netsnmp_session *session,
           int reqid, netsnmp_pdu *pdu, void *magic)
{
    struct get_req_state *state = (struct get_req_state *) magic;

    if (op == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {
        if (pdu->command == SNMP_MSG_GET)
        {
            if (state->type == EVENT_GET_REQ)
            {
                /*
                 * this is just the ack to our inform pdu
                 */
                return 1;
            }
        }
    }
    else if (op == NETSNMP_CALLBACK_OP_TIMED_OUT)
    {
        if (state->type == ALARM_GET_REQ)
        {
            /*
             * Need a mechanism to replace obsolete SNMPv2p alarm
             */
        }
    }

    return 1;

}                               /* end snmp_input() */



/*
 * Windows Service Related functions
 */
#ifdef WIN32SERVICE
/************************************************************
* main function for Windows
* Parse command line arguments for startup options,
* to start as service or console mode application in windows.
* Invokes appropriate startup functions depending on the
* parameters passed
*************************************************************/
int
__cdecl
_tmain(int argc, TCHAR *argv[])
{
    /*
     * Define Service Name and Description, which appears in windows SCM
     */
    LPCTSTR         lpszServiceName = app_name_long;      /* Service Registry Name */
    LPCTSTR         lpszServiceDisplayName = _T("Net-SNMP Agent");       /* Display Name */
    LPCTSTR         lpszServiceDescription =
#ifdef IFDESCR
        _T("SNMPv2c / SNMPv3 command responder from Net-SNMP. Supports MIB objects for IP,ICMP,TCP,UDP, and network interface sub-layers.");
#else
        _T("SNMPv2c / SNMPv3 command responder from Net-SNMP");
#endif
    InputParams     InputOptions;


    int             nRunType = RUN_AS_CONSOLE;
    int             quiet = 0;

    nRunType = ParseCmdLineForServiceOption(argc, argv, &quiet);

    switch (nRunType)
    {
        case REGISTER_SERVICE:
            /*
             * Register As service
             */
            InputOptions.Argc = argc;
            InputOptions.Argv = argv;
            exit(RegisterService(lpszServiceName,
                                 lpszServiceDisplayName,
                                 lpszServiceDescription, &InputOptions, quiet));
            break;

        case UN_REGISTER_SERVICE:
            /*
             * Unregister service
             */
            exit(UnregisterService(lpszServiceName, quiet));
            break;

        case RUN_AS_SERVICE:
            /*
             * Run as service
             */
            /*
             * Register Stop Function
             */
            RegisterStopFunction(StopSnmpAgent);
            return RunAsService(SnmpDaemonMain);
            break;

        default:
            /*
             * Run in console mode
             */
            return SnmpDaemonMain(argc, argv);
            break;
    }
}

/*
 * To stop Snmp Agent daemon
 * This portion is still not working
 */
void
StopSnmpAgent(void)
{
    /*
     * Shut Down Agent
     */
    SnmpdShutDown(1);

    /*
     * Wait till agent is completely stopped
     */

    while (agent_status != AGENT_STOPPED)
    {
        Sleep(100);
    }
}

#endif /*WIN32SERVICE*/

#ifdef POWERDOWN_ALARM
int netlink_group_mask(int group)
{
    return group ? 1 << (group - 1) : 0;
}

void alarm_trap_poweroff()
{
    struct timeval tm;
    gettimeofday(&tm, NULL);

#if 0
    /*Ö´ÐÐTRAPÉÏ±¨¶¯×÷*/
    sTrapMsg.iOpCode      = (int)IPC_OPCODE_ADD;
    sTrapMsg.iAlmLevel    = (int)ALM_LEVEL_CRITICAL;
    sTrapMsg.iSerialNo    =  100;
    sTrapMsg.iAlmCode     =  ALM_CODE_DEV_PWR_DOWN;
    sTrapMsg.iAlmCount    =  0;
    sTrapMsg.iLocateIdx1  =  0;
    sTrapMsg.iLocateIdx2  =  0;
    sTrapMsg.iReportTime  =  tm.tv_sec;
    sTrapMsg.iClearTime   =  0;

    strncpy(sTrapMsg.bAlmDesc, almNote, sizeof(almNote));

    convert2TrapAndSend(&sTrapMsg);
#endif

    stSnmpAlmCurrDSTableInfo infoCurr;
    memset(&infoCurr, 0, sizeof(stSnmpAlmCurrDSTableInfo));
    infoCurr.data.iCurrAlmLevel = (int)GPN_ALM_RANK_EMERG;
    infoCurr.data.iCurrAlmType =  GPN_ALM_TYPE_DEV_POW_FAIL;

    infoCurr.data.iAlmCurrThisProductTime = tm.tv_sec;
    pkt_alarm_product_trap(&infoCurr);

    printf("send trap sec:%d usec:%d\n", tm.tv_sec, tm.tv_usec);
}

#define NETLINK_POWEROFF    24
void *poweroff_thread(void *arg)
{
    struct sockaddr_nl saddr, daddr;
    struct nlmsghdr *nlhdr = NULL;
    struct iovec iov;
    struct msghdr msg;
    int sd;
    int ret = 1;
    unsigned char rx[10];

    sd = socket(AF_NETLINK, SOCK_RAW, NETLINK_POWEROFF);

    if (sd < 0)
    {
        return;
    }

    memset(&saddr, 0, sizeof(saddr));
    memset(&daddr, 0, sizeof(daddr));

    saddr.nl_family = AF_NETLINK;
    saddr.nl_pid = getpid();
    saddr.nl_groups = netlink_group_mask(NETLINK_MASK);
    bind(sd, (struct sockaddr *)&saddr, sizeof(saddr));

    nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));

    while (1)
    {
        memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));

        iov.iov_base = (void *)nlhdr;
        iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);
        msg.msg_name = (void *)&daddr;
        msg.msg_namelen = sizeof(daddr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        ret = recvmsg(sd, &msg, 0);

        if (ret <= 0)
        {
            continue;
        }

        memcpy(rx, NLMSG_DATA(nlhdr), 4);
        printf("RECV: value=%d \n", rx[0]);

        if (1 == rx[0])
        {
            alarm_trap_poweroff();
        }

        sleep(1);

    }

    close(sd);
}

#endif

