/******************************************************************************
 * Filename: aaa_main.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: main functions for aaa.
 *
 * History:
 * 2016.9.13  luoyz created
 *
******************************************************************************/

#include <zebra.h>
#include <sys/sysinfo.h>
#include <sched.h>

#include "version.h"
#include "getopt.h"
#include "thread.h"
#include "command.h"
#include "memory.h"
#include "keychain.h"
#include "vty.h"
#include "syslog.h"
#include "log.h"
#include "lib/hash1.h"
#include "lib/pid_file.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"
#include "lib/sigevent.h"

#include "aaa_config.h"
#include "aaa_msg_io.h"
#include "aaa_cmd.h"
#include "aaa_packet.h"
#include "aaa_radius.h"
#include "aaa_tac_plus.h"

#define AAA_DEFAULT_CONFIG   "aaa.conf"
#define PATH_AAA_PID "/var/run/aaa.pid"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_AAA_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = AAA_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR AAA_DEFAULT_CONFIG;
char *config_file = NULL;


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


extern void  vty_serv_un_sess ( char * );

void aaa_init(void);

struct thread_master *aaa_master = NULL;


/* Help information display. */
static void usage ( char *progname, int status )
{
    if ( status != 0 )
    { fprintf ( stderr, "Try `%s --help' for more information.\n", progname ); }
    else
    {
        printf ( "Usage : %s [OPTION...]\n\
Daemon which manages AAA authentication.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-A, --vty_addr     Set vty's bind address\n\
-r, --retain       When program terminates, retain added route by vrrp.\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS );
    }

    exit ( status );
}




/*---------------------------------- 修改共享内存时修改, lipf, 2018/8/23 --------------------*/
static void __attribute__ ((noreturn)) aaa_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(aaa_master) 
    {
        thread_master_free (aaa_master);
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

    aaa_exit(0);
}



struct quagga_signal_t aaa_signals[] =
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



/* Main routine */
int main ( int argc, char **argv )
{
    /*  VTY connection port. */
    char *p;
    int daemon_mode = 0;
    char *progname;
    struct thread thread;

    /* Set umask before anything for security */
    umask ( 0027 );

    /* Get program name. */
    progname = ( ( p = strrchr ( argv[0], '/' ) ) ? ++p : argv[0] );

	struct sched_param param;
	
	cpu_set_t mask; 
		
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 绑定cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);

	param.sched_priority = 30;
#ifndef HAVE_KERNEL_3_0	
	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif

    /* First of all we need logging init. */
	zlog_default = openzlog(progname, ZLOG_AAA,
		LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

    /* Command line option parse. */
    while ( 1 )
    {
        int opt;

        opt = getopt_long ( argc, argv, "df:hA:P:rv", longopts, 0 );

        if ( opt == EOF )
        { break; }

        switch ( opt )
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
                print_version ( progname );
                exit ( 0 );
                break;
            case 'h':
                usage ( progname, 0 );
                break;
            default:
                usage ( progname, 1 );
                break;
        }
    }

    /* Prepare master thread. */
    aaa_master = thread_master_create ();

	/* 初始化信号 */
	signal_init(aaa_master, array_size(aaa_signals), aaa_signals);
	

    /* Library initialization. */
    //_signal_init ();
    cmd_init ( 1 );
    vty_init ( aaa_master );
	memory_init();

    /* Change to the daemon program. */
    if ( daemon_mode )
    { daemon ( 0, 0 ); }
    pid_output ( pid_file ); /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock ( AAA_VTYSH_PATH );
	
	aaa_init();

    /* Execute each thread. */
    while ( thread_fetch ( aaa_master, &thread ) )
    { thread_call ( &thread ); }

    aaa_exit(0);

    exit ( 0 );
}

#if 0
static void aaa_thread_init ( void )
{
	/* thread to receive control msg from app */
    thread_add_event ( aaa_master, aaa_msg_rcv, NULL, 0 );
	thread_add_event ( aaa_master, aaa_pkt_rcv, NULL, 0 );
}
#endif

void aaa_init ( void )
{
	mem_share_attach();	/* 非第一个启动进程需调用attach */

	/* 定时器初始化，时间间隔单位200ms，满足最小定时器需求即可，该时间间隔越大越好 */
    high_pre_timer_init(200, MODULE_ID_AAA);

	/* 消息接收初始化 */
	if(ipc_recv_init(aaa_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	/* 启动接收线程 */
    if(ipc_recv_thread_start((char *)"AAAIpcRev", MODULE_ID_AAA, SCHED_OTHER, -1, aaa_msg_rcv_n, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }

	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }	
	
	aaa_cfgs_init();
	//ipc_connect_common();
    //aaa_thread_init();
    aaa_cmd_init();
}




