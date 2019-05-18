/*
*
*       main function for ip4 mc
*/

#include <zebra.h>
#include <sys/sysinfo.h>
#include <lib/version.h>
#include <lib/getopt.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/vty.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pid_file.h>
#include <lib/ifm_common.h>
#include <lib/index.h>
#include <lib/errcode.h>
#include <sched.h>
#include <config.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
#include "sigevent.h"

#include "ipmc_main.h"
#include "ipmc_if.h"
#include "pim/pim.h"
#include "pim/pim_cmd.h"
#include "igmp/igmp.h"
#include "ipmc_group.h"
#include "ipmc_ip.h"
#include "ipmc_mac.h"
#include "pim/pim_pkt.h"

int ipmc_errno = ERRNO_SUCCESS;

/* 定义默认的配置文件 */
#define IPMC_DEFAULT_CONFIG   "ipmc.conf"

/* 保存进程 id ，用于进程初始化，定义在 config.h 中 */
char *pid_file = (char *)PATH_IPMC_PID;

/* 配置文件及目录 */
char config_default[] = SYSCONFDIR IPMC_DEFAULT_CONFIG;
char *config_file = NULL;

/* ipmc 进程主线程 */
struct thread_master *ipmc_master;

/*ipmc发送报文的缓存空间*/
char *ipmc_send_buf = NULL;

/* 提示信息 */
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


/* 打印提示信息 */
static void usage(char *progname, int status)
{
     if (status != 0)
     {
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
     }
    else
    {
        printf ("Usage : %s [OPTION...]\n\
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

    exit (status);
}


static void *signal_set(int signo, void (*func)(int))
{
    int ret;
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset (&sig.sa_mask);
    sig.sa_flags = 0;
#ifdef SA_RESTART
    sig.sa_flags |= SA_RESTART;
#endif

    ret = sigaction (signo, &sig, &osig);

    if (ret < 0)
        return (SIG_ERR);
    else
        return (osig.sa_handler);
}


/* SIGHUP 信号处理 */
static void sighup(void)
{
    zlog_notice("SIGHUP received");

    /* 加载配置文件 */
    //vty_read_config (config_file, config_default);

    /* 创建 vty  套接字 */
    vty_serv_sock (IPMC_VTYSH_PATH);

    /* 尝试恢复正常 */
}


/* SIGINT 信号处理 */
static void sigint (void)
{
    zlog_notice("SIGINT received\n");

    ipmc_die();

    exit(1);
}

/* SIGTERM handler. */
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    ipmc_die();
    //...
}

/* SIGUSR1 信号处理 */
static void sigusr1 (void)
{
    zlog_notice("SIGUSR1 received\n");

    return;
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
    zlog_notice("%s: ipmc received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}

/* SIGUSR2 信号处理 */
static void sigusr2 (void)
{
    zlog_notice("SIGUSR2 received\n");

    return;
}

#if 0
/* 初始化 mpls 进程信号处理 */
static void signal_init ()
{
    signal_set (SIGHUP, sighup);
    signal_set (SIGINT, sigint);
    signal_set (SIGTERM, sigint);
    signal_set (SIGPIPE, SIG_IGN);
    signal_set (SIGUSR1, sigusr1);
    signal_set (SIGUSR2, sigusr2);
    ipmc_send_buf = (char *)malloc(IPMC_SEND_BUF_SIZE);
	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
    if(NULL == ipmc_send_buf)
	{
		perror("\n  ip4_mc_send_buf malloc failed ");
		return ;
	}
}
#endif

struct quagga_signal_t ipmc_signals[] =
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

/* mpls 主函数 */
int main (int argc, char **argv)
{
    struct thread thread;
    char *progname;
    char *p;
    int daemon_mode = 0;

    /* 配置 umask */
    umask (0027);

    /* 获取进程名 */
    progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);

	struct sched_param param;

	cpu_set_t mask;

	CPU_ZERO(&mask);
	CPU_SET(1, &mask);	 /* 绑定cpu1*/
	sched_setaffinity(0, sizeof(mask), &mask);

	param.sched_priority = 50;
#ifndef HAVE_KERNEL_3_0		
	if (sched_setscheduler(0, SCHED_RR, &param))
	{
		perror("\n  priority set: ");
	}
#endif
    /* log 初始化 */
    zlog_default = openzlog(progname, ZLOG_IPMC, LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

    /* 命令行参数解析 */
    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "df:hA:P:rv", longopts, 0);
        if (opt == EOF)
            break;

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
                print_version (progname);
                exit (0);
                break;
            case 'h':
                usage (progname, 0);
                break;
            default:
                usage (progname, 1);
                break;
        }
    }

    /* 创建主线程 */
    ipmc_master = thread_master_create ();

    /* 公用数据初始化 */
    //signal_init();
    signal_init(ipmc_master, array_size(ipmc_signals), ipmc_signals);
    cmd_init(1);
    vty_init(ipmc_master);
    memory_init();
    
    if(mem_share_attach() == -1)
    {
       printf(" share memory attach fail\r\n");
       exit(0);
    }
      
    high_pre_timer_init(1000, MODULE_ID_IPMC);

    /* 创建守护进程 */
    if (0 != daemon_mode)
    {
        daemon (0, 0);
    }

    /* 创建进程 id */
    pid_output(pid_file);

    /* 创建 vty 套接字 */
    vty_serv_sock(IPMC_VTYSH_PATH);

    /* ipmc进程相关初始化 */
    ipmc_init();
    
    if(ipc_recv_init(ipmc_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

    if(ipc_recv_thread_start("IpmcIpcMsgRev", MODULE_ID_IPMC, SCHED_OTHER, -1, ipmc_msg_rcv, 0) == -1)
    {
        printf(" ipmc ipc msg receive thread start fail\r\n");
        exit(0);
    }  

    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }
    
    /* 执行线程 */
    while (thread_fetch(ipmc_master, &thread))
    {
        thread_call(&thread);
    }

    ipmc_die();
}

static void ipmc_table_init()
{
	ipmc_if_table_init(IPMC_IF_MAX);
	
	/*PIM相关表项初始化*/
	
	
	/*IGMP相关表项初始化*/
	mc_group_table_init(IPMC_MGROUP_NUM);
	mc_ip_table_init(IPMC_MFIB_NUM);
	mc_mac_table_init(IPMC_MMAC_NUM);
}


static void ipmc_cmd_init()
{
	/*组播全局命令初始化*/
	ifm_l3if_init(NULL);
	pim_cmd_init();
}

static void ipmc_ipc_init()
{
    ipc_connect_common();
}

static void ipmc_event_init()
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_IPMC, IFNET_IFTYPE_L3IF);          /* 注册接口 down 事件*/
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_IPMC, IFNET_IFTYPE_L3IF);            /* 注册接口 up 事件*/
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_IPMC, IFNET_IFTYPE_L3IF);     /* 注册接口删除事件*/
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_IPMC, IFNET_IFTYPE_L3IF);   /* 注册接口模式切换事件*/
}

void ipmc_init(void)
{
	/*ipmc相关的初始化*/	
    ipmc_table_init();
    ipmc_cmd_init();
    //ipmc_ipc_init();
    ipmc_event_init();
	
	/*PIM初始化*/
	pim_pkt_register();
	pim_instance_init();
	/*IGMP初始化*/
	igmp_init();
	
	/*所有组播相关的报文的接收*/
    //thread_add_event (ipmc_master, ipmc_msg_rcv, NULL, 0); 
	
    return;
}

void ipmc_die()
{
    exit( 0 );
}

