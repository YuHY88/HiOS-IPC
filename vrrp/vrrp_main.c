/*       main function for vrrpd
*
*
*
*/

#include "../config.h"
#include <zebra.h>
#include <sys/sysinfo.h>

#include "version.h"
#include "getopt.h"
#include "thread.h"
#include "command.h"
#include "radcli/common.h"
#include "memory.h"
#include "prefix.h"
#include "filter.h"
#include "keychain.h"
#include "if.h"
#include "linklist.h"
#include "vty.h"

#include "vrrpd.h"
#include "log.h"
#include "zlog.h"

#define VRRP_VTYSH_PATH        "/tmp/.vrrpd"
#define VRRP_VTY_PORT 	       2650
#define VRRP_DEFAULT_CONFIG   "vrrpd.conf"

/* Process ID saved for use by init system */
#define PATH_VRRP_PID "/var/run/vrrpd.pid"
char *pid_file = PATH_VRRP_PID;

/* VRRP VTY bind address. */
char *vty_addr = NULL;

/* VRRP VTY connection port. */
int vty_port = VRRP_VTY_PORT;

/* Configuration file and directory. */
char config_current[] = VRRP_DEFAULT_CONFIG;
char config_default[] = SYSCONFDIR VRRP_DEFAULT_CONFIG;
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


extern void  vty_serv_un_sess(char *);                                  

/* Master of threads. */
struct thread_master *master = NULL;

void  vrrp_init(void);
void  vrrp_die(void);


extern void vrrp_if_init();
extern void vrrp_zclient_init();
extern void vrrp_temp_init();
extern void vrrp_temp_clean();

/* Help information display. */
static void
usage (char *progname, int status)
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
Report bugs to %s\n", progname, "huahuan");
    }

  exit (status);
}

/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

RETSIGTYPE *
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
void
sighup (int sig)
{
  zlog_notice("SIGHUP received");

  /* Reload config file */
  //vty_read_config (config_file, config_current, config_default);

  /* Create VTY's socket */
  vty_serv_sock (vty_addr, vty_port, VRRP_VTYSH_PATH);

  /* Try to return to normal operation. */
}

/* SIGINT handler. */
void
sigint (int sig)
{
    zlog_notice("SIGINT received\n");

    vrrp_die();

    exit(1);
}

/* SIGUSR1 handler. */
void
sigusr1 (int sig)
{
	;
}

/* SIGUSR2 handler. */
void
sigusr2 (int sig)
{
	;
}

/* Initialization of signal handles. */
void
signal_init ()
{
  signal_set (SIGHUP, sighup);
  signal_set (SIGINT, sigint);
  signal_set (SIGTERM, sigint);
  signal_set (SIGPIPE, SIG_IGN);
  signal_set (SIGUSR1, sigusr1);
  signal_set (SIGUSR2, sigusr2);
}


/* Main routine of vrrp. */
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

  /* First of all we need logging init. */
  zlog_default = openzlog(progname, LOG_NDELAY, LOG_TYPE_VRRP);

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
  master = thread_master_create ();

  /* Library initialization. */
  signal_init ();
  type_init ();
  cmd_init (1);
  vty_init ();
  keychain_init ();

  vrrp_memory_init();
  vrrp_init();
  
  /* Sort all installed commands. */
  sort_node ();

  /* Get configuration file. */
  //vty_read_config (config_file, config_current, config_default);
    
  /* Change to the daemon program. */
  if (daemon_mode)
    daemon (0, 0);

  /* Pid file create. */
  pid_output (pid_file);

  /* Create VTY's socket */
  vty_serv_sock (vty_addr, vty_port, VRRP_VTYSH_PATH);
  
  vty_serv_un_sess(VRRPD_VTYSH_SESS_PATH); /*for session vrrp*/

  /* Execute each thread. */
  while (thread_fetch (master, &thread))
    thread_call (&thread);

  vrrp_die();
  
  exit (0);
}


void vrrp_init(void)
{
   int ret = 0;

   if(rtnl_open(&rtnl_sock, 0) < 0)
   {
	vrrp_die();
	exit (1);
   }

   if(open_ioctl_sock() < 0)
   {
	vrrp_die();
	exit (1);
   }

   if(open_send_sock() < 0)
   {
	vrrp_die();
	exit (1);
   }

   vrrp_if_init();
   vrrp_zclient_init();

   vrrp_temp_init();
}


void vrrp_die()
{
    vrrp_stop_all_routers();
    vrrp_temp_clean();

    rtnl_close(&rtnl_sock);
    close_ioctl_sock();
    close_send_sock();

    exit( 0 );
}

