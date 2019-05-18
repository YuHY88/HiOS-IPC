/*
 * Copyright (C) 1999 Yasuhiro Ohara
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <zebra.h>
#include <lib/version.h>

#include "getopt.h"
#include "thread.h"
#include "log.h"
#include "command.h"
#include "vty.h"
#include "memory.h"
#include "if.h"
#include "filter.h"
#include "prefix.h"
#include "plist.h"
#include "privs.h"
#include "sigevent.h"
#include "vrf.h"

#include "ospf6d/ospf6d.h"
#include "ospf6d/ospf6_top.h"
#include "ospf6d/ospf6_message.h"
#include "ospf6d/ospf6_asbr.h"
#include "ospf6d/ospf6_lsa.h"
#include "ospf6d/ospf6_interface.h"
#include "ospf6d/ospf6_zebra.h"
#include "ospf6d/ospf6_ipc.h"

extern pid_t pid_output (const char *path);

/* Default configuration file name for ospf6d. */
#define OSPF6_DEFAULT_CONFIG       "ospf6d.conf"

/* Default port values. */
#define OSPF6_VTY_PORT             2606

/* OSPF instance wide configuration pointer to export. */
//struct ospf6d_master *om6;


/* ospf6d privileges */
zebra_capabilities_t _caps_p [] =
{
    ZCAP_NET_RAW,
    ZCAP_BIND
};
#if 0
struct zebra_privs_t ospf6d_privs =
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
/* ospf6d options, we use GNU getopt library. */
struct option longopts[] =
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "pid_file",    required_argument, NULL, 'i'},
    { "socket",      required_argument, NULL, 'z'},
    { "vty_addr",    required_argument, NULL, 'A'},
    { "vty_port",    required_argument, NULL, 'P'},
    { "user",        required_argument, NULL, 'u'},
    { "group",       required_argument, NULL, 'g'},
    { "version",     no_argument,       NULL, 'v'},
    { "dryrun",      no_argument,       NULL, 'C'},
    { "help",        no_argument,       NULL, 'h'},
    { 0 }
};

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR OSPF6_DEFAULT_CONFIG;

/* ospf6d program name. */
char *progname;

/* is daemon? */
int daemon_mode = 0;

/* Master of threads. */
struct thread_master *master_ospf6;

/* Process ID saved for use by init system */
const char *pid_file = PATH_OSPF6D_PID;

/* Help information display. */
static void
usage(char *progname, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    }
    else
    {
        printf("Usage : %s [OPTION...]\n\n\
Daemon which manages OSPF version 3.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-z, --socket       Set path of zebra socket\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-u, --user         User to run as\n\
-g, --group        Group to run as\n\
-v, --version      Print program version\n\
-C, --dryrun       Check configuration for validity and exit\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }

    exit(status);
}

static void __attribute__((noreturn))
ospf6_exit(int status)
{
    struct listnode *node  = NULL, *nnode = NULL;
    struct interface *ifp = NULL;
    struct ospf6 *ospf6 = NULL;
    struct routefifo *routefifo_entry = NULL;

	/* Free ROUTE FIFO*/
    while (!FIFO_EMPTY(&om6->routefifo))
    {
        routefifo_entry = (struct routefifo *)FIFO_TOP(&om6->routefifo);
        if (routefifo_entry)
        {
            FIFO_DEL(routefifo_entry);
            XFREE (MTYPE_ROUTE_FIFO, routefifo_entry);
        }
    }

    for (ALL_LIST_ELEMENTS(om6->ospf6, node, nnode, ospf6))
    { 
    	ospf6_finish_route_table(ospf6);
        ospf6_delete(ospf6);
		ospf6 = NULL;
    }
    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        if (ifp->info != NULL)
        {
            ospf6_interface_delete(ifp->info);
        }
		ifp->info = NULL;
    }

    ospf6_message_terminate();
    ospf6_asbr_terminate();
    ospf6_lsa_terminate();
    vrf_terminate();
    //vty_terminate();
    //cmd_terminate ();

    if (master_ospf6)
    {	
    	//THREAD_OFF(om6->t_read_msg);
		//THREAD_OFF(om6->t_read_pkt);
        thread_master_free(master_ospf6);
		master_ospf6 = NULL;
    }

    if (zlog_default)
    {
        closezlog(zlog_default);
    }

    exit(status);
}

/* SIGHUP handler. */
static void
sighup(void)
{
    zlog_notice("SIGHUP received");
}

/* SIGINT handler. */
static void
sigint(void)
{
    zlog_notice("Terminating on signal SIGINT");
    ospf6_exit(0);
}

/* SIGTERM handler. */
static void
sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");
    ospf6_clean();
    ospf6_exit(0);
}

/* SIGUSR1 handler. */
static void
sigusr1(void)
{
    zlog_notice("SIGUSR1 received");
    zlog_rotate(NULL);
}

static void 
sigpipe (void)
{
    zlog_err ("%s: ospfd received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}

#if 0
static void recvSignal(void)
{
	signal(SIGHUP, sighup);
	signal(SIGUSR1, sigusr1);
	signal(SIGINT, sigint);
	signal(SIGTERM, sigterm);
	signal(SIGPIPE, sigpipe);
	/*signal(SIGSEGV, sigsegvfault);
	signal(SIGABRT, sigabrtfault);*/
}
#endif

/***************Start*************IPC optimization add by zhangzl*****************/
struct quagga_signal_t ospf6_signals[] =
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
        .handler = &sigterm,
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


/* Main routine of ospf6d. Treatment of argument and starting ospf finite
   state machine is handled here. */
int
main(int argc, char *argv[], char *envp[])
{
    char *p;
    int opt;
    int vty_port = 0;
    struct thread thread;
    int dryrun = 0;
    /* Set umask before anything for security */
    umask(0027);

    /* Preserve name of myself. */
    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    /* Command line argument treatment. */
    while (1)
    {
        opt = getopt_long(argc, argv, "df:i:z:hp:A:P:u:g:vC", longopts, 0);

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
                //zclient_serv_path_set (optarg);
                break;
            case 'P':

                /* Deal with atoi() returning 0 on failure, and ospf6d not
                    listening on ospf6d port... */
                if (strcmp(optarg, "0") == 0)
                {
                    vty_port = 0;
                    break;
                }

                vty_port = atoi(optarg);

                if (vty_port <= 0 || vty_port > 0xffff)
                {
                    vty_port = OSPF6_VTY_PORT;
                }

                break;
            case 'u':
                //ospf6d_privs.user = optarg;
                break;
            case 'g':
                //ospf6d_privs.group = optarg;
                break;
            case 'v':
                print_version(progname);
                exit(0);
                break;
            case 'C':
                dryrun = 1;
                break;
            case 'h':
                usage(progname, 0);
                break;
            default:
                usage(progname, 1);
                break;
        }
    }

    if (geteuid() != 0)
    {
        errno = EPERM;
        perror(progname);
        exit(1);
    }

    
    /* Initializations. */
    zlog_default = openzlog(progname, ZLOG_OSPF6,
                            LOG_CONS | LOG_NDELAY | LOG_PID,
                            LOG_DAEMON);

	/* thread master */
    master_ospf6 = thread_master_create();
    
    //zprivs_init(&ospf6d_privs);
    /* initialize zebra libraries */
    signal_init(master_ospf6, array_size(ospf6_signals), ospf6_signals);

/******************************************************************/
	/* 非第一个启动进程需调用attach */
	mem_share_attach();
	
	/* 定时器初始化，时间间隔单位ms */
    high_pre_timer_init(10, MODULE_ID_OSPF6);
	
/******************************************************************/

	ospf6_master_init();
	
    //recvSignal();
    cmd_init(1);
    vty_init(master_ospf6);
    memory_init();
    vrf_init();
    access_list_init();
    prefix_list_init();
    /* initialize ospf6 */
    ospf6_init();

    //ospf6_ipc_init();

    /* parse config file */

    //vty_read_config (config_file, config_default);
    /* Start execution only if not in dry-run mode */
    if (dryrun)
    {
        return(0);
    }

    if (daemon_mode && daemon(0, 0) < 0)
    {
        zlog_err("OSPF6d daemon failed: %s", strerror(errno));
        exit(1);
    }

    /* pid file create */
    pid_output(pid_file);

    /* Make ospf6 vty socket. */
    if (!vty_port)
    {
        vty_port = OSPF6_VTY_PORT;
    }

    vty_serv_sock(OSPF6_VTYSH_PATH);
    /* Print start message */
    zlog_notice("OSPF6d (Quagga-%s ospf6d-%s) starts: vty@%d",
                QUAGGA_VERSION, OSPF6_DAEMON_VERSION, vty_port);

	/* 消息接收初始化 */
	if(ipc_recv_init(master_ospf6) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	ospf6_ipc_init();

	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }
	
    /* Start finite state machine, here we go! */
    while (thread_fetch(master_ospf6, &thread))
    {
        thread_call(&thread);
    }

    /* Log in case thread failed */
    zlog_warn("Thread failed");
    /* Not reached. */
    ospf6_exit(0);
}



