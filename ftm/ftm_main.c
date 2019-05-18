/*
*
*            main function for ftm
*/

#include "../config.h"
#include <sched.h>
#include <sys/sysinfo.h>
#include <lib/version.h>
#include <lib/module_id.h>
#include <lib/zebra.h>
#include <lib/getopt.h>
#include <lib/pid_file.h>
#include <lib/thread.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/keychain.h>
#include <lib/vty.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include <lib/msg_ipc_n.h>
#include <pthread.h>

#include "sigevent.h"

#include "ftm.h"
#include "ftm_nhp.h"
#include "ftm_ilm.h"
#include "ftm_nhlfe.h"
#include "ftm_lsp.h"
#include "ftm_fib.h"
#include "ftm_pw.h"
#include "ftm_ifm.h"
#include "ftm_arp.h"
#include "ftm_vpls.h"
#include "ftm_vlan.h"
#include "ftm_pkt.h"
#include "pkt_eth.h"
#include "ftm_debug.h"
#include <lib/alarm.h>


/* 初始化模块 ID */
int mymodule = MODULE_ID_FTM;

/* 定义默认的配置文件 */
#define FTM_DEFAULT_CONFIG   "ftm.conf"

/* Process ID saved for use by init system */
const char *pid_file = PATH_FTM_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = FTM_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR FTM_DEFAULT_CONFIG;
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

struct ftm_devm_status ftm_devm;

void  ftm_init(void); /* init all of ftm */
void  ftm_die(void);  /* free all of ftm */
//static void  ftm_ipc_init(void);
void ftm_msg_init(void);
static void ftm_ipc_close(void);
void  ftm_thread_init(void);
void ftm_table_init(void);
uint32_t ftm_gpn_function_init (void);
int ftm_board_master_slave_status_init(void);
int ftm_brard_status_change_process(struct devm_com *pdevm);



/* Help information display. */
static void usage (char *progname, int status)
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", progname);
  else
    {
      printf ("Usage : %s [OPTION...]\n\
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

  exit (status);
}

/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

#if 0
static RETSIGTYPE *
signal_set (int signo, void (*func)(int))
{
  int ret;
  struct sigaction sig;
  struct sigaction osig;

  sig.sa_handler = func;
  sigemptyset (&sig.sa_mask);
  sig.sa_flags = 0;
#ifdef SA_RESTART
  sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

  ret = sigaction (signo, &sig, &osig);

  if (ret < 0)
    return (SIG_ERR);
  else
    return (osig.sa_handler);
}
#endif

/* SIGTERM handler. */
static void sigterm(void)
{
	zlog_notice("Terminating on signal SIGTERM");

	ftm_die();

	exit(1);
	//...
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
	zlog_notice("%s: ftm received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}


/* SIGHUP handler. */
static void sighup (int sig)
{
  zlog_notice("SIGHUP received");

  /* Reload config file */
  //vty_read_config (config_file, config_default);

  /* Create VTY's socket */
  vty_serv_sock (FTM_VTYSH_PATH);

  /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint (int sig)
{
    zlog_notice("SIGINT received\n");

    ftm_die();

    exit(1);
}

/* SIGUSR1 handler. */
static void sigusr1 (int sig)
{
    zlog_notice("SIGUSR1 received\n");

    return;
}

/* SIGUSR2 handler. */
static void sigusr2 (int sig)
{
    zlog_notice("SIGUSR2 received\n");

    return;
}

/* Initialization of signal handles. 
static void signal_init ()
{
  signal_set (SIGHUP, sighup);
  signal_set (SIGINT, sigint);
  signal_set (SIGTERM, sigint);
  signal_set (SIGPIPE, SIG_IGN);
  signal_set (SIGUSR1, sigusr1);
  signal_set (SIGUSR2, sigusr2);
}*/

/*add for new ipc by renxy*/
struct quagga_signal_t ftm_signals[] =
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


/* Main routine */
int main (int argc, char **argv)
{
	char *p;
	int daemon_mode = 0;
	char *progname;
	struct thread thread;

	/* Set umask before anything for security */
	umask (0027);

	/* Get program name. */
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);

	
  	cpu_set_t mask;

	CPU_ZERO(&mask);
	CPU_SET(0, &mask);	 /* 绑定cpu0 */
	sched_setaffinity(0, sizeof(mask), &mask);
	
#ifndef HAVE_KERNEL_3_0	
	struct sched_param param;
	param.sched_priority = 60;

	if ( sched_setscheduler ( 0, SCHED_RR, &param ) )
	{
	  	zlog_err ( "%s, %d priority set:%s\n", __FUNCTION__, __LINE__, strerror ( errno ) );
	}
#endif

	/* First of all we need logging init. */
	zlog_default = openzlog(progname, ZLOG_FTM, LOG_NDELAY,  LOG_DAEMON); /* defined in log.h */

	/* Command line option parse. */
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
				vty_addr = optarg;
				break;
			case 'i':
				pid_file = optarg;
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

	/* Prepare master thread. */
	ftm_master = thread_master_create ();
	ftm_master->config = (void *)ftm_config_finish_func;

	/* Library initialization. */

	/*signal_init (); changed by renxy for new ipc */
	signal_init(ftm_master, array_size(ftm_signals), ftm_signals);

	cmd_init (1);
	vty_init (ftm_master);
	memory_init();

	/*attach share mem*/
	if(-1 == mem_share_attach())
	{
		printf(" share memory attach fail\r\n");
		exit(0);
	}

	/*init timer*/
	high_pre_timer_init(100, MODULE_ID_FTM);

	/* Change to the daemon program. */
	if (daemon_mode)
		daemon (0, 0);
	pid_output (pid_file); /* Pid file create. */

	/* Create VTY's socket */
	vty_serv_sock (FTM_VTYSH_PATH);

	/*init ipc*/
	if(ipc_recv_init(ftm_master)< 0)
	{
		printf(" ipc receive init fail\r\n");
		exit(-1);
	}

	ftm_init();

	/* start timer*/
	if(high_pre_timer_start() < 0)
	{
		printf(" timer start fail\r\n");
		exit(-1);
	}

	/* Execute each thread. */
	while (thread_fetch (ftm_master, &thread))
		thread_call (&thread);

	ftm_die();

	exit (0);
}

static uint32_t gpnFtmCoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
    return 1;
}


uint32_t ftm_gpn_function_init (void)
{
    uint32_t reVal;

    reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_FTM, (usrMsgProc)gpnFtmCoreProc);
    if (reVal == 2)
    {
        return 2;
    }

    return 1;
}

int ftm_board_master_slave_status_init(void)
{
    struct devm_unit devm_msg; 
    enum DEV_TYPE dev_type = 0;
    int ret = 0;

	memset(&ftm_devm,0,sizeof(ftm_devm));

	ret = devm_comm_get_dev_type(MODULE_ID_FTM,&dev_type);
	if( 0 != ret )
	{
		zlog_err("In func %s, line:%d devm_comm_get_dev_type err,ret = %d\n", __func__, __LINE__,ret);
		return -1;
	}
	if( DEV_TYPE_SHELF == dev_type )
	{
        ftm_devm.devm_type = DEV_TYPE_SHELF;
	}  
	if (0 != devm_comm_get_unit(1, MODULE_ID_FTM, &devm_msg)) 
    {
        zlog_err("In func %s, line:%d\n", __func__, __LINE__);
        return DEV_BOARD_INFO_GET_FAIL;
    }
    
    /*if it is  slave board,return 1*/
    if (devm_msg.slot_slave_board == devm_msg.myslot)
    {
    	ftm_devm.ms_status = DEV_BOARD_IS_SLAVE;
    }
    else
    {
    	ftm_devm.ms_status = DEV_BOARD_IS_MASTER;
    }
	if(devm_msg.slot_slave_board != 0)
	{
		ftm_devm.slave_in = DEV_BOARD_IN_PLACE;
	}
	else
	{
		ftm_devm.slave_in = DEV_BOARD_OUT_PLACE;
	}
    if( DEV_TYPE_BOX != dev_type )
    {
    	devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_FTM, MODULE_ID_FTM);
		devm_event_register(DEV_EVENT_SLOT_ADD, MODULE_ID_FTM, MODULE_ID_FTM);
    	devm_event_register(DEV_EVENT_SLOT_DELETE, MODULE_ID_FTM, MODULE_ID_FTM);
    }
	return ret;
}

int ftm_brard_status_change_process(struct devm_com *pdevm)
{
	int borad_status = SLOT_TYPE_INVALID;
	int slave_in = 0;
    struct devm_unit devm_msg; 
    int ret = 0;
	
	if (0 != devm_comm_get_unit(1, MODULE_ID_FTM, &devm_msg)) 
    {
        zlog_err("In func %s, line:%d\n", __func__, __LINE__);
        return DEV_BOARD_INFO_GET_FAIL;
    }
    
    /*if it is  slave board,return 1*/
    if (devm_msg.slot_slave_board == devm_msg.myslot)
    {
       	borad_status = DEV_BOARD_IS_SLAVE;	
		zlog_debug(0,"\r\n borad_status = %d, %d, %s\n",borad_status,__LINE__,__FUNCTION__);
    }
    else
    {
        borad_status = DEV_BOARD_IS_MASTER;
		zlog_debug(0,"\r\n borad_status = %d, %d, %s\n",borad_status,__LINE__,__FUNCTION__);
    }
	if(devm_msg.slot_slave_board != 0)
	{
		slave_in = DEV_BOARD_IN_PLACE;
	    zlog_debug(0,"\r\n slave_in = %d, %d, %s\n",slave_in,__LINE__,__FUNCTION__);
	}
	else
	{
		slave_in = DEV_BOARD_OUT_PLACE;
		zlog_debug(0,"\r\n slave_in = %d, %d, %s\n",slave_in,__LINE__,__FUNCTION__);
	}

	if(borad_status != ftm_devm.ms_status)
    {
        ftm_devm.ms_status = borad_status;
	}
	if(slave_in != ftm_devm.slave_in)
	{
        ftm_devm.slave_in = slave_in;
	}
	return ret;
}


void ftm_init(void)
{
    //ftm_ipc_init();
    ftm_table_init();
    ftm_debug_init();
	ftm_fib_init();
	ftm_mpls_init();
	ftm_pkt_init();
	//ftm_msg_init();
    //ftm_thread_init();
	ftm_gpn_function_init();
	ftm_board_master_slave_status_init();

	if(ipc_recv_thread_start("FTMIpcMsgRev", MODULE_ID_FTM, SCHED_OTHER, -1, ftm_msg_rcv_n, 0)< 0)
	{
		printf(" ftm ipc msg receive thread start fail\r\n");
		exit(-1);
	}
	printf("ftm ipc msg receive thread start success!\n");
}


void ftm_die(void)
{
    ftm_ipc_close();
    exit( 0 );
}


/* init the ipc queue */
#if 0
static void ftm_ipc_init(void)
{
    ftm_msg_ipc_init();
}
#endif


/* close the ipc queue */
static void ftm_ipc_close(void)
{
    ftm_msg_ipc_close();
}


/* ipc 接收发送处理 */
static int ftm_thread_excute(struct thread *pthread)
{
#if 0
	int ret1 = 0;
	int ret2 = 0;
	int ret3 = 0;
	int ret4 = 0;
	static struct ipc_pkt mesg;

	ret1 = ftm_pkt_rcv_from_app(&mesg); /* 先发送 app 的报文 */
	ret2 = ftm_pkt_rcv_msg((struct ipc_mesg *)(&mesg));      /* 接收需要 pkt 应答的消息 */
	ret3 = ftm_msg_rcv((struct ipc_mesg *)(&mesg));          /* 接收控制消息 */
	ret4 = ftm_pkt_rcv(&mesg);          /* 最后接收报文 */

	//usleep(1000);
	if(( ret1 == -1) && ( ret2 == -1) && ( ret3 == -1) && ( ret4 == -1)) usleep(5000);

	thread_add_event (ftm_master, ftm_thread_excute, NULL, 0);
	//thread_add_timer_msec(ftm_master, ftm_thread_excute, NULL, 0);
    //thread_add_timer_msec(ftm_master, ftm_thread_excute, NULL, 1);
#endif
    return 0;
}


void ftm_msg_init(void)
{
#if 0
	pthread_t ftm_msg_pthread;
	
	pthread_mutex_init(&ftm_msg_lock, NULL);
	pthread_create(&ftm_msg_pthread, NULL, (void *)ftm_msg_rcv_control, NULL);
	pthread_detach(ftm_msg_pthread);
#endif
}


void ftm_thread_init(void)
{
	thread_add_event (ftm_master, ftm_thread_excute, NULL, 0);
}


void ftm_table_init(void)
{
	ftm_ifm_table_init(IFM_NUM_MAX);
	ftm_vlan_table_init();
}

void ftm_config_finish_func(void)
{   
    int8_t ret = 0;
    
    zlog_debug(0,"Enter func %s.\n",__func__);
    /*if this board is not slave ,nothing to do*/
    if (DEV_BOARD_IS_SLAVE != ftm_arp_cur_board_is_backup())
    {
        zlog_debug(0,"func %s line:%d.\n",__func__, __LINE__);
        return;
    }

    /*if it is slave board,notify mastere sync*/
    //ret = ipc_send_ha(NULL, 0, 0, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, IPC_OPCODE_EVENT, 0);
	ret = ipc_send_msg_ha_n2(NULL, 0, 0, MODULE_ID_FTM, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, IPC_OPCODE_EVENT, 0, 0);
	if (ret < 0)
    {
        zlog_err("In func %s, line:%d .config load finish notify master err!\n",__func__, __LINE__);
    }
    zlog_debug(0,"Leave func %s,line:%d\n",__func__,__LINE__);
}
