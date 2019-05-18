/******************************************************************************
 * Filename: system_main.c
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for system module main.
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/


#include <zebra.h>
#include <sys/sysinfo.h>
#include <lib/pid_file.h>
#include <lib/vty.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
#include "lib/sigevent.h"

#include "version.h"
#include "getopt.h"
#include "thread.h"
#include "command.h"
#include "memory.h"
#include "keychain.h"
#include "vty.h"
#include "syslog.h"
#include "log.h"
#include "system.h"
#include "system_cmd.h"
#include "system_usage.h"
#include "system_alarm.h"

#include <sched.h>

#define SYSTEM_DEFAULT_CONFIG   "system.conf"
#define PATH_SYSTEM_PID "/var/run/system.pid"

/* Process ID saved for use by init system */
const char *pid_file = PATH_SYSTEM_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = SYSTEM_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR SYSTEM_DEFAULT_CONFIG;
char *config_file = NULL;

sys_global_conf_t sys_conf;

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


extern void  vty_serv_un_sess(char *);

/* Master of threads. */
struct thread_master *system_master = NULL;

void  system_init(void);    /* init all of system */
void  system_die(void);     /* free all of system */



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

struct quagga_signal_t system_signals[] =
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
    zlog_default = openzlog(progname, ZLOG_SYSTEM, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);    /* ZLOG_ping defined in log.h */

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
                vty_addr = optarg;
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
    system_master = thread_master_create();

    /* Library initialization. */
    cmd_init(1);
    vty_init(system_master);
    memory_init();

    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }
    signal_init(system_master, array_size(system_signals), system_signals);
    pid_output(pid_file);  /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(SYSTEM_VTYSH_PATH);

    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    
    high_pre_timer_init(1000, MODULE_ID_SYSTEM);
    
    system_init();
    
    if(ipc_recv_init(system_master) == -1)
    {
        printf(" clock module ipc receive init fail\r\n");
        exit(0);
    }
    if(ipc_recv_thread_start("SystemIpcMsgRev", MODULE_ID_SYSTEM, SCHED_OTHER, -1, system_msg_rcv_msg, 0) == -1)
    {
        printf("System ipc msg receive thread start fail\r\n");
        exit(0);
    }
    
    high_pre_timer_add("SystemMainTimer", LIB_TIMER_TYPE_LOOP, system_update_status, NULL, SYSTEM_UPDATE_INTERVAL);

    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }

    /* Execute each thread. */
    while (thread_fetch(system_master, &thread))
    {
        thread_call(&thread);
    }

    system_die();

    exit(0);
}


void system_die()
{
    exit(0);
}

void system_init(void)
{
    memset(&sys_conf,0,sizeof(sys_global_conf_t));
    sys_conf.system_cpu_alarm_threshold = SYSTEM_CPU_USAGE_ALARM_THRESHOLD_DEFALUT;
    sys_conf.system_mem_alarm_threshold = SYSTEM_MEM_USAGE_ALARM_THRESHOLD_DEFALUT;
    sys_conf.sys_timezone = SYSTEM_TIMEZONE_DAFAULT;
    system_cmd_init();

    update_sys_mem_usage();


    
    system_timezone_set(sys_conf.sys_timezone);
    system_gpn_function_init();
    sysm_init_proc_total_data();
}







