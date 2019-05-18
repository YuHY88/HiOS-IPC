/*
 * vtysh_log.c - deal with vtysh config log info.
 */

#include <zebra.h>
#include <linux/sysctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include "lib/inet_ip.h"
#include "lib/command.h"
#include "lib/memory.h"
#include "lib/log.h"
#include "syslog_cmd.h"
#include "syslogd.h"


#define SERVER_IP_NUM 5 //The maximum number of syslog server IP can be added

pthread_mutex_t mutex_log= PTHREAD_MUTEX_INITIALIZER;

struct log_item   *item = NULL;
struct _ratelimit rate_limit[LIMIT_RULE_NUM] = {//log 输出的端口有console，file，memory和5个IP地址，
	{SYSLOG_CONSOLE_NAME, 0, 0, 100},			//共8个端口。所以定义有8个元素的结构体数组存储端口信息
	{"\0", 0, 0, -1},
	{"\0", 0, 0, -1},
	{"\0", 0, 0, -1},
	{"\0", 0, 0, -1},
	{"\0", 0, 0, -1},
	{"\0", 0, 0, -1},
	{"\0", 0, 0, -1}
};

/*
	二维数组:
	第一维表示该 vty 的 fd
	第二维表示该 vty 是否置位，发送log到当前终端
*/
int log_to_monitor[2][USER_NUM] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},};

extern volatile vector vtyvec;
extern CODE prinames[];
extern CODE facnames[];
extern int  memsize;
extern int  filesize;
extern int  syslog_debug;
extern unsigned int devicehandle;

static int  memsize_set = 0;
static int  filesize_set = 0;
int         console_to_monitor = 0;
int         log_timestamp_en = 1;
int         log_record_en = 1;
char       *hostname = NULL;
static int  add_server_ip = 0;
static int  log_init = 0;


const char *priority_names[] = {
	"emergency",
	"alert",
	"critical",
	"error",
	"notification",
	"warning",
	"information",
	"debug"
};

const char *maskflagnames[] = {
	"except",  
	"only",
	"upto"
};

const char *locnames[] = {
	"console",
	"file",
	"memory",
	"NULL"
};

struct cmd_node log_node = {
	SYSLOG_NODE,
	"%s(log)# ",
	1
};

/* 字符串拷贝 */
int syslog_sec_strcpy (char *dest, unsigned int destsize, char *src)
{
	int len;
	if (!src || !dest)
	{
		return -1;
	}

	len = ((destsize - 1) > strlen (src)) ? strlen (src) : (destsize - 1);
	strncpy (dest, src, len);
	dest[len] = '\0';

	return 1;
}

/* log 规则比较
 * = 1  -- same:
 * = 0  -- not same 
 * - 1  -- parameters err
 */
static int syslog_compare_log (struct log_item *dest, struct log_item *src,
	     enum match_type need_match)
{
	char tmp_locname[MAXNAMELEN + 1];
	char tmp_maskflagname[MAXNAMELEN + 1];
	char tmp_priname[MAXNAMELEN + 1];

	if (!dest || !src)
	{
		return -1;
	}
	else
	{
		if(dest->log_id == src->log_id)
		{
			return 1;
		}
	}

	switch (need_match)
	{
	case dont_match:
		return 1;

	case location_match:
		if (!dest->locname || !src->locname)
		{
			return -1;
		}
		syslog_sec_strcpy (tmp_locname, MAXNAMELEN + 1, src->locname);
		if (strncmp (tmp_locname, "*", strlen ("*")) == 0)
		{
			syslog_sec_strcpy (tmp_locname, MAXNAMELEN + 1, dest->locname);
		}
		if ((strncmp (dest->locname, tmp_locname, MAXNAMELEN) == 0) || dest->log_id == src->log_id)
		{
			return 1;
		}

	case exact_match:
		if (!dest->locname || !dest->maskflagname || !dest->priname || 
			!src->locname || !src->maskflagname || !src->priname)
		{
			return -1;
		}
			

		syslog_sec_strcpy (tmp_locname, MAXNAMELEN + 1, src->locname);
		syslog_sec_strcpy (tmp_maskflagname, MAXNAMELEN + 1, src->maskflagname);
		syslog_sec_strcpy (tmp_priname, MAXNAMELEN + 1, src->priname);

		if (strncmp (tmp_locname, "*", strlen ("*")) == 0)
		{
			syslog_sec_strcpy (tmp_locname, MAXNAMELEN + 1, dest->locname);
		}
			
		if (strncmp (tmp_maskflagname, "*", strlen ("*")) == 0)
		{
			syslog_sec_strcpy (tmp_maskflagname, MAXNAMELEN + 1, dest->maskflagname);
		}

		if (strncmp (tmp_priname, "*", strlen ("*")) == 0)
		{
			syslog_sec_strcpy (tmp_priname, MAXNAMELEN + 1, dest->priname);
		}

		if (strncmp (dest->locname, tmp_locname, MAXNAMELEN) == 0 &&
			strncmp (dest->maskflagname, tmp_maskflagname, 3) == 0 &&
			strncmp (dest->priname, tmp_priname, 3) == 0)
		{
			return 1;
		}
	default:
		break;

    }

  return 0;
}

/* @brief 返回在字符串s中最后出现字符c的位置给ptr
 * @param ptr -- 返回的指针
 * @param s -- 检查的字符串
 * @param c -- 出现的字符
 * @retval -1 在检查的字符串中找不到该字符
*/
#define STRRCHR(ptr, s, c) \
	if ( (ptr = (char *)strrchr(s, c)) == NULL) \
	{	return -1; } 

#define STRTOL(A, ptr) \
	if ( *ptr == '\0' ) { \
		return -1; \
	} \
	else { \
		A = strtol(ptr, (char **)NULL, 10); \
	}
	
#define STRCKE(ptr) \
        if ((*ptr == '0') && (*(ptr+1) != '\0')) \
         {return -1;}

/*
 * @brief 检查ip是否是真正的IP地址，即是否符合A.B.C.D的形式
 *
 * @param ip -- 要检查的IP地址
 * @retval 0  是
 * @retval -1 不是
*/	
static int syslog_bad_ip_check(char *ip)
{
	char *ptr, buf[20];
	int A, B, C, D;
	
    	//not number
	if ((ip[0] < 48) || (ip[0] > 57))
           return -1;
	
	strncpy(buf, ip, 20);
	buf[19] = 0;
	if ( buf != NULL )
	{
		//find the first '.'
		STRRCHR(ptr, buf, '.');
		*ptr = '\0';
		++ptr;

		//find the second '.'
       		STRCKE(ptr);
		STRTOL(D, ptr);
		STRRCHR(ptr, buf, '.');
		*ptr = '\0';
		++ptr;

		//find the third '.'
        	STRCKE(ptr);
		STRTOL(C, ptr);
		STRRCHR(ptr, buf, '.');
		*ptr = '\0';
		++ptr;

		//find the forth '.'
        	STRCKE(ptr);
		STRTOL(B, ptr);
		STRTOL(A, buf);
		ptr = buf;
        	STRCKE(ptr);

		//if ((A > 255) || (B > 255) || (C > 255) || (D > 255) || (D == 0))
		if ((A > 255) || (B > 255) || (C > 255) || (D > 255))
			return -1;
		else 
			return 0;
	}

	return -1;
}

/* add log item. */
int syslog_add_log (struct vty *vty, struct log_item *src, enum match_type need_match)
{
	struct log_item *ptr, *ptr1, *ptr2;
	int same = 0;

	ptr = ptr1 = item;
	/*find the existing log item */
	while (ptr)
    {
		same = syslog_compare_log (ptr, src, need_match);
		if (same == -1)
		{
			if(NULL != vty)
			{
				vty_out (vty, "Error occurs while find log item! %s", VTY_NEWLINE);
			}
			else
			{
				printf("Error occurs while find log item! \r\n");
			}

			return CMD_WARNING;
		}

		if (same)
		{
			if(NULL != vty)
			{
				vty_out (vty, "This log item is existed!%s", VTY_NEWLINE);
			}
			else
			{
				printf("This log item is existed! \r\n");
			}

			return CMD_WARNING;
		}

		ptr1 = ptr;
		ptr = ptr->next;
    }

	ptr = src;
	if(syslog_bad_ip_check(src->locname) == 0)
	{
		if(add_server_ip >= SERVER_IP_NUM)
		{
			vty_out (vty, "%% Adding excessive number of syslog server .%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		add_server_ip++;
	}

	ptr->log_id = src->log_id;// add by lihz
	if((ptr->log_id < 1) || (ptr->log_id > 100))
	{
		return CMD_ERR_NO_MATCH;
	}

	ptr->location = l_server;
	if (!strcmp (src->locname, "memory"))
	{
		ptr->location = l_memory;
		syslog_sec_strcpy (ptr->filename, MAXNAMELEN + 1, (char *)SYSLOG_MEMORY_NAME);
	}
	else if (!strcmp (src->locname, "console"))
	{
		ptr->location = l_console;
		syslog_sec_strcpy (ptr->filename, MAXNAMELEN + 1, (char *)SYSLOG_CONSOLE_NAME);
	}
	else if (!strcmp (src->locname, "file"))
	{
		ptr->location = l_file;
		syslog_sec_strcpy (ptr->filename, MAXNAMELEN + 1, (char *)SYSLOG_FILE_NAME);
	}
	else 
	{

		if (syslog_bad_ip_check (src->locname) < 0)
		{
			if(NULL != vty)
			{
				vty_out (vty, "Please input the correct type%s", VTY_NEWLINE);
			}
			else
			{
				printf("Please input the correct type \r\n");
			}
			//XFREE (MTYPE_LOG_ITEM, ptr);
			return CMD_WARNING;
		}
		syslog_sec_strcpy (ptr->filename, MAXNAMELEN + 1, src->locname);
	}


	if (!strcmp (src->maskflagname, "upto"))
	{
		ptr->maskflag = MASKFLAG_UPTO;
	}
	else if (!strcmp (src->maskflagname, "only"))
	{
		ptr->maskflag = MASKFLAG_EQ;
	}
	else if (!strcmp (src->maskflagname, "except"))
	{
		ptr->maskflag = MASKFLAG_EXCT;
	}

	ptr->priority = syslog_decode(ptr->priname, prinames);

	if (!item)
	{
		item = ptr;
		ptr->next = NULL;
	}
	else
	{
		ptr1 = ptr2 = item;
		if(ptr->log_id < item->log_id)
		{
			ptr->next = item;
			item = ptr;
		}
		else
		{
			while((ptr1 && (ptr1->log_id <= ptr->log_id)))
			{
				ptr2 = ptr1;
				ptr1 = ptr1->next;
			}
			ptr2->next = ptr;
			ptr->next = ptr1;
		}
	}

	syslog_reload (item);

	return CMD_SUCCESS;
}


int syslog_del_log (unsigned int lognum)
{
	struct log_item *ptr, *ptr1;
	unsigned int log_num = lognum;
	int meet = 0;

	/*find the log item */
	ptr = ptr1 = item;

	if(!item)
	{
		return CMD_WARNING;
	}
	if(0 == log_num)
	{
		add_server_ip = 0;
		while(ptr)
		{
			ptr1 = ptr;
			ptr = ptr->next;
			XFREE (MTYPE_LOG_ITEM, ptr1);
		}
		item = NULL;
		syslog_reload (item);
		return CMD_SUCCESS;
	}
	else if(item->log_id == log_num)
	{
		item = item->next;
		meet = 1;
	}
	else
	{
		while (ptr)
		{
			if(ptr->log_id == log_num)
			{
				ptr1->next = ptr->next;
				meet = 1;
				break;
			}
			ptr1 = ptr;
			ptr = ptr->next;
		}
	}
	if(1 == meet)
	{
		if(syslog_bad_ip_check(ptr->locname) == 0)
		{
			add_server_ip--;
			if(add_server_ip < 0)
			{
				return CMD_WARNING;
			}
		}

		XFREE (MTYPE_LOG_ITEM, ptr);
		syslog_reload (item);
		return CMD_SUCCESS;
	}
	else
	{
		return CMD_WARNING;
	}

}



int syslog_set_ratelimit (struct vty *vty, char *localname, int max_num)
{
	int i;
	char buftemp[MAXNAMELEN + 1];

	memset(buftemp, 0, sizeof(buftemp));
	if(strncmp("console", localname, 4) == 0)
	{
		strcpy(buftemp, SYSLOG_CONSOLE_NAME);
	}
	else if(strncmp("file", localname, 4) == 0)
	{
		strcpy(buftemp, SYSLOG_FILE_NAME);
	}
	else if(strncmp("memory", localname, 4) == 0)
	{
		strcpy(buftemp, SYSLOG_MEMORY_NAME);
	}
	else
	{
		if(syslog_bad_ip_check(localname) == 0)
		{
			strcpy(buftemp, localname);
		}
		else
		{
			vty_out (vty, "Please input the correct IP address %s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}
	}

	for(i = 0; i < LIMIT_RULE_NUM; ++i)
	{
		if(rate_limit[i].rate_max_num != -1)//判断数组是否存在限速规则
		{
			if(strcmp(rate_limit[i].localname, buftemp) == 0)
			{
				rate_limit[i].rate_max_num = max_num;
				vty_out (vty, "Rate limit maximum number of %s has changed! %s", localname, VTY_NEWLINE);
				return CMD_SUCCESS;
			}			
		}
		else
		{
			if(strncmp("console", localname, 4) == 0)
			{
				strcpy(rate_limit[i].localname, SYSLOG_CONSOLE_NAME);
			}
			else if(strncmp("file", localname, 4) == 0)
			{
				strcpy(rate_limit[i].localname, SYSLOG_FILE_NAME);
			}
			else if(strncmp("memory", localname, 4) == 0)
			{
				strcpy(rate_limit[i].localname, SYSLOG_MEMORY_NAME);
			}
			else
			{
				strcpy(rate_limit[i].localname, localname);
			}
		
			rate_limit[i].rate_max_num = max_num;
			
			vty_out (vty, "Rate limit rule of %s add success! %s", localname, VTY_NEWLINE);
			return CMD_SUCCESS;
		}
	}
	vty_out (vty, "Rate limit rules is exceded the maximum number! %s", VTY_NEWLINE);

	return CMD_WARNING;
}

int syslog_unset_ratelimit (struct vty *vty, char *localname)
{
	int i, j;
	char buftemp[100];

	memset(buftemp, 0, sizeof(buftemp));
	if(strncmp("console", localname, 4) == 0)
	{
		strcpy(buftemp, SYSLOG_CONSOLE_NAME);
	}
	else if(strncmp("file", localname, 4) == 0)
	{
		strcpy(buftemp, SYSLOG_FILE_NAME);
	}
	else if(strncmp("memory", localname, 4) == 0)
	{
		strcpy(buftemp, SYSLOG_MEMORY_NAME);
	}
	else
	{
		if(syslog_bad_ip_check(localname) == 0)
		{
			strcpy(buftemp, localname);
		}
		else
		{
			vty_out (vty, "Rate limit rules is not exist! %s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}
	}

	for(i = 0; i < LIMIT_RULE_NUM; ++i)
	{
		if(strcmp(rate_limit[i].localname, buftemp) == 0)
		{
			memset(rate_limit[i].localname, 0, sizeof(rate_limit[i].localname));
			rate_limit[i].rate_limit_count = 0;
			rate_limit[i].rate_start_time = 0;
			rate_limit[i].rate_max_num = -1;

			for(j = (i+1);j < LIMIT_RULE_NUM; ++j)
			{
				if(rate_limit[j].rate_max_num != -1)
				{
					strcpy(rate_limit[i].localname, rate_limit[j].localname);
					rate_limit[i].rate_limit_count = rate_limit[j].rate_limit_count;
					rate_limit[i].rate_start_time = rate_limit[j].rate_start_time;
					rate_limit[i].rate_max_num = rate_limit[j].rate_max_num;
					i++;
					memset(rate_limit[j].localname, 0, sizeof(rate_limit[j].localname));
					rate_limit[j].rate_limit_count = 0;
					rate_limit[j].rate_start_time = 0;
					rate_limit[j].rate_max_num = -1;
				}

			}

			vty_out (vty, "Rate limit rules is removed success! %s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}
	}
	
	vty_out (vty, "Rate limit rules is not exist! %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}


static int syslog_find_fd_from_tab(struct vty *log_vty)
{
	int count = 0;

	for(count = 0; count < USER_NUM; ++count)
	{
		if(log_to_monitor[0][count] == log_vty->fd)
		{
			return count;
		}
	}

	return -1;
}


DEFUN (show_log, show_logging_cmd,
	"show log", SHOW_STR "Logging information\n")
{
	struct log_item *ptr, *ptr1;
	int i = 0;
	int index = 0;

	vty_out(vty, "%sLog info:%s", VTY_NEWLINE, VTY_NEWLINE);
	vty_out(vty, "log memory size:    %-5d Kilobytes %s", memsize, VTY_NEWLINE);
	vty_out(vty, "log file size:      %-5d Kilobytes %s", filesize, VTY_NEWLINE);
	vty_out(vty, "log timestamp:      %-8s %s", log_timestamp_en ? "enabled":"disabled", VTY_NEWLINE);
	//vty_out(vty, "log record:         %-8s %s", log_record_en ? "enabled":"disabled", VTY_NEWLINE);

	/* show terminal status */
	index = syslog_find_fd_from_tab(vty);
	if(index != -1)
	{
		vty_out(vty, "log terminal:       %-8s %s", log_to_monitor[1][index]? "enabled":"disabled", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "log terminal:       %-8s %s", "disabled", VTY_NEWLINE);
	}
	
	if(rate_limit[0].rate_max_num != -1)
	{
		vty_out(vty, "%sLog ratelimit: %s", VTY_NEWLINE, VTY_NEWLINE);
		for(i = 0; i < LIMIT_RULE_NUM; i++)
		{
			if(rate_limit[i].rate_max_num != -1)
			{
				if(strncmp(rate_limit[i].localname, SYSLOG_CONSOLE_NAME, 4) == 0)
				{
					vty_out(vty, "%-15s     %-5d line/sec %s", "console",rate_limit[i].rate_max_num, VTY_NEWLINE);
				}
				else if(strncmp(rate_limit[i].localname, SYSLOG_FILE_NAME, 4) == 0)
				{
					vty_out(vty, "%-15s     %-5d line/sec %s", "file",rate_limit[i].rate_max_num, VTY_NEWLINE);
				}
				else if(strncmp(rate_limit[i].localname, SYSLOG_MEMORY_NAME, 4) == 0)
				{
					vty_out(vty, "%-15s     %-5d line/sec %s", "memory",rate_limit[i].rate_max_num, VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, "%-15s     %-5d line/sec %s", rate_limit[i].localname, rate_limit[i].rate_max_num, VTY_NEWLINE);
				}
			}

		}
	}

	ptr = ptr1 = item;
	if(item)
	{
		/* show vpn status */
		while (ptr)
		{
			if(ptr->location == l_server)
			{
				vty_out(vty, "%slog service vpn: %s", VTY_NEWLINE, VTY_NEWLINE);
				break;
			}
			ptr1 = ptr;
			ptr = ptr->next;
		}
		
		ptr = ptr1 = item;
		while (ptr)
		{
			if(ptr->location == l_server)
			{
				vty_out(vty, "%-15s 	%-5d %s", ptr->locname, ptr->vpn, VTY_NEWLINE);
			}
				
			ptr1 = ptr;
			ptr = ptr->next;
		}
	
		vty_out (vty, "%sLog rule:  %s", VTY_NEWLINE, VTY_NEWLINE);
		vty_out (vty, "%-5s    %-13s    %-14s    %-15s    %s %s", "LOGID",
			"PRINAME","NASKFLAGNAME","LOCNAME","FILENAME", VTY_NEWLINE);		
	}
	
	ptr = ptr1 = item;
	/*find the existing log item */
	while (ptr)
	{
		vty_out (vty, "%-5d    %-13s    %-14s    %-15s    %s %s", ptr->log_id, ptr->priname,
				ptr->maskflagname, ptr->locname, ptr->filename, VTY_NEWLINE);
		ptr1 = ptr;
		ptr = ptr->next;
	}

	vty_out (vty, "%s", VTY_NEWLINE);
	return 1;
}

DEFUN (terminal_monitor,
	terminal_monitor_cmd,
	"log terminal",
	"Logging control\n"
	"Set console line to terminal\n")
{
	//console_to_monitor = 1;
	int i;
	int index = 0;

	//printf("log terminal: fd = %d \n", vty->fd);

#if 0
	/* 查找该 vty 是否已经存在，存在则置位 */
	for(i = 0; i < USER_NUM; i++)
	{
		if(log_to_monitor[0][i] == syslog_find_fd_from_tab(vty))
		{
			log_to_monitor[1][i] = 1;
			return CMD_SUCCESS;
		}
	}
#endif

	index = syslog_find_fd_from_tab(vty);
	if(index != -1)
	{
		log_to_monitor[1][index] = 1;
	}
	else
	{
		/* 找一个空位标记该 vty 的 fd 并置位*/
		for(i = 0; i < USER_NUM; i++)
		{
			if(log_to_monitor[0][i] == 0)
			{
				log_to_monitor[0][i] = vty->fd;
				log_to_monitor[1][i] = 1;
				break;
			}
		}
	}
#if 0
	int j, k;
	for(j = 0; j < USER_NUM; j++)
	{
		for(k = 0; k < 2; k++)
		{
			printf("%d ", log_to_monitor[k][j]);
		}
		printf("\n");
	}
	printf("\n");
#endif

	return CMD_SUCCESS;
}

DEFUN (terminal_no_monitor,
	terminal_no_monitor_cmd,
	"no log terminal",
	NO_STR
	"Logging control\n"
	"Disable console line to terminal\n")
{
	//console_to_monitor = 0;
	//int i;
	int index = 0;
	//printf("no log terminal: fd = %d \n", vty->fd);

	index = syslog_find_fd_from_tab(vty);
	if(index != -1)
	{
		log_to_monitor[1][index] = 0;
	}

#if 0
	int j, k;
	for(j = 0; j < USER_NUM; j++)
	{
		for(k = 0; k < 2; k++)
		{
			printf("%d ", log_to_monitor[k][j]);
		}
		printf("\n");
	}
	printf("\n");
#endif
#if 0
	/* 查找该 vty 是否已经存在，存在则置位 */
	for(i = 0; i < USER_NUM; i++)
	{
		if(log_to_monitor[0][i] == syslog_find_fd_from_tab(vty))
		{
			log_to_monitor[1][i] = 0;
			return CMD_SUCCESS;
		}
	}
#endif

	return CMD_SUCCESS;
}

DEFUN (log_timestamp,
	log_timestamp_cmd,
	"log timestamp",
	"Logging control\n"
	"Set log timestamp\n")
{
	log_timestamp_en = 1;

	return CMD_SUCCESS;
}

DEFUN (log_no_timestamp,
	log_no_timestamp_cmd,
	"no log timestamp",
	NO_STR
	"Logging control\n"
	"Disable log timestamp\n")
{
	log_timestamp_en= 0;

	return CMD_SUCCESS;
}


DEFUN (log_rule,
	log_rule_cmd,
	"log <1-100>"
	"(A.B.C.D|memory|console|file) (upto|only|except) (emergency|alert|critical|error|warning|notification|information|debug) [vpn-instance <1-128>]",
	"Logging control\n" 
	"Add log rule ID\n" 
	"Logging to host whose ip address is A.B.C.D\n" "Logging to memory\n"
	"Logging to console\n" "Logging to file\n"
	"Logging include specified priority and all up priorities\n"
	"Logging only specified priority\n"
	"Logging all priorities except specified priority\n"
	"Logging emergencies level -> 0\n" "Logging alerts level -> 1\n"
	"Logging critical level -> 2\n" "Logging errors level -> 3\n"
	"Logging warnings level -> 4\n" "Logging notifications level -> 5\n"
	"Logging information level -> 6\n" "Logging debug level -> 7\n"
	"L3 vpn instance, only for log server\n" "L3 vpn tag\n")
{
	struct log_item *src;
	int ret = 0;
	int i = 0;

	//if(strlen(argv[3]) < 2)
	if(argc < 4)
	{
		vty_out (vty, "%% Command incomplete.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	src = (struct log_item *) XMALLOC (MTYPE_LOG_ITEM, sizeof (struct log_item));
	if (!src)
	{
		vty_out (vty, "System can not alloc memory!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset (src, 0, sizeof (struct log_item));
	src->log_id = atoi(argv[0]);
	if((src->log_id < 1) || (src->log_id > 100))// 判断 log id 是否合法
	{
		vty_out (vty, "%% Log rule ID error.%s", VTY_NEWLINE);
		XFREE (MTYPE_LOG_ITEM, src);
		return CMD_WARNING;
	}

	/* init default log rule */
	if(((src->log_id == 1) || (src->log_id == 2)) && log_init < 2)
	{
		syslog_del_log (src->log_id);
		log_init++;
	}

	for(i = 0; i < 3; ++i)// locname 赋值，如果输入缩写则自动补全
	{
		if(syslog_bad_ip_check((char *)argv[1]) == 0)// ip
		{
			if(!inet_valid_ipv4(inet_strtoipv4((char *)argv[1])))// 如果IP 地址不是合法的单播地址则直接返回
			{
				vty_out (vty, "Error: Invalid IP address.%s", VTY_NEWLINE);
				XFREE (MTYPE_LOG_ITEM, src);
				return CMD_WARNING;
			}

			if(argv[4] != NULL)
			{
				src->vpn = atoi(argv[4]);
			}
			else
			{
				src->vpn = 0;
			}
			
			syslog_sec_strcpy (src->locname, MAXNAMELEN + 1, (char *)argv[1]);
			break;
		}
		else// console, file or memory 
		{
			if(argv[4] != NULL)
			{
				vty_out (vty, "%% Not support vpn for %s %s", argv[1], VTY_NEWLINE);
				XFREE (MTYPE_LOG_ITEM, src);
				return CMD_WARNING;
			}
			if(strncmp(argv[1],locnames[i], 1) == 0)
			{
				syslog_sec_strcpy (src->locname, MAXNAMELEN + 1, (char *)locnames[i]);
				break;
			}
		}
	}

	for(i = 0; i < 3; ++i)// maskflagname 赋值，如果输入缩写则自动补全
	{
		if(strncmp(argv[2],maskflagnames[i], 1) == 0)
		{
			syslog_sec_strcpy (src->maskflagname, MAXNAMELEN + 1, (char *)maskflagnames[i]);
			break;
		}
	}

	for(i = 0; i < 8; ++i)// priname 赋值，如果输入缩写则自动补全
	{
		if(strncmp(argv[3],priority_names[i], 2) == 0)
		{
			syslog_sec_strcpy (src->priname, MAXNAMELEN + 1, (char *)priority_names[i]);
			break;
		}
	}

	ret = syslog_add_log (vty, src, exact_match);
	if (ret != CMD_SUCCESS)
	{
		XFREE (MTYPE_LOG_ITEM, src);		
	}
	syslog_reload (item);	
	
	return ret;
}


DEFUN (no_log_rule,
	no_log_rule_cmd,
	"no log <1-100>", 
	NO_STR "Logging control\n"
	"delete log rule\n")
{
	int ret = 0;
	int lognum = 0;

	lognum = atoi(argv[0]);

	if(lognum <= 0)
	{
		vty_out (vty, "Log ID  illegal <1-100> %s", VTY_NEWLINE);
			return CMD_WARNING;
	}

	ret = syslog_del_log (lognum);

	if(CMD_SUCCESS != ret)
		vty_out (vty, "Log ID %d not found! %s", lognum, VTY_NEWLINE);

	return ret;
}

DEFUN (no_log, no_log_cmd, "no log", NO_STR "Logging control\n")
{
	int ret = 0;
	int lognum = 0;
	
	ret = syslog_del_log (lognum);
	
	if(CMD_SUCCESS != ret)
	  vty_out (vty, "no log found %s", VTY_NEWLINE);
	
	return ret;

}



DEFUN (clear_log,
	clear_log_cmd,
	"clear log (memory | file)", 
	CLEAR_STR 
	"clear log information\n"
	"clear memory log\n"
	"clear file log\n")
{
	if(strncmp(argv[0], "memory", 1) == 0)
	{
		unlink (SYSLOG_MEMORY_NAME);
		unlink (SYSLOG_MEMORY_NAME_BAK);		
	}
	else if(strncmp(argv[0], "file", 1) == 0)
	{
		unlink (SYSLOG_FILE_NAME);
		unlink (SYSLOG_FILE_NAME_BAK);
	}
	
	syslog_reload (item);
	return CMD_SUCCESS;
}

DEFUN (log_memsize,
	log_memsize_cmd,
	"log memory size <4-256>",
	"Logging control \n"
	"Set memory type \n" 
	"Memory log size \n" 
	"Scope of memory size (kilobytes) \n")
{
	memsize_set = 1;
	memsize = atoi (argv[0]);
	if( memsize<4 || memsize>256)
	{
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_log_memsize,
	no_log_memsize_cmd,
	"no log memory size", 
	NO_STR 
	"Logging control \n" 
	"Memroy type\n"  
	"Unset memory size \n")
{
	memsize_set = 0;
	memsize = DEFAULT_MEM_SIZE;
	
	return CMD_SUCCESS;
}

DEFUN (log_filesize,
	log_filesize_cmd,
	"log file size <1-20000>",
	"Logging control \n"
	"Set file type \n" 
	"File log size\n"  
	"Scope of file size (kilobytes) \n")
{
	filesize_set = 1;
	filesize = atoi (argv[0]);
	
	return CMD_SUCCESS;
}

DEFUN (no_log_filesize,
	no_log_filesize_cmd,
	"no log file size", 
	NO_STR 
	"Logging control \n" 
	"File type\n" 
	"Unset file size \n")
{
	filesize_set = 0;
	if(devicehandle == 1)
	{
		filesize = 1000*2;
	}
	else
	{
		filesize = DEFAULT_FILE_SIZE;
	}	
	
	return CMD_SUCCESS;
}

static int syslog_print_log_file(struct vty *log_vty, const char *filename, const char *filename_bak)
{
	FILE 	*fd = NULL, *fd_bak = NULL ;
	char 	buf[1026];
	int  	cur_fpos=0;
	int  	fpos = 0;

	fd = fopen (filename, "r");
	if (fd != NULL)
	{
		while (fgets (buf, 1024, fd) != NULL)
		{
			fpos++;
		}
		fclose(fd);
	}
	// read /var/log/memory.bak first
	fd_bak = fopen (filename_bak, "r");
	if (fd_bak != NULL)
	{
		cur_fpos = 0;
		while (fgets (buf, 1024, fd_bak) != NULL)
		{
			if(cur_fpos >= fpos) 
			{
				break;
			}
			cur_fpos++;
		}
		memset(buf, 0, sizeof(buf));
		while (fgets (buf, 1024, fd_bak) != NULL)
		{
			vty_out (log_vty, buf);
			memset(buf, 0, sizeof(buf));
		}
		fclose (fd_bak);
	}

	// read /var/log/memory next
	fd = fopen (filename, "r");
	if (fd != NULL)
	{
		memset(buf, 0, sizeof(buf));
		while (fgets (buf, 1024, fd) != NULL)
		{
			vty_out (log_vty, buf);
			memset(buf, 0, sizeof(buf));
		}
		fclose (fd);
	}	

	return 0;
}



DEFUN (show_log_file_or_memroy,
	show_log_file_or_memroy_cmd,
	"show log memory", 
	SHOW_STR 
	"Display logging information\n"
	"memory information\n")
	
{
	//if(strncmp(argv[0], "memory", 1) == 0)
	{
		return syslog_print_log_file(vty, (char *)SYSLOG_MEMORY_NAME, (char *)SYSLOG_MEMORY_NAME_BAK);
	}
	/*else if(strncmp(argv[0], "file", 1) == 0)
	{
		syslog_print_log_file(vty, SYSLOG_FILE_NAME, SYSLOG_FILE_NAME_BAK);
	}*/
	
	//return CMD_SUCCESS;
}

// DEFUN_NOSH (syslogd_config_hostname,
	// syslogd_config_hostname_cmd,
	// "hostname WORD", "Configure hostname \n" "Name of host \n")
// {
	// extern char local_host_name[MAXHOSTNAMELEN + 1];	/* our hostname */

	// strncpy (local_host_name, argv[0], MAXHOSTNAMELEN);
	
	// return CMD_SUCCESS;
// }


/*
 * reset syslogd configure.
 */
void syslog_reload (struct log_item *item)
{
	struct log_item *ptr = NULL;
	int 			ret = 0;

	ret = pthread_mutex_lock(&mutex_log);	
	if(ret != 0)
	{
		syslog_logerror("%s[%d]: %s: SYSLOG pthread_mutex_unlock error, ret = %d", 
			__FILE__, __LINE__, __func__, ret);

		return ;
	}
	
	syslog_del_all_conf ();
	ptr = item;

	while (ptr)
	{
		syslog_enforce_log_item (ptr);
		ptr = ptr->next;
	}
	
	ret = pthread_mutex_unlock(&mutex_log);
	if(ret != 0)
	{
		syslog_logerror("%s[%d]: %s: SYSLOG pthread_mutex_unlock error, ret = %d", 
			__FILE__, __LINE__, __func__, ret);
	}

	return ;
}

/* strcat功能的安全实现，把src追加到dest后面 */
int sec_strcat (char *dest, int destsize, char *src)
{
	int len;
	
	if (!src || !dest)
	{
		return -1;
	}

	len = ((destsize - 1 - strlen (dest)) > strlen (src)) ? strlen (src) : (destsize - 1 - strlen (dest));
	strncat (dest, src, len);

	return 1;
}

DEFUN (ratelimit_rule,
   ratelimit_rule_cmd,
   "log ratelimit (A.B.C.D|memory|console|file) <1-10000>",
   "Logging control\n"
   "Rate limit of specified facility \n"
   "Logging to host whose ip address is A.B.C.D\n" 
   "Logging to memory\n"
   "Logging to console\n" "Logging to file\n"
   "Seconds per syslog information \n")
{
	return syslog_set_ratelimit (vty, (char *)argv[0], atoi(argv[1]));
}

DEFUN (no_ratelimit_rule,
   no_ratelimit_rule_cmd,
   "no log ratelimit (A.B.C.D|memory|console|file)",
   NO_STR
   "Logging control\n" "Rate limit of specified facility \n"
   "Logging to host whose ip address is A.B.C.D\n" "Logging to memory\n"
   "Logging to console\n" "Logging to file\n")
{
	return syslog_unset_ratelimit (vty, (char *)argv[0]);
}

DEFUN (no_ratelimit,
	no_ratelimit_all_cmd,
	"no log ratelimit",
	NO_STR "Logging control\n" "Rate limit of specified facility \n")
{
	int i;

	if(rate_limit[0].rate_max_num == -1)
	{
		vty_out (vty, "no log ratelimit rule found %s", VTY_NEWLINE);
	}

	for(i = 0; i < LIMIT_RULE_NUM; ++i)
	{
		strcpy(rate_limit[i].localname, "\0");
		rate_limit[i].rate_start_time = 0;
		rate_limit[i].rate_limit_count = 0;
		rate_limit[i].rate_max_num = -1;
	}

	return CMD_SUCCESS;
}

DEFUN (syslog_name,
   syslog_name_cmd,
   "syslog",
   NO_STR
   "enter syslog cmd\n")
{
	vty->node = SYSLOG_NODE;
	return CMD_SUCCESS;
}

static int syslog_level_match(const char *s)
{
	int level ;

	for ( level = 0 ; zlog_priority [level] != NULL ; level ++ )
	{
		if (!strncmp (s, zlog_priority[level], 2))
		{
			return level;
		}
	}

	return ZLOG_DISABLED;
}



DEFUN (config_logmsg,
   config_logmsg_cmd,
   "logmsg "LOG_LEVELS" .MESSAGE",
   "Send a message to enabled logging destinations\n"
   LOG_LEVEL_DESC
   "The message to send\n")
{
	int level;
	char *message;

	if ((level = syslog_level_match(argv[0])) == ZLOG_DISABLED)
	{
		return CMD_ERR_NO_MATCH;
	}
	
	zlog(NULL, level, "%s", ((message = argv_concat(argv, argc, 1)) ? message : ""));
	if (message)
	{
		XFREE(MTYPE_TMP, message);
	}
	
	return CMD_SUCCESS;
}


DEFUN (log_record,
	log_record_cmd,
	"log record (enable|disable)",
	"Logging control\n"
	"Set operation record file\n"
	"Enable operation record to file\n"
	"Disable operation record to file\n")

{
	unsigned int enable = 0;

	enable = (strncmp(argv[0], "enable", 1) == 0) ? 1 : 0;
	if(enable)
	{
		log_record_en = 1;
	}
	else
	{
		log_record_en = 0;
	}

	return CMD_SUCCESS;
}
	
const struct message syslog_dbg_name[] = {
	{.key = SYSLOG_DBG_RECVMSG, .str = "recvmsg"},
	{.key = SYSLOG_DBG_DOMASK,	.str = "domask"},
	{.key = SYSLOG_DBG_LOCK,	.str = "lock"},
	{.key = SYSLOG_DBG_CLI, 	.str = "cli"},
	{.key = SYSLOG_DBG_ALL, 	.str = "all_debug"},
};


DEFUN (syslog_debug_monitor,
	syslog_debug_monitor_cmd,
	"debug syslog (enable|disable) (recvmsg|domask|lock|cli|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable/disatble statue\n"
	"Debug informations type name\n"
	"Type name of recive messege\n"
	"Type name of flush log queue \n"
	"Type name of lock status\n"
	"Type name of all debug\n")
{
	unsigned int typeid = 0;
	int zlog_num;
	
	for(zlog_num = 0; zlog_num < array_size(syslog_dbg_name); zlog_num++)
	{
		if(!strncmp(argv[1], syslog_dbg_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, syslog_dbg_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN (show_syslog_debug_monitor,
	show_syslog_debug_monitor_cmd,
	"show syslog debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	int type_num;
	
	vty_out(vty, "debug type	status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(syslog_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", syslog_dbg_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/* 把 syslog 配置写入配置文件 */
int syslog_log_config_write (struct vty *vty)
{
	struct log_item *ptr, *ptr1;
	int 			i;
	char			buftemp[MAXNAMELEN + 1];

	vty_out (vty, "syslog%s", VTY_NEWLINE);
	vty_out (vty, " log memory size %d %s", memsize, VTY_NEWLINE);
	vty_out (vty, " log file size %d %s", filesize, VTY_NEWLINE);
	vty_out(vty, "%s %s",log_timestamp_en ? " log timestamp" : " no log timestamp"	, VTY_NEWLINE);
	//vty_out(vty, "%s %s",log_record_en? " log record enable" : " log record disable"	, VTY_NEWLINE);

	for (i = 0; i < LIMIT_RULE_NUM; i++)
	{
		if (rate_limit[i].rate_max_num != -1)
		{
			if(strcmp(rate_limit[i].localname, SYSLOG_CONSOLE_NAME) == 0)
			{
				strcpy(buftemp, "console"); 			
			}
			else if(strcmp(rate_limit[i].localname, SYSLOG_FILE_NAME) == 0)
			{
				strcpy(buftemp, "file");
			}
			else if(strcmp(rate_limit[i].localname, SYSLOG_MEMORY_NAME) == 0)
			{
				strcpy(buftemp, "memory");
			}
			else
			{
				strcpy(buftemp, rate_limit[i].localname);
			}

			vty_out (vty, " log ratelimit %s %d %s",
				buftemp, rate_limit[i].rate_max_num, VTY_NEWLINE);
		}
	}

	ptr = ptr1 = item;
	/*find the existing log item */
	while (ptr)
	{
		if((syslog_bad_ip_check(ptr->locname) == 0) && ptr->vpn != 0)// ip
		{
			vty_out (vty, " log %d %s %s %s vpn-instance %d %s", 
			ptr->log_id, ptr->locname, ptr->maskflagname, ptr->priname, ptr->vpn,VTY_NEWLINE);
			ptr1 = ptr;
			ptr = ptr->next;		
		}
		else
		{
			vty_out (vty, " log %d %s %s %s %s", 
			ptr->log_id, ptr->locname, ptr->maskflagname, ptr->priname, VTY_NEWLINE);
			ptr1 = ptr;
			ptr = ptr->next;
		}
	}
	return 1;
}


void syslog_log_rule_init (void)
{
	install_node (&log_node, syslog_log_config_write);
	install_default (SYSLOG_NODE);

	install_element (CONFIG_NODE, &syslog_name_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_logging_cmd, CMD_LOCAL);
	install_element (SYSLOG_NODE, &show_logging_cmd, CMD_LOCAL);	
	
	install_element (SYSLOG_NODE, &log_rule_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &no_log_rule_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &no_log_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &terminal_monitor_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &terminal_no_monitor_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &log_timestamp_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &log_no_timestamp_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &clear_log_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &ratelimit_rule_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &no_ratelimit_rule_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &no_ratelimit_all_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &log_filesize_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &no_log_filesize_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &log_memsize_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &no_log_memsize_cmd, CMD_SYNC);
	install_element (SYSLOG_NODE, &show_log_file_or_memroy_cmd, CMD_LOCAL);
	// install_element (VIEW_NODE, &syslogd_config_hostname_cmd);
	// install_element (ENABLE_NODE, &syslogd_config_hostname_cmd);
	// install_element (SYSLOG_NODE, &syslogd_config_hostname_cmd);

	install_element (SYSLOG_NODE, &config_logmsg_cmd, CMD_LOCAL);
	//install_element (SYSLOG_NODE, &log_record_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &syslog_debug_monitor_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_syslog_debug_monitor_cmd, CMD_LOCAL);

	//vty_close_func = no_terminal_monitor;
	//set_all_priority (0, LOG_MASK_ZERO);
}
