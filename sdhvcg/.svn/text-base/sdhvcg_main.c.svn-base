/*
*
*       main function for vcg
*/

#include "../config.h"

//#include <time.h>
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
//#include <lib/msg_ipc.h>
#include <lib/thread.h>
#include <lib/pid_file.h>
#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"
#include "lib/hptimer.h"

#include <ifm/ifm.h>
#include <sdhvcg/sdhvcg_main.h>
#include <sdhvcg/sdhvcg_if.h>
#include <sdhvcg/sdhvcg_cmd.h>
#include <sdhvcg/sdhvcg_msg.h>
#include <sched.h>

/* ¶¨ÒåÄ¬ÈÏµÄÅäÖÃÎÄ¼þ */
#define SDHVCG_DEFAULT_CONFIG   "sdhvcg.conf"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_SDHVCG_PID;  /* defined in config.h */

/* Configuration file and directory. */
char config_default[] = SYSCONFDIR SDHVCG_DEFAULT_CONFIG;
char *config_file = NULL;

int      g_vcg_config_finish = 0;
uint32_t g_vcg_config_timer  = 0;


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
static void sighup(int sig)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"SIGHUP received");

    /* Reload config file */
    //vty_read_config (config_file, config_default);

    /* Create VTY's socket */
    vty_serv_sock(SDHVCG_VTYSH_PATH);

    /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint(int sig)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"SIGINT received\n");

    vcg_die();

    exit(1);
}

/* SIGUSR1 handler. */
static void sigusr1(int sig)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"SIGUSR1 received\n");

    return;
}

/* SIGUSR2 handler. */
static void sigusr2(int sig)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"SIGUSR2 received\n");

    return;
}

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

static void vcg_table_init()
{
    vcg_if_table_init(1024);
}

void vcg_config_finish_func(void *a)
{
    g_vcg_config_finish = 1;
}

void vcg_init(void)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"%s[%d]: Entering vcg init\n", __FILE__, __LINE__);

    vcg_table_init();
    vcg_cmd_init();

    //thread_add_event(vcg_master, vcg_msg_rcv, NULL, 0);
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"%s[%d]: Leaving vcg init\n", __FILE__, __LINE__);

    return;
}

void vcg_die(void)
{
    exit(0);
}


/* vcg routine */
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
    CPU_SET(1, &mask);   /* °ó¶¨cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);

#ifndef HAVE_KERNEL_3_0	
    //struct sched_param param;

    //param.sched_priority = 40;

    //if (sched_setscheduler(0, SCHED_RR, &param))
    //{
    //    perror("\n  priority set: ");
    //}
#endif

    /* First of all we need logging init. */
   // zlog_default = openzlog(progname, ZLOG_VCG, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON); /* defined in log.h */

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
    
	zlog_default = openzlog(progname, ZLOG_VCG, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON); /* defined in log.h */

	if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
	
    g_vcg_config_finish = 0;
    g_vcg_config_timer  = (uint32_t)time(NULL);

    /* Prepare master thread. */
    vcg_master = thread_master_create();
    vcg_master->config = vcg_config_finish_func;

    /* Library initialization. */
    signal_init();
    cmd_init(1);
    vty_init(vcg_master);
    memory_init();

	
    /* Change to the daemon program. */
    if (daemon_mode)
    {
        daemon(0, 0);
    }

    pid_output(pid_file);  /* Pid file create. */

    /* Create VTY's socket */
    vty_serv_sock(SDHVCG_VTYSH_PATH);

    /*
    if(ipc_init_n() == -1)
    {
       printf(" ipc queue init fail\r\n");
       exit(0);
    }
    */
    high_pre_timer_init(100, MODULE_ID_VCG);

	/* SDHVCG related initialization.  */
	vcg_init();

	if(ipc_recv_init(vcg_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

    if(ipc_recv_thread_start("VcgIpcRev", MODULE_ID_VCG, SCHED_OTHER, -1, vcg_msg_rcv, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }
	
	if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }

	/* æ‰§è¡Œä¸»çº¿ç¨‹ */
    while(thread_fetch (vcg_master, &thread)) thread_call (&thread);
    /* Execute each thread. */

#if 0	
    
    vcg_die();
#endif
	
    exit(0);
}




