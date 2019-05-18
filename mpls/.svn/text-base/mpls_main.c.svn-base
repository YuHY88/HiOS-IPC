/**
 * @file      : mpls_main.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月28日 10:50:47
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <zebra.h>
#include <sys/sysinfo.h>
#include <lib/version.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/pid_file.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/index.h>
#include <lib/mpls_common.h>
#include <lib/devm_com.h>
#include <sched.h>
#include "lsp_static.h"
#include "lspm.h"
#include "lsp_cmd.h"
#include "tunnel.h"
#include "tunnel_cmd.h"
#include "mpls_main.h"
#include "pw_cmd.h"
#include "lsp_fec.h"
#include "mpls.h"
#include "labelm.h"
#include "vpls_cmd.h"
#include "l3vpn_cmd.h"
#include "l3vpn.h"
#include "l2vpn_cmd_h3c.h"
#include "lsp_cmd_h3c.h"
#include "l2vpn_h3c.h"
#include "mpls_alarm.h"
#include "mpls_msg.h"
#include "mpls_register.h"
#include "mpls/bfd/bfd_packet.h"
#include "mpls/mpls_ping.h"
#include "mpls/mpls_ping_cmd.h"
#include "mpls/mpls_oam/mpls_oam_cmd.h"
#include "mpls_oam/mpls_oam_pkt.h"
#include "mpls/ldp/ldp.h"
#include "mpls_aps/mpls_aps.h"
#include "mpls/mpls_oam/mpls_oam_lck.h"
#include "mpls/mpls_oam/mpls_oam_csf.h"
#include "mpls/mpls_alarm.h"
#include "lib/sigevent.h"


int mpls_errno = ERRNO_SUCCESS;

/* mpls 全局数据 */
struct mpls_global gmpls;

/* 定义默认的配置文件 */
#define MPLS_DEFAULT_CONFIG   "mpls.conf"

/* 保存进程 id ，用于进程初始化，定义在 config.h 中 */
char *pid_file = (char *)PATH_MPLS_PID;

/* 配置文件及目录 */
char config_default[] = SYSCONFDIR MPLS_DEFAULT_CONFIG;
char *config_file = NULL;

/* mpls 进程主线程 */
struct thread_master *mpls_master;

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


/* SIGHUP 信号处理 */
static void sighup(int sig)
{
    zlog_notice("SIGHUP received");

    /* 加载配置文件 */
    //vty_read_config (config_file, config_default);

    /* 创建 vty  套接字 */
    vty_serv_sock (MPLS_VTYSH_PATH);

    /* 尝试恢复正常 */
}


/* SIGINT 信号处理 */
static void sigint (int sig)
{
    zlog_notice("SIGINT received\n");

    mpls_die();

    exit(1);
}


/* 初始化 mpls 进程信号处理 */
struct quagga_signal_t mpls_signals[] =
{
    {
        .signal  = SIGHUP,
        .handler = (void *)(&sighup),
        .caught  = 0,
    },
    {
        .signal  = SIGINT,
        .handler = (void *)(&sigint),
        .caught  = 0,
    },
    {
        .signal  = SIGALRM,
        .handler = (void *)(&high_pre_timer_dotick),
        .caught  = 0,
    },
};


int mpls_board_master_slave_status_init(void)
{
    struct devm_unit devm_msg; 
    enum DEV_TYPE dev_type = 0;
    int ret = 0;
    ret = devm_comm_get_dev_type(MODULE_ID_MPLS, &dev_type);

    if((DEV_TYPE_SHELF != dev_type) || ( 0 != ret))
    {
        gmpls.board_status =  SLOT_TYPE_BOARD_MAIN;
        return ret;
    }
    
    if (0 != devm_comm_get_unit(1, MODULE_ID_MPLS, &devm_msg)) 
    {
        printf("In func %s, line:%d error ---------\n", __func__, __LINE__);
        gmpls.board_status =  SLOT_TYPE_INVALID;
    }
    
    if (devm_msg.slot_main_board == devm_msg.myslot)
    {
        gmpls.board_status =  SLOT_TYPE_BOARD_MAIN;
    }
    else
    {
        gmpls.board_status =  SLOT_TYPE_BOARD_SLAVE;
    }

    /* */
    //devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_MPLS, IPC_MSG_COMMON);
    return ret;
}

int mpls_brard_status_change_process(struct devm_com *pdevm)
{
    int borad_status = SLOT_TYPE_INVALID;
    struct devm_unit devm_msg; 
    enum DEV_TYPE dev_type = 0;
    int ret = 0;

    ret = devm_comm_get_dev_type(MODULE_ID_MPLS, &dev_type);

    if((DEV_TYPE_SHELF != dev_type) || ( 0 != ret))
    {
        gmpls.board_status =  SLOT_TYPE_BOARD_MAIN;
        return ret;
    }

    if (0 != devm_comm_get_unit(1, MODULE_ID_MPLS, &devm_msg)) 
    {
        printf("In func %s, line:%d error ---------\n", __func__, __LINE__);
        borad_status =  SLOT_TYPE_INVALID;
    }
    
    if (devm_msg.slot_main_board == devm_msg.myslot)
    {
        borad_status =  SLOT_TYPE_BOARD_MAIN;
    }
    else
    {
        borad_status =  SLOT_TYPE_BOARD_SLAVE;
    }

    if(borad_status != gmpls.board_status)
    {
        gmpls.board_status = borad_status;
        MPLS_LOG_ERROR(" MPLS receive borad status change event \n");
        if(SLOT_TYPE_BOARD_MAIN == borad_status)
        {
            /**/
        }
        else if(SLOT_TYPE_BOARD_SLAVE == borad_status)
        {
            ldp_session_clear();
        }
        else
        {
            MPLS_LOG_ERROR(" MPLS receive invalid borad status change event \n");
        }
    }

    return ret;
}

/* mpls 主函数 */
int main (int argc, char **argv)
{
    struct thread thread;
    char *progname;
    char *p;
    int daemon_mode = 0;
	struct sched_param param;
	cpu_set_t mask;

    /* 配置 umask */
    umask (0027);

    /* 获取进程名 */
    progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);

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
    zlog_default = openzlog(progname, ZLOG_MPLS, LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

    /* 进程启动参数解析 */
    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "df:hA:P:rv", longopts, 0);
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
    mpls_master = thread_master_create ();

    mem_share_attach();	/* 非第一个启动进程需调用attach */
	/* 定时器初始化，时间间隔单位ms */
    high_pre_timer_init(100, MODULE_ID_MPLS);
    /* 公用数据初始化 */
    signal_init(mpls_master, array_size(mpls_signals), mpls_signals);
    cmd_init(1);
    vty_init(mpls_master);
    memory_init();

	mpls_master ->config = mpls_config_finish_func;

    /* 创建守护进程 */
    if (0 != daemon_mode)
    {
        daemon (0, 0);
    }

    /* 创建进程 id */
    pid_output(pid_file);

    /* 创建 vty 套接字 */
    vty_serv_sock(MPLS_VTYSH_PATH);

    /* mpls 进程相关初始化 */
    mpls_init();

    mpls_ipc_init();

    /* 执行线程 */
    while (thread_fetch(mpls_master, &thread))
    {
        thread_call(&thread);
    }

    mpls_die();
}


/**
 * @brief      : mpls 模块存储表项初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:11:20
 * @note       :
 */
static void mpls_table_init(void)
{
    index_register(INDEX_TYPE_LSP, LSP_NUM_MAX);
    static_lsp_table_init(LSP_STATIC_NUM_MAX);
    mpls_lsp_table_init(LSP_NUM_MAX);
    tunnel_if_table_init(TUNNEL_NUM_MAX);
    mpls_if_table_init (PW_NUM_MAX);
    lsp_fec_tree_init(32);
    pw_table_init(PW_NUM_MAX);
    l2vc_table_init(PW_NUM_MAX);
    vsi_table_init(VSI_NUM_MAX);
    bfd_session_table_init(BFD_NUM_MAX);
	bfd_template_table_init(BFD_NUM_MAX);
	mplsoam_session_table_init(MPLSTP_OAM_NUM);
    mpls_l3vpn_table_init(L3VPN_NUM_MAX);
    mpls_gpn_function_init();
    pw_class_table_init(PW_CLASS_MAX_NUM);
    xc_group_table_init(XC_GROUP_MAX_NUM);
    service_instance_table_init(SERVICE_INSTANCE_MAX_NUM);

    return;
}


/**
 * @brief      : mpls 模块命令初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:12:29
 * @note       :
 */
static void mpls_cmd_init(void)
{
    mpls_global_cmd_init();
    mpls_if_cmd_init();
    tunnel_if_cmd_init();
    mpls_vsi_cmd_init();
    mpls_lsp_cmd_init();
    mpls_pw_cmd_init();
    mpls_ping_CmdInit();
    bfd_cmd_init();
	mplstp_oam_cmd_init();
    mpls_l3vpn_cmd_init();
    h3c_static_lsp_cmd_init();
    l2vpn_cmd_h3c_init();

    return;
}


/**
 * @brief      : mpls 模块事件注册初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:13:09
 * @note       :
 */
static void mpls_event_init(void)
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_MPLS, IFNET_IFTYPE_L2IF);
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_MPLS, IFNET_IFTYPE_L2IF);
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_MPLS, IFNET_IFTYPE_L2IF);
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_MPLS, IFNET_IFTYPE_L2IF);
    ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_MPLS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_MPLS, IFNET_IFTYPE_L3IF);

    return;
}

void mpls_ipc_init ( void )
{
	/* 消息接收初始化 */
	if(ipc_recv_init(mpls_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	/* 启动接收线程 */
   if(ipc_recv_thread_start((char *)"mpls_ipc_msg_rcv", MODULE_ID_MPLS, SCHED_OTHER, -1, mpls_ipc_msg_process, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }

   if(ipc_recv_thread_start((char *)"mpls_bfd_ipc_msg_rcv", MODULE_ID_BFD, SCHED_OTHER, -1, mpls_bfd_ipc_msg_process, 0) == -1)
    {
        printf(" bfd ipc receive thread start fail\r\n");
        exit(0);
    }

	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf(" minum internal timer start fail\r\n");
       exit(0);
    }
	
}


/**
 * @brief      : mpls 模块初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:14:06
 * @note       :
 */
void mpls_init(void)
{
	memset(&gmpls, 0, sizeof(struct mpls_global));
	gmpls.pw_limit         = PW_NUM_MAX;
	gmpls.lsp_limit        = LSP_NUM_MAX;
	gmpls.tunnel_limit     = TUNNEL_NUM_MAX;
    gmpls.vsi_pw_mac_limit = VSI_MAC_NUM_MAX;

    mpls_board_master_slave_status_init();

    label_pool_init();
    mpls_table_init();
    mpls_cmd_init();
    mpls_event_init();
    mpls_lsp_init();
	mpls_ping_init();
	ldp_init();
    l3vpn_event_list_init();
	mplsoam_packet_register();
	mplsoam_ais_register();
	mplsoam_lck_register();
	mplsoam_csf_register();
	mplsaps_init();
	bfd_init();
    mpls_service_table_init(MPLS_SERVICE_TABLE_MAX);
    
    //thread_add_event (mpls_master, mpls_msg_process_event, NULL, 0);

    

    return;
}


/**
 * @brief      : mpls 模块结束执行
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:14:38
 * @note       : 进程运行正常，不会调用到此函数
 */
void mpls_die(void)
{
    exit( 0 );
}


