/*       main function for ftm
*
*
*/


#include <lib/zebra.h>
#include <sys/sysinfo.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>
#include <lib/version.h>
#include <lib/getopt.h>
#include <lib/thread.h>
#include <lib/command.h>
#include <lib/keychain.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/msg_ipc.h>
#include "lib/pkt_buffer.h"
#include "lib/devm_com.h"
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnSockCommFuncApi.h>
#include <lib/gpnSocket/socketComm/gpnSockCommRoleMan.h>
#include <lib/gpnSocket/socketComm/gpnStatTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnSockStatMsgDef.h>
//#include "../config.h"

#include "vlan.h"
#include "l2_if.h"
#include "lldp/lldp_if.h"
#include "lldp/lldp.h"
#include "lldp/lldp_msg.h"
#include "trunk.h"
#include "mac_static.h"
#include "l2_msg.h"
#include "loopd/loop_detect.h"
#include "mstp/mstp_init.h"

#include "cfm/cfm_session.h"
#include "cfm/cfm.h"
#include "efm/efm.h"
#include <lib/pid_file.h>
#include "aps/elps.h"
#include "raps/erps.h"
#include <sched.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
#include "sigevent.h"
#include "port_isolate.h"
#include "l2cp/l2cp.h"

#define ZEBRA_BUG_ADDRESS "https://bugzilla.quagga.net"

/*for clean build error */
int l2_alarm_socket_msg_rcv(struct thread *t);

#define L2_DEFAULT_CONFIG   "l2.conf"
#define L2_VTY_PORT       2674

/* Process ID saved for use by init system */
const char *pid_file = PATH_L2_PID;  /* defined in config.h */

/*  VTY connection port. */
int vty_port = L2_VTY_PORT;    /* defined in vty.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = L2_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR L2_DEFAULT_CONFIG;
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

unsigned char			l2_mac[MAC_LEN];


struct thread_master *l2_master = NULL;

void l2_cli_init(void);
int l2_mac_get(void);
void  l2_init(void); /* init all of l2 module */
void  l2_die(void);  /* free all of l2 */
static void  l2_ipc_init(void);
//void  l2_thread_init(void);
void  l2_table_init(void);
//uint32_t l2_gpn_function_init (void);
int l2_alarm_socket_msg_rcv_n(void *pmsg);


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
-P, --vty_port     Set vty's port number\n\
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


/* SIGHUP handler. */
static void sighup (void)
{
  zlog_notice("SIGHUP received");

  /* Reload config file */
  //vty_read_config (config_file, config_default);

  /* Create VTY's socket */
  vty_serv_sock (L2_VTYSH_PATH);

  /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint (void)
{
    zlog_notice("SIGINT received\n");

    l2_die();

    exit(1);
}

/* SIGTERM handler. */
static void sigterm(void)
{
	zlog_notice("Terminating on signal SIGTERM");

	l2_die();

	exit(1);
	//...
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
	zlog_notice("%s: l2 received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}

/* SIGUSR1 handler. */
static void sigusr1 (void)
{
    zlog_notice("SIGUSR1 received\n");

    return;
}

/* SIGUSR2 handler. */
static void sigusr2 (void)
{
    zlog_notice("SIGUSR2 received\n");

    return;
}

#if 0
/* Initialization of signal handles. */
static void signal_init ()
{
  signal_set (SIGHUP, sighup);
  signal_set (SIGINT, sigint);
  signal_set (SIGTERM, sigint);
  signal_set (SIGPIPE, SIG_IGN);
  signal_set (SIGUSR1, sigusr1);
  signal_set (SIGUSR2, sigusr2);
}
#endif

struct quagga_signal_t l2_signals[] =
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
  CPU_SET(1, &mask);   /* 绑定cpu1*/ 
  sched_setaffinity(0, sizeof(mask), &mask);

  #if 0
  struct sched_param param;
  param.sched_priority = 30;
  if (sched_setscheduler(0, SCHED_RR, &param)) 
  {   
	  perror("\n  priority set: ");
  }
  #endif

  /* First of all we need logging init. */
  zlog_default = openzlog(progname, ZLOG_L2,LOG_NDELAY, LOG_DAEMON); /* defined in log.h */

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
        case 'P':
          vty_port = atoi (optarg);
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
  l2_master = thread_master_create ();

  /* Library initialization. */
  //signal_init ();
  signal_init(l2_master, array_size(l2_signals), l2_signals);
  cmd_init (1); 
  vty_init (l2_master);
  memory_init();
  
  l2_master ->config = config_finish_func;

	/*绑定共享内存到该进程*/
	if(-1 == mem_share_attach())
	{
		printf(" share memory attach fail\r\n");
		exit(0);
	}

	/*定时器初始化*/
	high_pre_timer_init(500, MODULE_ID_L2);
  /* Change to the daemon program. */
  if (daemon_mode)
	{
    daemon (0, 0);  
	}
  pid_output (pid_file); /* Pid file create. */

  /* Create VTY's socket */
  vty_serv_sock (L2_VTYSH_PATH);  

  l2_init();

	/*接收消息初始化*/
	if(ipc_recv_init(l2_master)< 0)
	{
		printf(" ipc receive init fail\r\n");
		exit(-1);
	}
    
	if(ipc_recv_thread_start("L2IpcMsgRev", MODULE_ID_L2, SCHED_OTHER, -1, l2_msg_rcv_n, 0)< 0)
	{
		printf(" l2 ipc msg receive thread start fail\r\n");
		exit(-1);
	}

	/*启动精准定时器*/
	if(high_pre_timer_start() < 0)
	{
		printf(" timer start fail\r\n");
		exit(-1);
	}
	
	/* Execute each thread. */
	while (thread_fetch (l2_master, &thread))
		thread_call (&thread);

  l2_die();
  
  exit (0);
}

int l2_mac_get(void)
{
	int			ret = 1;

	memset(l2_mac, 0x00, MAC_LEN);
	ret = devm_comm_get_mac(1, 0, MODULE_ID_L2, l2_mac);
	if(ret)
	{
		zlog_err ( "%s[%d] fail to get mac", __FUNCTION__, __LINE__ );
		return ret;
	}

	return ret;
}


void l2_init(void)
{
	int  ret = 1;

	l2_table_init();
	//l2_thread_init();
    high_pre_timer_add("L2AlarmTimer", LIB_TIMER_TYPE_LOOP, l2_alarm_socket_msg_rcv_n, NULL, 1*1000);
	l2_ipc_init();
	l2_cli_init();

	ret = l2_mac_get();
	if(ret)
	{
		zlog_err ( "%s[%d] L2 init get mac error!!!", __FUNCTION__, __LINE__ );
		return;
	}
	
	l2cp_init();
	/* lldp 初始化*/
	lldp_init();

	mstp_init();
	
    /*trunk初始化*/
    trunk_init();
	
	/*loop detect初始化*/
	loopdetect_init();
	
	/*cfm初始化*/
	cfm_init();

	/*ELPS初始化*/
	elps_init();
	efm_init();
    /*ERPS初始化*/
    erps_init();
   	
	l2_if_init();

	/*port isolate initialization*/
	port_isolate_init();
}


void l2_die(void)
{
    exit( 0 );
}


/* init the ipc queue */
static void l2_ipc_init(void)
{
	ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_L2, IFNET_IFTYPE_PORT);
	ifm_event_register(IFNET_EVENT_IF_ADD, MODULE_ID_L2, IFNET_IFTYPE_PORT);	
	ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_L2, IFNET_IFTYPE_PORT);
	ifm_event_register(IFNET_EVENT_UP, MODULE_ID_L2, IFNET_IFTYPE_PORT);
	ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_L2, IFNET_IFTYPE_PORT);
	ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_L2, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_IF_ADD, MODULE_ID_L2, IFNET_IFTYPE_L3IF);	
	ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_L2, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_UP, MODULE_ID_L2, IFNET_IFTYPE_L3IF);
}
#if 0
void l2_thread_init(void)
{
    thread_add_event ( l2_master, l2_msg_rcv, NULL, 0 );
}
#endif
void l2_table_init(void)
{
	l2if_table_init (HASHTAB_SIZE);	
    vlan_table_init();
  	mac_static_table_init(MAC_STATIC_NUM);  
}


/* 命令行初始化 */
void l2_cli_init(void)
{
	l2if_cli_init(); /* 注册端口下的命令衿*/
	vlan_cli_init(); /* 注册全局 VLAN 的命令行 */
	mac_static_cmd_init();/* 注册 静态MAC  命令衿*/
}

int l2_alarm_socket_msg_rcv(struct thread *t)
{
	gpnSockCommApiNoBlockProc();
	//usleep (100000);
	//thread_add_event (l2_master, l2_alarm_socket_msg_rcv, NULL, 0);
	return 0;
}
int l2_alarm_socket_msg_rcv_n(void *pmsg)
{
	gpnSockCommApiNoBlockProc();
	
	return 0;
}

#if 0
static uint32_t gpnL2CoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
		UINT32	   stat_type; 

		if(pgnNsmMsgSp == NULL)
		{
			zlog_err("%s,pgnNsmMsgSp:%p",__FUNCTION__,pgnNsmMsgSp);
			return 1;
		}
		
	L2_COMMON_LOG_DBG("%s : rcv msg(%08x) from (%d) !\n\r",\
			__FUNCTION__, pgnNsmMsgSp->iMsgType, pgnNsmMsgSp->iSrcId);
		
		stat_type = pgnNsmMsgSp->iMsgPara7;
		
		switch(pgnNsmMsgSp->iMsgType)
		{		
			case GPN_STAT_MSG_PORT_STAT_MON_ENABLE:
				if(stat_type == GPN_STAT_T_PTN_MEP_M_TYPE)
				{
					cfm_session_perfm_enable(pgnNsmMsgSp);
				}
				break;
				
			case GPN_STAT_MSG_PORT_STAT_MON_DISABLE:
				if(stat_type == GPN_STAT_T_PTN_MEP_M_TYPE)
				{
					cfm_session_perfm_disable(pgnNsmMsgSp);
				}
				break;	
				
			case GPN_STAT_MSG_PTN_MEP_M_STAT_GET:				
				if(stat_type == GPN_STAT_T_PTN_MEP_M_TYPE)
				{
					cfm_session_perfm_get(pgnNsmMsgSp);
				}
				break;
	
			default:
				break;
		}
		
		return 0;
}


uint32_t l2_gpn_function_init (void)
{
	uint32_t reVal;
	
	reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_L2, (usrMsgProc)gpnL2CoreProc);
	if(reVal == 2)
	{
		return 2;
	}

	return 1;
}
#endif
