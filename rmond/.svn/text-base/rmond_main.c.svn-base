/******************************************************************************
 * Filename: rmond_main.c
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: main functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/

#include <zebra.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include "prefix.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "memory.h"
#include "keychain.h"
#include "vty.h"
#include "syslog.h"
#include "log.h"
#include <lib/thread.h>
#include "lib/hash1.h"
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/ifm_common.h>
#include <lib/rmon_common.h>

#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"
#include "lib/sigevent.h"


#include "rmon_api.h"
#include "rmond_main.h"
#include "rmon_cli.h"

#define RMOND_DEFAULT_CONFIG   "rmond.conf"
#define PATH_RMOND_PID "/var/run/rmond.pid"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_RMOND_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = RMOND_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR RMOND_DEFAULT_CONFIG;
char *config_file = NULL;
struct thread_master *rmond_master_inst;

/* common options. */
static struct option longopts[] =
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "pid_file",    required_argument, NULL, 'i'},
    { "help",        no_argument,       NULL, 'h'},
    { "vty_addr",    required_argument, NULL, 'A'},
    { "retain",      no_argument,       NULL, 'r'},
    { "version",     no_argument,       NULL, 'v'},
    { 0 }
};

extern void  vty_serv_un_sess(char *);

void rmond_init(void);
void rmond_die(void);

/* Help information display. */
static void usage(char *progname, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    }
    else
    {
        printf("Usage : %s [OPTION...]\n\
Daemon which manages AAA authentication.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-A, --vty_addr     Set vty's bind address\n\
-r, --retain       When program terminates, retain added route by vrrp.\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }

    exit(status);
}

#if 0
/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

RETSIGTYPE *
signal_set(int signo, void (*func)(int))
{
    int ret;
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
#ifdef SA_RESTART
    sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

    ret = sigaction(signo, &sig, &osig);

    if (ret < 0)
    {
        return (SIG_ERR);
    }
    else
    {
        return (osig.sa_handler);
    }
}

/* SIGHUP handler. */
void sighup(int sig)
{
    zlog_notice("SIGHUP received");

    /* Create VTY's socket */
    vty_serv_sock(RMOND_VTYSH_PATH);

    /* Try to return to normal operation. */
}

/* SIGINT handler. */
void sigint(int sig)
{
    zlog_notice("SIGINT received\n");

    rmond_die();

    exit(1);
}

/* SIGUSR1 handler. */
void sigusr1(int sig)
{
    zlog_notice("SIGUSR1 received\n");
}

/* SIGUSR2 handler. */
void sigusr2(int sig)
{
    zlog_notice("SIGUSR2 received\n");
}

/* Initialization of signal handles. */
void _signal_init()
{
    signal_set(SIGHUP, sighup);
    signal_set(SIGINT, sigint);
    signal_set(SIGTERM, sigint);
    signal_set(SIGPIPE, SIG_IGN);
    signal_set(SIGUSR1, sigusr1);
    signal_set(SIGUSR2, sigusr2);
}
#endif

#if 1
/*---------------------------------ipc reform------------------------------*/
static void __attribute__ ((noreturn)) rmon_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(rmond_master_inst)
    {
        thread_master_free (rmond_master_inst);
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

    rmon_exit(0);
}

struct quagga_signal_t rmon_signals[] =
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
#endif
/*-------------------------------ipc reform ---------------------------*/


/* Main routine */
int main(int argc, char **argv)
{
    /*  VTY connection port. */
    char *p;
    int daemon_mode = 0;
    char *progname;
    struct thread thread;

    /* Set umask before anything for security */
    umask(0027);

    /* Get program name. */
    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    struct sched_param param;

    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(1, &mask);   /* ç»‘å®šcpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);

    param.sched_priority = 30;
#ifndef HAVE_KERNEL_3_0

    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }
#endif
    /* First of all we need logging init. */
    zlog_default = openzlog(progname, ZLOG_RMOND,
                            LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

    /* Command line option parse. */
    while (1)
    {
        int opt;

        opt = getopt_long(argc, argv, "df:hA:P:rv", longopts, 0);

        if (opt == EOF)
        {
            break;
        }

        switch (opt)
        {
            case 0:
                break;

            case 'd':
                daemon_mode = 1;
                break;

            case 'f':
                config_file = optarg;
                break;

            case 'A':
                vty_addr = optarg;
                break;

            case 'i':
                pid_file = optarg;
                break;

            case 'v':
                print_version(progname);
                exit(0);
                break;

            case 'h':
                usage(progname, 0);
                break;

            default:
                usage(progname, 1);
                break;
        }
    }

    /* Prepare master thread. */
    rmond_master_inst = thread_master_create();
    /* Library initialization. */
//    _signal_init();

    signal_init(rmond_master_inst, array_size(rmon_signals), rmon_signals);
    cmd_init(1);
    vty_init(rmond_master_inst);
    memory_init();

    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }

    pid_output(pid_file);    /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(RMOND_VTYSH_PATH);

    rmond_init();

    /* Execute each thread. */
    while (thread_fetch(rmond_master_inst, &thread))
    {
        thread_call(&thread);
    }

    rmond_die();
    rmon_exit(0);
    exit(0);
}

#if 0
int rmon_msg_rcv(struct thread *t)
{
    static struct ipc_mesg mesg;
    struct ifm_event *pevent = NULL;
    uint32_t ifindex;

    memset(&mesg, 0, sizeof(struct ipc_mesg));

    if (ipc_recv_common(&mesg, MODULE_ID_RMON) == -1)
    {
        zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d] recv msg err (%s)\n", __FUNCTION__, __LINE__, strerror(errno));
        goto out;
    }

    switch (mesg.msghdr.msg_type)
    {
        case IPC_TYPE_SNMP:
            rmon_handle_snmp_msg(&mesg);
            break;

        case  IPC_TYPE_IFM:
            if (mesg.msghdr.opcode == IPC_OPCODE_EVENT)
            {
                pevent = (struct ifm_event *)mesg.msg_data;
                ifindex = pevent->ifindex;

                if (mesg.msghdr.msg_subtype == IFNET_EVENT_UP)
                {
                    zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d] recv event notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, ifindex);
                    rmon_if_up(ifindex);
                }
                else if (mesg.msghdr.msg_subtype == IFNET_EVENT_IF_DELETE)
                {
                    zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d] recv event notify ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, ifindex);
                    rmon_if_delete(ifindex);
                }
            }

            break;

        default:
            break;
    }

out:
    usleep(100 * 1000); //sleep CPU 100ms
    thread_add_event(rmond_master_inst, rmon_msg_rcv, NULL, 0);

    return 0;
}
#endif
int rmon_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    struct ifm_event *pevent = NULL;
    uint32_t ifindex;
    switch (pmsg->msghdr.msg_type)
    {
        case IPC_TYPE_SNMP:
            rmon_handle_snmp_msg(pmsg);
            break;

        case  IPC_TYPE_IFM:
            if (pmsg->msghdr.opcode == IPC_OPCODE_EVENT)
            {
                pevent = (struct ifm_event *)pmsg->msg_data;
                ifindex = pevent->ifindex;

                if (pmsg->msghdr.msg_subtype == IFNET_EVENT_UP)
                {
                    zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d] recv event notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, ifindex);
                    rmon_if_up(ifindex);
                }
                else if (pmsg->msghdr.msg_subtype == IFNET_EVENT_IF_DELETE)
                {
                    zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d] recv event notify ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, ifindex);
                    rmon_if_delete(ifindex);
                }
            }

            break;

        default:
            break;
    }
    return 0;
}


static void rmond_thread_init(void)
{
    thread_add_event(rmond_master_inst, rmon_msg_rcv, NULL, 0);
}

struct thread_master *get_rmond_thread_master()
{
    assert(rmond_master_inst);
    return (rmond_master_inst);
}

void rmond_init(void)
{
#if 0
    ipc_connect_common();
    init_rmon_master();
    rmon_cli_init();
    rmond_thread_init();
    register_if_msg();
#endif
#if 1
//    ipc_connect_common();
    init_rmon_master();
    rmon_cli_init();

    mem_share_attach(); /* éžç¬¬ä¸€ä¸ªå¯åŠ¨è¿›ç¨‹éœ€è°ƒç”¨attach */

    /* å®šæ—¶å™¨åˆå§‹åŒ–ï¼Œæ—¶é—´é—´éš”å•ä½ms */
    high_pre_timer_init(1000, MODULE_ID_RMON);

    /* æ¶ˆæ¯æŽ¥æ”¶åˆå§‹åŒ?*/
    if(ipc_recv_init(rmond_master_inst) == -1)
    {
        printf( " %s[%d] ipc receive init fail\n", __FUNCTION__, __LINE__);
        exit(0);
    }

    /* å¯åŠ¨æŽ¥æ”¶çº¿ç¨‹ */
    if(ipc_recv_thread_start((char *)"RMONIpcRev", MODULE_ID_RMON, SCHED_OTHER, -1, rmon_msg_rcv, 0) == -1)
    {
        printf( " %s[%d] ipc receive thread start fail\n", __FUNCTION__, __LINE__);
        exit(0);
    }

#if 1
    /* å¯åŠ¨åŸºå‡†å®šæ—¶å™?*/
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }
 #endif

    register_if_msg();
#endif
}

void rmond_die(void)
{
    if (NULL != zlog_default)
    {
        closezlog(zlog_default);
    }

    exit(0);
}
