/*
*      main function for device management
*
*/

#include <lib/zebra.h>
#include <sys/sysinfo.h>
#include <lib/pid_file.h>
#include <lib/vty.h>
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include <lib/version.h>
#include <lib/getopt.h>
#include <lib/thread.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/keychain.h>
#include <lib/vty.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include <lib/alarm.h>
#include "lib/hptimer.h"
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include "devm.h"
#include "devm_timer.h"
#include <sched.h>
#include "lib/hptimer.h"
#include "lib/msg_ipc.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/sigevent.h"


#define DEVM_DEFAULT_CONFIG   "devm.conf"
#define ZEBRA_BUG_ADDRESS "https://bugzilla.quagga.net"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_DEVM_PID;  /* defined in config.h */



/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = DEVM_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR DEVM_DEFAULT_CONFIG;
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


extern void  vty_serv_un_sess(char *);

/* Master of threads. */
struct thread_master *devm_master = NULL;

void  devm_init(void);    /* init all of devm */
//void  devm_die(void);     /* free all of devm */



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


/*------------IPC optimization yuhy,20180905--------------*/
static void __attribute__ ((noreturn)) devm_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(devm_master) 
    {
        thread_master_free (devm_master);
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

    devm_exit(0);
}



struct quagga_signal_t devm_signals[] =
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

    /* Reload config file */
    //vty_read_config ( config_file, config_default );

    /* Create VTY's socket */
    vty_serv_sock(DEVM_VTYSH_PATH);

    /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint(int sig)
{
    zlog_notice("SIGINT received\n");

    devm_die();

    exit(1);
}

/* SIGUSR1 handler. */
static void sigusr1(int sig)
{
    zlog_notice("SIGUSR1 received\n");

    return;
}

/* SIGUSR2 handler. */
static void sigusr2(int sig)
{
    zlog_notice("SIGUSR2 received\n");

    return;
}

/* Initialization of signal handles. */
static void _signal_init()
{
    signal_set(SIGHUP, sighup);
    signal_set(SIGINT, sigint);
    signal_set(SIGTERM, sigint);
    signal_set(SIGPIPE, SIG_IGN);
    signal_set(SIGUSR1, sigusr1);
    signal_set(SIGUSR2, sigusr2);
}

#endif      /*delete for IPC optimization*/

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
    CPU_SET(0, &mask);   /* 绑定cpu0*/
    sched_setaffinity(0, sizeof(mask), &mask);
    param.sched_priority = 80;
#ifndef HAVE_KERNEL_3_0	
    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }
#endif
    /* First of all we need logging init. */
    zlog_default = openzlog(progname, ZLOG_DEVM, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);    /* ZLOG_DEVM defined in log.h */

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
                config_file = optarg;
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
    devm_master = thread_master_create();
    
    /*Init signal*/
    signal_init(devm_master, array_size(devm_signals), devm_signals);

    /* Library initialization. */
//    _signal_init();
    cmd_init(1);
    vty_init(devm_master);
    memory_init();

    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }

    pid_output(pid_file);    /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(DEVM_VTYSH_PATH);
    devm_init();
    
    /* Execute each thread. */
    while (thread_fetch(devm_master, &thread))
    {
        thread_call(&thread);
    }
    
    /* 进程退出 */
    zlog_warn("Thread failed");

    devm_exit(0);
    return(0);
}

#if 0
void devm_die()
{
    exit(0);
}

void devm_thread_init(void)
{
    thread_add_event(devm_master, devm_msg_rcv, NULL, 0);    /* thread to receive control msg from app */
}
#endif

static void devm_alarm_init()
{
    int i = 0;

    /* modify for ipran by lipf, 2018/4/25 */
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = 1;
    usp.slot = 0;
    usp.port = 0;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_ENV_TYPE;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
    ipran_alarm_port_register(&gPortInfo);

    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = 1;
    usp.port = 0;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_EQU_TYPE;

    for (i = 0; i < SLOT_NUM_MAX; i++)
    {
        usp.slot = i;
        gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
        ipran_alarm_port_register(&gPortInfo);
    }
    return;
}


void devm_init(void)
{
    
    if(mem_share_attach() == -1)
    {
        printf(" share memory init fail\r\n");
        exit(0);
    }

    high_pre_timer_init(1000, MODULE_ID_DEVM);
    /* 消息接收初始化 */
    if(ipc_recv_init(devm_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }
    
    devm_alarm_init();
    devm_init_dev();         /* 先初始化本机设备 */
//  ipc_init();              /* 初始化系统的 IPC */
    devm_event_list_init();
//    devm_thread_init();
    devm_cli_init();
    devm_init_mul_timer();
    devm_init_default_event_module();

    /* 启动接收线程 */
    if(ipc_recv_thread_start((char *)"DEVMIpcRev", MODULE_ID_DEVM, SCHED_OTHER, -1, devm_msg_rcv, 0) == -1)
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
	high_pre_timer_add("DevmGetSlaveTemperTimer", LIB_TIMER_TYPE_LOOP, temper_send_to_master, NULL, 3000);
//    devm_init_cmd_timer();
//    devm_pthread_timer_init_sec(1, devm_cmd_pthread_timer_func);  /*注册一个1s为单位的定时器， 用于定时执行命令与启动计时*/
}


