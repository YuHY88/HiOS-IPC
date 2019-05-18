/**
 * @file      : route_main.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 9:20:16
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include "../config.h"
#include <lib/zebra.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <lib/version.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/mpls_common.h>
#include <lib/prefix.h>
#include <lib/index.h>
#include <lib/errcode.h>
#include <lib/pid_file.h>
#include "route_main.h"
#include "route_static.h"
#include "route_register.h"
#include "route_if.h"
#include "route_cmd.h"
#include "route_msg.h"
#include "route_nhp.h"
#include "route_rib.h"
#include "arp.h"
#include "arp_static.h"
#include "arp_cmd.h"
#include "ndp_static.h"
#include "ndp_cmd.h"
#include "lib/sigevent.h"


/* 初始化arp全局参数 */
struct arp_global g_arp;

/* 初始化ndp 全局参数 */
struct ndp_global gndp;

/* route 模块的全局数据 */
struct route_global g_route;

/* 路由模块主线程 */
struct thread_master *route_master;

/* 定义默认的配置文件 */
#define ROUTE_DEFAULT_CONFIG   "route.conf"

/* 进程ID，用于系统初始化，定义在 config.h 中 */
char *pid_file = (char *)PATH_ROUTE_PID;

/* 配置文件存储路径 */
char config_default[] = SYSCONFDIR ROUTE_DEFAULT_CONFIG;
char *config_file = NULL;


/* 通用选项 */
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


/* 打印帮助信息 */
static void usage(char *progname, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
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


/* 捕捉进程信号设置 */
static void *signal_set(int signo, void (*func)(int))
{
    int ret;
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

#ifdef SA_RESTART
    sig.sa_flags |= SA_RESTART;
#endif

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


/* SIGHUP 信号处理 */
static void sighup(int sig)
{
    zlog_notice("SIGHUP received");

    /* 读取配置文件 */
    //vty_read_config(config_file, config_default);

    /* åˆ›å»º vty's socket */
    //vty_serv_sock(ROUTE_VTYSH_PATH);

    /* Try to return to normal operation. */
}


/* SIGINT 信号处理 */
static void sigint(int sig)
{
    zlog_notice("SIGINT received\n");

    route_die();

    exit(1);
}


/* SIGUSR1 信号处理 */
static void sigusr1(int sig)
{
    zlog_notice("SIGUSR1 received\n");

    return;
}


/* SIGUSR2 信号处理 */
static void sigusr2(int sig)
{
    zlog_notice("SIGUSR2 received\n");

    return;
}

struct quagga_signal_t route_signals[] =
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
        .signal  = SIGALRM,
        .handler = &high_pre_timer_dotick,
        .caught  = 0,
    },
};

/* 主函数 */
int main(int argc, char **argv)
{
    struct thread thread;
    char *progname;
    char *p;
    int daemon_mode = 0;
    struct sched_param param;
    cpu_set_t mask;

    /* 初始化 umask */
    umask (0027);

    /* 获取进程名 */
    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    /* 绑定 cpu1，并设定进程优先级为 50 */
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

    param.sched_priority = 50;
#ifndef HAVE_KERNEL_3_0		
    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }
#endif
    zlog_default = openzlog(progname, ZLOG_ROUTE, LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

    /* 进程启动参数解析 */
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
                exit (0);
                break;
            case 'h':
                usage(progname, 0);
                break;
            default:
                usage(progname, 1);
                break;
        }
    }

    /* 初始化线程 */
    route_master = thread_master_create();

    /* å…¬ç”¨æ•°æ®åˆå§‹åŒ? */
    signal_init(route_master, array_size(route_signals), route_signals);

	mem_share_attach();	/* éžç¬¬ä¸?ä¸ªå¯åŠ¨è¿›ç¨‹éœ€è°ƒç”¨attach */

	/* å®šæ—¶å™¨åˆå§‹åŒ–ï¼Œæ—¶é—´é—´éš”å•ä½ms */
    high_pre_timer_init(1000, MODULE_ID_ROUTE);

    
    cmd_init(1);
    memory_init();
    vty_init(route_master);

    /* 创建守护进程 */
    if (0 != daemon_mode)
    {
        daemon(0, 0);
    }

    /* pid 文件创建 */
    pid_output(pid_file);

    /* 创建 vty's socket */
    vty_serv_sock(ROUTE_VTYSH_PATH);

    /* 路由模块初始化 */
    route_init();

    /*arp 模块初始化*/
    arp_init();

    /*ndp 模块初始化*/
    ndp_init();

    route_ipc_init();

    /* 执行线程 */
    while (thread_fetch(route_master, &thread))
    {
        thread_call(&thread);
    }

    route_die();
}


/**
 * @brief      : 生成 127.0.0.0/8 路由
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:32:45
 * @note       : 调用必须在路由表初始化之后
 */
static void route_loopback_routing_init(void)
{
    struct route_entry route;

    memset(&route, 0, sizeof(struct route_entry));

    route.prefix.addr.ipv4 = inet_strtoipv4((char *)"127.0.0.0");
    route.prefix.prefixlen = 8;
    route.prefix.type      = INET_FAMILY_IPV4;
    route.vpn              = 0;
    route.nhp_num          = 1;
    route.nhp[0].protocol  = ROUTE_PROTO_CONNECT;
    route.nhp[0].action    = NHP_ACTION_TOCPU;
    route.nhp[0].nhp_type  = NHP_TYPE_CONNECT;
    route.nhp[0].vpn       = 0;
    route.nhp[0].distance  = ROUTE_METRIC_CONNECT;
    route.nhp[0].cost      = ROUTE_COST;
    route.nhp[0].active    = ROUTE_STATUS_INACTIVE;
    route.nhp[0].down_flag = LINK_DOWN;

    route.nhp[0].ifindex   = ifm_get_ifindex_by_name((char *)"loopback", (char *)"0");
    if (0 == route.nhp[0].ifindex)
    {
        ROUTE_LOG_ERROR("Add loopback routing\n");

        return;
    }

    if (ERRNO_SUCCESS != rib_add(&route.prefix, route.vpn, &route.nhp[0]))
    {
        ROUTE_LOG_ERROR("Loopback routing add failed !\n");

        return;
    }

    return;
}


/**
 * @brief      : 生成 ::1/128 路由
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:42:43
 * @note       : 调用必须在路由表初始化之后
 */
static void routev6_loopback_routing_init(void)
{
    struct route_entry route;
    struct prefix_ipv6 destip;

    memset(&route, 0, sizeof(struct route_entry));

    str2prefix_ipv6((char *)"::1", &destip);
    memcpy(route.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);

    route.prefix.prefixlen = 128;
    route.prefix.type      = INET_FAMILY_IPV6;
    route.nhp_num          = 1;
    route.nhp[0].protocol  = ROUTE_PROTO_CONNECT;
    route.nhp[0].action    = NHP_ACTION_TOCPU;
    route.nhp[0].nhp_type  = NHP_TYPE_CONNECT;
    route.nhp[0].distance  = ROUTE_METRIC_CONNECT;
    route.nhp[0].cost      = ROUTE_COST;
    route.nhp[0].active    = ROUTE_STATUS_INACTIVE;
    route.nhp[0].down_flag = LINK_DOWN;

    route.nhp[0].ifindex   = ifm_get_ifindex_by_name((char *)"loopback", (char *)"0");
    if (0 == route.nhp[0].ifindex)
    {
        ROUTE_LOG_ERROR("Add loopback routing\n");
        return;
    }

    if (ERRNO_SUCCESS != ribv6_add(&route.prefix, route.vpn, &route.nhp[0]))
    {
        ROUTE_LOG_ERROR("Loopback routing add failed !\n");
        return;
    }

    return;
}


/**
 * @brief      : route 模块存储表项初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:47:29
 * @note       : 数据结构初始化应该放在最前面
 */
static void route_table_init(void)
{
    route_static_table_init(HASHTAB_SIZE);
    route_tree_init();
    rib_tree_init();
    routev6_static_table_init(HASHTAB_SIZE);
    routev6_tree_init();
    ribv6_tree_init();
    route_if_table_init(4*HASHTAB_SIZE);
    route_nhp_table_init(NHP_NUM_MAX);
    route_nexthop_table_init(NHP_NUM_MAX);
    route_ecmp_table_init(ECMP_NUM_MAX);
    l3if_event_list_init();
    route_event_list_init();
    routev6_event_list_init();
    index_register(INDEX_TYPE_NHP, NHP_NUM_MAX*2);

    return;
}


/**
 * @brief      : route 模块命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:49:37
 * @note       : 同一进程不允许重复注册相同节点，进程内用到的所有节点必须先 install
 */
static void route_cmd_init(void)
{
    route_if_cmd_init();
    route_static_cmd_init();
    h3c_route_static_cmd_init();
    h3c_route_if_cmd_init();
    return;
}

void route_ipc_init ( void )
{

	/* 消息接收初始化 */
	if(ipc_recv_init(route_master) == -1)
    {
        printf("route ipc receive init fail\r\n");
        exit(0);
    }

	/* 启动接收线程 */
    if(ipc_recv_thread_start((char *)"route_ipc_msg_rcv", MODULE_ID_ROUTE, SCHED_OTHER, -1, route_ipc_msg_process, 0) == -1)
    {
        printf("route ipc receive thread start fail\r\n");
        exit(0);
    }

	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf("route minum internal timer start fail\r\n");
       exit(0);
    }
	
}


/**
 * @brief      : route 模块事件注册
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:50:44
 * @note       :
 */
static void route_event_register_init(void)
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_ROUTE, IFNET_IFTYPE_L3IF);        //注册接口 down 事件
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_ROUTE, IFNET_IFTYPE_L3IF);          //注册接口 up 事件
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_ROUTE, IFNET_IFTYPE_L3IF);   //注册接口删除事件
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_ROUTE, IFNET_IFTYPE_L3IF); //注册接口模式切换事件
    mpls_com_l3vpn_event_register(MODULE_ID_ROUTE);                                  //注册 l3vpn 实例删除事件

    return;
}


/**
 * @brief      : route 模块收发消息线程初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:52:08
 * @note       :
 */
static void route_msg_send_thread_start(void)
{
    pthread_t route_send_thread;

    pthread_mutex_init(&g_route.route_send_lock, NULL);

    pthread_create(&route_send_thread, NULL, (void *)route_msg_send_thread_callback, NULL);

    pthread_detach(route_send_thread);

    return;
}


/**
 * @brief      : route 模块初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:53:11
 * @note       :
 */
void route_init(void)
{
    ROUTE_LOG_DEBUG();

    memset(&g_route, 0, sizeof(struct route_global));
	g_route.route_limit       = ROUTE_NUM_MAX;
	g_route.routev6_limit     = ROUTEV6_NUM_MAX;
    g_route.conver_flag       = ENABLE;
    g_route.route_ecmp_flag   = ENABLE;
    g_route.route_frr_flag    = ENABLE;
    g_route.routev6_ecmp_flag = ENABLE;
    g_route.routev6_frr_flag  = ENABLE;

    route_table_init();
    route_cmd_init();

    route_loopback_routing_init();
    routev6_loopback_routing_init();
    route_event_register_init();
    
    route_msg_send_thread_start();

    //thread_add_event (route_master, route_msg_rcv, NULL, 0);


    return;
}


/**
 * @brief      : route 模块结束执行
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:54:36
 * @note       : 进程运行正常，不会调用到此函数
 */
void route_die(void)
{
    exit(0);
}


/************************************************
 *
 * Function: arp_init
 * Input:
 *      NONE
 * Output:
        NONE
 * Return:
 *        NONE
 * Description:
 *      Initialization of arp handles.
 ************************************************/
void arp_init(void)
{
	/* 初始化 arp 全局数据结构 */
    memset(&g_arp, 0, sizeof(struct arp_global));
	g_arp.num_limit = ARP_TOTAL_NUM;
	g_arp.age_time = ARP_AGED_TIME;
	g_arp.timer = ARP_TIMER;
    g_arp.fake_expire_time = ARP_FAKE_TIME;

    arp_static_table_init(ARP_STATIC_NUM);
    arp_cmd_init();

    zlog_debug("%s[%d]: Leaving arp init\n", __FILE__, __LINE__);

    return;
}


/*ndp 初始化*/
void ndp_init(void)
{
	/* 初始化 ndp 全局数据结构 */
    memset(&gndp, 0, sizeof(struct ndp_global));
	gndp.num_limit = ND_STATIC_NUM;
	gndp.reach_time = NDP_REACHABLE_TIME;
    gndp.stale_time = NDP_STALE_TIME;
    gndp.fake_time = NDP_INCOMPLETE_AGE_TIME;

	gndp.timer = NDP_TIMER;

    ndp_static_table_init(ND_STATIC_NUM);
    ndp_cmd_init();
}


