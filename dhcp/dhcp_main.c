/**@file dhcp_main.c
   @brief  This file contains the init part for dhcp module.
*/

/* Copyright (C) 2016 Huahuan, Inc. All Rights Reserved. */

#define _GNU_SOURCE
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sched.h>

#include <lib/version.h>
#include <lib/thread.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/keychain.h>
#include <lib/vty.h>
#include <lib/msg_ipc.h>
#include <lib/ifm_common.h>
#include <lib/hash1.h>
#include <lib/syslog.h>
#include <lib/log.h>
//#include "../config.h"
#include <lib/vty.h>

//#include <zebra.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
#include "sigevent.h"

#include "getopt.h"
#include "dhcp.h"
#include "dhcp_api.h"
#include "dhcp_msg.h"
#include "pool.h"
#include "dhcp_if.h"
#include "pool_cli.h"
#include "dhcp_relay.h"
#include "files.h"
#include "pool_address.h"
#include "pool_static_address.h"

#define ZEBRA_BUG_ADDRESS "https://bugzilla.quagga.net"

void dhcp_init(void);
void dhcp_die(void);

/* 定义默认的配置文件 */
#define DHCP_DEFAULT_CONFIG   "dhcp.conf"
/* dhcp PID */
#define PATH_DHCP_PID "/var/run/dhcp.pid"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_DHCP_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;

struct thread_master *dhcp_master;
int dhcp_ipc_id = 0;

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR DHCP_DEFAULT_CONFIG;
char *config_file = NULL;
pthread_mutex_t dhcp_lock;

extern pid_t pid_output (const char *path);
/* common options. */
static struct option longopts[] =
{
  { "daemon",      no_argument,       NULL, 'd'},
  { "config_file", required_argument, NULL, 'f'},
  { "pid_file",    required_argument, NULL, 'i'},
  { "help",        no_argument,       NULL, 'h'},
  { "vty_addr",    required_argument, NULL, 'A'},
  { "vty_port",    required_argument, NULL, 'P'},
  { "retain",      no_argument,       NULL, 'r'},
  { "version",     no_argument,       NULL, 'v'},
  { 0 }
};

/* Help information display. */
static void usage (char *progname, int status)
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", progname);
  else
    {
      printf ("Usage : %s [OPTION...]\n\
Daemon which manages VRRP version 1 and 2.\n\n\
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

  exit (status);
}

/* SIGHUP handler. */
static void sighup (void)
{
  DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "SIGHUP received");

  /* Reload config file */
 // vty_read_config (config_file, config_default);

  /* Create VTY's socket */
  vty_serv_sock (MPLS_VTYSH_PATH);

  /* Try to return to normal operation. */
}

/* SIGINT handler. */
static void sigint (void)
{
    DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "SIGINT received\n");

    dhcp_die();

    exit(1);
}

/* SIGTERM handler. */
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    dhcp_die();
    //...
}

/* SIGUSR1 handler. */
static void sigusr1 (void)
{
    DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "SIGUSR1 received\n");

	return;
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    zlog_notice("%s: ipmc received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}

/* SIGUSR2 handler. */
static void sigusr2 (void)
{
    DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "SIGUSR2 received\n");

	return;
}
#if 0
/* Initialization of signal handles. */
static void signal_init ()
{
  signal_set (SIGHUP, sighup);
  signal_set (SIGINT, sigint);
  signal_set (SIGTERM, sigint);
  signal_set (SIGPIPE, SIG_IGN);
  signal_set (SIGUSR1, sigusr1);
  signal_set (SIGUSR2, sigusr2);
}
#endif

dhcp_conf gDhcpSysConf =
{
     DHCP_CONF_DEF_DHCP,

     DHCP_CONF_DEF_INBANDVID,
 
     DHCP_CONF_DEF_INBANDPRI,

     DHCP_CONF_DEF_INBAND_IPV4_ADDR,
     
     DHCP_CONF_DEF_INBAND_IPV4_MASK,
     
     DHCP_CONF_DEF_INBAND_IPV4_ADDR,
    
    {DHCP_CONF_DEF_INBAND_IPV6_ADDR},
    
    {DHCP_CONF_DEF_INBAND_IPV6_MASK},
    
    {DHCP_CONF_DEF_INBAND_IPV6_ADDR},
    
    0
};
	 
struct quagga_signal_t dhcp_signals[] =
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
	 .signal  = SIGPIPE,
	 .handler = &sigpipe,
	 .caught  = 0,
	},
	{
	 .signal  = SIGUSR1,
	 .handler = &sigusr1,
	 .caught  = 0,
	},
	{
	 .signal  = SIGUSR2,
	 .handler = &sigusr2,
	 .caught  = 0,
	},
	{
	 .signal  = SIGALRM,
	 .handler = &high_pre_timer_dotick,
	 .caught  = 0,
	},
};

dhcp_globals         dhcpcm;

int main (int argc, char **argv)
{
	char *p;
	int daemon_mode = 0;
	char *progname;
	struct thread thread;

	/* Set umask before anything for security */
	umask (0027);

	/* Get program name. */
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);


	cpu_set_t mask; 
		
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 绑定cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);
#ifndef HAVE_KERNEL_3_0	
	struct sched_param param;

	param.sched_priority = 30;

	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif	
	/* First of all we need logging init. */
	zlog_default = openzlog (progname, ZLOG_DHCP, LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON); /* defined in log.h */

	/* Command line option parse. */    
	while (1)
	{
		int opt;

		opt = getopt_long (argc, argv, "df:hA:P:rv", longopts, 0);

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
			case 'v':
				print_version (progname);
				exit (0);
				break;
			case 'h':
				usage (progname, 0);
				break;
			default:
				usage (progname, 1);
				break;
	   }
	}

	/* Prepare master thread. */
	dhcp_master = thread_master_create ();

	signal_init(dhcp_master, array_size(dhcp_signals), dhcp_signals);
	cmd_init (1);	
	vty_init (dhcp_master);
	memory_init();
	
	/* 非第一个启动进程需调用attach */
	if(mem_share_attach() == -1)
	{
	   printf(" share memory attach fail\r\n");
	   exit(0);
	}
	  
	/* 定时器初始化，时间间隔单位1000ms，满足最小定时器需求即可，该时间间隔越大越好 */
	high_pre_timer_init(1000, MODULE_ID_DHCP);
	
	/* Change to the daemon program. */
	if (daemon_mode)
		daemon (0, 0);  
	pid_output (pid_file); /* Pid file create. */

	/* Create VTY's socket */
	vty_serv_sock (DHCP_VTYSH_PATH);  

	/* 消息接收初始化 */
	if(ipc_recv_init(dhcp_master) == -1)
	{
		printf(" ipc receive init fail\r\n");
		exit(0);
	}
	
	/* 启动接收线程 */
	if(ipc_recv_thread_start("DhcpIpcMsgRev", MODULE_ID_DHCP, SCHED_OTHER, -1, &dhcp_msg_rcv, 0) == -1)
	{
		printf(" dhcp ipc msg receive thread start fail\r\n");
		exit(0);
	}	
	
	/* 启动基准定时器 */
	if(high_pre_timer_start() == -1)
	{
		printf(" timer start fail\r\n");
		exit(0);
	}
	
	/* after mem_share_attach and high_pre_timer_init must */
	dhcp_init();	
	
	/* Execute each thread. */
	while(thread_fetch(dhcp_master, &thread))
	{    
	    thread_call(&thread);
	}

	dhcp_die();
}

void dhcp_init()
{
	dhcpcm.conf	  = &gDhcpSysConf;
	dhcpcm.ginfo = XCALLOC(MTYPE_TMP, sizeof (dhcp_ginfo));
    pthread_mutex_init(&dhcp_lock, NULL);
	dhcp_if_cmd_init();	
	
	dhcp_pool_cmd_init();
	dhcp_pool_table_init(HASHTAB_SIZE);  	
	//thread_add_timer(dhcp_master, dhcp_write_leases, NULL, 60); //  1   minutes
	//thread_add_timer(dhcp_master, relay_xid_del_timeout, NULL, 2); // 2 s
	high_pre_timer_add((char *)"DhcpWriteLease", LIB_TIMER_TYPE_LOOP, &dhcp_write_leases,   NULL, 60 * 1000);
	high_pre_timer_add((char *)"DhcpRelayXidDelTimeout", LIB_TIMER_TYPE_LOOP, &relay_xid_del_timeout,   NULL, 2 * 1000);

	dhcp_event_register();
	dhcp_pkt_register();

	dhcp_addr_table_init(HASHTAB_SIZE);
	dhcp_static_addr_table_init(HASHTAB_SIZE);
	dhcp_ip_address_save_init();
}

void dhcp_die()
{
	closezlog(zlog_default);
	exit(0);
}
