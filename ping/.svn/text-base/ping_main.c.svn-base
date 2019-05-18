/*       
*      main function for device management
*
*/

#include <zebra.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <lib/pid_file.h>
#include <lib/vty.h>
#include <lib/version.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/thread.h>
#include <lib/log.h>

#include "ping.h"
#include "ping6.h"
#include "ping_cmd.h"

#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
#include "sigevent.h"



#define PING_DEFAULT_CONFIG   "ping.conf"
#define PING_VTY_PORT         2678

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_PING_PID;  /* defined in config.h */

/*  VTY connection port. */
int vty_port = PING_VTY_PORT;    /* defined in vty.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = PING_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR PING_DEFAULT_CONFIG;
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
void  ping_init ( void ); /* init all of ping */
void  ping_die ( void );  /* free all of ping */



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
    { 
        return ( SIG_ERR );
    }
    else
    {
        return ( osig.sa_handler );
    }
}


/* SIGHUP handler. */
static void sighup ( void )
{
    zlog_notice ( "SIGHUP received" );

    /* Reload config file */
    //vty_read_config ( config_file, config_default );

    /* Create VTY's socket */
    vty_serv_sock (PING_VTYSH_PATH );

    /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint ( void  )
{
	zlog_notice ( "SIGINT received\n" );
	return;
}

/* SIGUSR1 handler. */
static void sigusr1 ( void  )
{
    zlog_notice ( "SIGUSR1 received\n" );

    return;
}

/* SIGUSR2 handler. */
static void sigusr2 ( void  )
{
    zlog_notice ( "SIGUSR2 received\n" );

    return;
}

/* SIGTERM handler. */
static void sigterm(void)
{
	zlog_notice("Terminating on signal SIGTERM");

	ping_die();

	exit(1);
	//...
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
	zlog_notice("%s: ftm received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}


/* Initialization of signal handles. 
static void _signal_init ()
{
    signal_set ( SIGHUP, sighup );
    signal_set ( SIGINT, sigint );
    signal_set ( SIGTERM, sigint );
    signal_set ( SIGPIPE, SIG_IGN );
    signal_set ( SIGUSR1, sigusr1 );
    signal_set ( SIGUSR2, sigusr2 );
}*/

/*add for new ipc by renxy*/
struct quagga_signal_t ping_signals[] =
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


/* Main routine */
int main ( int argc, char **argv )
{
    char *p;
    int daemon_mode = 0;
    char *progname;
    struct thread thread;

    /* Set umask before anything for security */
    umask ( 0027 );

    /* Get program name. */
    progname = ( ( p = strrchr ( argv[0], '/' ) ) ? ++p : argv[0] );

	
	cpu_set_t mask; 
	
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* °ó¶¨cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);
	
	
#ifndef HAVE_KERNEL_3_0	
	struct sched_param param;
	param.sched_priority = 50;

	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif
    /* First of all we need logging init. */
    zlog_default = openzlog ( progname, ZLOG_PING, LOG_NDELAY,  LOG_DAEMON); /* ZLOG_ping defined in log.h */

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
    ping_master = thread_master_create ();

    /* Library initialization. */
    //_signal_init ();
	signal_init(ping_master, array_size(ping_signals), ping_signals);
    cmd_init ( 1 );
    vty_init ( ping_master );
	memory_init();

	if(mem_share_attach() == -1)
	{
		printf(" share memory attach fail\r\n");
		exit(0);
	}

	high_pre_timer_init(1000, MODULE_ID_PING);

    /* Change to the daemon program. */
    if ( daemon_mode )
    { 
        daemon ( 0, 0 );
    }
    pid_output ( pid_file ); /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock (PING_VTYSH_PATH );

    ping_init();


	if(ipc_recv_init(ping_master) == -1)
	{
		printf(" ipc receive init fail\r\n");
		exit(0);
	}

	if(ipc_recv_thread_start("PingIpcMsgRev", MODULE_ID_PING, SCHED_OTHER, -1, icmp_ping_msg_rcv_n, 0) == -1)
	{
		printf(" l2 ipc msg receive thread start fail\r\n");
		exit(0);
	}  
	  
	if(high_pre_timer_start() == -1)
	{
		printf(" timer start fail\r\n");
		exit(0);
	}

    /* Execute each thread. */
    while ( thread_fetch ( ping_master, &thread ) )
    {thread_call ( &thread ); }
		
    ping_die();

    exit ( 0 );
}


void ping_die()
{
    exit ( 0 );
}
#if 0
static void ping_thread_init ( void )
{
    thread_add_event (ping_master, icmp_ping_msg_rcv_n, NULL, 0 ); /* thread to receive control msg from app */
}
#endif
void ping_init ( void )
{
	ping_CmdInit();
    icmp_ping_init();
    icmpv6_ping_init();
    //ping_thread_init();
}







