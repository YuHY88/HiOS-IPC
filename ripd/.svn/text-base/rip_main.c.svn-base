/* RIPd main routine.
 * Copyright (C) 1997, 98 Kunihiro Ishiguro <kunihiro@zebra.org>
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
#include "lib/pid_file.h"
#include <lib/version.h>
#include "getopt.h"
#include "thread.h"
#include "command.h"
#include "memory.h"
#include "prefix.h"
#include "filter.h"
#include "keychain.h"
#include "log.h"
#include "privs.h"
#include "sigevent.h"
#include "vrf.h"

#include "ripd/rip_ipc.h"
#include "ripd/ripd.h"
#include "ripd/rip_msg.h"

#include "ripngd/ripngd.h"


/* ripd options. */
static struct option longopts[] =
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "pid_file",    required_argument, NULL, 'i'},
    { "socket",      required_argument, NULL, 'z'},
    { "help",        no_argument,       NULL, 'h'},
    { "dryrun",      no_argument,       NULL, 'C'},
    { "vty_addr",    required_argument, NULL, 'A'},
    { "vty_port",    required_argument, NULL, 'P'},
    { "retain",      no_argument,       NULL, 'r'},
    { "user",        required_argument, NULL, 'u'},
    { "group",       required_argument, NULL, 'g'},
    { "version",     no_argument,       NULL, 'v'},
    { 0 }
};

/* ripd privileges */
zebra_capabilities_t _caps_p [] =
{
    ZCAP_NET_RAW,
    ZCAP_BIND
};
#if 0
struct zebra_privs_t ripd_privs =
{
#if defined(QUAGGA_USER)
    .user = QUAGGA_USER,
#endif
#if defined QUAGGA_GROUP
    .group = QUAGGA_GROUP,
#endif
#ifdef VTY_GROUP
    .vty_group = VTY_GROUP,
#endif
    .caps_p = _caps_p,
    .cap_num_p = 2,
    .cap_num_i = 0
};
#endif
/* Configuration file and directory. */
char config_default[] = SYSCONFDIR RIPD_DEFAULT_CONFIG;
char *config_file = NULL;

/* ripd program name */

/* Route retain mode flag. */
int retain_mode = 0;

/* RIP VTY bind address. */
char *vty_addr = NULL;

/* RIP VTY connection port. */
int vty_port = RIP_VTY_PORT;

/* Master of threads. */
struct thread_master *master_rip;

/* Process ID saved for use by init system */
const char *pid_file = PATH_RIPD_PID;

//void recvSignal(void);

/* Help information display. */
static void
usage(char *progname, int status)
{
    if (status != 0)
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    else
    {
        printf("Usage : %s [OPTION...]\n\
Daemon which manages RIP version 1 and 2.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-z, --socket       Set path of zebra socket\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-C, --dryrun       Check configuration for validity and exit\n\
-r, --retain       When program terminates, retain added route by ripd.\n\
-u, --user         User to run as\n\
-g, --group        Group to run as\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }

    exit(status);
}

/* SIGHUP handler. */
static void sighup(void)
{
    //zlog_err("%s[%d] : Receive sig = %d!\n", __FILE__, __LINE__, sig);
    rip_clean_all();
    ripng_clean_all();
    rip_reset();
}


/* SIGUSR1 handler. */
static void sigusr1(void)
{
	//zlog_err("%s[%d] : Receive sig = %d!\n", __FILE__, __LINE__, sig);
    zlog_rotate(NULL);
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    //zlog_err ("%s: ripd received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}


/* SIGINT handler. */
static void sigint(void)
{
    //zlog_err("%s[%d] : Receive sig = %d!\n", __FILE__, __LINE__, sig);

    if (!retain_mode)
    {
        rip_clean_all();
        ripng_clean_all();
        global_rip_m_free();
		thread_master_free(master_rip);
    }

    exit(0);
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
struct quagga_signal_t rip_signals[] =
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


/* Main routine of ripd. */
int
main(int argc, char **argv)
{
    char *p;
    int daemon_mode = 0;
    int dryrun = 0;
    char *progname;
    struct thread thread;

    /* Set umask before anything for security */
    umask(0027);

    /* Get program name. */
    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

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
    /* First of all we need logging init. */
    zlog_default = openzlog(progname, ZLOG_RIP,
                            LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

    /* Command line option parse. */
    while (1)
    {
        int opt;

        opt = getopt_long(argc, argv, "df:i:z:hA:P:u:g:rvC", longopts, 0);

        if (opt == EOF)
            break;

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
            case 'P':

                /* Deal with atoi() returning 0 on failure, and ripd not
                   listening on rip port... */
                if (strcmp(optarg, "0") == 0)
                {
                    vty_port = 0;
                    break;
                }

                vty_port = atoi(optarg);

                if (vty_port <= 0 || vty_port > 0xffff)
                    vty_port = RIP_VTY_PORT;

                break;
            case 'r':
                retain_mode = 1;
                break;
            case 'C':
                dryrun = 1;
                break;
//            case 'u':
//                ripd_privs.user = optarg;
//                break;
//            case 'g':
//                ripd_privs.group = optarg;
//                break;
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
    master_rip = thread_master_create();

    FIFO_INIT(&rip_m.routefifo);
    FIFO_INIT(&rip_m.ifmfifo);
    FIFO_INIT(&rip_m.pktfifo);
    rip_m.pktsend_buf = stream_new(1500);

    /* Library initialization. */
	
	signal_init(master_rip, array_size(rip_signals), rip_signals);
    cmd_init(1);
    vty_init(master_rip);
    memory_init();

	/* 非第一个启动进程需调用attach */
	mem_share_attach();	

	/* 定时器初始化，时间间隔单位ms */
    high_pre_timer_init(10, MODULE_ID_RIP);
	
    keychain_init();
    vrf_init();
	
    /* RIP related initialization. */
    //ripc_init();
    rip_init();
	
    rip_if_init();

    /* Get configuration file. */
	// vty_read_config (config_file, config_default);

    /* Start execution only if not in dry-run mode */
    if (dryrun)
        return (0);

    /* Change to the daemon program. */
    if (daemon_mode && daemon(0, 0) < 0)
    {
        zlog_err("RIPd daemon failed: %s", strerror(errno));
        exit(1);
    }

    /* Pid file create. */
    pid_output(pid_file);

    /* Create VTY's socket */
    vty_serv_sock(RIP_VTYSH_PATH);

	
    /*rip_m.t_thread_routefifo =
        thread_add_timer_msec(master_rip, rip_send_route_fifo, NULL, 1000);*/
	rip_m.t_thread_routefifo =
        high_pre_timer_add ((char *)"rip_send_route_timer", LIB_TIMER_TYPE_NOLOOP, \
        					rip_send_route_fifo, NULL, 1000);

	/* 消息接收初始化 */
	if(ipc_recv_init(master_rip) == -1)
    {
        printf(" Rip ipc receive init fail\r\n");
        exit(0);
    }

	if(ipc_recv_thread_start((char *)"RipMsgRev", MODULE_ID_RIP, SCHED_OTHER, -1, rip_pkt_msg_rcv, 0) == -1)
    {
        printf("Rip msg receive thread start fail\r\n");
        exit(0);
    }

	rip_pkt_register();
	ripc_init();
	
	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf(" Rip timer start fail\r\n");
       exit(0);
    }
	
    /* Print banner. */
    zlog_notice("RIPd %s starting: vty@%d", QUAGGA_VERSION, vty_port);

    /* Execute each thread. */
    while (thread_fetch(master_rip, &thread))
        thread_call(&thread);

    /* Not reached. */
    return (0);
}
