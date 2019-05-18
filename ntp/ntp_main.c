/**@file ntp_main.c
   @brief  This file contains the init part for ntp module.
*/

/* Copyright (C) 2013 Huahuan, Inc. All Rights Reserved. */

//#include "../config.h"


#include "zebra.h"
#include "getopt.h"
#include "thread.h"
#include "log.h"
#include "command.h"
#include "vty.h"
#include "syslog.h"
#include "keychain.h"
#include "sigevent.h"
#include "lib/version.h"
#include "lib/pkt_type.h"
#include "lib/memtypes.h"
#include "lib/memory.h"
#include "lib/pid_file.h"
#include "lib/msg_ipc_n.h"
#include <lib/hptimer.h>


#include "module_id.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sched.h>

#include "ntp.h"
#include "ntp_ntpclient.h"
#include "ntp_cli.h"

/* Global variable container. */
extern ntp_globals    ntp_global;
struct thread_master *ntp_master;

#define NTP_DEFAULT_CONFIG   "ntp.conf"
/* Configuration file and directory. */
char config_current[] = NTP_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR NTP_DEFAULT_CONFIG;
char *config_file = NULL;


/* Process ID saved for use by init system */
const char *pid_file = PATH_NTP_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;


/* sdm options. */
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
static void ntp_usage(int status, char *progname)
{
    if (status != 0)
    {
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    }
    else
    {
        printf("Usage : %s [OPTION...]\n\
Daemon which manages PTP \n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n\
-c, --ha_cold      High availability - cold start\n\
\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }

    exit(status);
}



static void ntp_stop(void)
{
    vty_terminate();
    //cmd_terminate ();  //函数不存在，暂时删除 lihz


    if (ntp_master)
    {
        thread_master_free(ntp_master);
    }

}



/*
 * Signal handlers
 */


/* SIGHUP handler. */
static void sighup(void)
{
    zlog_notice("SIGHUP received\n");


    /* Reload config file */
    //vty_read_config (config_file , config_default);

    /* Create VTY's socket */
    vty_serv_sock(NTP_VTYSH_PATH);
    /* Try to return to normal operation. */

    ntp_stop();
}

/* SIGINT handler. */
static void sigint(void)
{
    zlog_notice("Terminating on signal\n");

    ntp_stop();

    exit(0);
}

/* SIGUSR1 handler. */
static void sigusr1(void)
{
    zlog_rotate(NULL);
}

static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");
    exit(0);
}


struct quagga_signal_t ntpd_signals[] =
{
    {
        .signal = SIGHUP,
        .handler = &sighup,
    },
    {
        .signal = SIGUSR1,
        .handler = &sigusr1,
    },
    {
        .signal = SIGINT,
        .handler = &sigint,
    },
    {
        .signal = SIGTERM,
        .handler = &sigterm,
    },
};

/*
 * end of Signal handlers
 */


/* Main routine of ptpd. */
int main(int argc, char **argv)
{
    //int   ret;
    char *p;
    int   daemon_mode = 0;
    char *progname;
    struct thread thread;

    /* Set umask before anything for security */
    umask(0027);

#ifdef VTYSH
    /* Unlink vtysh domain socket. */
    unlink(NTP_VTYSH_PATH);
#endif /* VTYSH */

    /* Get program name. */
    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    
    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(1, &mask);   /* 绑定cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);
    
#ifndef HAVE_KERNEL_3_0	
    struct sched_param param;
    param.sched_priority = 60;
    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }
#endif
    zlog_default = openzlog(progname, ZLOG_NTP, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

    /* Command line option parse. */
    while (1)
    {
        int opt;

        opt = getopt_long(argc, argv, "df:hP:rvc", longopts, 0);

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
                ntp_usage(0, progname);
                break;

            default:
                ntp_usage(1, progname);
                break;
        }
    }

    /* Make master thread emulator. */
    ntp_master = thread_master_create();

    memset(&thread, 0, sizeof(struct thread));

    /* Initialize signal.  */
    signal_init(ntp_master, array_size(ntpd_signals), ntpd_signals);
    /* Vty related initialize. */
    cmd_init(1);
    vty_init(ntp_master);
    memory_init();

    //vty_read_config (config_file, config_default);

    if (daemon_mode && daemon(0, 1) < 0)
    {
        zlog_err("NTP daemon failed: %s", strerror(errno));
        exit(1);
    }

    /* pid file create */
    pid_output(pid_file);

    /* Make NYP vty socket. */
    vty_serv_sock(NTP_VTYSH_PATH);
    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    high_pre_timer_init(1000, MODULE_ID_NTP);

    if(ipc_recv_init(ntp_master) == -1)
    {
        printf(" clock module ipc receive init fail\r\n");
        exit(0);
    }

    ntp_init();
    if(ipc_recv_thread_start("NtpIpcMsgRev", MODULE_ID_NTP, SCHED_OTHER, -1, ntp_msg_rcv_msg_n, 0) == -1)
    {
        printf(" ntp ipc msg receive thread start fail\r\n");
        exit(0);
    }

    if (ntp_global.uiClientEnable && ntp_check_ip_valid(ntp_global.uiServerIP) == 0)
    {
        /* call ntp script */
        ntp_client_enable(NULL);
    }

    /* Execute each thread. */
    while (thread_fetch(ntp_master, &thread))
    {
        thread_call(&thread);
    }

    /* Not reached. */
    exit(0);
}
