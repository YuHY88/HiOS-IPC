/*       main function for ifm
*
*
*/

#include <zebra.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <lib/pid_file.h>
#include "lib/sigevent.h"
#include <lib/msg_ipc_n.h>
#include "lib/hptimer.h"
#include "lib/memshare.h"

#include "config.h" //

#include "version.h"
#include "getopt.h"
#include "thread.h"
#include "command.h"
#include "memory.h"
#include "keychain.h"
#include "vty.h"
#include "syslog.h"
#include "log.h"
#include "ifm_cli.h"
#include "ifm_message.h"
#include "ifm_init.h"
#include "ifm_register.h"
#include "ifm_qos.h"
#include "ifm_alarm.h"

#define IFM_DEFAULT_CONFIG   "ifm.conf"//"config.startup"
//#define PATH_IFM_PID "/var/run/ifmd.pid"
#define IFM_VTY_PORT                    2672
/* Process ID saved for use by init system */
const char *pid_file = PATH_IFM_PID;  /* defined in config.h */

/*  VTY connection port. */
int vty_port = IFM_VTY_PORT;    /* defined in vty.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = IFM_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR IFM_DEFAULT_CONFIG;
char *config_file = NULL;


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


extern void  vty_serv_un_sess ( char * );

/* Master of threads. */
struct thread_master *ifm_master = NULL;

void  ifm_init ( void ); /* init all of ifm */
void  ifm_die ( void ); /* free all of ifm */



/* Help information display. */
static void usage ( char *progname, int status )
{
    if ( status != 0 )
    { fprintf ( stderr, "Try `%s --help' for more information.\n", progname ); }
    else
    {
        printf ( "Usage : %s [OPTION...]\n\
Daemon which manages VRRP version 1 and 2.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-r, --retain       When program terminates, retain added route by vrrp.\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS );
    }

    exit ( status );
}

/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

static RETSIGTYPE *
signal_set ( int signo, void ( *func ) ( int ) )
{
    int ret;
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset ( &sig.sa_mask );
    sig.sa_flags = 0;
#ifdef SA_RESTART
    sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

    ret = sigaction ( signo, &sig, &osig );

    if ( ret < 0 )
    { return ( SIG_ERR ); }
    else
    { return ( osig.sa_handler ); }
}

#if 0
/* SIGHUP handler. */
static void sighup ( int sig )
{
    zlog_notice ( "SIGHUP received" );

    /* Reload config file */
    //vty_read_config ( config_file, config_default );

    /* Create VTY's socket */
    vty_serv_sock ( IFM_VTYSH_PATH );

    /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint ( int sig )
{
    zlog_notice ( "SIGINT received\n" );

    ifm_die();

    exit ( 1 );
}

/* SIGUSR1 handler. */
static void sigusr1 ( int sig )
{
    zlog_notice ( "SIGUSR1 received\n" );

    return;
}

/* SIGUSR2 handler. */
static void sigusr2 ( int sig )
{
    zlog_notice ( "SIGUSR2 received\n" );

    return;
}

/* Initialization of signal handles. */
static void _signal_init ()
{
    signal_set ( SIGHUP, sighup );
    signal_set ( SIGINT, sigint );
    signal_set ( SIGTERM, sigint );
    signal_set ( SIGPIPE, SIG_IGN );
    signal_set ( SIGUSR1, sigusr1 );
    signal_set ( SIGUSR2, sigusr2 );
}
#endif
/* SIGHUP handler. */
static void sighup ( void )
{
    zlog_notice ( "SIGHUP received" );

    /* Reload config file */
    //vty_read_config ( config_file, config_default );

    /* Create VTY's socket */
    vty_serv_sock ( IFM_VTYSH_PATH );

    /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint ( void )
{
    zlog_notice ( "SIGINT received\n" );

    ifm_die();

    exit ( 1 );
}

/* SIGUSR1 handler. */
static void sigusr1 ( void )
{
    zlog_notice ( "SIGUSR1 received\n" );

    return;
}

/* SIGUSR2 handler. */
static void sigusr2 ( void )
{
    zlog_notice ( "SIGUSR2 received\n" );

    return;
}
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    exit(0);
}
static void sigpipe(void)
{
	zlog_notice("Terminating on signal SIGPIPE");

    exit(0);
}

struct quagga_signal_t  ifm_signals[] =
{
	{
		.signal    = SIGHUP,
		.handler   = &sighup,
		.caught    = 0,
	},
		
	{
		.signal    = SIGINT,
		.handler   = &sigint,
		.caught    = 0,
	},

	{
		.signal    = SIGTERM,
		.handler   = &sigterm,
		.caught    = 0,
	},
	
	{
		.signal   = SIGPIPE,
		.handler  = &sigpipe,
		.caught   = 0,
	},
	
	{
		.signal   = SIGUSR1,
		.handler  = &sigusr1,
		.caught   = 0,
	},
	
	{
		.signal   = SIGUSR2,
		.handler  = &sigusr2,
		.caught   = 0,
	},
	
	{
		.signal    = SIGALRM,
		.handler   = &high_pre_timer_dotick,
		.caught    = 0,
	},
	
};

#if 0
static ifm_ipc_init(void)
{
	if( mem_share_attach() == -1 )/* 非第一个启动进程需调用attach 绑定共享内存*/
	{
		printf(" share memory init fail\r\n");
		exit(0);
	}
	/* 定时器初始化，时间间隔单位ms */
	high_pre_timer_init( 1000, MODULE_ID_IFM );
	
	/* 消息接收初始化 */
	if( ipc_recv_init( ifm_master) == -1 )
	{
		printf(" ifm module ipc receive init fail\r\n ");
		exit(0);
	}
	/* 启动接收线程 */
	if(ipc_recv_thread_start( "IfmIpcMsgRev", MODULE_ID_IFM, SCHED_OTHER , -1 , ifm_msg_rcv_handle_n, 0 ) == -1 )
	{
		printf(" ifm ipc msg receive thread start fail\r\n");
		exit(0);
	}
	if(ipc_recv_thread_start( "IfmIpcAlarmMsgRev", MODULE_ID_IFM, SCHED_OTHER, -1, ifm_alarm_socket_msg_rcv, 0) == -1 )
	{
		printf(" ifm ipc msg receive thread start fail\r\n");
		exit(0);
	}
	/* 启动基准定时器 */
	if(high_pre_timer_start() == -1 )
	{
		printf(" timer start fail\r\n");
		exit(0);
	}
}
#endif
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

    CPU_ZERO ( &mask );
    CPU_SET ( 1, &mask ); /* ?? cpu1 */
    sched_setaffinity ( 0, sizeof ( mask ), &mask );
    param.sched_priority = 70;
#ifndef HAVE_KERNEL_3_0		
    if ( sched_setscheduler ( 0, SCHED_RR, &param ) )
    {
        perror ( "\n  priority set: " );
    }
#endif
    /* First of all we need logging init. */
    zlog_default = openzlog ( progname, ZLOG_IFM, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON ); /* ZLOG_IFM defined in log.h */

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
            case 'P':
                vty_port = atoi ( optarg );
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
    ifm_master = thread_master_create ();

    /* Library initialization. */
    //_signal_init ();
    signal_init( ifm_master, array_size(ifm_signals), ifm_signals);
    cmd_init ( 1 );
    vty_init ( ifm_master );
    memory_init();
//ifm_ipc_init();
	if( mem_share_attach() == -1 )/* éžç¬¬ä¸?ä¸ªå¯åŠ¨è¿›ç¨‹éœ€è°ƒç”¨attach ç»‘å®šå…±äº«å†…å­˜*/
	{
		printf(" share memory init fail\r\n");
		exit(0);
	}
	/* å®šæ—¶å™¨åˆå§‹åŒ–ï¼Œæ—¶é—´é—´éš”å•ä½ms */
	high_pre_timer_init( 1000, MODULE_ID_IFM );
	

    /* Change to the daemon program. */
    if ( daemon_mode )
    { daemon ( 0, 0 ); }
    pid_output ( pid_file ); /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock ( IFM_VTYSH_PATH );

    ifm_init();

	/* æ¶ˆæ¯æŽ¥æ”¶åˆå§‹åŒ? */
	if( ipc_recv_init( ifm_master) == -1 )
	{
		printf(" ifm module ipc receive init fail\r\n ");
		exit(0);
	}
	/* å¯åŠ¨æŽ¥æ”¶çº¿ç¨‹ */
	if(ipc_recv_thread_start( "IfmIpcMsgRev", MODULE_ID_IFM, SCHED_OTHER , -1 , ifm_msg_rcv_handle_n, 0 ) == -1 )
	{
		printf(" ifm ipc msg receive thread start fail\r\n");
		exit(0);
	}

	/* å¯åŠ¨åŸºå‡†å®šæ—¶å™? */
	if(high_pre_timer_start() == -1 )
	{
		printf(" timer start fail\r\n");
		exit(0);
	}

    /* Execute each thread. */
    while ( thread_fetch ( ifm_master, &thread ) )
    { thread_call ( &thread ); }

    ifm_die();

    exit ( 0 );
}


void ifm_die()
{
    exit ( 0 );
}

#if 0
/* æ”¶æ¶ˆæ¯çº¿ç¨? */
static void ifm_rmsg_pthread()
{
    pthread_t msg_thread;

    pthread_mutex_init ( &ifm_rmsg_lock, NULL );
    pthread_create ( &msg_thread, NULL, ( void * ) ifm_msg_rcv_pthread, NULL );
    pthread_detach ( msg_thread );
}

/* 发消息线程 */
static void ifm_smsg_pthread()
{
    pthread_t msg_thread;

    pthread_mutex_init ( &ifm_smsg_comm_lock, NULL );
    pthread_mutex_init ( &ifm_smsg_ftm_lock, NULL );
    pthread_mutex_init ( &ifm_smsg_hal_lock, NULL );
    pthread_create ( &msg_thread, NULL, ( void * ) ifm_msg_send_pthread, NULL );
    pthread_detach ( msg_thread );
}


static void ifm_thread_init ( void )
{
    //thread_add_event ( ifm_master, ifm_msg_rcv_handle, NULL, 0 ); /* thread to receive control msg from app */
	//thread_add_event( ifm_master, ifm_alarm_socket_msg_rcv , NULL, 0 );	
}

#endif
void ifm_init ( void )
{
    ifm_event_list_init();
    ifm_table_init ( IFM_NUM_MAX );
    //ifm_rmsg_pthread();
    //ifm_smsg_pthread();
    //ifm_thread_init();
    ifm_interface_init();
    ifm_cli_init();
    ifm_qos_init();
	//ifm_gpn_function_init();
}

