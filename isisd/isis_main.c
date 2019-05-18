/*
 * IS-IS Rout(e)ing protocol - isis_main.c
 *
 * Copyright (C) 2001,2002   Sampo Saaristo
 *                           Tampere University of Technology      
 *                           Institute of Communications Engineering
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public Licenseas published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
 * more details.

 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <zebra.h>
#include <sched.h>

#include "getopt.h"
#include "thread.h"
#include "log.h"
#include <lib/version.h>
#include "command.h"
#include "vty.h"
#include "memory.h"
#include "stream.h"
#include "if.h"
#include "privs.h"
#include "sigevent.h"
#include "filter.h"
#include "plist.h"
#include "vrf.h"

#include "isisd/dict.h"
#include "include-netbsd/iso.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_circuit.h"
#include "isisd/isisd.h"
#include "isisd/isis_dynhn.h"
#include "isisd/isis_spf.h"
#include "isisd/isis_route.h"
#include "isisd/isis_routemap.h"
#include "isisd/isis_zebra.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_ipc.h"
#include "isisd/isis_msg.h"


/* Default configuration file name */
#define ISISD_DEFAULT_CONFIG "isisd.conf"
/* Default vty port */
#define ISISD_VTY_PORT       2608

/* isisd privileges */
zebra_capabilities_t _caps_p[] = {
  ZCAP_NET_RAW,
  ZCAP_BIND
};

/*struct zebra_privs_t isisd_privs = {
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
  .cap_num_p = sizeof (_caps_p) / sizeof (*_caps_p),
  .cap_num_i = 0
};*/

/* isisd options */
struct option longopts[] = {
  {"daemon",      no_argument,       NULL, 'd'},
  {"config_file", required_argument, NULL, 'f'},
  {"pid_file",    required_argument, NULL, 'i'},
  {"socket",      required_argument, NULL, 'z'},
  {"vty_addr",    required_argument, NULL, 'A'},
  {"vty_port",    required_argument, NULL, 'P'},
  {"user",        required_argument, NULL, 'u'},
  {"group",       required_argument, NULL, 'g'},
  {"version",     no_argument,       NULL, 'v'},
  {"dryrun",      no_argument,       NULL, 'C'},
  {"help",        no_argument,       NULL, 'h'},
  {0}
};

extern pid_t pid_output (const char *path);

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR ISISD_DEFAULT_CONFIG;
char *config_file = NULL;

/* isisd program name. */
char *progname;

int daemon_mode = 0;

/* Master of threads. */
struct thread_master *master;

struct isis_statistics isis_sta = {0};

/* Process ID saved for use by init system */
const char *pid_file = PATH_ISISD_PID;

/* for reload */
char _cwd[MAXPATHLEN];
char _progpath[MAXPATHLEN];
int _argc;
char **_argv;
char **_envp;

/*
 * Prototypes.
 */
void sighup (void);
void sigint (void);
void sigterm (void);
void sigusr1 (void);
void recvSignal(void);
void reload(void);


/* Help information display. */
static void
usage (int status)
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", progname);
  else
    {
      printf ("Usage : %s [OPTION...]\n\n\
Daemon which manages IS-IS routing\n\n\
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

  exit (status);
}


void
reload ()
{
    zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"Reload");
    /* FIXME: Clean up func call here */
    //vty_reset ();
    execve (_progpath, _argv, _envp);
    zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"Reload failed: cannot exec %s: %s", _progpath,
                safe_strerror (errno));
}

static void
terminate (int i)
{
  exit (i);
}

/*
 * Signal handlers
 */

void sighup (void)
{
  zlog_err ("%s: isisd received SIGHUP signal! isisd terminated!", __func__);
  reload ();

  return;
}

void sigint (void)
{
  zlog_err ("%s: isisd received SIGINT signal! isisd terminated!", __func__);
  terminate (0);
}

void sigterm (void)
{
  zlog_err ("%s: isisd received SIGTERM signal! isisd terminated!", __func__);
  terminate (0);
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    zlog_err ("%s: isisd received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}


void sigusr1 (void)
{
  zlog_err ("%s: isisd received SIGUSR1 signal! isisd terminated!", __func__);
  zlog_rotate (NULL);
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


struct quagga_signal_t isis_signals[] =
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


/*
 * Main routine of isisd. Parse arguments and handle IS-IS state machine.
 */
int
main (int argc, char **argv, char **envp)
{
  char *p;
  int opt, vty_port = ISISD_VTY_PORT;
  struct thread thread;
  int dryrun = 0;

  /* Get the programname without the preceding path. */
  progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);

	struct sched_param param;
	cpu_set_t mask; 
	
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 绑定cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);
	
	param.sched_priority = 40;
#ifndef HAVE_KERNEL_3_0		
	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif
  zlog_default = openzlog (progname, ZLOG_ISIS,
			   LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

  /* for reload */
  _argc = argc;
  _argv = argv;
  _envp = envp;
  getcwd (_cwd, sizeof (_cwd));
  if (*argv[0] == '.')
    snprintf (_progpath, sizeof (_progpath), "%s/%s", _cwd, _argv[0]);
  else
    snprintf (_progpath, sizeof (_progpath), "%s", argv[0]);

  /* Command line argument treatment. */
  while (1)
    {
      opt = getopt_long (argc, argv, "df:i:z:hA:p:P:u:g:vC", longopts, 0);

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
	case 'i':
	  pid_file = optarg;
	  break;
	case 'z':
//	  zclient_serv_path_set (optarg);
	  break;
	case 'A':
	  break;
	case 'P':
	  /* Deal with atoi() returning 0 on failure, and isisd not
	     listening on isisd port... */
	  if (strcmp (optarg, "0") == 0)
	    {
	      vty_port = 0;
	      break;
	    }
	  vty_port = atoi (optarg);
	  vty_port = (vty_port ? vty_port : ISISD_VTY_PORT);
	  break;
	case 'u':
	  break;
	case 'g':
	  break;
	case 'v':
	  printf ("ISISd version %s\n", ISISD_VERSION);
	  printf ("Copyright (c) 2001-2002 Sampo Saaristo,"
		  " Ofer Wald and Hannes Gredler\n");
	  print_version ("Zebra");
            exit (0);
	  break;
	case 'C':
	  dryrun = 1;
	  break;
	case 'h':
	  usage (0);
	  break;
	default:
	  usage (1);
	  break;
	}
    }
	
  /* thread master */
  master = thread_master_create ();

  /* random seed from time */
  srandom (time (NULL));

  /*
   *  initializations
   */
    //recvSignal();
    signal_init(master,array_size(isis_signals),isis_signals);
  cmd_init (1);
  vty_init (master);
  memory_init ();

      if(-1 == mem_share_attach())
    {
        printf(" share memory attach fail\r\n");
        exit(0);
    }

    high_pre_timer_init(100, MODULE_ID_ISIS);
	  
  access_list_init();
  vrf_init ();
  prefix_list_init();
  isis_init ();
  isis_circuit_init ();
  isis_spf_cmds_init ();
  isis_redist_init ();
  isis_route_map_init();
  isis_l3vpn_event_register();

  /* create the global 'isis' instance */
  isis_new (1);

//  isis_zebra_init (master);
  isis_zebra_init(master);

  isis_msg_init();
  isis_pkt_init();
  /* parse config file */
  /* this is needed three times! because we have interfaces before the areas */
  //vty_read_config (config_file, config_default);
  /* Start execution only if not in dry-run mode */
  if (dryrun)
  {
        return(0);
  }
  /* demonize */
  if (daemon_mode)
    daemon (0, 0);

    /* Process ID file creation. */
    if (pid_file[0] != '\0')
    {
        pid_output (pid_file);
    }

  /* Make isis vty socket. */
  vty_serv_sock (ISIS_VTYSH_PATH);

    /*初始化向route 异步发送路由所用FIFO*/
    FIFO_INIT (&isis_m.routefifo);
	THREAD_TIMER_ON_NEW((char *)"ISISSendRouteTimer",isis_m.t_thread_routefifo,isis_asyc_send_route_cmd,NULL,1);

    //thread_add_timer_msec (master, isis_asyc_send_route_cmd, NULL, 1000);

	/*初始化接口事件FIFO*/
	FIFO_INIT (&isis_m.ifm_event_fifo);

    /*init fifo for receive route*/
    FIFO_INIT (&isis_m.routefifo_rcv);

    /* Print banner. */
    zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"Quagga-ISISd %s starting: vty@%d", QUAGGA_VERSION, vty_port);


	if(ipc_recv_init(master)< 0)
	{
		printf(" ipc receive init fail\r\n");
		exit(-1);
	}
	

    if(ipc_recv_thread_start("ISISIpcMsgRev", MODULE_ID_ISIS, SCHED_OTHER, -1, isis_msg_rcv_n, 0)< 0)
    {
        printf(" isis ipc msg receive thread start fail\r\n");
        exit(-1);
    }

    if(high_pre_timer_start() < 0)
    {
        printf(" timer start fail\r\n");
        exit(-1);
    }

    /* Start finite state machine. */
    while (thread_fetch (master, &thread))
    {
        thread_call (&thread);
    }

  /* Not reached. */
  exit (0);
}
