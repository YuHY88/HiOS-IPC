
#ifndef __SYSLOG_CMD_H__
#define __SYSLOG_CMD_H__

#define SYSLOG_FILE_NAME         "/data/dat/log.txt"
#define SYSLOG_FILE_NAME_BAK     "/data/dat/log.txt.bak"
#define SYSLOG_MEMORY_NAME       "/var/log/memory"
#define SYSLOG_MEMORY_NAME_BAK   "/var/log/memory.bak"
#define SYSLOG_CONSOLE_NAME      "/dev/console"	/*do not use ttyS0 or console,not suitable for pc and products by zhangjj 2016-6-21*/
#define LOG_MEMORY               "/var/log/memory"
#define SYSLOG_RECORD            "/data/dat/record_log.txt"
#define SYSLOG_RECORD_BAK        "/data/dat/record_log.txt.bak"


#define MAXNAMELEN 127
#define DEFAULT_MEM_SIZE 4
#define DEFAULT_FILE_SIZE 1000*20
#define NFACILITY 128
#define RATELIMIT_DEFAULT 0
#if 1 /*add by zhangjj 2016-5-17*/
#define CONFIG_LOGFILE
#define MASKFLAG_UPTO 0
#define MASKFLAG_EQ 1
#define MASKFLAG_EXCT 2
#define LOG_OFFSET	1
#define MAX_TTY_LEN	512
#define LOG_MASK_ZERO 0
#endif

#define LIMIT_RULE_NUM 8 //限速规则数量

#define USER_NUM 10

/* syslog debug define */
#define SYSLOG_DBG_RECVMSG   0x01
#define SYSLOG_DBG_DOMASK    0x02
#define SYSLOG_DBG_LOCK      0X04
#define SYSLOG_DBG_CLI       0x08
#define SYSLOG_DBG_FILE      0x10
#define SYSLOG_DBG_ALL       0x1F

enum flag_type
{
	log_flag,
	debug_flag,
	terminal_flag, // add for "terminal monitor"
	backup_flag // add for liuxiaoping
};

enum match_type
{
	dont_match,
	daemon_match,
	location_match,
	exact_match
};

enum location_type
{
	l_server = 1,
	l_memory,
	l_console,
	l_file,
	l_record
};
enum maskflag_type
{
	upto = 0,
	only, 
	except
};


struct submodule 
{
	char submodule[MAXNAMELEN + 1];/* submodule such as: login, logoff etc */
	struct submodule *next;
};

struct log_item 
{
	enum location_type location;		/* log location */
	unsigned int log_id;				/* 日志 ID: 1-100*/  
	char locname[MAXNAMELEN + 1];		/* log location: A.B.C.D, memory, stdout, file */
	enum maskflag_type maskflag;		/* 过滤条件: 0, 1, 2 */
	char maskflagname[MAXNAMELEN + 1];	/* 过滤条件名称: upto, only, except */
	unsigned int priority;				/* 日志优先级 */
	char priname[MAXNAMELEN + 1];		/* 日志优先级名 */
	unsigned short vpn;

	char filename[MAXNAMELEN + 1]; 		/* 目标文件名 */

	// char ttyname[MAXNAMELEN + 1];	/* vtysh's terminal */
	struct log_item *next;
};

struct _ratelimit
{
	char localname[MAXNAMELEN + 1];/* log location */
	time_t rate_start_time;/* first rec start time */
	int rate_limit_count;/*  */
	int rate_max_num;/* Log output ceiling */
};

void syslog_log_rule_init (void);
int syslog_bad_ip_address (char *ip);
void syslog_reload (struct log_item *item);
int syslog_log_config_write (struct vty *vty);
int syslog_sec_strcpy (char *dest, unsigned int destsize, char *src);
int sec_strcat (char *dest, int destsize, char *src);
int syslog_add_log (struct vty *vty, struct log_item *src, enum match_type need_match);
int syslog_del_log (unsigned int lognum);
int syslog_set_ratelimit (struct vty *vty, char *localname, int max_num);
int syslog_unset_ratelimit (struct vty *vty, char *localname);
void syslog_trans_rule_to_item(char *cbuf);

unsigned short syslog_get_vpn(void);

#endif

