/**********************************************************
* file name: sdhmgt_main.c
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan chengquan 2018-01-15
* function:
*
* modify:
*
***********************************************************/
#include <lib/zebra.h>
#include <sys/sysinfo.h>
#include <sys/msg.h>
#include <lib/pid_file.h>
#include <lib/vty.h>
#include <lib/msg_ipc.h>
#include <lib/version.h>
#include <lib/getopt.h>
#include <lib/thread.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/keychain.h>
#include <lib/vty.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>

#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"
#include "lib/sigevent.h"

#include "sdhmgt_msg.h"
#include "sdhmgt_socket.h"
#include "sdhmgt.h"
#include "sdhmgt_api.h"
#include "sdhmgt_cmd.h"
#include "sdhmgt_main.h"

/* vty socket */
//#define SDHMGT_VTYSH_PATH       "/var/run/sdhmgt.vty"

/* Process ID saved for use by init system */
const char  *pid_file = PATH_SDHMGT_PID;  /* defined in config.h */

/* VTY bind address. */
char        *vty_addr = NULL;

//void sdhmgt_die(void);
void sdhmgt_socket_recv_pthread(void);
//void _signal_init(void);
void sdhmgt_init(void);


/* Master of threads. */
extern struct thread_master *g_psdhmgt_master;


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

    exit(status);
}


#if 0
/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

static RETSIGTYPE *signal_set(int signo, void (*func)(int))
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
static void sighup(int sig)
{
    zlog_notice("SIGHUP received");

    /* Create VTY's socket */
    vty_serv_sock(SDHMGT_VTYSH_PATH);
}


/* SIGINT handler. */
static void sigint(int sig)
{
    zlog_notice("SIGINT received\n");

    sdhmgt_die();

    exit(1);
}

/* SIGUSR1 handler. */
static void sigusr1(int sig)
{
    zlog_notice("SIGUSR1 received\n");
}

/* SIGUSR2 handler. */
static void sigusr2(int sig)
{
    zlog_notice("SIGUSR2 received\n");
}

/* Initialization of signal handles. */
void _signal_init(void)
{
    signal_set(SIGHUP, sighup);
    signal_set(SIGINT, sigint);
    signal_set(SIGTERM, sigint);
    signal_set(SIGPIPE, SIG_IGN);
    signal_set(SIGUSR1, sigusr1);
    signal_set(SIGUSR2, sigusr2);
}
#endif

/*---------------------------------- 修改共享内存时修改, lipf, 2018/8/23 --------------------*/
static void __attribute__ ((noreturn)) sdhmgt_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(g_psdhmgt_master) 
    {
        thread_master_free (g_psdhmgt_master);
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

    sdhmgt_exit(0);
}



struct quagga_signal_t sdhmgt_signals[] =
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



#if 0
void sdhmgt_die(void)
{
    if (g_psdhmgt_master)
    {
        thread_master_free(g_psdhmgt_master);
    }

    if (zlog_default)
    {
        closezlog(zlog_default);
    }

    exit(0);
}
#endif

void sdhmgt_socket_recv_pthread(void)
{
    pthread_t socket_recv_thread;

    pthread_create(&socket_recv_thread, NULL, (void *) sdhmgt_socket_select_loop, NULL);
    pthread_detach(socket_recv_thread);
}

void sdhmgt_init(void)
{
    memset(&g_sdhmgt_entry, 0x00, sizeof(g_sdhmgt_entry));
    sdhmgt_get_system_device_id();    

    if (0 == g_sdhmgt_entry.h9molmxe_vx)
    {
        sdhmgt_exit(0);

        return;
    }


	mem_share_attach();	/* 非第一个启动进程需调用attach */

	/* 定时器初始化，时间间隔单位1000ms，满足最小定时器需求即可，该时间间隔越大越好 */
    high_pre_timer_init(1000, MODULE_ID_SDHMGT);

	/* 消息接收初始化 */
	if(ipc_recv_init(g_psdhmgt_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	/* 启动接收线程 */
    if(ipc_recv_thread_start((char *)"SdhMgtIpcRev", MODULE_ID_SDHMGT, SCHED_OTHER, -1, sdhmgt_msg_rcv_n, 0) == -1)
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

	sdhmgt_ipc_init();

    sdhmgt_info_init();

    sdhmgt_cmd_init();

    /*New thread listening socket.*/
    sdhmgt_socket_recv_pthread();	
}



/* Main routine */
int main(int argc, char **argv)
{
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
    CPU_SET(0, &mask);   /* cpu0 */
    sched_setaffinity(0, sizeof(mask), &mask);
    param.sched_priority = 80;

#ifndef HAVE_KERNEL_3_0

    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }

#endif

    /* First of all we need logging init. */
    zlog_default = openzlog(progname, ZLOG_SDHMGT, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);    /* ZLOG_DXCD defined in log.h */

    /* Command line option parse. */
    while (1)
    {
        int opt;

        opt = getopt_long(argc, argv, "df:hA:rv", longopts, 0);

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
    g_psdhmgt_master = thread_master_create();

	/* 初始化信号 */
	signal_init(g_psdhmgt_master, array_size(sdhmgt_signals), sdhmgt_signals);

    /* Library initialization. */
    //_signal_init();
    cmd_init(1);
    vty_init(g_psdhmgt_master);
    memory_init();

    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }

    pid_output(pid_file);    /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(SDHMGT_VTYSH_PATH);

    sdhmgt_init();

    /* Execute each thread. */
    while (thread_fetch(g_psdhmgt_master, &thread))
    {
        thread_call(&thread);
    }

    sdhmgt_exit(0);
}


