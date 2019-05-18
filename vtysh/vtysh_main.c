/******************************************************************************
 * Filename: vtysh_main.c
 *  Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
******************************************************************************/


#include <zebra.h>
#include "../config.h"

#include <sys/un.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <pwd.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <lib/version.h>
#include <lib/alarm.h>

#include "getopt.h"
#include "command.h"
#include "memory.h"
#include "vty.h"
#include "thread.h"
#include "sigevent.h"
#include "hptimer.h"
#include "vtysh/vtysh.h"
#include "vtysh/vtysh_user.h"

#include <pthread.h>
#include <sched.h>

#include <sched.h>
#include "vtysh_session.h"
#include "vtysh_msg.h"
#include "sshd/vtysh_sshd.h"
#include "vtysh_sync.h"

/* VTY shell program name. */
char *progname;

int vtysh_port = VTYSH_PORT;

extern struct list *user_session_list;

//char *pid_file = PATH_VTYSH_PID;  /* defined in config.h */


/* Configuration file name and directory. */
char config_default[] = VTYSH_STARTUP_CONFIG;
char config_init[] = VTYSH_INIT_CONFIG;

char history_file[MAXPATHLEN];

/* Flag for indicate executing child command. */
int execute_flag = 0;

/* For sigsetjmp() & siglongjmp(). */
static sigjmp_buf jmpbuf;

/* Flag for avoid recursive siglongjmp() call. */
static int jmpflag = 0;

/* A static variable for holding the line. */
static char *line_read;

/* Master of threads. */
struct thread_master *master;

/* Command logging */
FILE *logfile;

struct thread_master *vtysh_master;
extern vector telvec;

#if 0
/* SIGTSTP handler.  This function care user's ^Z input. */
static void
sigtstp(int sig)
{
    /* Execute "end" command. */
    vtysh_execute("end", vty);

    /* Initialize readline. */
    rl_initialize();
    printf("\n");

    /* Check jmpflag for duplicate siglongjmp(). */
    if(! jmpflag)
    {
        return;
    }

    jmpflag = 0;

    /* Back to main command loop. */
    siglongjmp(jmpbuf, 1);
}
#endif

/* SIGINT handler.  This function care user's ^Z input.  */
static void
sigint(void)
{
    struct login_session *session;
    struct vty *vty;

    session = vty_console_session_get();
    vty = session->vty;

    /* Check this process is not child process. */
    if(! execute_flag)
    {
        rl_initialize();

        /*a cmd is not finish, we do not update display*/
        if(!vty->cmd_block)
        {
            rl_forced_update_display();
        }
    }

    vty = session->vty;

    if(session->client == SERVER)
    {
        vty->sig_int_recv = 1;
        vtysh_execute_sigint(vty);
        vty->sig_int_recv = 0;
    }
    else
    {
        vty->sig_int_recv = 1;
    }

#if 0
    fd = open(PATH_PING_PID, O_RDONLY);

    if(fd < 0)
    {
        fprintf(stdout, "Failed to open file "PATH_PING_PID"\n");
    }

    bytes = read(fd, buf_tmp, 10);

    if(bytes < 0)
    {
        fprintf(stdout, "File "PATH_PING_PID" read error\n");
    }

    pid = atoi(buf_tmp);

    mysigval.sival_int = pid;

    if(sigqueue(pid, SIGINT, mysigval) == -1)
    {
        perror("sigqueue error:");
    }

#endif
}

/* SIGTSTP handler. */
static void sigtstp (void)
{
	zlog_notice("%s: vtysh received SIGTSTP signal!", __func__);
	signal(SIGTSTP, SIG_IGN);
}

/* SIGPIPE handler. */
static void sigpipe (void)
{
	zlog_notice("%s: vtysh received SIGPIPE signal!", __func__);
	signal(SIGPIPE, SIG_IGN);
}

/* SIGTTOU handler. */
static void sigttou (void)
{
	zlog_notice("%s: vtysh received SIGTTOU signal!", __func__);
	signal(SIGTTOU, SIG_IGN);
}


struct quagga_signal_t vtysh_signals[] =
{
    {
        .signal  = SIGINT,
        .handler = &sigint,
        .caught  = 0,
    },
    
    {
        .signal  = SIGTSTP,
        .handler = &sigtstp,
        .caught  = 0,
    },    
    
    {
        .signal  = SIGPIPE,
        .handler = &sigpipe,
        .caught  = 0,
    }, 
    
    {
        .signal  = SIGTTOU,
        .handler = &sigttou,
        .caught  = 0,
    }, 
    
    {
        .signal  = SIGALRM,
        .handler = &high_pre_timer_dotick,
        .caught  = 0,
    },    
};
		
/* VTY shell options, we use GNU getopt library. */
struct option longopts[] =
{
    { "boot",                 no_argument,             NULL, 'b'},
    /* For compatibility with older zebra/quagga versions */
    { "eval",                 required_argument,       NULL, 'e'},
    { "command",              required_argument,       NULL, 'c'},
    { "daemon",               required_argument,       NULL, 'd'},
    { "echo",                 no_argument,             NULL, 'E'},
    { "dryrun",           no_argument,         NULL, 'C'},
    { "help",                 no_argument,             NULL, 'h'},
    { "noerror",          no_argument,         NULL, 'n'},
    { 0 }
};

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
static char *
vtysh_rl_gets(struct login_session *session)
{
    HIST_ENTRY *last;

    /* If the buffer has already been allocated, return the memory
     * to the free pool. */
    if(line_read)
    {
        free(line_read);
        line_read = NULL;
    }

    /* Get a line from the user.  Change prompt according to node.  XXX. */
    line_read = readline(vtysh_prompt());

    if(line_read == NULL)
    {
        return NULL;
    }

    if(strlen(line_read) >= VTY_BUFSIZ)
    {
        zlog_err("line read long lines %d \n", strlen(line_read));
        return NULL;
    }

#if 0

    if(vty->t_timeout)
    {
        pthread_mutex_lock(&session_timer_lock);
        thread_cancel(vty->t_timeout);
        vty->t_timeout = NULL;
        pthread_mutex_unlock(&session_timer_lock);
    }

#endif

    if(session->timer_count > 0)
    {
        session->timer_count = 0;
    }

#if 0
    pthread_mutex_lock(&session_timer_lock);
    /*no need to add timer if session is busy*/
    vty->t_timeout =
        //thread_add_timer(vtysh_master, vty_timeout, vty, vty->v_timeout);
    pthread_mutex_unlock(&session_timer_lock);
#endif

    /* If the line has any text in it, save it on the history. But only if
     * last command in history isn't the same one. */
    if(line_read && *line_read)
    {
        using_history();
        last = previous_history();

        if(!last || strcmp(last->line, line_read) != 0)
        {
            add_history(line_read);
            append_history(1, history_file);
        }
    }

    return (line_read);
}

void *
thr_readline(void *arg)
{
    int ret;
    struct vty *vty;
    int delays = 30;
    int reboot_vty_flag = 0;
    prctl(PR_SET_NAME, " thr_readline");

    struct login_session *session_console = (struct login_session *) arg;
    vty = session_console->vty;
    while(1)
    {
        if(syncvty.vty_slot == VTY_MAIN_SLOT
                || syncvty.vty_slot == VTY_SLAVE_SLOT)
        {
            /*只有配置恢复或者批量同步完成才允许用户登入*/
            if(syncvty.self_status == VTYSH_UNREADY
                    || syncvty.self_status == VTYSH_START
                    || syncvty.self_status == VTYSH_BATCH_CONFIG_START
                    || syncvty.self_status == VTYSH_BATCH_CONFIG_RECV)
            {
                if(syncvty.vtysh_start_record <= 1 && syncvty.ms_x_flag == 0)
                {
                    usleep(10000);
                    continue;
                }
                /* vty 重启过，请求同步vty*/
                else if(syncvty.vty_slot == VTY_SLAVE_SLOT && syncvty.vtysh_start_record > 1 && reboot_vty_flag == 0)
                {
                    syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
                    syncvty.all_cards_ready_flag = 1;
                    vtysh_slave_req_sync_vty();  
                    reboot_vty_flag = 1;
                    
                }
            }
        }
        else if(syncvty.vty_slot == VTY_UNKNOW_SLOT
                && config_recover_finish == 0)
        {
            if(syncvty.vtysh_start_record <= 1 && syncvty.ms_x_flag == 0)
            {
                usleep(10000);
                continue;
            }
        }

        while(delays > 0 && syncvty.vtysh_start_record <= 1)
        {
            delays--;
            sleep(1);
            continue;
        }        

        if(session_console->client == CLIENT)
        {
            usleep(10000);
            continue;
        }
        /*Ctrl+D������һ��EOF. readline����EOF:1����������ǿյģ�����NULL
        2���������ǿգ�EOF��Ϊ���д���� vtysh_rl_gets����NULLʱcontinue*/
        if(!vtysh_rl_gets(session_console))
        {
            printf("\n");
            continue;
        }

        if(vty->node == AUTH_NODE)
        {
            vty_auth(vty, line_read);
        }
        else
        {
            vty->cmd_block = 1;
//            VTYSH_CMD_EXEC_LOCK;
            ret = vtysh_execute(line_read, vty);
//            VTYSH_CMD_EXEC_UNLOCK;
            if(ret == CMD_SUCCESS)
            {
                strcpy(vty->buf, line_read);
                vty_hist_add(vty);
            }

            vty->cmd_block = 0;
        }

        if(vty->node == AUTH_NODE && session_console->auth_prompt == 0)
        {
            system("stty -echo");
        }
        else
        {
            system("stty echo");
        }
    }

    history_truncate_file(history_file, 1000);
    return ((void *)0);
}

static void vty_thread_init(void)
{
    telvec = vector_init(1);
}


/************************** lipf add for trap write cmd 2018/11/6 *************************************/

static int vtysh_alarm_socket_msg_rcv(struct thread *t)
{
    gpnSockCommApiNoBlockProc();
    usleep (100000); //CPU sleep 100ms
    thread_add_event (vtysh_master, vtysh_alarm_socket_msg_rcv, NULL, 0);
	return 0;
}

static uint32_t gpnVtyshCoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
	return 1;
}


static uint32_t vtysh_gpn_function_init (void)
{
	uint32_t reVal;
	
	reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_VTYSH, (usrMsgProc)gpnVtyshCoreProc);
	if(reVal == 2)
	{
		return 2;
	}

	return 1;
}

static void vtysh_alarm_init(void)
{
	vtysh_gpn_function_init();
	thread_add_event(vtysh_master, vtysh_alarm_socket_msg_rcv, NULL, 0);

	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_SOFT_TYPE;

	ipran_alarm_port_register(&gPortInfo);
}

/*************************************** end **********************************************/


/* VTY shell main routine. */
int
main(int argc, char **argv, char **env)
{
    char *p;
    int opt;
    int dryrun = 0;
    int boot_flag = 0;
    char *homedir = NULL;
    struct thread thread;
    pthread_t tid;
    int err;
    struct login_session *session_console;
    struct sched_param param;

    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(1, &mask);  /* ��cpu1*/
    sched_setaffinity(0, sizeof(mask), &mask);

    param.sched_priority = 60;
#ifndef HAVE_KERNEL_3_0

    if(sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n	priority set: ");
    }

#endif
    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, SIGINT);

    /* Preserve name of myself. */
    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);
    /*enable syslog*/
    zlog_default = openzlog(progname,  ZLOG_VTYSH, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

    /* Option handling. */
    while(1)
    {
        opt = getopt_long(argc, argv, "be:c:dnEhC", longopts, 0);

        if(opt == EOF)
        {
            break;
        }

        switch(opt)
        {
            case 0:
                break;

            case 'b':
                boot_flag = 1;
                break;

            case 'e':
            case 'c':
            {
            }
            break;

            case 'd':
                break;

            case 'n':
                break;

            case 'E':
                break;

            case 'C':
                dryrun = 1;
                break;

            case 'h':
                break;

            default:
                break;
        }
    }

    vtysh_master = thread_master_create();

    /* Initialize user input buffer. */
    line_read = NULL;
    setlinebuf(stdout);

	/* Set console output width */
	system("stty cols 132");

    /* Signal and others. */
    signal_init(vtysh_master, array_size(vtysh_signals), vtysh_signals);
    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    high_pre_timer_init(1000, MODULE_ID_VTY);
    vtysh_sshd_init();

    vty_init_vtysh();
    vtysh_debug_init();
	vtysh_user_line_init();

    vtysh_host_init();
    /*Creat session list and creat consol snmp session*/
    vty_session_init();
    session_console = vty_console_session_creat();

    vty_cmd_msg_thread_creat();


    /*install command*/
    vtysh_install_cmd();  /*ע���������*/

    vty_vtysh_init(vtysh_master);

    vtysh_config_init(); /*����������*/

    vty_msg_init();

	vtysh_alarm_init();

    /* Start execution only if not in dry-run mode */
    if(dryrun)
    {
        return(0);
    }


    vtysh_init_sync();
    vtysh_init_slave();

    vtysh_get_slot();

    /*slave send start to ha*/
    vtysh_send_start_to_ha();

    vtysh_send_start_record_ha();

    vty_thread_init();

    /*
     * Setup history file for use by both -c and regular input
     * If we can't find the home directory, then don't store
     * the history information
     */
    homedir = vtysh_get_home();

    if(homedir)
    {
        snprintf(history_file, sizeof(history_file), "%s/.history_quagga", homedir);

        if(read_history(history_file) != 0)
        {
            int fp;

            fp = open(history_file, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

            if(fp)
            {
                close(fp);
            }

            read_history(history_file);
        }
    }

    /* Boot startup configuration file. */
    if(boot_flag)
    {
        if(vtysh_read_config(integrate_default, NULL))
        {
            fprintf(stderr, "Can't open configuration file [%s]\n",
                    integrate_default);
            exit(1);
        }
        else
        {
            exit(0);
        }
    }

    vtysh_pager_init();

    vtysh_readline_init();

	if(ipc_recv_init(vtysh_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }
    if(ipc_recv_thread_start("vtyshIpcRev", MODULE_ID_VTY, SCHED_OTHER, -1, vtysh_msg_recv_n, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }
	
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }    

    telnetc_init();
    vty_hello(vty);
    vty_out(vty, "%sUser Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    (void)vtysh_line_config_update(session_console);
    /* Preparation for longjmp() in sigtstp(). */
    sigsetjmp(jmpbuf, 1);
    jmpflag = 1;

    err = pthread_create(&(session_console->tid_consol), NULL, thr_readline, (void *) session_console);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    err = pthread_create(&tid, NULL, vtysh_select_all_fd, NULL);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    high_pre_timer_add("vtyshTimer", LIB_TIMER_TYPE_LOOP, vtysh_timer_handle, NULL, 1000);
    /*main loop for main pthread*/
    while(thread_fetch(vtysh_master, &thread))
    {
        thread_call(&thread);
    }

    history_truncate_file(history_file, 1000);

    /* Rest in peace. */
}

