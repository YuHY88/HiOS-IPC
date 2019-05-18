/*
 * OSPFd main routine.
 *   Copyright (C) 1998, 99 Kunihiro Ishiguro, Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>
#include <sched.h>

#include <lib/version.h>
#include "thread.h"
#include "prefix.h"
#include "linklist.h"
#include "if.h"
#include "vector.h"
#include "vty.h"
#include "command.h"
#include "plist.h"
#include "stream.h"
#include "log.h"
#include "memory.h"
#include "privs.h"
#include "sigevent.h"
#include "vrf.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_vty.h"
#include "ospfd/ospf_ipc.h"
#include "ospfd/ospf_packet.h"

#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"


extern pid_t pid_output (const char *path);

/* ospfd privileges */
zebra_capabilities_t _caps_p [] =
{
    ZCAP_NET_RAW,
    ZCAP_BIND,
    ZCAP_NET_ADMIN,
};

/*struct zebra_privs_t ospfd_privs =
{
#if defined(QUAGGA_USER) && defined(QUAGGA_GROUP)
    .user = QUAGGA_USER,
    .group = QUAGGA_GROUP,
#endif
#if defined(VTY_GROUP)
    .vty_group = VTY_GROUP,
#endif
    .caps_p = _caps_p,
    .cap_num_p = array_size(_caps_p),
    .cap_num_i = 0
};*/

/* Configuration filename and directory. */
char config_default[] =  OSPF_DEFAULT_CONFIG;

/* OSPFd options. */
struct option longopts[] =
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "pid_file",    required_argument, NULL, 'i'},
    { "socket",      required_argument, NULL, 'z'},
    { "dryrun",      no_argument,       NULL, 'C'},
    { "help",        no_argument,       NULL, 'h'},
    { "vty_addr",    required_argument, NULL, 'A'},
    { "vty_port",    required_argument, NULL, 'P'},
    { "user",        required_argument, NULL, 'u'},
    { "group",       required_argument, NULL, 'g'},
    { "apiserver",   no_argument,       NULL, 'a'},
    { "version",     no_argument,       NULL, 'v'},
    { 0 }
};

/* OSPFd program name */

/* Master of threads. */
struct thread_master *master_ospf;
struct ospf_statics ospf_sta;


/* Process ID saved for use by init system */
const char *pid_file = PATH_OSPFD_PID;

#ifdef SUPPORT_OSPF_API
extern int ospf_apiserver_enable;
#endif /* SUPPORT_OSPF_API */

/* Help information display. */
static void __attribute__ ((noreturn))
usage (char *progname, int status)
{
    if (status != 0)
    {
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
    }
    else
    {
        printf ("Usage : %s [OPTION...]\n\
Daemon which manages OSPF.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-z, --socket       Set path of zebra socket\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-u, --user         User to run as\n\
-g, --group        Group to run as\n\
-a. --apiserver    Enable OSPF apiserver\n\
-v, --version      Print program version\n\
-C, --dryrun       Check configuration for validity and exit\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }
    exit (status);
}

void recvSignal(void);

/* SIGHUP handler. */
static void sighup (void)
{
    zlog_err ("%s: ospfd received SIGHUP signal! ospfd terminated!", __func__);
}

/* SIGINT / SIGTERM handler. */
static void sigint (void)
{
    zlog_err ("%s: ospfd received SIGINT|SIGTERM signal! ospfd terminated!", __func__);
    zlog_notice ("Terminating on signal");
    ospf_terminate ();
}

/* SIGUSR1 handler. */
static void sigusr1 (void)
{
    zlog_err ("%s: ospfd received SIGUSR1 signal! ospfd terminated!", __func__);
    zlog_rotate (NULL);
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    zlog_err ("%s: ospfd received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}


/* SIGSEGV handler. */
/*static void sigsegvfault(int sig)
{
    zlog_err("%s[%d] : Receive sig = %d!\n", __FILE__, __LINE__, sig);

    signal(sig, SIG_DFL);
}*/

/* SIGABRT handler. */
/*static void sigabrtfault(int sig)
{
    zlog_err("%s[%d] : Receive sig = %d!\n", __FILE__, __LINE__, sig);

    signal(sig, SIG_DFL);
}*/

#if 0
void recvSignal(void)
{
	signal(SIGHUP, sighup);
	signal(SIGUSR1, sigusr1);
	signal(SIGINT, sigint);
	signal(SIGTERM, sigint);
	signal(SIGPIPE, sigpipe);
	/*signal(SIGSEGV, sigsegvfault);
	signal(SIGABRT, sigabrtfault);*/
}
#endif
/***************Start*************IPC optimization add by zhangzl*****************/
struct quagga_signal_t ospf_signals[] =
{
    {
        .signal  = SIGHUP,
        .handler = &sighup,
        .caught  = 0,
    },
    {
        .signal  = SIGUSR1,
        .handler = &sigusr1,
        .caught  = 0,
    },
    {
        .signal  = SIGINT,
        .handler = &sigint,
        .caught  = 0,
    },
    {
        .signal  = SIGTERM,
        .handler = &sigint,
        .caught  = 0,
    },
    {
		.signal  = SIGPIPE,
		.handler = &sigpipe,
		.caught  = 0,
	},
    {
        .signal  = SIGALRM,
        .handler = &high_pre_timer_dotick,
        .caught  = 0,
    },
};
/***************End*************IPC optimization add by zhangzl*******************/


/* OSPFd main routine. */
int
main (int argc, char **argv)
{
    char *p  = NULL;
    int vty_port = OSPF_VTY_PORT;
    int daemon_mode = 0;
    char *progname = NULL;
    struct thread thread;
    int dryrun = 0;
    /* Set umask before anything for security */
    umask (0027);
#ifdef SUPPORT_OSPF_API
    /* OSPF apiserver is disabled by default. */
    ospf_apiserver_enable = 0;
#endif /* SUPPORT_OSPF_API */
    /* get program name */
    progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
    struct sched_param param;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);   /* 绑定cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);
    param.sched_priority = 40;
#ifndef HAVE_KERNEL_3_0		
    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }
#endif	
    while (1)
    {
        int opt;
        opt = getopt_long (argc, argv, "df:i:z:hA:P:u:g:avC", longopts, 0);
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
            break;
        case 'A':
            break;
        case 'i':
            pid_file = optarg;
            break;
        case 'z':
            //ospf_serv_path_set (optarg);
            break;
        case 'P':
            /* Deal with atoi() returning 0 on failure, and ospfd not
                listening on ospfd port... */
            if (strcmp(optarg, "0") == 0)
            {
                vty_port = 0;
                break;
            }
            vty_port = atoi (optarg);
            if (vty_port <= 0 || vty_port > 0xffff)
            {
                vty_port = OSPF_VTY_PORT;
            }
            break;
        case 'u':
            break;
        case 'g':
            break;
#ifdef SUPPORT_OSPF_API
        case 'a':
            ospf_apiserver_enable = 1;
            break;
#endif /* SUPPORT_OSPF_API */
        case 'v':
            print_version (progname);
            exit (0);
            break;
        case 'C':
            dryrun = 1;
            break;
        case 'h':
            usage (progname, 0);
            break;
        default:
            usage (progname, 1);
            break;
        }
    }
    /* Invoked by a priviledged user? -- endo. */
    if (geteuid () != 0)
    {
        errno = EPERM;
        perror (progname);
        zlog_err ("%s(%d): OSPFd geteuid error.", __func__,__LINE__);
        exit (1);
    }
    zlog_default = openzlog (progname, ZLOG_OSPF,
                             LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

	master_ospf = thread_master_create ();

	/* Library inits. */
    signal_init(master_ospf, array_size(ospf_signals), ospf_signals);

/***********************************************************************/
	
	cmd_init (1);
    vty_init (master_ospf);
    memory_init ();
	
	/* 非第一个启动进程需调用attach */
	mem_share_attach();
	
	/* 定时器初始化，时间间隔单位ms */
    high_pre_timer_init(10, MODULE_ID_OSPF);

/************************************************************************/
	
    /* OSPF master init. */
    ospf_master_init ();
    
    vrf_init ();
    access_list_init ();
    prefix_list_init ();

	/* Start execution only if not in dry-run mode */
    if (dryrun)
    {
        zlog_err ("%s(%d): dry-run mode error.", __func__,__LINE__);
        return(0);
    }
    
    /* Change to the daemon program. */
    if (daemon_mode && daemon (0, 0) < 0)
    {
        zlog_err("OSPFd daemon failed: %s", strerror(errno));
        exit (1);
    }
	
	/* Process id file create. */
    pid_output (pid_file);
	
	/* Create VTY socket */
    vty_serv_sock (OSPF_VTYSH_PATH);

    /* OSPFd inits. */
    ospf_if_init ();
    ospf_buf_init ();
	
    /* OSPF vty command inits. */
	debug_init ();
    ospf_vty_init ();
    ospf_vty_show_init ();
    ospf_route_map_init ();
#ifdef HAVE_SNMP
    //ospf_snmp_init ();
#endif /* HAVE_SNMP */
    ospf_opaque_init ();

    /*om->t_thread_routefifo =
        thread_add_timer_msec (master_ospf, ospf_asyc_send_route_cmd, NULL, 1000);*/
	om->t_thread_routefifo =
        high_pre_timer_add ((char *)"ospf_asyc_route_timer", LIB_TIMER_TYPE_NOLOOP, ospf_asyc_send_route_cmd, NULL, 1000);

    /* Print banner. */
    zlog_notice ("OSPFd %s starting: vty@%d", QUAGGA_VERSION, vty_port);

	/* 消息接收初始化 */
	if(ipc_recv_init(master_ospf) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	ospf_init();

	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }

    /* Fetch next active thread. */
    while (thread_fetch (master_ospf, &thread))
    {
        thread_call (&thread);
    }
    
    /* Not reached. */
    return (0);
}



