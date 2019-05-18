/*
*
*       main function for clock
*/

#include "../config.h"
#include <zebra.h>
#include <sys/sysinfo.h>
#include <lib/vrf.h>
#include <lib/privs.h>
#include <lib/version.h>
#include <lib/getopt.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/keychain.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <lib/prefix.h>
#include <lib/msg_ipc_n.h>
#include <lib/thread.h>
#include <lib/pid_file.h>
#include "lib/pkt_buffer.h"
#include "lib/sigevent.h"
#include "lib/hptimer.h"
#include <ifm/ifm.h>
#include <sched.h>
#include <clock_main.h>
#include <clock_msg.h>
#include <clock_src.h>
#include <clock_if.h>
#include <clock_alarm.h>
#include <clock_cmd.h>
#include <synce/synce.h>

/* 定义默认的配置文件 */
#define CLOCK_DEFAULT_CONFIG   "clock.conf"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_CLOCK_PID;  /* defined in config.h */

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR CLOCK_DEFAULT_CONFIG;
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
-P, --vty_port     Set vty's port number\n\
-r, --retain       When program terminates, retain added route by vrrp.\n\
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

    exit(0);
}
struct quagga_signal_t clock_signals[] =
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

static void clock_ipc_init(void)
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_CLOCK, IFNET_IFTYPE_L2IF);        /* 注册接口 down 事件*/
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_CLOCK, IFNET_IFTYPE_L2IF);          /* 注册接口 up 事件*/

    return;
}

static void clock_thread_init(void)
{
    high_pre_timer_add("ClockMainTimer", LIB_TIMER_TYPE_LOOP, synce_timer_start, NULL, 1000);
    return;
}

void clock_init(void)
{
    zlog_debug(CLOCK_DBG_INFO,"%s[%d]: Entering clock init\n", __FILE__, __LINE__);

    clock_ipc_init();
    clock_thread_init();
    clock_src_init();

    synce_init();
    clockif_init();
    clock_gpn_function_init();
    zlog_debug(CLOCK_DBG_INFO,"%s[%d]: Leaving clock init\n", __FILE__, __LINE__);

    return;
}

void clock_die(void)
{
    exit(0);
}


/* Ces routine */
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

    

    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(1, &mask);   /* 绑定cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);

    
#ifndef HAVE_KERNEL_3_0	
    struct sched_param param;
    param.sched_priority = 40;
    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }
#endif
    /* First of all we need logging init. */
    zlog_default = openzlog(progname, ZLOG_CLOCK, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON); /* defined in log.h */

    /* Command line option parse. */
    while (1)
    {
        int opt;

        opt = getopt_long(argc, argv, "df:hA:P:rv", longopts, 0);

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
                break;

            case 'i':
                pid_file = optarg;
                break;

            case 'P':
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
    clock_master = thread_master_create();
    
    signal_init(clock_master, array_size(clock_signals), clock_signals);

    /* Library initialization. */
    //signal_init();
    cmd_init(1);
    vty_init(clock_master);
    memory_init();

    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }

    pid_output(pid_file);  /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(CLOCK_VTYSH_PATH);
    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    
    high_pre_timer_init(500, MODULE_ID_CLOCK);

    clock_init();
    
    if(ipc_recv_init(clock_master) == -1)
    {
        printf(" clock module ipc receive init fail\r\n");
        exit(0);
    }
    
    if(ipc_recv_thread_start("ClockIpcMsgRev", MODULE_ID_CLOCK, SCHED_OTHER, -1, clock_msg_rcv_msg_n, 0) == -1)
    {
        printf(" clock ipc msg receive thread start fail\r\n");
        exit(0);
    }

    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }
    


    /* Execute each thread. */
    while (thread_fetch(clock_master, &thread))
    {
        thread_call(&thread);
    }

    clock_die();

    exit(0);
}

