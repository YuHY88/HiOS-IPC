/*
*
*       main function for ces
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
#include <lib/msg_ipc.h>
#include <lib/thread.h>
#include <lib/pid_file.h>
#include <ifm/ifm.h>
#include <ces/ces_main.h>
#include <ces/ces_if.h>
#include <ces/ces_cmd.h>
#include <ces/ces_msg.h>
#include <ces/ces_alarm.h>
#include <sched.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
#include "sigevent.h"

/* 定义默认的配置文件 */
#define CES_DEFAULT_CONFIG   "ces.conf"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_CES_PID;  /* defined in config.h */

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR CES_DEFAULT_CONFIG;
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


/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

static RETSIGTYPE *
signal_set(int signo, void (*func)(int))
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
static void sighup(void)
{
    zlog_notice("SIGHUP received");

    /* Reload config file */
    //vty_read_config (config_file, config_default);

    /* Create VTY's socket */
    vty_serv_sock(CES_VTYSH_PATH);

    /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint(void)
{
    zlog_notice("SIGINT received\n");

    ces_die();

    exit(1);
}

/* SIGTERM handler. */
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    ces_die();

    exit(1);
    //...
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    zlog_notice("%s: ces received SIGPIPE signal!", __func__);
    signal(SIGPIPE, SIG_IGN);
}

/* SIGUSR1 handler. */
static void sigusr1(void)
{
    zlog_notice("SIGUSR1 received\n");

    return;
}

/* SIGUSR2 handler. */
static void sigusr2(void)
{
    zlog_notice("SIGUSR2 received\n");

    return;
}

#if 0
/* Initialization of signal handles. */
static void signal_init()
{
    signal_set(SIGHUP, sighup);
    signal_set(SIGINT, sigint);
    signal_set(SIGTERM, sigint);
    signal_set(SIGPIPE, SIG_IGN);
    signal_set(SIGUSR1, sigusr1);
    signal_set(SIGUSR2, sigusr2);
}
#endif

struct quagga_signal_t ces_signals[] =
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

static void ces_table_init()
{
    ces_if_table_init(1024);
    stm_if_table_init(256);
}

void ces_init(void)
{
    zlog_debug(CES_DEBUG_OTHER, "%s[%d]: Entering ces init\n", __FILE__, __LINE__);

    ces_table_init();
    ces_cmd_init();
    //ces_gpn_function_init();
    stm_cmd_init();
    stm_if_init();  //初始化创建stm接口

    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_CES, IFNET_IFTYPE_CES);        /* 注册接口 down 事件*/
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_CES, IFNET_IFTYPE_CES);          /* 注册接口 up 事件*/
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_CES, IFNET_IFTYPE_CES);   /* 注册接口删除事件*/
    ifm_event_register(IFNET_EVENT_IF_HIDE_RECOVER, MODULE_ID_CES, IFNET_IFTYPE_CES);   /* 注册接口hide 恢复事件*/
    ifm_event_register(IFNET_EVENT_IF_ADD, MODULE_ID_CES, IFNET_IFTYPE_CES);    /*注册接口添加事件，用于设备已上电，新插入stm板卡时执行stm_if_init*/

    //thread_add_event(ces_master, ces_msg_rcv, NULL, 0);
    //thread_add_event (ces_master, ces_alarm_socket_msg_rcv, NULL, 0);
    high_pre_timer_add("CesAlarmTimer", LIB_TIMER_TYPE_LOOP, ces_alarm_socket_msg_rcv, NULL, 1*1000);
    zlog_debug(CES_DEBUG_OTHER, "%s[%d]: Leaving ces init\n", __FILE__, __LINE__);

    return;
}

void ces_die(void)
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
    zlog_default = openzlog(progname, ZLOG_CES, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON); /* defined in log.h */

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
    ces_master = thread_master_create();

    /* Library initialization. */
    //signal_init();
    signal_init(ces_master, array_size(ces_signals), ces_signals);
    cmd_init(1);
    vty_init(ces_master);
    memory_init();

    if(mem_share_attach() == -1)
    {
        printf(" share memory attach fail\r\n");
        exit(0);
    }

    high_pre_timer_init(10, MODULE_ID_CES);

    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }

    pid_output(pid_file);  /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(CES_VTYSH_PATH);

    ces_init();

    if(ipc_recv_init(ces_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

    if(ipc_recv_thread_start("CesIpcMsgRev", MODULE_ID_CES, SCHED_OTHER, -1, ces_msg_rcv_msg, 0) == -1)
    {
        printf(" ces ipc msg receive thread start fail\r\n");
        exit(0);
    }

    if(high_pre_timer_start() == -1)
    {
        printf(" timer start fail\r\n");
        exit(0);
    }

    /* Execute each thread. */
    while (thread_fetch(ces_master, &thread))
    {
        thread_call(&thread);
    }

    ces_die();

    exit(0);
}



