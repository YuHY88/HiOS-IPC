  /*       main function for qos
*
*
*/
#include <lib/zebra.h>
#include <sys/sysinfo.h>
#include <lib/zassert.h>
#include <lib/types.h>
#include <lib/module_id.h>
#include <lib/sigevent.h>
#include <sched.h>

#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"

#include "getopt.h"
#include "config.h"
#include "thread.h"
#include "command.h"
#include "memory.h"
#include "keychain.h"
#include "vty.h"
#include "syslog.h"
#include "acl.h"
#include "log.h" 
#include "hash1.h"
#include "ifm_common.h"
#include "acl_cli.h"
#include "qos_if.h"
#include "qos_main.h"
#include "qos_mapping.h"
#include "qos_mapping_cmd.h"
#include "qos_policy.h"
#include "qos_car.h"
#include "qos_mirror.h"
#include "qos_cpcar.h"
#include "qos_msg.h"
#include "sla/sla_session.h"
#include "hqos.h"
#include "acl_h3c_cli.h"
#include "qos_policy_h3c.h"

#define ZEBRA_BUG_ADDRESS "https://bugzilla.quagga.net"

#define QOS_DEFAULT_CONFIG   "qos.conf"//"config.startup"

/* Process ID saved for use by init system */
const char *pid_file = PATH_QOS_PID;  /* defined in config.h */

/* VTY bind address. */
char *vty_addr = NULL;

/* Configuration file and directory. */
char config_current[] = QOS_DEFAULT_CONFIG;
char config_default[] =SYSCONFDIR QOS_DEFAULT_CONFIG;
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

/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

static void qos_table_init( void );
static void qos_cmd_init( void );
extern void  vty_serv_un_sess(char *); 
extern pid_t pid_output (const char *path);

/* Master of threads. */
struct thread_master *qos_master = NULL;

void  qos_init(void); /* init all of qos */
void  qos_die(void);  /* free all of ifm */



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

/* SIGHUP handler. */
static void sighup (void)
{
  QOS_LOG_DBG("SIGHUP received");

  /* Reload config file */
  //vty_read_config (config_file, config_default);

  /* Create VTY's socket */
  vty_serv_sock (QOS_VTYSH_PATH);

  /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint (void)
{
    QOS_LOG_DBG("SIGINT received\n");

    qos_die();

    exit(1);
}

/* SIGTERM handler. */
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    qos_die();

    exit(0);
}


/* Initialization of signal handles. */
struct quagga_signal_t qos_signals[] =
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

const struct message qos_dbg_name[] = {
	{.key = QOS_DBG_MSG,    .str = "message"},
	{.key = QOS_DBG_CFG,    .str = "config"},
	{.key = QOS_DBG_TRACE,  .str = "trace"},
	{.key = QOS_DBG_SLA,  	.str = "sla"}
};

extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);

DEFUN (qos_debug_monitor,
	qos_debug_monitor_cmd,
	"debug qos bitmap <0-65535>",
	"Debug information to moniter\n"
	"Module name\n"
	"Debug control bitmap\n"
	"Debug bitmap value")
{
#if 0
	unsigned int typeid = 0;
	int zlog_num;
	
	for(zlog_num = 0; zlog_num < array_size(qos_dbg_name); zlog_num++)
	{
		if(!strncmp(argv[1], qos_dbg_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, qos_dbg_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);
#else
	uint16_t i = 0;
	uint16_t bitmap = 0;
    VTY_GET_INTEGER_RANGE("Debug bitmap value", bitmap, argv[0], 0, 65535);
    for(i = 0; i < array_size(qos_dbg_name); i++)
	{
		zlog_debug_set( vty, qos_dbg_name[i].key, (qos_dbg_name[i].key & bitmap));
	}
#endif
	return CMD_SUCCESS;
}



DEFUN (show_qos_debug_monitor,
	show_qos_debug_monitor_cmd,
	"show qos debug",
	SHOW_STR
	"QoS"
	"Debug status\n")
{
	uint32_t type_num;
	
	vty_out(vty, "debug type         status      bitmap %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(qos_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s  0x%x%s", qos_dbg_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "on" : "off",
			qos_dbg_name[type_num].key, VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}

/* Main routine */
int main (int argc, char **argv)
{
  /*  VTY connection port. */
  char *p;
  int daemon_mode = 0;
  char *progname;
  struct thread thread;

  /* Set umask before anything for security */
  umask (0027);

  /* Get program name. */
  progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);

//	struct sched_param param;
	
	cpu_set_t mask; 
		
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 锟斤拷cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);

#if 0
	param.sched_priority = 10;
	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif

  /* First of all we need logging init. */
  zlog_default = openzlog(progname, ZLOG_QOS, LOG_NDELAY, LOG_DAEMON); /* ZLOG_QOS defined in log.h */

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
  qos_master = thread_master_create ();

  /* Library initialization. */
  signal_init(qos_master, array_size(qos_signals), qos_signals);
  
  cmd_init (1);
  vty_init (qos_master);
  memory_init();
#if 1
	mem_share_attach();
#else
	if(mem_share_init() == -1)
	{
		printf(" share memory init fail\r\n");
		exit(0);
	}

	if(ipc_init_n() == -1)
	{
		printf(" ipc queue init fail\r\n");
		exit(0);
	}

	high_pre_timer_init(10, MODULE_ID_FILE);
#endif
    
  /* Change to the daemon program. */
  if (daemon_mode)
    daemon (0, 0);  
  pid_output (pid_file); /* Pid file create. */

  /* Create VTY's socket */
  vty_serv_sock (QOS_VTYSH_PATH);  


	if(ipc_recv_init(qos_master) == -1)
	{
	  printf(" ipc receive init fail\r\n");
	  exit(0);
	}

	if(ipc_recv_thread_start("QosIpcRev", MODULE_ID_QOS, SCHED_OTHER, -1, qos_msg_rcv, 0) == -1)
	{
	  printf(" ipc receive thread start fail\r\n");
	  exit(0);
	}

    qos_init();

	if(high_pre_timer_start() == -1)
	{
		printf(" timer start fail\r\n");
		exit(0);
	}
    
  /* Execute each thread. */
  while (thread_fetch (qos_master, &thread))
    thread_call (&thread);

  qos_die();
  
  exit (0);
}


/* qos 数据结构初始化*/
void qos_table_init()
{
	acl_group_table_init(HASHTAB_SIZE);
  	qos_if_table_init (QOS_IF_TAB_SIZE);
	qos_domain_init();
	qos_phb_init();
	qos_car_init(QOS_CAR_TABLE_SIZE);
	qos_mirror_init();
	qos_cpcar_init();
	sla_session_table_init(SLA_NUM);
	hqos_table_init();
	qos_policy_h3c_init();
}


/* qos 命令行初始化*/
void qos_cmd_init()
{	
	qos_if_cmd_init();
	acl_cli_init();
	qos_mapping_cmd_init();
	qos_policy_cmd_init();
	qos_car_cmd_init();
	qos_mirror_cmd_init();
	qos_cpcar_cmd_init();
	sla_cli_init();
	hqos_cmd_init();
	acl_h3c_cli_init();
	qos_policy_h3c_cmd_init();
	
	install_element (CONFIG_NODE, &qos_debug_monitor_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_qos_debug_monitor_cmd, CMD_LOCAL);
}


void qos_init()
{	
	qos_policy_global_init();
	qos_table_init();
	qos_cmd_init();
	
	/* 注册接口删除，模式切换事件 */
	ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_QOS, IFNET_IFTYPE_ALL);
	ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_QOS, IFNET_IFTYPE_ALL);
	//thread_add_event ( qos_master, qos_msg_rcv, NULL, 0 );
}

void qos_die()
{
	vty_terminate();

    if(qos_master) 
    {
        thread_master_free (qos_master);
    }

    if(zlog_default)  
    {
        closezlog(zlog_default);
    }
    // exit( 0 );
}



