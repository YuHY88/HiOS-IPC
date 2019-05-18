#define _GNU_SOURCE
//#include <utmp.h>
//#include <unistd.h>
//#include <setjmp.h>
#include <sys/wait.h>
//#include <sys/un.h>
#include <sys/time.h>
//#include <sys/resource.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sched.h>
#include <pthread.h>
#include <errno.h>
#include "zebra.h"
#include "lib/thread.h"
#include "lib/vty.h"
#include "lib/memory.h"
#include "lib/command.h"
#include "lib/version.h"
#include "lib/getopt.h"
#include "lib/keychain.h"
#include "lib/msg_ipc.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"
#include "sigevent.h"


#include "lib/pkt_buffer.h"
#include "lib/pkt_type.h"
#include "lib/devm_com.h"
#include "lib/inet_ip.h"
#include "lib/pid_file.h"
#include "lib/syslog.h"
#include "ftm/pkt_ip.h"
#include "syslogd.h"

#define POWERDOWN_ALARM
#ifdef POWERDOWN_ALARM
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#ifndef NETLINK_MASK
#define NETLINK_MASK 3
#endif
#define MAX_MSGSIZE 1024
#endif

#define	MAXLINE		1024	/* log 信息的最大长度 */
#define	MAXSVLINE	240		/* 保存 log 信息的最大长度 */
#define DEFUPRI		(LOG_USER|LOG_NOTICE) /* 默认优先级 */
#define DEFSPRI		(LOG_KERN|LOG_CRIT)
#define TIMERINTVL	30		/* 检查刷新间隔 mark */
#define RATE_LIMIT_TIME 1 	/* 限速的单位时间，1秒 */

#define INTERNAL_NOPRI	0x10	/* the "no priority" priority */
#define TABLE_ALLPRI    0xFF	/* Value to indicate all priorities in f_pmask */

/* syslog_logmsg() 标记 */
#define IGN_CONS	0x001	/* 不输出到 console */
#define SYNC_FILE	0x002	/* 日志输出后同步内存中修改的文件到硬盘 */
#define ADDDATE		0x004	/* add a date to the message */
#define MARK		0x008	/* this message is a mark */

#define SYSLOG_FILE "/data/dat/syslog.txt"


struct filed
{
	enum location_type location;/* 日志输出位置，server、console、file、memory */
	enum maskflag_type maskflag;/* 过滤条件，upto、except、only */
	unsigned int priority;		/* 日志优先级 */
	short f_type;				/* 日志输出类型*/
	short f_file;				/* 文件描述符 */
	time_t f_time;				/* 日志最后写入的时间 */
	char f_fname[MAXNAMELEN];

	unsigned long f_server_ip;	/* 日志服务器 IP 地址 */
	unsigned short f_vpn;
	
	char f_prevline[MAXSVLINE];	/* 最后一条日志记录 */
	char f_lasttime[16];		/* 最后一条日志产生的时间 */
	char f_prevhost[MAXHOSTNAMELEN + 1];	/* 产生日志的主机名 */
	int f_prevpri;				/* 最后一条日志优先级 */
	int f_prevlen;				/* 最后一条日志的长度 */
	int f_prevcount;			/* 最后一条日志的重复计数 */
	int f_repeatcount;			/* 重复信息的数量 */
	int f_flags;				/* 存储附加标记 */

	int f_fullflag;				/* 文件是否已满 */
	long f_pos;					/* 文件的当前位置 */
};

time_t repeatinterval[] = { 30, 60 };	/* # of secs before flush */
#define	MAXREPEAT		((int)((sizeof(repeatinterval) / sizeof(repeatinterval[0])) - 1))
#define	REPEATTIME(f)	((f)->f_time + repeatinterval[(f)->f_repeatcount])
#define	BACKOFF(f)		{ if (++(f)->f_repeatcount > MAXREPEAT) \
							(f)->f_repeatcount = MAXREPEAT; \
						}

/* values for f_type */
#define F_UNUSED	0	/* unused entry */
#define F_FILE		1	/* regular file */
#define F_TTY		2	/* terminal */
#define F_CONSOLE	3	/* console terminal */
#define F_FORW		4	/* remote machine */
#define F_USERS		5	/* list of users */
#define F_WALL		6	/* everyone logged on */
#define F_FORW_SUSP	7	/* suspended host forwarding */
#define F_FORW_UNKN	8	/* unknown host forwarding */
#define F_PIPE		9	/* named pipe */

#define SYSLOG_SERVER_PORT	514
#define PIDFILE_MASK 0644

const char *type_name[] = {
	"UNUSED", "FILE", "TTY", "CONSOLE",
	"FORW", "USERS", "WALL", "FORW(SUSPENDED)",
	"FORW(UNKNOWN)", "PIPE"
};


static char    local_host_name[MAXHOSTNAMELEN + 1] = "hios"; /* our hostname */
static time_t  now;
static int     MarkInterval = 20 * 60;	/* interval between marks in seconds */
static int     MarkSeq = 0;				/* mark sequence number */
static int     syslog_nodetach = 0;		/* 前台运行 syslog */
static int     syslog_dbg = 0;		/* debug 模式 */
static int     priority = 0;
static int     nlogs = -1;
static unsigned int trans_rule_count = 0;	/* 默认规则 log id */
static struct filed syslog_file;
static struct filed record_logfile;;


struct filed         *Files = NULL;
struct thread_master *syslog_master = NULL; /* 主线程 */
//struct thread        *ptimer = NULL;        /* 定时器线程 */
int   memsize  = DEFAULT_MEM_SIZE;          /* 日志缓存大小 */
int   filesize = DEFAULT_FILE_SIZE;         /* 日志文件大小 */
char *pid_file = (char *)PATH_SYSLOG_PID;
char *config_file = NULL;                   /* 配置文件 */
char *progname = NULL;                      /* 进程名 */
unsigned int devicehandle = 0;

/* syslog 接收 log 的 ipc */
extern int ipc_syslog_id;
extern int log_timestamp_en;
extern int log_record_en;
extern struct log_item   *item;
extern struct _ratelimit  rate_limit[];		/* 限速规则 */
extern int log_to_monitor[2][USER_NUM];		/* 输出到当前终端的 fd */
extern pthread_mutex_t mutex_log;

CODE prinames[] =
{
	{ "alert", LOG_ALERT },
	{ "crit", LOG_CRIT },
	{ "debug", LOG_DEBUG },
	{ "emerg", LOG_EMERG },
	{ "error", LOG_ERR },
	{ "info", LOG_INFO },
	{ "notice", LOG_NOTICE },
	{ "warn", LOG_WARNING }, 	   /* DEPRECATED */
	{ "warning", LOG_WARNING },
	{ NULL, -1 }
};

CODE facnames[] =
{
   { "auth", LOG_AUTH },
   { "authpriv", LOG_AUTHPRIV },
   { "cron", LOG_CRON },
   { "daemon", LOG_DAEMON },
   { "ftp", LOG_FTP },
   { "kern", LOG_KERN },
   { "lpr", LOG_LPR },
   { "mail", LOG_MAIL },
   { "mark", INTERNAL_MARK },	   /* INTERNAL */
   { "news", LOG_NEWS },
   { "security", LOG_AUTH },	   /* DEPRECATED */
   { "syslog", LOG_SYSLOG },
   { "user", LOG_USER },
   { "uucp", LOG_UUCP },
   { "local0", LOG_LOCAL0 },
   { "local1", LOG_LOCAL1 },
   { "local2", LOG_LOCAL2 },
   { "local3", LOG_LOCAL3 },
   { "local4", LOG_LOCAL4 },
   { "local5", LOG_LOCAL5 },
   { "local6", LOG_LOCAL6 },
   { "local7", LOG_LOCAL7 },
   { NULL, -1 }
};

struct option longopts[] = 
{
    { "debug",         no_argument,       NULL, 'd'},
    { "config_file",   required_argument, NULL, 'f'},
    { "host name",     required_argument, NULL, 'l'},
    { "mark interval", required_argument, NULL, 'm'},
    { "no detach",     required_argument, NULL, 'n'},
    { "print version", required_argument, NULL, 'v'},
    { "help",          required_argument, NULL, 'h'},
    { 0 }
};
	
#define MAXLOGNUM  500

typedef struct queue   
{  
    char *pbase[MAXLOGNUM];  
    int   front;    // first element of queue  
    int   rear;     // last element of queue
    int   maxsize;  // max size of log number  
}QUEUE,*PQUEUE;  


static QUEUE syslog_filequeue;

static int syslog_creatqueue(PQUEUE queue)  
{  
	int i;
	
	for(i = 0; i < MAXLOGNUM; ++i)
	{
		queue->pbase[i] = XMALLOC (MTYPE_LOG_ITEM, MAXLINE);
		if(NULL == queue->pbase)  
		{  
			printf("Memory allocation failure");  
			exit(-1);        //exit  
		}
	}	
	
    queue->front = 0;           
    queue->rear = 0;  
    queue->maxsize = MAXLOGNUM;  
	
	return 0;
} 
 

static int syslog_fullqueue(PQUEUE queue)  
{  
    if(queue->front == (queue->rear+1) % queue->maxsize)    // queue is full ???  
        return 1;  
    else  
        return 0;  
} 

 
static int syslog_emptyqueue(PQUEUE queue)  
{  
    if(queue->front == queue->rear)    // queue is empty ???  
        return 1;  
    else  
        return 0;  
}  


static int syslog_enqueue(PQUEUE queue, char *val)  
{  
	unsigned int loglen = 0;
	
    if(syslog_fullqueue(queue))  
	{
        return -1; 
	}		
    else  
    {  
		if(val == NULL)
		{
			printf("error logline, exiting \n");
			return -1;
		}
		loglen = strlen(val);
		if(loglen > MAXLINE)
			loglen = MAXLINE;
		
		memset(queue->pbase[queue->rear], 0, MAXLINE);
		memcpy(queue->pbase[queue->rear], val, loglen);
        queue->rear = (queue->rear+1) % queue->maxsize;  
		
        return 0;  
    }  
}  


static char *syslog_dequeue(PQUEUE queue)  
{
	char * val = NULL;
    if(syslog_emptyqueue(queue))  
    {  
        return NULL;  
    }  
    else  
    {  
        val = queue->pbase[queue->front];  
        queue->front = (queue->front+1) % queue->maxsize;
        return val;  
    }  
}



/* syslog 输出调试信息，debug 模式有效 */
static void syslog_dbg_print(const char *fmt, ...)
{
	va_list ap;

	if (!syslog_dbg)
	{
		return;
	}

	va_start (ap, fmt);
	vfprintf (stdout, fmt, ap);
	va_end (ap);

	fflush (stdout);
	return;
}


/*
 * 函数名: syslog_backup_file
 * 功  能: 备份文件，把文件 .bak 的文件
 * 参  数: filename: 文件名称
 * 返回值: 无
*/
static void syslog_backup_file(const char *filename)
{
	char cmd[128];
	char file_sav[MAXNAMELEN];
	
	syslog_dbg_print ((char *)"SYSLOG: bakup the file :%s \n", filename);
	strcpy (file_sav, filename);
	strcat (file_sav, ".bak");
	unlink (file_sav);

	snprintf (cmd, sizeof(cmd) - 1, "cp %s %s", filename, file_sav);
	cmd[sizeof cmd - 1] = '\0';
	system (cmd);

	unlink (filename);
}


/* 输出 syslogd 的错误，所有错误都写入文件 */
void syslog_logerror(const char *fmt, ...)
{
	int 		ret = 0;
	int			pri = DEFUPRI;
	char		buf[1024];
	char		buf_tmp[1024];
	va_list		ap;
	time_t 		cur_time;
	char        *timestamp = NULL;
	struct stat st;

	va_start (ap, fmt);
	vsprintf (buf_tmp, fmt, ap);
	va_end (ap);

	time (&cur_time);
	pri = LOG_ERR;

	memset(buf, 0, sizeof(buf));

	/* 按照 syslog 协议标准组包,即: <PRI>TIMESTAM HOSTNAME TAG CONTENT */
	timestamp = ctime (&cur_time) + 4;
	timestamp[15] = '\0';

	snprintf (buf, sizeof (buf), "<%d>%s %s syslog[%d]: %s\r\n", 
		pri, timestamp, local_host_name, getpid(), buf_tmp);

	syslog_dbg_print ((char *)"%s\n", buf);
	//syslog_logmsg (pri, buf, ADDDATE);

	/* 判断文件是否存在，不存在则创建 */
	if(access(syslog_file.f_fname, F_OK) != 0)
	{
		close (syslog_file.f_file);
		syslog_file.f_file =open (syslog_file.f_fname, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);
		printf("%s[%d]: SYSLOG creat log file %s", __FILE__, __LINE__, syslog_file.f_fname);
		if(syslog_file.f_file < 0)
		{
			printf("%s[%d]: SYSLOG creat log file %s error", __FILE__, __LINE__, syslog_file.f_fname);
			return ;
		}
	}

	ret = fstat (syslog_file.f_file, &st);
	if (ret != 0)
	{
		printf ("%s[%d]: SYSLOG stat the file  error", __FILE__, __LINE__);
		
		return ;
	}
	
	/* 限制文件大小为 1M */
	if (st.st_size  > 1000 * 1000)
	{
		syslog_dbg_print("SYSLOG file is %d kilobyes, but it is controled in %d kilobytes \n",
			(int)st.st_size, 1000 * 1000);
		
		syslog_backup_file(syslog_file.f_fname);
	}
	
	ret = write (syslog_file.f_file, buf, strlen(buf));
	if(ret < 0)
	{
		printf("%s[%d]: SYSLOG can not write to the file %s, we close it", 
			__FILE__, __LINE__, syslog_file.f_fname);
		close (syslog_file.f_file);
	}
	else
	{
		fsync (syslog_file.f_file);
	}

	return ;
}


/* 发送 log 到日志服务器 */
static int syslog_send_pkt_msg(unsigned int dest_ip, unsigned short dest_port, unsigned short vpn, char *msg, int msg_len)
{
	union pkt_control pktcontrol;
	int   ret;

	memset(&pktcontrol, 0, sizeof(pktcontrol));
	//pktcontrol.ipcb.sip = source_ip;	
	//pktcontrol.ipcb.sport = source_port;
	//pktcontrol.ipcb.dip = ntohl(dest_ip);
	pktcontrol.ipcb.dip = dest_ip;
	pktcontrol.ipcb.dport = dest_port;
	pktcontrol.ipcb.vpn = vpn;
	pktcontrol.ipcb.protocol = IP_P_UDP;
	pktcontrol.ipcb.ttl	= 64;
	pktcontrol.ipcb.is_changed = 1;
	pktcontrol.ipcb.tos = 3;

	ret = pkt_send(PKT_TYPE_UDP, &pktcontrol, msg, msg_len);
	if(ret == NET_FAILED)
	{
		syslog_logerror("%s[%d]: SYSLOG send ipc error", __FILE__, __LINE__);
		return -1;		
	}

	return 0;
}


/* 获取参数 -l 传入的主机名 */
static int syslog_set_hostname(char *hostname)
{
	if(hostname != NULL)
	{
		strcpy(local_host_name, hostname);
		printf("set local host name is: %s \n", local_host_name);	
		
		return 0;
	}
	
	printf("set local host name error, argv is NULL, hostname is: %s \n", local_host_name);	

	return -1;
}


/* 帮助信息 */
static void syslog_usage(char *progname, int status)
{
    if(status != 0)
    {
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
    }
    else
    {    
        printf("Usage : %s [OPTION...]\n\n\
	Daemon which manages SYSLOG version 3.\n\n\
	-d, --debug          Pint debug information (implies --no-detach)\n\
	-f, --config_file    Set configuration file name\n\
	-l, --hostname       Set host name in log information\n\
	-m, --markinterval   Specify timestamp interval in minutes (0 for no timestamping)\n\
	-n, --no fork        Do not enter daemon mode\n\
	-v, --version        Print program version\n\
	-h, --help           Display this help and exit\n\n\
	Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }
    exit(status);
}

/* 根据优先级获取优先级名称 */
static char *syslog_text_priority (int pri)
{
	static char res[20];
	CODE *c_pri;

	for (c_pri = prinames; c_pri->c_name && !(c_pri->c_val == LOG_PRI (pri)); c_pri++)
	{
		;
	}
	if (!c_pri->c_name)
	{
		return NULL;
	}
	strncpy (res, c_pri->c_name, 19);

	return res;
}


/* 限速，1s 内输出日志的条数 */
static int syslog_ratelimit_check(char *filename, time_t cur_time)
{
	int i;

	for(i = 0; i < LIMIT_RULE_NUM; ++i)
	{
		if(strcmp(rate_limit[i].localname, filename) == 0)
		{
			if(rate_limit[i].rate_start_time == 0)
			{
				rate_limit[i].rate_start_time = cur_time;
			}
RATE_CHECK_FLAG:
			if((cur_time - rate_limit[i].rate_start_time) < RATE_LIMIT_TIME)//时间在 1s 以内
			{
				if(rate_limit[i].rate_limit_count < rate_limit[i].rate_max_num)
				{
					rate_limit[i].rate_limit_count++;
					return 0;
				}
				else
				{
					return 1;
				}
			}
			else//超出1s
			{
				rate_limit[i].rate_start_time = cur_time;
				rate_limit[i].rate_limit_count = 0;	
				goto RATE_CHECK_FLAG;
			}
		}		
	}

	return 0;
}


/*
 * 函数名: syslog_send_log_to_server
 * 功  能: 发送日志消息到服务端
 * 参  数: f: filed 结构体
		msg_line: 日志消息
		cur_time: 产生日志的当前时间
 * 返回值: 无
*/
static int syslog_send_log_to_server(struct filed *f, char *msg_line, time_t cur_time)
{
	int msg_len;
	int ret = 0;
	
	syslog_dbg_print ((char *)"SYSLOG: fname: %s, msg_line: %s\n",f->f_fname, msg_line);

	f->f_time = cur_time;
	msg_len = strlen(msg_line);

	ret = syslog_send_pkt_msg(f->f_server_ip, SYSLOG_SERVER_PORT, f->f_vpn, msg_line, msg_len);
	if (ret == -1)
	{
		syslog_logerror("%s[%d]: SYSLOG send packt to server error, ip = %s, port = %d", 
			__FILE__, __LINE__, f->f_fname, SYSLOG_SERVER_PORT);

		return -1;
	}

	return 0;
}


/*
 * 函数名: syslog_file_is_full
 * 功  能: 检查文件是否超出设置的大小
 * 参  数: fd: 文件描述符
		file_size: 文件预设值，单位 KB
 * 返回值: -1: 检查失败
		0: 文件没有超出预设值大小
		1: 文件超出预设值大小
*/
static int syslog_file_is_full(int fd, int file_size)
{
	struct stat st;
	int ret;

	ret = fstat (fd, &st);
	if (ret != 0)
	{
		syslog_logerror ("%s[%d]: SYSLOG stat the file  error", __FILE__, __LINE__);
		
		return -1;
	}
	if (st.st_size  > file_size * 1000)
	{
		//syslog_dbg_print("SYSLOG file is %d kilobyes, but it is controled in %d kilobytes \n",
		//	(int)st.st_size, file_size * 1000);
		
		return 1;
	}
	
	return 0;
}


/*
 * 函数名: syslog_write_log_to_console
 * 功  能: 发送日志到console
 * 参  数: f: struct filed指针
           msg_line: 日志信息
           cur_time: 当前时间
           flags: 标记
 * 返回值: 无
*/
static int syslog_write_log_to_console(struct filed *f, char *msg_line, time_t cur_time, int flags)
{
	int ret = 0;
	struct iovec iov[2];

	if (flags & IGN_CONS )//如果不输出到 console，返回
	{
		syslog_dbg_print ((char *)"SYSLOG: this log do not output from console\n");
		return 0;
	}

	f->f_time = cur_time;
	iov[0].iov_base = msg_line;
	iov[0].iov_len = strlen(msg_line);
	iov[1].iov_base = (void *)"\r\n";
	iov[1].iov_len = 2;

	ret = writev (f->f_file, iov, 2);
	if(ret < 0)
	{
		syslog_logerror ("%s[%d]: SYSLOG write to the file %s error, close it",
			__FILE__, __LINE__, f->f_fname);
		
		close (f->f_file);
		/* 文件描述符失效则关闭重新打开 */
		if (errno == EBADF)
		{
			f->f_file = open (f->f_fname, O_WRONLY | O_APPEND | O_NOCTTY);
			if (f->f_file < 0)
			{
				f->f_type = F_UNUSED;
				syslog_logerror ("%s[%d]: SYSLOG open file %s error, close it",
					__FILE__, __LINE__, f->f_fname);
				
			}
		}
		else
		{
			f->f_type = F_UNUSED;
			syslog_logerror ("%s[%d]: SYSLOG send log to %s error", __FILE__, __LINE__, f->f_fname);			

			return -1;
		}
	}

	return 0;
}


/*
 * 函数名: syslog_write_log_to_file
 * 功  能: 发送日志到console
 * 参  数: msg_line: 日志信息
 * 返回值: 0: log 未输出到当前终端
 *         1: log 已输出到当前终端
*/
static int syslog_write_log_to_terminal(char *msg_line)
{
	unsigned int   i;
	struct iovec   iov[2];
	int            flag = 0;

	iov[0].iov_base = msg_line;
	iov[0].iov_len = strlen(msg_line);
	iov[1].iov_base = (void *)"\r\n";
	iov[1].iov_len = 2;
	
	for(i = 0; i < USER_NUM; ++i)
	{
		/* 输出到当前终端 */
		if(log_to_monitor[1][i] != 0)
		{
			/* 当前终端的文件描述符合法 */
			if(log_to_monitor[0][i] > 0)
			{
				if(writev(log_to_monitor[0][i], iov, 2) != -1)
				{
					flag = 1;
				}
				else
				{
					log_to_monitor[0][i] = 0;
					log_to_monitor[1][i] = 0;
				}
			}
			else
			{
				log_to_monitor[0][i] = 0;
				log_to_monitor[1][i] = 0;
			}
		}
	}

	return flag;
}


/*
 * 函数名: syslog_write_log_to_file
 * 功  能: 发送日志到console
 * 参  数: f: struct filed指针
           msg_line: 日志信息
           cur_time: 当前时间
           flags: 标记
 * 返回值: 无
*/
static int syslog_write_log_to_file(struct filed *f, char *msg_line, time_t cur_time, int flags)
{
	int ret = 0;
	struct iovec iov[2];
	unsigned int file_size = 0;

	if (f->f_file == -1)
	{
		syslog_logerror ("%s[%d]: SYSLOG we open the file %s error", __FILE__, __LINE__, f->f_fname);
		
		return -1;
	}

	file_size = ((strcmp(f->f_fname, SYSLOG_FILE_NAME) == 0) || (strcmp(f->f_fname, SYSLOG_RECORD) == 0)) ? filesize : memsize;
	if(syslog_file_is_full(f->f_file, file_size) == 1)
	{
		close (f->f_file);
		syslog_backup_file(f->f_fname);
		f->f_file =open (f->f_fname, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);
		if (f->f_file < 0)
		{
			f->f_file = -1;
			syslog_logerror ("%s[%d]: SYSLOG opening log file %s error\n", __FILE__, __LINE__, f->f_fname);

			return -1;
		}
	}	

	f->f_time = cur_time;
	iov[0].iov_base = msg_line;
	iov[0].iov_len = strlen(msg_line);
	iov[1].iov_base = (void *)"\r\n";
	iov[1].iov_len = 2;

	/* 判断文件是否存在，不存在则创建 */
	if(access(f->f_fname, F_OK) != 0)
	{
		close (f->f_file);
		f->f_file =open (f->f_fname, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);
		syslog_logerror("%s[%d]: SYSLOG creat log file %s", __FILE__, __LINE__, f->f_fname);
		if(f->f_file < 0)
		{
			syslog_logerror("%s[%d]: SYSLOG creat log file %s error", __FILE__, __LINE__, f->f_fname);
			return -1;
		}
	}
	
	ret = writev (f->f_file, iov, 2);
	if(ret < 0)
	{
		syslog_logerror("%s[%d]: SYSLOG can not write to the file %s, we close it", 
			__FILE__, __LINE__, f->f_fname);
		close (f->f_file);
		f->f_type = F_UNUSED;
	}
	else if (flags & SYNC_FILE)
	{
		fdatasync(f->f_file);
	}
	

	f->f_pos = lseek (f->f_file, 0L, SEEK_CUR);
	
	return 0;
}

static int syslog_write_log_to_filequeue(char *msg_line)
{
	int ret = 0;
	int i = 0;
	char line[1024];
	int fd = 0;
	unsigned int file_size = 0;
	unsigned int msglen = 0;
	char *logline = NULL;
	int enqueue_flag = 0;
	
	if(msg_line == NULL)// sync file from queue to flash
	{
		enqueue_flag = 1;
		goto SYNCGILE;
	}

	msglen = strlen(msg_line);
	if(msglen > 1020)
		msglen = 1020;
	memcpy(line, msg_line, msglen);
	line[msglen] = '\r';
	line[msglen+1] = '\n';
	line[msglen+2] = '\0';
	if(!syslog_fullqueue(&syslog_filequeue))
	{
		syslog_enqueue(&syslog_filequeue, (char *)line);
		return 0;
	}
	else
	{
SYNCGILE:

		fd = open (SYSLOG_FILE_NAME, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);

		if (fd == -1)
		{
			syslog_logerror ("%s[%d]: SYSLOG we open the file %s error", __FILE__, __LINE__, SYSLOG_FILE_NAME);
			
			return -1;
		}
		lseek (fd, 0L, SEEK_END);
		file_size = filesize;
		for(i = 0; i < syslog_filequeue.maxsize; ++i)
		{
			if(syslog_file_is_full(fd, file_size) == 1)
			{
				close (fd);
				syslog_backup_file(SYSLOG_FILE_NAME);
				fd =open (SYSLOG_FILE_NAME, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);
				if (fd < 0)
				{
					syslog_logerror ("%s[%d]: SYSLOG opening log file %s error\n", __FILE__, __LINE__, SYSLOG_FILE_NAME);
			
					return -1;
				}
				lseek (fd, 0L, SEEK_END);
			}	

			logline = syslog_dequeue(&syslog_filequeue);
			if(logline == NULL)
			{
				close (fd);
				return 0;
			}
			ret = write (fd, logline, strlen(logline));
			if(ret < 0)
			{
				syslog_logerror("%s[%d]: SYSLOG can not write to the file %s, we close it", 
					__FILE__, __LINE__, SYSLOG_FILE_NAME);
				close (fd);
			}

			if(enqueue_flag == 0)
			{
				syslog_enqueue(&syslog_filequeue, line);
				enqueue_flag = 1;
			}			
		}

		fdatasync(fd);
		close(fd);
		
		return 0;
	}
}


/*
 * 函数名: syslog_fprintlog
 * 功  能: 发送日志到console
 * 参  数: f: struct filed指针
           msg1: 日志信息
           from:  
           flags: 标记
 * 返回值: 无
*/
static void syslog_fprintlog (register struct filed *f, int flags, char *msg1)
{
	char            repbuf[80];
	static char     msg[MAXLINE];
	char            *tmpmsg = NULL;
	char            *timestamp = NULL;
	struct tm       *tm;
	time_t          now;
	int             log_to_terminal = 0;//默认输出到console

	if (msg1)
	{
		tmpmsg = msg1;
	}
	else if (f->f_prevcount > 1)
	{
		(void)snprintf (repbuf, sizeof (repbuf),
			"last message repeated %d times", f->f_prevcount);
		tmpmsg = repbuf;
	}
	else
	{
		tmpmsg = f->f_prevline;
	}

	time (&now);
	if(f->location == l_server)/* 发送日志到 server */
	{
		/* 按照 syslog 协议标准组包,即: <PRI>TIMESTAM HOSTNAME TAG CONTENT */
		memset(msg, 0, sizeof(msg));
		/* 加上时间戳 */
		timestamp = ctime (&now) + 4;
		timestamp[15] = '\0';
		snprintf(msg, sizeof (msg), "<%d>%s %s %s\n", priority, timestamp, local_host_name, tmpmsg);
	}
	else
	{
		tm = localtime (&now);
		//设置时间戳
		if(log_timestamp_en) 
		{
			snprintf (msg, sizeof (msg), "<%s><%d-%02d-%02d %02d:%02d:%02d><%s><%s>", local_host_name,
				tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,tm->tm_min, tm->tm_sec, 
				syslog_text_priority (f->f_prevpri), tmpmsg);
		}
		else
		{
			snprintf (msg, sizeof (msg), "<%s><%s><%s>", local_host_name,
				syslog_text_priority (f->f_prevpri), tmpmsg);
		}
	}

	/* 限速规则过滤 */
	if(syslog_ratelimit_check(f->f_fname, now))
	{
		syslog_logerror("%s[%d]: SYSLOG ratelimit check: time = %s, file name = %s", 
			__FILE__, __LINE__, ctime (&now) + 4, f->f_fname);
			
		return ;
	}

	syslog_dbg_print ((char *)"SYSLOG: type = %s, msg: %s\n",type_name[f->f_type], msg);

	switch (f->f_type)
	{
	case F_UNUSED:/* 无意义的消息直接丢弃 */
		f->f_time = now;
		break;
	case F_FORW:/* 发送日志信息到远程服务器 */
		syslog_send_log_to_server(f, msg, now);
		break;
	case F_CONSOLE:/* 发送日志到 console */
	case F_TTY:/* 发送日志到 terminal */
		f->f_time = now;
		log_to_terminal = syslog_write_log_to_terminal(msg);
		if(log_to_terminal == 0)
		{
			syslog_write_log_to_console(f, msg, now, flags);
		}
		break;
	case F_FILE:/* 写日志到文件或者内存 */
		if(strcmp(f->f_fname, SYSLOG_FILE_NAME) == 0)
		{
			f->f_time = now;
			syslog_write_log_to_filequeue(msg);
		}
		else
		{
			syslog_write_log_to_file(f, msg, now, flags);
		}
		break;
	}/* switch */
	if (f->f_type != F_FORW_UNKN)
	{
		f->f_prevcount = 0;
	}
	
	return;
}


/* 把优先级解析成字符串，格式为: auth.emerg */
static char *syslog_textpri (int pri)
{
	static char res[128];
	CODE *c_pri, *c_fac;

	for (c_fac = (CODE *) facnames; c_fac->c_name && !(c_fac->c_val == LOG_FAC (pri) << 3); c_fac++);
	for (c_pri = (CODE *) prinames; c_pri->c_name && !(c_pri->c_val == LOG_PRI (pri)); c_pri++);

	snprintf (res, sizeof (res), "%s.%s<%d>", c_fac->c_name, c_pri->c_name, pri);

	return res;
}


/* log 过滤 */
static void syslog_printbycfg(unsigned int prilev, char *msg, int flags)
{
	register struct filed *f;
	int 		lognum;
	int 		msglen;
	int 		maskflag;
	int			ret = 0;
    char 		*timestamp;

	time (&now);
	timestamp = ctime (&now) + 4;

	msglen = strlen(msg);

	ret = pthread_mutex_lock(&mutex_log);
	if(ret != 0)
	{
		syslog_logerror("%s[%d]: %s: SYSLOG pthread_mutex_lock error, ret = %d", 
			__FILE__, __LINE__, __func__, ret);

		return ;
	}
	for (lognum = 0; lognum <= nlogs; lognum++)
	{
		f = &Files[lognum];
		syslog_dbg_print ((char *)"SYSLOG: type = %d, flags = %d, prilev = %d, maskflga = %d, priority = %d\n",	\
			f->f_type, flags, prilev, f->maskflag, f->priority);
	
		maskflag = f->maskflag;
		/* 跳过不符合条件的日志 */
		if ((maskflag == upto) && (f->priority <  prilev))/*upto*/
		{
			continue ;
		}		
		if ((maskflag == only) && (f->priority != prilev) )/*eq*/
		{
			continue ;
		}		
		if ((maskflag == except) && (f->priority == prilev))/*exct*/
		{	
			continue ;
		}

		syslog_dbg_print ((char *)"SYSLOG: mark:%d, msglen:%d-%d, msg:%s-%s, from:%s.\n",(flags & MARK), 
			msglen, f->f_prevlen, msg, f->f_prevline, f->f_prevhost);
		/* 禁止输出重复的信息 */
		if ((msglen == f->f_prevlen) && (!strcmp (msg, f->f_prevline)))
		{
			(void) strncpy (f->f_lasttime, timestamp, 15);
			f->f_prevcount++;
			syslog_dbg_print ((char *)"SYSLOG: msg repeated %d times, %ld sec of %ld.\n", 
				f->f_prevcount, now - f->f_time, repeatinterval[f->f_repeatcount]);
			
			/* 如果 syslog_domark() 已经记录了该 log，并且超出刷新间隔，现在刷新输出 */			
			if (now > REPEATTIME (f))
			{
				syslog_fprintlog (f, flags, (char *) NULL);
				BACKOFF (f);
			}
		}
		else
		{
			/* 新的日志信息则输出并保存 */
			if (f->f_prevcount)
			{
				syslog_fprintlog (f, 0, (char *) NULL);
			}
			f->f_prevpri = prilev;
			f->f_repeatcount = 0;
			strncpy (f->f_lasttime, timestamp, 15);

			if (msglen < MAXSVLINE)//日志信息没有超出保存日志的最大长度则保存
			{
				f->f_prevlen = msglen;
				strcpy (f->f_prevline, msg);
				syslog_fprintlog (f, flags, (char *) NULL);
			}
			else
			{
				f->f_prevline[0] = 0;
				f->f_prevlen = 0;
				syslog_fprintlog (f, flags, msg);
			}
		}
	}
	ret = pthread_mutex_unlock(&mutex_log);
	if(ret != 0)
	{
		syslog_logerror("%s[%d]: %s: SYSLOG pthread_mutex_unlock error, ret = %d", 
			__FILE__, __LINE__, __func__, ret);
	}

	return ;
}


/* log 完整性检查，提取优先级 */
static void syslog_logmsg (int pri, char *msg, int flags)
{
	unsigned int        prilev;
//	int                      msglen;
//	char                   *timestamp = NULL;
//	time_t                 now;

	syslog_dbg_print ((char *)"SYSLOG: %s, flags %x, msg %s\n", syslog_textpri (pri), flags, msg);
#if 0
	/* 检查log完整性 */
	msglen = strlen (msg);
	if (msglen < 16 || msg[3] != ' ' || msg[6] != ' ' ||
		msg[9] != ':' || msg[12] != ':' || msg[15] != ' ')
	{
		flags |= ADDDATE;
	}

	(void) time (&now);
	if (flags & ADDDATE)
	{
		timestamp = ctime (&now) + 4;
	}
	else
	{
		timestamp = msg;
		msg += 16;
		msglen -= 16;
	}
#endif
	/* 删除时间戳 */
	msg += 16;

	/* 提取优先级 */
	prilev = LOG_PRI (pri);
	if((prilev == record_logfile.priority) && (log_record_en == 1))
	{
		syslog_fprintlog (&record_logfile, flags, msg);

		return ;
	}
	
	syslog_printbycfg(prilev, msg, flags);
}


/* 输出 log 到日志文件 */
static void syslog_printline (char *msg)
{
	register char *p, *q;
	register unsigned char c;
	char line[MAXLINE + 1];

	/* test for special codes */
	//priority = DEFUPRI;
	p = msg;

	/* 解析优先级 facility.severity */
	if (*p == '<')
	{
		priority = 0;
		while (isdigit (*++p))
		{
			priority = 10 * priority + (*p - '0');
		}
		if (*p == '>')
		{
			++p;
		}		
	}
	
	if (priority & ~(LOG_FACMASK | LOG_PRIMASK))
	{
		priority = DEFUPRI;
	}
	
	/* 丢弃 facility 错误的log */
	if (priority < (LOG_OFFSET << 3))
	{
		return ;
	}
	
	/* 特殊字符处理，去除不可打印的字符等 */
	memset (line, 0, sizeof (line));
	q = line;
	while ((c = *p++) && q < &line[sizeof (line) - 4])
	{
		if (c == '\n')
		{
			*q++ = ' ';
		}
		else if (c == '\t')
		{
			*q++ = c;
		}
		else if (c < 040)
		{
			*q++ = '^';
			*q++ = c ^ 0100;
		}
		else if (c == 0177 || (c & 0177) < 040)
		{
			*q++ = '\\';
			*q++ = '0' + ((c & 0300) >> 6);
			*q++ = '0' + ((c & 0070) >> 3);
			*q++ = '0' + (c & 0007);
		}
		else
		{
			*q++ = c;
		}
	}
	*q = '\0';
	syslog_logmsg (priority, line, SYNC_FILE);
	
	return;
}


static void syslog_reapchild ()
{
	int saved_errno = errno;

	(void) signal (SIGCHLD, syslog_reapchild);	/* reset signal handler -ASP */
	wait ((int *) 0);

	errno = saved_errno;
}


/* 定时器，定时刷新未输出的日志信息 */
static int syslog_domark (void *para)
{
	register struct filed *f;
	int lognum;
	int ret = 0;

	if (MarkInterval > 0)
	{
		now = time (0);
		MarkSeq += TIMERINTVL;
		if (MarkSeq >= MarkInterval)
		{
			// syslog_logmsg (LOG_INFO, "-- MARK --", local_host_name, ADDDATE | MARK);
			MarkSeq = 0;
		}
		/* 尝试加锁，成功则刷新未输出的日志，否则不处理 */
		ret = pthread_mutex_trylock(&mutex_log);
		if(ret == 0)
		{
			for (lognum = 0; lognum <= nlogs; lognum++)
			{
				f = &Files[lognum];
				if (f->f_prevcount && now >= REPEATTIME (f))
				{
					syslog_dbg_print ((char *)"flush %s: repeated %d times, %ld sec.\n",
						type_name[f->f_type], f->f_prevcount, repeatinterval[f->f_repeatcount]);
					syslog_fprintlog (f, 0, (char *) NULL);
					BACKOFF (f);
				}
			}

			syslog_write_log_to_filequeue(NULL);
			ret = pthread_mutex_unlock(&mutex_log);
			if(ret != 0)
			{
				syslog_logerror("%s[%d]: %s: SYSLOG pthread_mutex_unlock error, ret = %d", 
					__FILE__, __LINE__, __func__, ret);
			}
		}
		else
		{
			syslog_logerror("%s[%d]: %s: SYSLOG pthread_mutex_trylock error, ret = %d", 
				__FILE__, __LINE__, __func__, ret);
		}
	}

	//ptimer = thread_add_timer(syslog_master, syslog_domark, 0, TIMERINTVL);
	return 0;
}

/* SIGUSR1 显示 debug 状态，仅debug模式有效 */
static void syslog_dbg_switch ()
{
	syslog_dbg_print ((char *)"Switching debugging_on to %s\n", (syslog_dbg == 0) ? "true" : "false");
	signal (SIGUSR1, syslog_dbg_switch);
}

/* 退出 syslog */
static void syslog_die (int sig)
{
	struct filed  *f = NULL;
	int           lognum = 0;

	/* 输出未输出的日志信息 */
	for (lognum = 0; lognum <= nlogs; lognum++)
	{
		f = &Files[lognum];
		if (f->f_prevcount)
		{
			syslog_fprintlog (f, 0, (char *) NULL);
		}
	}

	if (sig)
	{
		syslog_logerror("%s[%d]: SYSLOG exiting on signal %d", __FILE__, __LINE__, sig);
	}

	unlink(pid_file);
	exit (0);
}


/* 由 log 级别名称获取数值 */
int syslog_decode (char *name, CODE *codetab)
{
	register CODE *c = NULL;
	register char         *p = NULL;
	char                  buf[80];

	syslog_dbg_print ((char *)"SYSLOG: symbolic name: %s\n", name);
	if (isdigit (*name))
	{
		return (atoi (name));
	}
	
	strncpy (buf, name, 79);
	for (p = buf; *p; p++)
	{
		if (isupper (*p))
		{
			*p = tolower (*p);
		}
	}
	for (c = codetab; c->c_name; c++)
	{
		if (!strncmp (buf, c->c_name, 3))
		{
			syslog_dbg_print ((char *)"SYSLOG: priority val = %d\n", c->c_val);
			
			return (c->c_val);
		}
	}
	
	syslog_logerror("%s[%d]: SYSLOG get code by name error", __FILE__, __LINE__);
	
	return (-1);
}


/* 添加默认规则 */
void syslog_trans_rule_to_item(char *cbuf)
{
	struct log_item  *src = NULL;
	register char    *p = NULL;
	register char    *q = NULL;
	char             *bp = NULL;
	int              pri;
	int              ret = 0;
	char             buf[30]={0};

	syslog_dbg_print ((char *)"SYSLOG trans rule is: %s\n", cbuf);
	src = (struct log_item *) XMALLOC (MTYPE_LOG_ITEM, sizeof (struct log_item));
	if (!src)
	{
		syslog_logerror ("%s[%d]: SYSLOG can not alloc memory", __FILE__, __LINE__);
		return ;
	}

	memset (src, 0, sizeof (struct log_item));

	/* 跳过空格和\t */
	for (p = cbuf; *p == '\t' && *p == ' '; p++);

	q = p; 

	/* collect priority name */
	for (bp = buf; *q && !strchr ("\t ,;", *q);)
	{
		*bp++ = *q++;
	}
	*bp = '\0';

	/* skip cruft */
	while (strchr (",;", *q))
	{
		q++;
	}

	/* syslog_decode priority name */
	if((*buf == '!')||(*buf == '='))
	{
		p = &buf[1];
		pri = syslog_decode (p, prinames);
		if(pri < 0)
		{
			syslog_logerror("%s[%d]: SYSLOG parse the syslog_rule.conf pri error *p=%c", 
				__FILE__, __LINE__, *p);
			return ;
		}
		if(*buf == '!')
		{
			syslog_sec_strcpy(src->maskflagname,  MAXNAMELEN+1, (char *)"except");
		}
		else
		{
			syslog_sec_strcpy(src->maskflagname,  MAXNAMELEN+1, (char *)"only");
		}
	}
	else
	{
		p = buf;
		pri = syslog_decode (p, prinames);
		if(pri < 0)
		{
			syslog_logerror("%s[%d]: SYSLOG parse the syslog_rule.conf pri error *p=%c", 
				__FILE__, __LINE__, *p);
			
			return ;
		}
		syslog_sec_strcpy(src->maskflagname,  MAXNAMELEN+1, (char *)"upto");
	}
	syslog_sec_strcpy(src->priname, MAXNAMELEN+1, p);
	p = q;

	/* skip to action part */
	while (*p == '\t' || *p == ' ')
	{
		p++;
	}
	syslog_sec_strcpy(src->locname, MAXNAMELEN + 1, p);
	src->log_id = ++trans_rule_count;
	ret = syslog_add_log(NULL, src, exact_match);

	if (ret != CMD_SUCCESS)
	{
		syslog_logerror("%s[%d]: SYSLOG add default log rule error", __FILE__, __LINE__, *p);
		
		XFREE (MTYPE_LOG_ITEM, src);
	}
	syslog_reload (item);

	return;
}


/* 分配保存配置的数组 */
static void syslog_allocate_log ()
{
	struct filed *ptr = NULL;
	
	/* 初始化 filed 结构体数组 */
	if (nlogs == -1)
	{
		ptr = (struct filed *) malloc (sizeof (struct filed));
		if (ptr == NULL)
		{
			syslog_logerror("%s[%d]: SYSLOG cannot initialize log structure", __FILE__, __LINE__);

			return ;
		}
	}
	else
	{
		/* 增加数组的长度 */
		ptr = (struct filed *)realloc (Files, (nlogs + 2) * sizeof (struct filed));
		if( ptr == NULL)
		{
			syslog_logerror("%s[%d]: SYSLOG cannot grow log structure", __FILE__, __LINE__);

			return ;
		}
	}
	/* 初始化数组元素和数量 */
	Files = ptr;
	++nlogs;
	memset (&Files[nlogs], '\0', sizeof (struct filed));

	return ;
}

void syslog_enforce_log_item (struct log_item *ptr)
{
	struct filed *f;
	char locate = 0;

	if(!ptr)
	{
		return;
	}
	syslog_allocate_log ();
	f = &Files[nlogs];
	f->priority = ptr->priority;
	f->maskflag = ptr->maskflag;
	f->location = ptr->location;
	strcpy (f->f_fname, ptr->filename);
	locate = ptr->location;
	syslog_dbg_print ((char *)"SYSLOG: locate = %d, filename = %s, priority = %d, maskflag = %d\n", 
		locate, f->f_fname, f->priority, f->maskflag);
	switch (locate)
    {
    case l_server:
		f->f_server_ip = inet_strtoipv4(ptr->filename);
		f->f_vpn = ptr->vpn;
		f->f_type = F_FORW;
		break;
    case l_memory:
    case l_console:
    case l_file:
		if(strcmp(f->f_fname, SYSLOG_FILE_NAME) != 0)//not open logfile
		{
			f->f_file = open (ptr->filename, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);
			if (f->f_file < 0)
			{
				f->f_file = -1;
				syslog_logerror("%s[%d]: SYSLOG open log file %s error", 
					__FILE__, __LINE__, ptr->filename);
				break;
			}
		}
		
		f->f_type = F_FILE;

		if (strcmp (ptr->filename, SYSLOG_CONSOLE_NAME) == 0)
		{
			f->f_type = F_CONSOLE;
		}
		break;
	default:
		break;
    }

	return;
}

/* 清空 filed 结构体数组 */
void syslog_del_all_conf (void)
{
	struct filed *f;
	int lognum;
	
	/* 关闭所有打开的文件描述符 */
	if (nlogs > -1)
    {
		syslog_dbg_print ((char *)"SYSLOG: delete all log item in Files.\n");

		for (lognum = 0; lognum <= nlogs; lognum++)
		{
			f = &Files[lognum];
			/* 刷新未输出的日志并输出 */
			if (f->f_prevcount)
			{
				syslog_fprintlog (f, 0, (char *) NULL);
			}

			switch (f->f_type)
			{
				case F_FILE:
				case F_PIPE:
				case F_TTY:
				case F_CONSOLE:
					close (f->f_file);
					break;
			}
		}
		/* 释放申请的内存 */
		nlogs = -1;
		free ((void *) Files);
		Files = (struct filed *) 0;
    }

	return;
}

/* 参数解析 */
static void syslog_parse_opt(int argc, char **argv)
{
	int opt;

    while(1) 
    {
        opt = getopt_long(argc, argv, "df:l:m:nvh", longopts, 0);
    
        if(opt == EOF) break;

        switch(opt) 
		{
		case 'd':		/* debug 模式 */
			syslog_dbg = 1;
			break;
		case 'f':		/* 指定配置文件 */
			config_file = optarg;
			break;
		case 'l':		/* 指定主机名 */
			if(syslog_set_hostname (optarg))
				printf("Syslog set host name error \n");
			break;
		case 'm':		/* 设置统计重复log的时间间隔 */
			MarkInterval = atoi (optarg) * 60;
			break;
		case 'n':		/* 前台运行 */
			syslog_nodetach = 1;
			break;
		case 'v':		/* 输出版本 */
			print_version ( progname );
			exit ( 0 );
			break;
		case 'h':		/* 输出帮助信息 */
			syslog_usage(progname, 0);
		default:
			syslog_usage(progname, 1);
		}
	}
	
	return ;
}


#ifdef POWERDOWN_ALARM

static int netlink_group_mask(int group)
{
	return group ? 1 <<(group - 1) : 0 ;
}

#define NETLINK_POWEROFF 24

static void *poweroff_thread(void *arg)
{
	struct sockaddr_nl saddr, daddr;
	struct nlmsghdr *nlhdr = NULL;
	struct iovec iov;
	struct msghdr msg;
	int sd;
	int ret = 1;
	unsigned char rx[10];

	printf("in poweroff_thread\n");
	sd = socket(AF_NETLINK, SOCK_RAW, NETLINK_POWEROFF);
	if(sd < 0)
	{
		return NULL;
	}

	memset(&saddr, 0, sizeof(saddr));
	memset(&daddr, 0, sizeof(daddr));

	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid = getpid();
	saddr.nl_groups = netlink_group_mask(NETLINK_MASK);
	bind(sd, (struct sockaddr *)&saddr, sizeof(saddr));

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));

	while(1)
	{
		memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));

		iov.iov_base = (void *)nlhdr;
		iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);
		msg.msg_name = (void *)&daddr;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

		ret = recvmsg(sd, &msg, 0);
		if(ret <= 0)
		{
			continue;
		}
		memcpy(rx, NLMSG_DATA(nlhdr), 4);
		if(1 == rx[0])
		{
			char		buf[1024];
			time_t 		cur_time;
			char        *timestamp = NULL;
			int pri = 0;

			time (&cur_time);
			pri = 24;//LOG_EMERG;

			memset(buf, 0, sizeof(buf));

			/* 按照 syslog 协议标准组包,即: <PRI>TIMESTAM HOSTNAME TAG CONTENT */
			timestamp = ctime (&cur_time) + 4;
			timestamp[15] = '\0';

			snprintf (buf, sizeof (buf), "<%d>%.15s syslog[%d]: %s ", 	
				pri, timestamp, getpid(), "device is poweroff");
			syslog_printline(buf);
		}
		//printf("RECV: value = %d \n", rx[0]);
		sleep(1);
	}

	return NULL;
}

#endif

struct quagga_signal_t syslog_signals[] =
{
  /*  {
        .signal  = SIGHUP,
        .handler = SIG_IGN,
        .caught  = 0,
    },*/
    {
        .signal  = SIGINT,
        .handler = &syslog_die,
        .caught  = 0,
    },
    {
        .signal  = SIGTERM,
        .handler = &syslog_die,
        .caught  = 0,
    },
    {
        .signal  = SIGALRM,
        .handler = &high_pre_timer_dotick,
        .caught  = 0,
    },
};


static void syslog_sys_init(void)
{
#if 0
	/* 信号初始化 */
	signal (SIGPIPE, SIG_IGN); /* 忽略该信号，否则会导致的syslog退出 */
	signal (SIGTERM, syslog_die);
	signal (SIGINT, syslog_nodetach ? syslog_die : SIG_IGN);
	signal (SIGQUIT, syslog_nodetach ? syslog_die : SIG_IGN);
	signal (SIGCHLD, syslog_reapchild);
	signal (SIGUSR1, syslog_dbg ? syslog_dbg_switch : SIG_IGN);
	signal (SIGALRM, high_pre_timer_dotick);
#endif
	//if( NULL == ptimer )
    {
        //ptimer = thread_add_timer(syslog_master, syslog_domark, 0, TIMERINTVL);
		//high_pre_timer_add("domark", LIB_TIMER_TYPE_LOOP, syslog_domark, NULL, TIMERINTVL*1000);
    }

	/* 添加默认规则 */
	syslog_trans_rule_to_item((char *)"critical	console");
	syslog_trans_rule_to_item((char *)"debug		file");

	/* 初始化 syslog 命令行 */
	syslog_log_rule_init ();

	memset(&syslog_file, 0, sizeof(struct filed));
	strcpy(syslog_file.f_fname,  SYSLOG_FILE);
	syslog_file.f_file =open (syslog_file.f_fname, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);

	/* 初始化操作记录文件 */
	memset(&record_logfile, 0, sizeof(struct filed));
	strcpy(record_logfile.f_fname,  SYSLOG_RECORD);
	record_logfile.location = l_file;
	record_logfile.priority = LOG_INFO;
	record_logfile.f_prevpri = LOG_INFO;
	record_logfile.f_type = F_FILE;
	record_logfile.f_file =open (record_logfile.f_fname, O_WRONLY | O_APPEND | O_CREAT | O_NOCTTY, 0644);

	syslog_creatqueue(&syslog_filequeue);

	syslog_dbg_print ((char *)"SYSLOG: starting... \n");

	return ;
}

/* syslog 接收、处理 IPC 消息 */
static int syslog_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int retva = 0;
    int revln = 0;
	FILE *fd = NULL;
	char buf[2048];

	unsigned int para_dev_id = 0;

	if((pmsg->msghdr.sender_id == MODULE_ID_DEVM) && pmsg->msghdr.msg_subtype ==  DEVM_INFO_DEV_NAME)
	{
		if(pmsg->msghdr.data_len > 0)
		{
			memcpy(local_host_name, pmsg->msg_data, revln);
			devm_comm_get_id(1, 0, MODULE_ID_SYSLOG, &para_dev_id);
			printf("Syslog recive host name: %s, device id: 0x%x \n", local_host_name, para_dev_id);
			
			/* 根据 CPU  类型配置规则 */
			fd = fopen("/proc/cpuinfo", "r");
			if(fd == NULL)
			{
				printf("get cpu type error \n");
				mem_share_free(pmsg, MODULE_ID_SYSLOG);  
				return 0;
			}
			
			while(fgets(buf, 2000, fd))
			{
				//printf("buf: \n", buf);
				if(strstr(buf, "Vitesse VCore-III") != NULL)
				{
					//printf("%s \n", buf);
					syslog_del_log (1);
					syslog_del_log (2);
					trans_rule_count = 0;
					syslog_trans_rule_to_item((char *)"critical console");
					syslog_trans_rule_to_item((char *)"critical 	file");
					filesize = 1000*2;
					devicehandle = 1;	
					mem_share_free(pmsg, MODULE_ID_SYSLOG);  
					fclose(fd);
					return 0;
				}
			}
			fclose(fd);

		}
	}
	else
	{
		//syslog 处理
		syslog_printline ((char *)(pmsg->msg_data));	
	}
	
	mem_share_free(pmsg, MODULE_ID_SYSLOG);  

	return 0;
}


/* 主函数 */
int main (int argc, char **argv)
{
	char *p;
	struct thread thread;
	cpu_set_t mask; 
	
	/* 设置 umask */
	umask (0027);

	/* 获取进程名 */
	progname = ((p = strchr (argv[0], '/')) ? ++p : argv[0]);

	/*  绑定CPU */
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 绑定cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);

	/* 参数解析 */
	syslog_parse_opt(argc, argv);
	signal_init(syslog_master, array_size(syslog_signals), syslog_signals);

	/* 创建主线程 */
	syslog_master = thread_master_create ();

	/* 公共接口初始化 */
	cmd_init (1);
	vty_init (syslog_master);
	memory_init();
	//mem_share_init();
	high_pre_timer_init(1000, MODULE_ID_SYSLOG);
	/* 创建 VTY 服务端 */


    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
	
	vty_serv_sock (SYSLOG_VTYSH_PATH);
	/* syslog 系统初始化 */
	syslog_sys_init();

	/* log 初始化 */
	zlog_default = openzlog (progname, ZLOG_SYSLOG, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
		
	/* 非 debug 模式和前台运行，创建守护进程 */
	if (!(syslog_dbg || syslog_nodetach))
	{
		/* 创建守护进程 */
		if(daemon(0, 0) < 0)
		{
			syslog_logerror("%s[%d]: SYSLOG already running", __FILE__, __LINE__);
			printf("syslogd: Already running.\n");
			exit (1);
		}

		/* 创建 pid 文件 */
		pid_output(pid_file);
	}


    if(ipc_recv_init(syslog_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }


    if(ipc_recv_thread_start("syslog_ipc_recv_log", MODULE_ID_SYSLOG, SCHED_OTHER, -1, syslog_msg_rcv, 0) == -1)
    {
        printf(" syslog ipc receive thread start fail\r\n");
        exit(0);
    }

    if(high_pre_timer_start() == -1)
    {
       printf(" syslog  timer start fail\r\n");
       exit(0);
    }

	if(high_pre_timer_add("domark", LIB_TIMER_TYPE_LOOP, syslog_domark, NULL, TIMERINTVL*1000) == -1)
	{
        printf(" syslog  ipc add timer fail\r\n");
        exit(0);	
	}

#ifdef POWERDOWN_ALARM
	pthread_t poid;
	pthread_attr_t attr;
	struct sched_param param;
#if 1
	param.sched_priority = 60;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
#ifndef HAVE_KERNEL_3_0 	
	pthread_attr_setschedparam(&attr, &param);
#endif
#endif
	printf("creat thread ... \n");
	pthread_create(&poid, &attr, (void *)poweroff_thread, NULL);
#endif

	printf("syslog starting... \n");
	


	/* 执行主线程 */
	while (thread_fetch (syslog_master, &thread))
	{
		thread_call (&thread);
	}
	
	/* 关闭 syslog 并退出 */
	closezlog (zlog_default);
	exit (0);

	return 0;
}


