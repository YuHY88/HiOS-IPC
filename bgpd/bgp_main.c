/* Main routine of bgpd.
   Copyright (C) 1996, 97, 98, 1999 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#include <zebra.h>
#include <sched.h>
#include <lib/version.h>
#include <lib/vector.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/getopt.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"
#include "lib/hptimer.h"
#include <lib/prefix.h>
#include <lib/log.h>
#include <lib/privs.h>
#include <lib/sigevent.h>
#include <lib/routemap.h>
#include <lib/filter.h>
#include <lib/plist.h>
#include <lib/stream.h>
#include <lib/vrf.h>
#include <lib/workqueue.h>

#include "bgpd/bgpd.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_mplsvpn.h"
#include "bgpd/bgp_aspath.h"
#include "bgpd/bgp_dump.h"
#include "bgpd/bgp_route.h"
#include "bgpd/bgp_nexthop.h"
#include "bgpd/bgp_regex.h"
#include "bgpd/bgp_clist.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_filter.h"
#include "bgpd/bgp_zebra.h"
#include "bgpd/bgp_ipc.h"
#include "bgpd/bgp_packet.h"

extern pid_t pid_output (const char *path);

/* bgpd options, we use GNU getopt library. */
static const struct option longopts[] =
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "pid_file",    required_argument, NULL, 'i'},
    { "socket",      required_argument, NULL, 'z'},
    { "bgp_port",    required_argument, NULL, 'p'},
    { "listenon",    required_argument, NULL, 'l'},
    { "vty_addr",    required_argument, NULL, 'A'},
    { "vty_port",    required_argument, NULL, 'P'},
    { "retain",      no_argument,       NULL, 'r'},
    { "no_kernel",   no_argument,       NULL, 'n'},
    { "user",        required_argument, NULL, 'u'},
    { "group",       required_argument, NULL, 'g'},
    { "version",     no_argument,       NULL, 'v'},
    { "dryrun",      no_argument,       NULL, 'C'},
    { "help",        no_argument,       NULL, 'h'},
    { 0 }
};

/* signal definitions */
void sighup (void);
void sigint (void);
void sigusr1 (void);
//void recvSignal(void);

static void bgp_exit (int);

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR BGP_DEFAULT_CONFIG;

/* Route retain mode flag. */
static int retain_mode = 0;

/* Manually specified configuration file name.  */
char *config_file = NULL;

/* Process ID saved for use by init system */
static const char *pid_file = PATH_BGPD_PID;

/* VTY port number and address.  */
int vty_port = BGP_VTY_PORT;
char *vty_addr = NULL;

/* Master of threads. */
//struct thread_master *master;

/* Help information display. */
static void
usage (char *progname, int status)
{
    if (status != 0)
    {
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
    }
    else
    {
        printf ("Usage : %s [OPTION...]\n\n\
Daemon which manages kernel routing table management and \
redistribution between different routing protocols.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-z, --socket       Set path of zebra socket\n\
-p, --bgp_port     Set bgp protocol's port number\n\
-l, --listenon     Listen on specified address (implies -n)\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-r, --retain       When program terminates, retain added route by bgpd.\n\
-n, --no_kernel    Do not install route to kernel.\n\
-u, --user         User to run as\n\
-g, --group        Group to run as\n\
-v, --version      Print program version\n\
-C, --dryrun       Check configuration for validity and exit\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }
    exit (status);
}

/* SIGHUP handler. */
void sighup (void)
{
	zlog_err ("%s: bgpd received SIGHUP signal! bgpd terminated!", __func__);
	
    /* Terminate all thread. */
    bgp_terminate ();
    bgp_reset ();
    zlog_debug (BGP_DEBUG_TYPE_OTHER,"bgpd restarting!");
	
    /* Create VTY's socket */
    vty_serv_sock (BGP_VTYSH_PATH);
}

/* SIGINT handler. */
void sigint (void)
{
	zlog_err ("%s: bgpd received SIGINT signal! bgpd terminated!", __func__);
	
    if (! retain_mode)
    {
        bgp_terminate ();
    }
	
    bgp_exit (0);
}

/* SIGUSR1 handler. */
void sigusr1 (void)
{
	zlog_err ("%s: bgpd received SIGUSR1 signal! bgpd terminated!", __func__);
    zlog_rotate (NULL);
}

/*
  Try to free up allocations we know about so that diagnostic tools such as
  valgrind are able to better illuminate leaks.

  Zebra route removal and protocol teardown are not meant to be done here.
  For example, "retain_mode" may be set.
*/
static void
bgp_exit (int status)
{
    struct bgp *bgp;
    struct listnode *node, *nnode;
	
    struct interface *ifp;
	
    /* it only makes sense for this to be called on a clean exit */
    //assert (status == 0);
	if(status)
	{
		return ;
	}
	
    /* reverse bgp_master_init */
    for (ALL_LIST_ELEMENTS (bm->bgp, node, nnode, bgp))
    {
        bgp_delete (bgp);
    }
	
    list_free (bm->bgp);
    bm->bgp = NULL;
	
    /*
     * bgp_delete can re-allocate the process queues after they were
     * deleted in bgp_terminate. delete them again.
     *
     * It might be better to ensure the RIBs (including static routes)
     * are cleared by bgp_terminate() during its call to bgp_cleanup_routes(),
     * which currently only deletes the kernel routes.
     */
    if (bm->process_main_queue)
    {
        work_queue_free (bm->process_main_queue);
        bm->process_main_queue = NULL;
    }
    if (bm->process_rsclient_queue)
    {
        work_queue_free (bm->process_rsclient_queue);
        bm->process_rsclient_queue = NULL;
    }
	
    /* reverse bgp_zebra_init/if_init */
    if (retain_mode)
    {
        if_add_hook (IF_DELETE_HOOK, NULL);
    }
	
    for (ALL_LIST_ELEMENTS_RO (iflist, node, ifp))
    {
        struct listnode *c_node, *c_nnode;
        struct connected *c;
        for (ALL_LIST_ELEMENTS (ifp->connected, c_node, c_nnode, c))
        {
            bgp_connected_delete (c);
        }
    }
	
    /* reverse bgp_attr_init */
    bgp_attr_finish ();
	
    /* reverse bgp_dump_init */
    bgp_dump_finish ();
	
    /* reverse bgp_route_init */
    bgp_route_finish ();
	
    /* reverse bgp_route_map_init/route_map_init */
    route_map_finish ();
	
    /* reverse bgp_scan_init */
    bgp_scan_finish ();
	
    /* reverse access_list_init */
    access_list_add_hook (NULL);
    access_list_delete_hook (NULL);
    access_list_reset ();
	
    /* reverse bgp_filter_init */
    as_list_add_hook (NULL);
    as_list_delete_hook (NULL);
    bgp_filter_reset ();
	
    /* reverse prefix_list_init */
    prefix_list_add_hook (NULL);
    prefix_list_delete_hook (NULL);
    prefix_list_reset ();
	
    /* reverse community_list_init */
    community_list_terminate (bgp_clist);
	
    bgp_address_destroy();
	
    /* reverse bgp_master_init */
    if (bm->master)
    {
        thread_master_free (bm->master);
    }
	
    if (zlog_default)
    {
        closezlog (zlog_default);
    }
	
    if (CONF_BGP_DEBUG (normal, NORMAL))
    {
        log_memstats_stderr ("bgpd");
    }
	
    exit (status);
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    zlog_err ("%s: bgpd received SIGPIPE signal!", __func__);
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

/***************Start*************IPC optimization add by zhudw*****************/
struct quagga_signal_t bgp_signals[] =
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
/***************End*************IPC optimization add by zhudw*******************/


/* Main routine of bgpd. Treatment of argument and start bgp finite
   state machine is handled at here. */
int
main (int argc, char **argv)
{
    char *p;
    int opt;
    int daemon_mode = 0;
    int dryrun = 0;
    char *progname;
    struct thread thread;
    int tmp_port;
	
    /* Set umask before anything for security */
    umask (0027);
	
    /* Preserve name of myself. */
    progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
	
    //struct sched_param param;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);   /* 绑定cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);
    //param.sched_priority = 40;
#ifndef HAVE_KERNEL_3_0	
  //  if (sched_setscheduler(0, SCHED_RR, &param))
  //  {
  //      perror("\n  priority set: ");
  //  }
#endif	
	
    /* Command line argument treatment. */
    while (1)
    {
        opt = getopt_long (argc, argv, "df:i:z:hp:l:A:P:rnu:g:vC", longopts, 0);
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
	        case 'i':
	            pid_file = optarg;
	            break;
	        case 'z':
	            break;
	        case 'p':
	            tmp_port = atoi (optarg);
	            if (tmp_port <= 0 || tmp_port > 0xffff)
	            {
	                bm->port = BGP_PORT_DEFAULT;
	            }
	            else
	            {
	                bm->port = tmp_port;
	            }
	            break;
	        case 'A':
	            vty_addr = optarg;
	            break;
	        case 'P':
	            /* Deal with atoi() returning 0 on failure, and bgpd not
	               listening on bgp port... */
	            if (strcmp(optarg, "0") == 0)
	            {
	                vty_port = 0;
	                break;
	            }
	            vty_port = atoi (optarg);
	            if (vty_port <= 0 || vty_port > 0xffff)
	            {
	                vty_port = BGP_VTY_PORT;
	            }
	            break;
	        case 'r':
	            retain_mode = 1;
	            break;
	        case 'l':
	            bm->address = optarg;
	            /* listenon implies -n */
	        case 'n':
	            bgp_option_set (BGP_OPT_NO_FIB);
	            break;
	        case 'u':
	            break;
	        case 'g':
	            break;
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
    zlog_default = openzlog (progname, ZLOG_BGP,
                             LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);
	
	//mem_share_init();
    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    
    /* BGP master init. */
    bgp_master_init ();
	
    /* Initializations. */
    srandom (time (NULL));
    //recvSignal();
    signal_init(bm->master, array_size(bgp_signals), bgp_signals);
    cmd_init (1);
    vty_init (bm->master);
    memory_init ();
    vrf_init ();

    /*
    if(ipc_init_n() == -1)
    {
       printf(" ipc queue init fail\r\n");
       exit(0);
    }
    */
    high_pre_timer_init(50, MODULE_ID_BGP);
	
    /* Start execution only if not in dry-run mode */
    if(dryrun)
    {
        return(0);
    }
	
    /* Turn into daemon if daemon_mode is set. */
    if (daemon_mode && daemon (0, 0) < 0)
    {
        zlog_err("BGPd daemon failed: %s", strerror(errno));
        return (1);
    }
    
    /* BGP related initialization.  */
    bgp_init ();
	
    /* Process ID file creation. */
    pid_output (pid_file);
	
    /* Make bgp vty socket. */
    vty_serv_sock (BGP_VTYSH_PATH);
	
    /* Print banner. */
    zlog_notice ("BGPd %s starting: vty@%d, bgp@%s:%d pid %d", QUAGGA_VERSION,
                 vty_port,
                 (bm->address ? bm->address : "<all>"),
                 bm->port,
                 getpid ());
	
    if(ipc_recv_init(bm->master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }
#if 0	
    /* add receive common message thread*/
	bgp_common_msg_rcv_init();
	
	/* add receive route thread */
	bgp_route_msg_rcv_init();
#endif


    if(ipc_recv_thread_start("BgpIpcRev", MODULE_ID_BGP, SCHED_OTHER, -1, bgp_msg_rcv, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }

	
    bm->routefifo_id =  high_pre_timer_add("bgp_send_route", LIB_TIMER_TYPE_LOOP, bgp_send_route_timer, NULL, 1000);
	bm->routefifo_vrf_id  =  high_pre_timer_add("bgp_send_route_vrf", LIB_TIMER_TYPE_LOOP, bgp_send_route_timer_vrf, NULL, 1000);
	
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }
	
    /* 执行主线程 */
    while(thread_fetch (bm->master, &thread)) thread_call (&thread);


#if 0 				 
    bm->t_thread_routefifo =
        thread_add_timer_msec (bm->master, bgp_send_route_timer, NULL, 1000);

    bm->t_thread_routefifo_vrf =
        thread_add_timer_msec (bm->master, bgp_send_route_timer_vrf, NULL, 1000);
			
    /* Start finite state machine, here we go! */
    while (thread_fetch (bm->master, &thread))
    {
        thread_call (&thread);
    }
#endif	
    /* Not reached. */
    return (0);
}

