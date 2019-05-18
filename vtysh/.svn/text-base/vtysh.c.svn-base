/******************************************************************************
 * Filename: vtysh.c
 *  Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
******************************************************************************/


#include <zebra.h>
#include <lib/types.h>
#include <sys/un.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "command.h"
#include "memory.h"
#include "vtysh/vtysh.h"
#include "log.h"
#include "ifm_common.h"
#include "vty.h"
#include "aaa_common.h"
#include "pkt_type.h"
#include <lib/pkt_buffer.h>
#include "vtysh_user.h"
#include "vtysh_session.h"
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/buffer.h>
#include <lib/errcode.h>
#include <lib/thread.h>
#include <lib/log.h>

#include "sshd/channels.h"
#include "vtysh_sync.h"
#include "vtysh_telnet.h"
#include <lib/sys_ha.h>
#include <lib/linklist.h>
#include <lib/snmp_common.h>
#include <lib/alarm.h>
#include <lib/ospf_common.h>


/* Struct VTY. */
struct vty *vty;

/* VTY shell pager name. */
char *vtysh_pager_name = NULL;

int vty_handle_cmd_flag;

sigset_t sig_mask;

int reading_config_file = 0;
int recv_slot_msg_flag = 0;
int config_recover_finish = 0;

pthread_mutex_t vtysh_config_lock;
pthread_mutex_t session_timer_lock;

int g_read_config_err_flag = 0;
static int g_slave_config_enable = 0;

extern int vtysh_user_line_write(void);

int g_int_flag = 0;
pthread_mutex_t vtysh_exec_cmd_lock;


extern int gVtyLocalUserLogin;



struct user_line user_lines[USER_LINE_NUM]; /* 0：console , 1-64  vty */

/* VTY shell client structure. */
struct vtysh_client
{
    int fd;
    const char *name;
    long long flag;
    const char *path;
}


/* app vty path */
vtysh_client[] =
{
    { .fd = -1, .name = "ifm", .flag = VTYSH_IFM, .path = IFM_VTYSH_PATH},
    { .fd = -1, .name = "route", .flag = VTYSH_ROUTE, .path = ROUTE_VTYSH_PATH},
    { .fd = -1, .name = "ripd", .flag = VTYSH_RIPD, .path = RIP_VTYSH_PATH},
    { .fd = -1, .name = "ospfd", .flag = VTYSH_OSPFD, .path = OSPF_VTYSH_PATH},
    { .fd = -1, .name = "bgpd", .flag = VTYSH_BGPD, .path = BGP_VTYSH_PATH},
    { .fd = -1, .name = "isisd", .flag = VTYSH_ISISD, .path = ISIS_VTYSH_PATH},
    { .fd = -1, .name = "system", .flag = VTYSH_SYSTEM, .path = SYSTEM_VTYSH_PATH},
    { .fd = -1, .name = "ces", .flag = VTYSH_CES, .path = CES_VTYSH_PATH},
    { .fd = -1, .name = "mpls", .flag = VTYSH_MPLS, .path = MPLS_VTYSH_PATH},
    { .fd = -1, .name = "dhcp", .flag = VTYSH_DHCP, .path = DHCP_VTYSH_PATH},
    { .fd = -1, .name = "hal", .flag = VTYSH_HAL, .path = HAL_VTYSH_PATH},
    { .fd = -1, .name = "syslog", .flag = VTYSH_SYSLOG, .path = SYSLOG_VTYSH_PATH},
    { .fd = -1, .name = "devm", .flag = VTYSH_DEVM, .path = DEVM_VTYSH_PATH},
    { .fd = -1, .name = "ftm", .flag = VTYSH_FTM, .path = FTM_VTYSH_PATH},
    { .fd = -1, .name = "filem", .flag = VTYSH_FILEM, .path = FILEM_VTYSH_PATH},
    { .fd = -1, .name = "ping", .flag = VTYSH_PING, .path = PING_VTYSH_PATH},
    { .fd = -1, .name = "ntp", .flag = VTYSH_NTP, .path = NTP_VTYSH_PATH},
    { .fd = -1, .name = "l2", .flag = VTYSH_L2, .path = L2_VTYSH_PATH},
    { .fd = -1, .name = "aaa", .flag = VTYSH_AAA, .path = AAA_VTYSH_PATH},
    { .fd = -1, .name = "snmpd", .flag = VTYSH_SNMPD, .path = SNMPD_VTYSH_PATH},
    { .fd = -1, .name = "alarm", .flag = VTYSH_ALARM, .path = ALARM_VTYSH_PATH},
    { .fd = -1, .name = "webd", .flag = VTYSH_WEB, .path = WEB_VTYSH_PATH},
    { .fd = -1, .name = "ipmc", .flag = VTYSH_IPMC, .path = IPMC_VTYSH_PATH},
    { .fd = -1, .name = "clock", .flag = VTYSH_CLOCK, .path = CLOCK_VTYSH_PATH},
    { .fd = -1, .name = "ospf6d", .flag = VTYSH_OSPF6D, .path = OSPF6_VTYSH_PATH},
    { .fd = -1, .name = "dhcpv6", .flag = VTYSH_DHCPV6, .path = DHCPV6_VTYSH_PATH},
    { .fd = -1, .name = "rmon", .flag = VTYSH_RMON, .path = RMOND_VTYSH_PATH},
    { .fd = -1, .name = "qosd", .flag = VTYSH_QOS, .path = QOS_VTYSH_PATH},
    { .fd = -1, .name = "statis", .flag = VTYSH_STATIS, .path = STATIS_VTYSH_PATH},
    { .fd = -1, .name = "sdhmgt", .flag = VTYSH_SDHMGT, .path = SDHMGT_VTYSH_PATH},
    { .fd = -1, .name = "sdhvcg", .flag = VTYSH_SDHVCG, .path = SDHVCG_VTYSH_PATH},
};


/* Using integrated config from Quagga.conf. Default is no. */
int vtysh_writeconfig_integrated = 0;

extern char config_default[];


char integrate_path[]	= VTYSH_CFG_FILE_PATH;

/* Integrated configuration file path */
char integrate_default[] = VTYSH_STARTUP_CONFIG;

/* Integrated factory configuration file path */
char integrate_addr_default[] = VTYSH_STARTUP_ADDR_CONFIG;
char current_config_default[] = VTYSH_CURRENT_CONFIG;


int g_vtysh_debug = 0;

int vtysh_is_app_connect(const char *name)
{
    unsigned int i = 0;
    int ret = 0;
    
    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(strcmp(vtysh_client[i].name, name) == 0)
        {
            if(vtysh_client[i].fd != -1)
            {
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            
            break;
        }
    } 

    return ret;
}

void vtysh_debug_init(void)
{
	FILE *fp = NULL;
	char buf[30];
	char *str = NULL;

	fp = fopen(VTYSH_DEBUG_SET, "r");
	if(fp == NULL)
	{
		return;
	}
	
	if(access(VTYSH_DEBUG_SET, F_OK) < 0)
	{
		g_vtysh_debug = 0;
	}
	else
	{
		
		g_vtysh_debug = VD_ALL;
	}

	fgets(buf, 30, fp);
	str = strstr(buf, "VD_CONFIG");
	if(str != NULL)
	{
		g_vtysh_debug = VD_CONFIG;
	}

	str = strstr(buf, "VD_ALL");
	if(str != NULL)
	{
		g_vtysh_debug = VD_ALL;
	}	

	fclose(fp);

	g_vtysh_debug = VD_CMD | VD_TELNET;
}

static int vtysh_debug_get(void)
{
	return g_vtysh_debug;
}

static void vtysh_debug_set(int val)
{
	g_vtysh_debug = val;
}

void vtysh_user_line_init(void)
{
	int i = 0;
	
	user_lines[0].index = 0;            // console 口只有一个
	user_lines[0].auth_mode = AUTH_MODE_SCHEME;
	user_lines[0].user_role = USER_ROLE_NETWORK_ADMIN;
	user_lines[0].iscfg = -1;
	user_lines[0].idle_time_set = VTYSH_IDLE_TIMEOUT_DEF;     // 默认用户无输入超时时间10分钟， 0 不计超时
	user_lines[0].idle_cnt_en = USER_LINE_IDLE_EN;
	for(i = USER_LINE_CONSOLE_NUM; i < USER_LINE_NUM; i++)
	{
		user_lines[i].index = i - 1;
		user_lines[i].islogin = USER_LINE_LOGIN_OUT;
		user_lines[i].auth_mode = AUTH_MODE_SCHEME;
		user_lines[i].user_role = USER_ROLE_NETWORK_OPERATOR;
		user_lines[i].iscfg = -1;
		user_lines[i].idle_time_set = VTYSH_IDLE_TIMEOUT_DEF;
		user_lines[0].idle_cnt_en = USER_LINE_IDLE_EN;
	}	
}

/*

line_index: console 0
            vty     0 - 63
*/
struct user_line *vtysh_user_line_get(int line, int line_index)
{
	int index = 0;
	
	if(line == USER_LINE_CONSOLE)
	{
		if(line_index < USER_LINE_CONSOLE_NUM)
		{
			index = line_index;
		}
		else
		{
			zlog_err("%s:err index %d\n",__FUNCTION__, index);
			return NULL;			
		}
	}
	else
	{
		if(line_index < USER_LINE_VTY_NUM)
		{
			index = line_index + USER_LINE_CONSOLE_NUM;
		}
		else
		{
			zlog_err("%s:err index %d\n",__FUNCTION__, index);
			return NULL;			
		}		
	}

	return &user_lines[index];
}


struct user_line *vtysh_idle_line_get(int line, int *index)
{
	int i = 0;
	struct user_line *ul = NULL;
	
	if(line == USER_LINE_CONSOLE) 
	{
		ul = vtysh_user_line_get(line, 0);		//console ֻ��һ�� 0
	}
	else
	{	
		for(i = 0; i < USER_LINE_VTY_NUM; i++) 
		{
			ul = vtysh_user_line_get(line, i);
			if((ul->islogin == USER_LINE_LOGIN_OUT) && (ul->index != -1))
			{				
				break;
			}
		}

		if(i >= USER_LINE_VTY_NUM) 
		{
			return NULL;
		}

		if(index)
		{
			*index = i;
		}		
	}

	VTY_DEBUG(VD_CMD, "get index %d, authmode %d", i, ul->auth_mode);

	return ul;
}

// index:console 0;   vty:0-63 
static int vtysh_auth_mode_set( int line, int index, AUTH_MODE_E mode)
{
	struct user_line *ul = NULL;

	VTY_DEBUG(VD_CMD, "line %d, index %d, mode %d", line, index, mode);
	if(((mode != AUTH_MODE_NONE) && (mode != AUTH_MODE_PASSWORD) && (mode != AUTH_MODE_SCHEME)) || (index >= 64)
		|| ((line != USER_LINE_CONSOLE) && (line != USER_LINE_VTY)))
	{
		zlog_err("%s: args err: line %d, index %d, mode %d\n", __FUNCTION__,line, index, mode);
		return -1;
	}
	
	if(line == USER_LINE_CONSOLE)
	{
		ul = vtysh_user_line_get(line, 0);  // CONSOLE ���� 0
	}
	else  // line vty 0 - 63 , ��Ӧuser_lines �� 1 - 64 ������ index + 1
	{
		ul = vtysh_user_line_get(line, index);
	}

	if(ul)
	{
		ul->auth_mode = mode;
		ul->index = index;
		ul->iscfg = 1;
	}
	else
	{
		return -1;
	}	
	
	return 0;
}

#if 0
static int vtysh_user_line_login_set( int line, int index, int login)
{
	struct user_line *ul = NULL;

	VTY_DEBUG(VD_CMD, "line %d, index %d, login %d", line, index, login);
	if(((login != USER_LINE_LOGIN_IN) && (login != USER_LINE_LOGIN_OUT)) || (index >= 64)
		|| ((line != USER_LINE_CONSOLE) && (line != USER_LINE_VTY)))
	{
		zlog_err("%s: args err: line %d, index %d, login %d \n", __FUNCTION__,line, index, login);
		return -1;
	}
	
	if(line == USER_LINE_CONSOLE)
	{
		ul = vtysh_user_line_get(line, 0);  // CONSOLE ���� 0
	}
	else  // line vty 0 - 63 , ��Ӧuser_lines �� 1 - 64 ������ index + 1
	{
		ul = vtysh_user_line_get(line, index);
	}

	if(ul)
	{
		ul->islogin = login;
	}
	else
	{
		return -1;
	}	
	
	return 0;
}
#endif

static int vtysh_user_role_set(int line, int index, USER_ROLE_E role)
{
	struct user_line *ul = NULL;

	VTY_DEBUG(VD_CMD, "line %s, index %d, role %s", (line == USER_LINE_CONSOLE)?"console":"vty",
			index, (role==USER_ROLE_NETWORK_ADMIN)?"admin":"operator");
			
	if(((role != USER_ROLE_NETWORK_ADMIN) && (role != USER_ROLE_NETWORK_OPERATOR)) || (index >= 64)
		|| ((line != USER_LINE_CONSOLE) && (line != USER_LINE_VTY)))
	{
		zlog_err("%s: args err: index %d, line %d, role %d \n", __FUNCTION__, index, line, role);
		return -1;
	}
	
	if(line == USER_LINE_CONSOLE)
	{
		ul = vtysh_user_line_get(line, 0);  // CONSOLE ���� 0
	}
	else  // line vty 0 - 63 , ��Ӧuser_lines �� 1 - 64 ������ index + 1
	{
		ul = vtysh_user_line_get(line, index);
	}

	if(ul)
	{
		ul->user_role = role;
		ul->iscfg = 1;
	}
	else
	{
		return -1;
	}	
	
	return 0;
}


static int vtysh_auth_pwd_set(int line, int index, const char *pwd)
{
	struct user_line *ul = NULL;

	VTY_DEBUG(VD_CMD, "line %s, index %d, pwd: %s", (line == USER_LINE_CONSOLE)?"console":"vty",
									index, pwd);
			
	if((index >= 64) || ((line != USER_LINE_CONSOLE) && (line != USER_LINE_VTY)) || !pwd)
	{
		zlog_err("%s: args err: index %d, line %d\n", __FUNCTION__, index, line);
		return -1;
	}

		
	if(line == USER_LINE_CONSOLE)
	{
		ul = vtysh_user_line_get(line, 0);  // CONSOLE ���� 0
	}
	else  // line vty 0 - 63 , ��Ӧuser_lines �� 1 - 64 ������ index + 1
	{
		ul = vtysh_user_line_get(line, index);
	}

	memset(ul->password, 0 , 8);
	memcpy(ul->password, pwd, strlen(pwd));
	ul->iscfg = 1;
	
	return 0;
}


static int vtysh_idle_time_set(int line, int index, int sec)
{
	struct user_line *ul = NULL;

	VTY_DEBUG(VD_CMD, "line %s, index %d, seconds: %d", (line == USER_LINE_CONSOLE)?"console":"vty",
									index, sec);
			
	if((index >= 64) || ((line != USER_LINE_CONSOLE) && (line != USER_LINE_VTY)))
	{
		zlog_err("%s: args err: index %d, line %d\n", __FUNCTION__, index, line);
		return -1;
	}
		
	if(line == USER_LINE_CONSOLE)
	{
		ul = vtysh_user_line_get(line, 0);  // CONSOLE ���� 0
	}
	else  // line vty 0 - 63 , ��Ӧuser_lines �� 1 - 64 ������ index + 1
	{
		ul = vtysh_user_line_get(line, index);
	}

	ul->idle_time_set = sec;
	if(sec == 0)
	{
        ul->idle_cnt_en = USER_LINE_IDLE_DIS;
	}
	
	ul->iscfg = 1;
	
	return 0;
}

int vtysh_line_config_update(struct login_session *session)
{
    struct vty *vty;

	if(session == NULL)
	{
		return -1;
	}
	if(session->session_type != SESSION_TELNET && session->session_type != SESSION_SSH 
	        && session->session_type != SESSION_CONSOLE)
	{
        return 0;
	}
	
	vty = session->vty;
	
	VTY_DEBUG(VD_CMD, "node %d, promt %d", vty->node, session->auth_prompt);
    if(session->ul->auth_mode == AUTH_MODE_NONE)
    {
		vty->node = CONFIG_NODE;   // xiawl AUTH_NODE -> CONFIG_NODE
		session->auth_prompt = 0;
    }
    else if(session->ul->auth_mode == AUTH_MODE_PASSWORD)
    {
		vty->node = AUTH_NODE; 
		session->auth_prompt = 0;
		if(session->session_type == SESSION_CONSOLE)
		{
			system("stty -echo");
		}		
    }
    else
    {
		vty->node = AUTH_NODE; 
		session->auth_prompt = 1;
    }	
	
	return 0;
}

/*
Login    ：1.3.6.1.4.1.9966.2.201.2.2.1.1.3.0.1
Logout   ：1.3.6.1.4.1.9966.2.201.2.2.1.1.3.0.2

Username ：1.3.6.1.4.1.9966.2.201.2.2.1.1.2.1            类型：字符串
Source   ：1.3.6.1.4.1.9966.2.201.2.2.1.1.2.2            类型：字符串
Reason   ：1.3.6.1.4.1.9966.2.201.2.2.1.1.2.3            类型：int32  （1：exceedRetries  2：authTimeout   3：otherReason）
*/
int vtysh_user_login_trap(struct login_session *session, int login)
{
	struct user_login_trap utrap;
	const char *src[3] = {"console", "telnet", "ssh"};

	if(session == NULL)
	{
		return -1;
	}

	if(session->ul->iscfg == -1)
	{
		return 0;
	}

	memset(&utrap, 0, sizeof(struct user_login_trap));	
	if(session->session_type == SESSION_TELNET)
	{
		memcpy(utrap.source, src[1], strlen(src[1]));
	}
	else if(session->session_type == SESSION_CONSOLE)
	{
		memcpy(utrap.source, src[0], strlen(src[0]));
	}
	else
	{
		memcpy(utrap.source, src[2], strlen(src[2]));
	}
	
	if(session->ul->auth_mode == AUTH_MODE_SCHEME)
	{
		memcpy(utrap.user_name, session->name_buf, 32);	
	}
	else
	{
		memcpy(utrap.user_name, utrap.source, strlen(utrap.source));
	}
	VTY_DEBUG(VD_CMD, "login %d, user_name:%s, source:%s", login, utrap.user_name, utrap.source);	

	/* First : add data struct to send trap msg to snmp */
	struct snmp_trap_hdr trap_hdr;
	struct snmp_trap_data trap_data;
	memset(&trap_hdr, 0, sizeof(struct snmp_trap_hdr));
	memset(&trap_data, 0, sizeof(struct snmp_trap_data));

	uchar buf[IPC_MSG_LEN];
	memset(buf, 0, IPC_MSG_LEN);

	/* Second : add trap msg header */
	oid oid_alarm_trap_terminal[] = {1, 3, 6, 1, 4, 1, 9966, 2, 201, 2, 2, 1, 1, 3, 0, 1};
	int oil_len = sizeof(oid_alarm_trap_terminal)/sizeof(oid_alarm_trap_terminal[0]);
	if(login)
	{
	    oid_alarm_trap_terminal[oil_len -1] = 1;
    }
    else
    {
        oid_alarm_trap_terminal[oil_len -1] = 2;
    }
    
	trap_hdr.trap_oid = oid_alarm_trap_terminal;
	trap_hdr.len_trap_oid = OID_LENGTH(oid_alarm_trap_terminal);	
	if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
	{
	    VTY_DEBUG(VD_CMD, "snmp_add_trap_msg_hdr ret 0");	
		return -1;
	}	

	/* Third : add trap msg data */
	oid oid_alarm_trap_terminal_row[] = {1, 3, 6, 1, 4, 1, 9966, 2, 201, 2, 2, 1, 1, 2, 0};

	const struct trap_info	trapInfo[]		= {
		{ 1 , SNMP_TYPE_OCTET_STR, (unsigned char *)(utrap.user_name), strlen(utrap.user_name)},
		{ 2 , SNMP_TYPE_OCTET_STR, (unsigned char *)(utrap.source), strlen(utrap.source)}   };	

	const int NodeNum = sizeof(trapInfo)/sizeof(trapInfo[0]);	
	
	int i = 0;
	size_t len = 0;
	for(i = 0; i < NodeNum; i++)
	{
		// index
		len 								 = sizeof(oid_alarm_trap_terminal_row)/sizeof(oid_alarm_trap_terminal_row[0]);
		oid_alarm_trap_terminal_row[len - 1] = trapInfo[i].iOid;

		trap_data.type = trapInfo[i].type;
		trap_data.len_data_oid = len;
		trap_data.len_data = trapInfo[i].len;
		trap_data.data_oid = oid_alarm_trap_terminal_row;
		trap_data.data = trapInfo[i].buf;

		if(0 == snmp_add_trap_msg_data(buf, &trap_data))
		{
		    VTY_DEBUG(VD_CMD, "snmp_add_trap_msg_data ret 0");	
			return 0;
		}
	}

	/* Last : send msg to snmp */
	snmp_send_trap_msg(buf, MODULE_ID_VTY, 0, 0);		
	
	return 0;
}

#if 0
// sec ��λ ��
static int vtysh_idle_time_get(int line, int index, int *sec)
{
	struct user_line *ul = NULL;

	VTY_DEBUG(VD_CMD, "line %s, index %d", (line == USER_LINE_CONSOLE)?"console":"vty",
									index);
			
	if((index >= 64) || ((line != USER_LINE_CONSOLE) && (line != USER_LINE_VTY)) || !sec)
	{
		zlog_err("%s: args err: index %d, line %d\n", __FUNCTION__, index, line);
		return -1;
	}

		
	if(line == USER_LINE_CONSOLE)
	{
		ul = vtysh_user_line_get(line, 0);  // CONSOLE ���� 0
	}
	else  // line vty 0 - 63 , ��Ӧuser_lines �� 1 - 64 ������ index + 1
	{
		ul = vtysh_user_line_get(line, index);
	}

	*sec = ul->idle_time_set;
	
	return 0;
}
#endif

static void
vclient_close(struct vtysh_client *vclient, int *connect_fd)
{
    if(vclient->fd >= 0 || *connect_fd >= 0)
    {
        fprintf(stderr,
                "Warning: closing connection to %s because of an I/O error!\n",
                vclient->name);
        close(vclient->fd);
        vclient->fd = -1;

        if(*connect_fd != -1)
        {
            *connect_fd = -1;
        }
    }
}

int vtysh_get_input(struct vty *vty)
{
    char input = '0';
    struct login_session *session;

    if(vty->type == VTY_SHELL)
    {
        struct termios term_new, term_init;
        FILE *cin;
        int err;
        int cin_fileno;

        cin = fopen("/dev/tty", "r");
        cin_fileno = fileno(cin);

        if(tcgetattr(cin_fileno , &term_init) == -1)
        {
            perror("Cannot get standard input description");
            exit(1);
        }

        term_new = term_init;

        term_new.c_lflag &= ~(ICANON | ECHO);
        term_new.c_cc[VMIN] = 1;
        term_new.c_cc[VTIME] = 0;

        err = tcsetattr(cin_fileno, TCSAFLUSH, &term_new);

        if(err == -1 || err == EINTR)
        {
            perror("Failed to change EOF character");
            exit(1);
        }

        while(1)
        {
            usleep(100000);
            input = getc(cin);

            if(input != 'n' && input != 'N' && input != 'y' && input != 'Y')
            {
                vty_out(vty, " %%Please input y/n:%s", VTY_NEWLINE);
            }
            else
            {
                break;
            }
        }

        err = tcsetattr(cin_fileno, TCSAFLUSH, &term_init);

        if(err == -1 || err == EINTR)
        {
            perror("Failed to change EOF character");
            exit(1);
        }
    }
    else if(vty->type == VTY_TERM  || vty->type == VTY_SSH)
    {
        vty->telnet_recv_tmp = 1;
        session = (struct login_session *)vty->session;

        while(1)
        {
            usleep(100000);

            if(session->tid_w_exit)
            {
                printf("read thread has exit, write break here\n");
                session->tid_r_exit = 1;
                pthread_exit(NULL);
//              break;
            }

            if(strlen(vty->buf_tmp) == 0)
            {
                continue;
            }

            if(*(vty->buf_tmp) != 'n' && *(vty->buf_tmp) != 'N' && *(vty->buf_tmp) != 'y' \
                    && *(vty->buf_tmp) != 'Y')
            {
                vty_out(vty, " %%Please input y/n:%s", VTY_NEWLINE);
                memset(vty->buf_tmp, 0, strlen(vty->buf_tmp));
                continue;
            }

            input = *(vty->buf_tmp);
            vty->telnet_recv_tmp = 0;
            memset(vty->buf_tmp, 0, strlen(vty->buf_tmp));
            break;
        }
    }

    if(input)
    {
        switch(input)
        {
            case 'n':
            case 'N':
                return VTY_REPLAY_NO;

            case 'y':
            case 'Y':
                return VTY_REPLAY_YES;

            default:
                break;
        }
    }

    return 0;
}

/* Following filled with debug code to trace a problematic condition
 * under load - it SHOULD handle it. */
#define ERR_WHERE_STRING "vtysh(): vtysh_client_execute(): "
static int
vtysh_client_execute(struct vtysh_client *vclient, const char *line, FILE *fp, struct vty *vty, int num)
{
    int ret;
    int write_bytes;
    char *buf;
    size_t bufsz;
    char *pbuf;
    size_t left;
    char *eoln;
    int nbytes;
    int i;
    int readln;
    int numnulls = 0;
    int connect_fd = 0;
    struct cmd_type send_line;
    char *cp;
    unsigned char input_buf[32];
    char read_from_consol;
    time_t last_time;
//  fd_set rset;
//  int n;
//  struct timeval tval;
//  tval.tv_sec = 3;
//  tval.tv_usec = 0;
    struct login_session *session;

    session = (struct login_session *)vty->session;

    if(vclient->fd < 0)
    {
        return CMD_SUCCESS;
    }

    if(vty->type == VTY_TERM || vty->type == VTY_SNMP
            || vty->type == VTY_FILE || vty->type == VTY_OFP
            || vty->type == VTY_SSH || vty->type == VTY_OSPF
            || vty->type == VTY_SYNC)
    {
        connect_fd = vty->connect_fd[num];
    }
    else
    {
        connect_fd = vclient->fd;
    }

    memset(&send_line, 0, sizeof(send_line));

    /*after ctrl^c, send sigint here*/
    if(vty->sig_int_recv)
    {
        send_line.cmd_flag |= SIG_INT_FLAG;
    }

    if(vty->type == VTY_FILE && reading_config_file)
    {
        vty->config_read_flag = 1;
        send_line.cmd_flag |= CONFIG_READ_FLAG;
    }
    else
    {
        vty->config_read_flag = 0;
    }

	memcpy(send_line.ttyname, vty->ttyname, SU_ADDRSTRLEN);

	if(session)
	{
		send_line.client_ip = session->client_ip;
		send_line.server_ip = session->server_ip;
		send_line.client_port = session->client_port;
		send_line.server_port = session->server_port;
	}

    /*do not allow snmp or file user to replay cmd*/
    if(vty->type == VTY_SNMP
            || vty->type == VTY_FILE
            || vty->type == VTY_SYNC)
    {
        send_line.cmd_flag |= (VTY_REPLAY_YES << VTY_CMD_REPLAY_BIT);
    }

    cp = send_line.lines;
    memcpy(cp, line, strlen(line));


    ret = write(connect_fd, &send_line, sizeof(send_line));

    if(ret <= 0)
    {
        vclient_close(vclient, &(vty->connect_fd[num]));
        FD_CLR(connect_fd, &vty->read_app_fd);

        return CMD_SUCCESS;
    }

    zlog_debug(VD_COMM, "send_str:%s  daemon:%s\n", line, vclient->name);

    /*we do not need to wait replay for sig_int msg*/
    if(vty->sig_int_recv)
    {
        g_int_flag = 1;
        return CMD_SUCCESS;
    }

    /* Allow enough room for buffer to read more than a few pages from socket. */
    bufsz = 5 * getpagesize() + 1;
    buf = XMALLOC(MTYPE_TMP, bufsz);
    memset(buf, 0, bufsz);
    pbuf = buf;

    last_time = time(NULL);

    while(1)
    {
        if(pbuf >= ((buf + bufsz) - 1))
        {
            fprintf(stderr, ERR_WHERE_STRING \
                    "warning - pbuf beyond buffer end.\n");
            XFREE(MTYPE_TMP, buf);
            return CMD_WARNING;
        }

        readln = (buf + bufsz) - pbuf - 1;
        nbytes = read(connect_fd, pbuf, readln);
        zlog_debug(VD_COMM, "%s:nbytes = %d \n",__FUNCTION__, nbytes);
        if(nbytes <= 0)
        {
            time_t time_curr = time(NULL);

            if(time_cmp_sec(last_time, time_curr, 3))
            {
                vty_out(vty, "%%Receive time out%s", VTY_NEWLINE);
                vclient_close(vclient, &(vty->connect_fd[num]));
                FD_CLR(connect_fd, &vty->read_app_fd);

                XFREE(MTYPE_TMP, buf);
                return CMD_WARNING;
            }

            if(errno == EINTR)
            {
                continue;
            }

            fprintf(stderr, ERR_WHERE_STRING "(%u)", errno);
            perror("");

            if(errno == EAGAIN || errno == EIO)
            {
                continue;
            }

            printf("vclient close\n");
            vclient_close(vclient, &(vty->connect_fd[num]));
            FD_CLR(connect_fd, &vty->read_app_fd);

            XFREE(MTYPE_TMP, buf);
            return CMD_WARNING;
        }

        /* If we have already seen 3 nulls, then current byte is ret code */
        if((numnulls == 3) && (nbytes == 1))
        {
            ret = pbuf[0];
            break;
        }

        pbuf[nbytes] = '\0';

        /* If the config needs to be written in file or stdout */
        if(fp)
        {

            if(vty->type == VTY_TERM || vty->type == VTY_OFP || vty->type == VTY_SSH) //not stdout
            {
                if(pbuf[0] != '\0')
                {
                    vty_out(vty, "%s", pbuf);
                }

                //                      vty_flush (vty->t_read);
            }
            else if(vty->type == VTY_SHELL)
            {
                if(ret == CMD_CONTINUE)
                {
                    fputs(pbuf, stdout);
                    fflush(stdout);
                }
                else
                {
//                    fputs(pbuf, fp);
//                    fflush(fp);
                    if(g_int_flag == 1)
                    {
                        fputs(pbuf, stdout);
                        fflush(stdout);                        
                    }
                    else
                    {
                        fputs(pbuf, fp);
                        fflush(fp);                        
                    }
              
                }
            }
        }

        /* At max look last four bytes */
        if(nbytes >= 4)
        {
            i = nbytes - 4;
            numnulls = 0;
        }
        else
        {
            i = 0;
        }

        /* Count the numnulls */
        while(i < nbytes && numnulls < 3)
        {
            if(pbuf[i++] == '\0')
            {
                numnulls++;
            }
            else
            {
                numnulls = 0;
            }
        }

        /* We might have seen 3 consecutive nulls so store the ret code before updating pbuf*/
        ret = pbuf[nbytes - 1];
        pbuf += nbytes;

        /* See if a line exists in buffer, if so parse and consume it, and
         * reset read position. If 3 nulls has been encountered consume the buffer before
         * next read.
         */
        if(((eoln = strrchr(buf, '\n')) == NULL) && (numnulls < 3))
        {
            continue;
        }

        if(eoln >= ((buf + bufsz) - 1))
        {
            fprintf(stderr, ERR_WHERE_STRING \
                    "warning - eoln beyond buffer end.\n");
        }

        /* If the config needs parsing, consume it */
        if(!fp)
        {
            vtysh_config_parse(buf, vclient->flag);
        }

        eoln++;
        left = (size_t)(buf + bufsz - eoln);

        /*
         * This check is required since when a config line split between two consecutive reads,
         * then buf will have first half of config line and current read will bring rest of the
         * line. So in this case eoln will be 1 here, hence calculation of left will be wrong.
         * In this case we don't need to do memmove, because we have already seen 3 nulls.
         */
        if(left < bufsz)
        {
            memmove(buf, eoln, left);
        }

        buf[bufsz - 1] = '\0';
        pbuf = buf + strlen(buf);

        /* got 3 or more trailing NULs? */
        if((numnulls >= 3) && (i < nbytes))
        {
            zlog_debug(VD_COMM, "%s: app ret code = %d \n", __FUNCTION__, ret);
            /*wait app for the result*/
            if(ret == CMD_WAIT)
            {
                if(vty->type == VTY_SNMP)
            	{
            	    XFREE(MTYPE_TMP, buf);
					return CMD_SUCCESS;
            	}
            	else
            	{
                    continue;
            	}                
            }
            else if(ret == CMD_REPLAY)
            {
                int replay = 0;
                replay = vtysh_get_input(vty);
                send_line.cmd_flag |= (replay << VTY_CMD_REPLAY_BIT);
                write_bytes = write(connect_fd, &send_line, sizeof(send_line));

                if(write_bytes <= 0)
                {
                    vclient_close(vclient, &(vty->connect_fd[num]));
                    XFREE(MTYPE_TMP, buf);
                    return CMD_SUCCESS;
                }
            }
            else if(ret == CMD_CONTINUE)
            {
                //flush 60 lines each time
                vty->flush_cp += FLUSH_LINE;
                numnulls = 0;
                memset(input_buf, 0, sizeof(input_buf));
                nbytes = 0;

                if(fp)
                {
                    char more[] = " --More-- ";
                    char erase[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                                     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
                                   };
//                        char erase1[] = {0x08, 0x08, 0x08, 0x08, ' ', ' ', ' ', ' ', 0x08, 0x08, 0x08, 0x08};
                    vty_out(vty, "%s", more);

                    if(vty->type == VTY_TERM || vty->type == VTY_SSH)
                    {
                        vty->telnet_recv_tmp = 1;

                        while(1)
                        {
                            usleep(100000);

                            if(session->tid_w_exit)
                            {
                                printf("read thread has exit, write break here\n");
                                session->tid_r_exit = 1;
                                break;
                            }

                            if(strlen(vty->buf_tmp) != 0)
                            {
                                zlog_debug(VD_COMM, "strlen(vty->buf_tmp):%d :%s\n", strlen(vty->buf_tmp), vty->buf_tmp);

                                if((vty->buf_tmp[0] == '\r' || vty->buf_tmp[0] == '\n' || vty->buf_tmp[0] == ' '))
                                {
                                    send_line.flush_cp = vty->flush_cp;
                                }
                                else if(vty->buf_tmp[0] == 'q' || vty->buf_tmp[0] == 'Q')
                                {
                                    send_line.flush_cp = 0xffffffff;
                                }
                                else
                                {
                                    memset(vty->buf_tmp, 0, strlen(vty->buf_tmp));
                                    continue;
                                }

                                memset(vty->buf_tmp, 0, strlen(vty->buf_tmp));
                                write_bytes = write(connect_fd, &send_line, sizeof(send_line));

                                if(write_bytes <= 0)
                                {
                                    vclient_close(vclient, &(vty->connect_fd[num]));
                                    XFREE(MTYPE_TMP, buf);
                                    return CMD_SUCCESS;
                                }

                                vty_out(vty, "%s", erase);
                                vty->telnet_recv_tmp = 0;
                                break;
                            }
                        }
                    }
                    else if(vty->type == VTY_SHELL)
                    {
                        struct termios term_new, term_init;
                        FILE *cin;
                        int err;
                        int cin_fileno;

                        cin = fopen("/dev/tty", "r");

                        if(cin == NULL)
                        {
                            zlog_err(" open /dev/tty err %d \n ", errno);
                            printf(" open /dev/tty err %d \n ", errno);
                            exit(1);
                        }

                        cin_fileno = fileno(cin);

                        if(cin_fileno == -1)
                        {
                            zlog_err(" fileno /dev/tty err %d \n ", errno);
                            exit(1);
                        }


                        if(tcgetattr(cin_fileno , &term_init) == -1)
                        {
                            perror("Cannot get standard input description");
                            exit(1);
                        }

                        term_new = term_init;

                        term_new.c_lflag &= ~(ICANON | ECHO);
                        term_new.c_cc[VMIN] = 1;
                        term_new.c_cc[VTIME] = 0;

                        err = tcsetattr(cin_fileno, TCSAFLUSH, &term_new);

                        if(err == -1 || err == EINTR)
                        {
                            perror("Failed to change EOF character");
                            exit(1);
                        }

                        while(1)
                        {
                            usleep(100000);
                            read_from_consol = getc(cin);

                            if((read_from_consol == '\r' || read_from_consol == '\n'    \
                                    || read_from_consol == ' '))
                            {
                                send_line.flush_cp = vty->flush_cp;
                            }
                            else if(read_from_consol == 'q' || read_from_consol == 'Q')
                            {
                                send_line.flush_cp = 0xffffffff;
                            }
                            else
                            {
                                continue;
                            }

                            write_bytes = write(connect_fd, &send_line, sizeof(send_line));

                            if(write_bytes <= 0)
                            {
                                vclient_close(vclient, &(vty->connect_fd[num]));
                                FD_CLR(connect_fd, &vty->read_app_fd);

                                fclose(cin);
                                XFREE(MTYPE_TMP, buf);
                                return CMD_SUCCESS;
                            }

                            printf("%s\n", erase);
                            //printf("1234567890987654321%s", erase);
                            //vty_out(vty, "%s", erase);
                            break;
                        }

                        err = tcsetattr(cin_fileno, TCSAFLUSH, &term_init);

                        if(err == -1 || err == EINTR)
                        {
                            perror("Failed to change EOF character");
                            exit(1);
                        }

                        fclose(cin);
                    }

                    continue;
                }
            }
            else
            {
                if(vty->flush_cp)
                {
                    vty->flush_cp = 0;
                }

                break;
            }
        }
    }


    if(!fp)
    {
        vtysh_config_parse(buf, vclient->flag);
    }

    zlog_debug(VD_COMM, "ret:%d\n", ret);

    XFREE(MTYPE_TMP, buf);
    return ret;
}


void
vtysh_pager_init(void)
{
    char *pager_defined;

    pager_defined = getenv("VTYSH_PAGER");

    if(pager_defined)
    {
        vtysh_pager_name = strdup(pager_defined);
    }
    else
    {
        vtysh_pager_name = strdup("more");
    }
}

/* Command execution over the vty interface. */
int
vtysh_execute_func(const char *line, int pager, struct vty *vty)
{
    int ret, cmd_stat = 0;;
    u_int i;
    vector vline;
    struct cmd_element *cmd = NULL;
    FILE *fp = NULL;
    int closepager = 0;
//  int saved_ret, saved_node;
    long long daemon;
    char *enable_passwd = NULL;
    int nbytes;
    unsigned char tel_enable_passwd[VTY_READ_BUFSIZ];
    int cp = 0;
    int fail = 0, passwd_match = 0;

    memset(&validity_check, 0 , sizeof(validity_check));

    /* Split readline string up into the vector. */
    vline = cmd_make_strvec(line);
    g_int_flag = 0;
    if(vline == NULL)
    {
        vty->sync_flag = CMD_LOCAL;
        return CMD_SUCCESS;
    }

    ret = cmd_execute_command(vline, vty, &cmd, 1);

    vty->pre_node = vty->node;

    if(cmd != NULL)
    {

        vty->sync_flag = cmd->sync;
        zlog_debug(VD_COMM, "vty->sync_flag:%d\n", vty->sync_flag);
    }
    else
    {
        zlog_debug(VD_COMM, "do not sync\n");
        vty->sync_flag = CMD_LOCAL;
    }

    cmd_free_strvec(vline);

    switch(ret)
    {
        case CMD_WARNING:
            if(vty->type == VTY_FILE)
            {
                fprintf(stdout, "Warning...\n");
            }

            cmd_stat = ret;
			break;

        case CMD_ERR_AMBIGUOUS:
            if(vty)
            {
                vty_warning_out(vty, "Ambiguous command.%s", VTY_NEWLINE);
            }
            else
            {
                fprintf(stdout, "WARNING: Ambiguous command.\n");
            }

            cmd_stat = ret;
			break;

        case CMD_ERR_NO_MATCH:
            if(vty)
            {
                if(validity_check.ipv4_check_faile)
                {
                    validity_check.ipv4_check_faile = 0;
//                      vty_out(vty, "%% Wrong IPV4 format. eg: A.B.C.D%s",VTY_NEWLINE);
                    vty_error_out(vty, "Wrong IPV4 format.%s", VTY_NEWLINE);

                }
                else if(validity_check.ipv4_prefix_check_faile)
                {
                    validity_check.ipv4_prefix_check_faile = 0;
//                      vty_out(vty, "%% Wrong IPV4 format. eg: A.B.C.D/M%s",VTY_NEWLINE);
                    vty_error_out(vty, "Wrong IPV4 format.%s", VTY_NEWLINE);
                }
                else if(validity_check.ifname_check_faile)
                {
                    validity_check.ifname_check_faile = 0;
//                      vty_out(vty, "%% Wrong USP format. eg: <0-7>/<0-31>/<1-255>.[<1-4095>]%s",VTY_NEWLINE);
                    vty_error_out(vty, "Wrong USP format.%s", VTY_NEWLINE);
                }
                else if(validity_check.mac_check_faile)
                {
                    validity_check.mac_check_faile = 0;
//                      vty_out(vty, "%% Wrong MAC format. eg: XX:XX:XX:XX:XX:XX%s",VTY_NEWLINE);
                    vty_error_out(vty, "Wrong MAC format.%s", VTY_NEWLINE);
                }
                else if(validity_check.range_check_faile)
                {
                    validity_check.range_check_faile = 0;
                    vty_error_out(vty, "Wrong data scale.%s", VTY_NEWLINE);
                }
                else if(validity_check.string_check_faile)
                {
                    validity_check.string_check_faile = 0;
                    vty_error_out(vty, "String is too long to match. string size <1-255>.%s", VTY_NEWLINE);
                }
                else if(validity_check.name_check_faile)
                {
                    validity_check.name_check_faile = 0;
                    vty_error_out(vty, "Cmd name is too long to match. name size <1-31>.%s", VTY_NEWLINE);
                }
                else
                {
                    vty_warning_out(vty, "Unknown command: %s, node %d %s", line, vty->node, VTY_NEWLINE);
                }
            }
            else
            {
                fprintf(stdout, "WARNING: Unknown command.%s, node %d\n", line , vty->node );
            }

            cmd_stat = ret;
			break;

        case CMD_ERR_INCOMPLETE:
            if(vty)
            {
                vty_warning_out(vty, "Command incomplete: %s, node %d %s",line , vty->node, VTY_NEWLINE);
            }
            else
            {
                fprintf(stdout, "WARNING: Command incomplete.%s, node %d\n", line , vty->node);
            }

            cmd_stat = ret;
			break;

        case CMD_SUCCESS_DAEMON:
        {
            /* FIXME: Don't open pager for exit commands. popen() causes problems
             * if exited from vtysh at all. This hack shouldn't cause any problem
             * but is really ugly. */
            if(vty->privilege < cmd->level
                    && (vty->type == VTY_TERM
                        || vty->type == VTY_SHELL
                        || vty->type == VTY_SSH))
            {
                vty_warning_out(vty, "Permission deny! Cmd need level %d%s", cmd->level, VTY_NEWLINE);
                return CMD_WARNING;
            }

            if(pager && vtysh_pager_name && (strncmp(line, "exit", 4) != 0))
            {
                fp = popen(vtysh_pager_name, "w");

                if(fp == NULL)
                {
                    perror("popen failed for pager");
                    fp = stdout;
                }
                else
                {
                    closepager = 1;
                }
            }
            else
            {
                fp = stdout;
            }


            /*enable node password*/
            if((!strcmp(cmd->string, "login") || !strcmp(cmd->string, "sysrun"))
                    && vty->type != VTY_FILE && vty->type != VTY_SNMP && vty->type != VTY_OFP
                    && vty->type != VTY_OSPF && vty->type != VTY_SYNC)
            {
//      struct login_session *session = (struct login_session *)vty->session;

                vty_out(vty, "%sInsert password: %s", VTY_NEWLINE, VTY_NEWLINE);

                if(vty->type == VTY_TERM || vty->type == VTY_SSH)
                {
                    vty->telnet_recv_tmp = 1;
                    memset(tel_enable_passwd, 0, sizeof(tel_enable_passwd));

                    while(1)
                    {
                        usleep(100000);

                        if(strlen(vty->buf_tmp) != 0)
                        {
                            nbytes = strlen(vty->buf_tmp);

                            for(int i = 0; i < nbytes; i++)
                            {
                                if(*(vty->buf_tmp + i) == '\r' || *(vty->buf_tmp + i) == '\n')
                                {
                                    if(!strcmp((char *)tel_enable_passwd, host.enable))
                                    {
                                        vty->telnet_recv_tmp = 0;
                                        memset(vty->buf_tmp, 0, nbytes);
                                        passwd_match = 1;
                                        vty_out(vty, "%s", VTY_NEWLINE);
                                        break;
                                    }
                                    else
                                    {
                                        memset(tel_enable_passwd, 0, sizeof(tel_enable_passwd));
                                        memset(vty->buf_tmp, 0, nbytes);
                                        cp = 0;
                                        fail++;

                                        if(fail >= 3)
                                        {
                                            vty->telnet_recv_tmp = 0;
                                            vty_error_out(vty, "Bad passwords, too many failures!%s", VTY_NEWLINE);
                                            return CMD_WARNING;
                                        }
                                        else
                                        {
                                            vty_out(vty, "Try again: %s", VTY_NEWLINE);
                                        }

                                        break;
                                    }
                                }
                                else if((int)(*(vty->buf_tmp + i)) == 0x7f || (int)(*(vty->buf_tmp + i)) == 0x08)
                                {
                                    if(cp > 0)
                                    {
                                        cp--;
                                        tel_enable_passwd[cp] = 0;
                                    }
                                }
                                else
                                {
                                    tel_enable_passwd[cp] = *(vty->buf_tmp + i);
                                    cp = cp + 1;
                                }

                                
                            }
							memset(vty->buf_tmp, 0, nbytes);
                            if(passwd_match)
                            {
                                break;
                            }
                        }

                    }
                }

                else if(vty->type == VTY_SHELL)
                {
                    system("stty -echo");

                    while(1)
                    {
                        usleep(100000);

                        if(enable_passwd)
                        {
                            free(enable_passwd);
                            enable_passwd = NULL;
                        }

                        enable_passwd = readline(NULL);

                        if(!strcmp(enable_passwd, host.enable))
                        {

                            system("stty echo");
                            break;
                        }
                        else
                        {
                            fail++;

                            if(fail >= 3)
                            {
                                vty_warning_out(vty, "Bad passwords, too many failures!%s", VTY_NEWLINE);
                                return CMD_WARNING;
                            }
                            else
                            {
                                vty_out(vty, "Try again: %s", VTY_NEWLINE);
                            }
                        }
                    }
                }
            }

            if(! strcmp(cmd->string, "configure terminal"))
            {
                vty->cmd_process_lock = 1;

                for(i = 0; i < array_size(vtysh_client); i++)
                {
                    cmd_stat = vtysh_client_execute(&vtysh_client[i], line, fp, vty, i);

                    if(cmd_stat == CMD_WARNING)
                    {
                        break;
                    }
                }

                vty->cmd_process_lock = 0;

                if(cmd_stat)
                {
                    line = "end";
                    vline = cmd_make_strvec(line);

                    if(vline == NULL)
                    {
                        if(pager && vtysh_pager_name && fp && closepager)
                        {
                            if(pclose(fp) == -1)
                            {
                                perror("pclose failed for pager");
                            }

                            fp = NULL;
                        }

                        return CMD_SUCCESS;
                    }

                    ret = cmd_execute_command(vline, vty, &cmd, 1); //only match do not excute,becuse of deamon
                    cmd_free_strvec(vline);

                    if(ret != CMD_SUCCESS_DAEMON)
                    {
                        break;
                    }
                }
                else if(cmd->func)
                {
                    (*cmd->func)(cmd, vty, 0, NULL);
                    break;
                }
            }

            cmd_stat = CMD_SUCCESS;

            /*we do not send "enable" to apps*/
            if(!strcmp(cmd->string, "login"))
            {
                cmd_stat = CMD_SUCCESS;
            }
            else
            {
                if(cmd->order)
                {
                    struct listnode *node, *nnode;
                    int *deamon_index;
                    int index;
                    vty->cmd_process_lock = 1;

                    for(ALL_LIST_ELEMENTS(cmd->order, node, nnode, deamon_index))
                    {
                        index = *deamon_index - 1;

                        if((cmd->daemon & vtysh_client[index].flag) && (vtysh_client[index].fd != -1))
                        {
                            zlog_debug(VD_CMD, "%s: c vtysh_client_execute .deamons\n", __FUNCTION__);
                            cmd_stat = vtysh_client_execute(&vtysh_client[index], line, fp, vty, index);

                            if(cmd_stat != CMD_SUCCESS)
                            {
                                break;
                            }
                        }
                    }

                    vty->cmd_process_lock = 0;
                }
                else
                {
                    vty->cmd_process_lock = 1;

                    for(i = 0; i < array_size(vtysh_client); i++)
                    {
                        if((cmd->daemon & vtysh_client[i].flag) && (vtysh_client[i].fd != -1))
                        {
                            zlog_debug(VD_CMD, "%s: c vtysh_client_execute \n", __FUNCTION__);
                            cmd_stat = vtysh_client_execute(&vtysh_client[i], line, fp, vty, i);

                            if(cmd_stat != CMD_SUCCESS)
                            {
                                break;
                            }
                        }
                    }

                    vty->cmd_process_lock = 0;
                }
            }

            if(cmd_stat != CMD_SUCCESS)
            {
                break;
            }

//  if (cmd->func)
//    (*cmd->func) (cmd, vty, 0, NULL);
            if(cmd->func)
            {
				pthread_mutex_lock(&cmd->daemon_lock);
                vline = cmd_make_strvec(line);
                daemon = cmd->daemon;
                cmd->daemon = 0;
                zlog_debug(VD_COMM, "%s: c cmd_execute_command \n", __FUNCTION__);
                ret = cmd_execute_command(vline, vty, &cmd, 1);
                cmd_free_strvec(vline);
                cmd->daemon = daemon;
				
				pthread_mutex_unlock(&cmd->daemon_lock);

                if(ret != CMD_SUCCESS)
                {
                    (*cmd->func)(cmd, vty, 0, NULL);
                }
            }
        }
    }

    if(pager && vtysh_pager_name && fp && closepager)
    {
        if(pclose(fp) == -1)
        {
            perror("pclose failed for pager");
        }

        fp = NULL;
    }

    return cmd_stat;
}

int
vtysh_execute_no_pager(const char *line, struct vty *vty)
{
    return vtysh_execute_func(line, 0, vty);
}

int
vtysh_execute(const char *line, struct vty *vty)
{
    int ret;
    struct login_session *session = NULL;
    vector vline;
    
    session = (struct login_session *)vty->session;
    if(session == NULL)
    {
        return -1;
    }
    
    vline = cmd_make_strvec(line);

    /*备卡不允许用户下发配置*/
    if(syncvty.vty_slot == VTY_SLAVE_SLOT
            && vty->type != VTY_SYNC
            && vty->type != VTY_OSPF
            && strstr(line, "show") == NULL
            && strstr(line, "debug") == NULL
            && strstr(line, "sysrun") == NULL
            && strstr(line, "slave_config") == NULL
            && strstr(line, "exit") == NULL)
    {
        if((vline != NULL) && (g_slave_config_enable == 0))
        {
            vty_out(vty, "WARNING:only show is allowed on slave board%s", VTY_NEWLINE);
            cmd_free_strvec(vline);
            return CMD_WARNING;
        }
    }

    if(vline != NULL)
    {
        cmd_free_strvec(vline);
    }

    /* session is busy while block for cmd replay. timer will
     * stop for this user*/
    if(vty->type == VTY_TERM || vty->type == VTY_SHELL || vty->type == VTY_SSH)
    {
        if(session->session_status != SESSION_CLOSE)
        {
            session->session_status = SESSION_BUSY;
        }

        session->timer_count = 0;

        /*cancle timer before exec cmd*/

    }

    zlog_debug(VD_COMM, "vtysh_execute:%s, node %d\n", line, vty->node);
    ret = vtysh_execute_func(line, 1, vty);

    if(syncvty.vty_slot == VTY_MAIN_SLOT
            && vty->sync_flag == CMD_SYNC
            && vty->type != VTY_FILE
            && ret == CMD_SUCCESS)
    {

        vtysh_mster_catch_realtime_cmd(line, vty);
        vty->sync_flag = CMD_LOCAL;
    }

    if(session->session_status != SESSION_CLOSE
            && session->session_status != SESSION_AUTH)
    {
        session->session_status = SESSION_NORMAL;
    }

        /*after */
//          vty->t_timeout =
//              thread_add_timer (vtysh_master, vty_timeout, vty, vty->v_timeout);


    return ret;
}

/* Set time out value. */
int
exec_timeout(struct vty *vty, const char *min_str, const char *sec_str)
{
    unsigned long timeout = 0;
    struct vty *v;
    unsigned int i;

    /* min_str and sec_str are already checked by parser.  So it must be
       all digit string. */
    if(min_str)
    {
        timeout = strtol(min_str, NULL, 10);
        timeout *= 60;
    }

    if(sec_str)
    {
        timeout += strtol(sec_str, NULL, 10);
    }

    for(i = 0; i < vector_active(vtyvec); i++)
        if((v = vector_slot(vtyvec, i)) != NULL)
        {
            if(v->type != VTY_TERM && v->type != VTY_SSH)
            {
                continue;
            }

            v->v_timeout = timeout;
            vtysh_add_timer(v);
//          vty_event (VTY_TIMEOUT_RESET, 0, v);
        }

    vty_timeout_val = timeout;

    return CMD_SUCCESS;
}

int
vty_timeout(struct thread *thread)
{
    struct vty *vty;
    struct login_session *session;
    /*超时后，原thread的内存，会被新的thread覆盖，如果vty->t_timeout
     *没有赋空，其他地方调用thread_cancle时，会把其他的thread取消掉*/

    vty = THREAD_ARG(thread);
    vty->t_timeout = NULL;
//  vty->v_timeout = 0;
    session = (struct login_session *)vty->session;

    zlog_debug(VD_COMM, "vty_timeout\n");

    if(session == NULL)
    {
        zlog_err("vty_timeout fail to find session! vty->ttyname:%s\n", vty->ttyname);
        return -1;
    }

    if(session->session_status == SESSION_CLOSE)
    {
        zlog_err("vty_timeout err: SESSION_CLOSE\n");
        return -1;
    }

    /**/
    session->timer_count++;
#if 0
    /*no need to add timer if session is busy*/
    if(session->session_status != SESSION_BUSY)
    {
        pthread_mutex_lock(&session_timer_lock);
        vty->t_timeout =
           // thread_add_timer(vtysh_master, vty_timeout, vty, vty->v_timeout);
        pthread_mutex_unlock(&session_timer_lock);
    }
#endif
    return 0;
}


void
vtysh_add_timer(struct vty *vty)
{
    zlog_debug(VD_COMM, "vtysh_add_timer\n");

    if(vty->t_timeout)
    {
        //thread_cancel(vty->t_timeout);
		high_pre_timer_delete(vty->t_timeout);
        vty->t_timeout = NULL;
    }

    if(vty->v_timeout)
    {
        pthread_mutex_lock(&session_timer_lock);
        //vty->t_timeout = thread_add_timer(vtysh_master, vty_timeout, vty, vty->v_timeout);
		vty->t_timeout = high_pre_timer_add("vtyshTimeOutTimer", LIB_TIMER_TYPE_NOLOOP, vty_timeout, vty, vty->v_timeout);
        pthread_mutex_unlock(&session_timer_lock);
    }
}


/* Configration make from file. */
int
vtysh_config_from_file(struct vty *vty, FILE *fp)
{
    int ret;
    long long daemon;
    vector vline;
    struct cmd_element *cmd;

    while(fgets(vty->buf, VTY_BUFSIZ, fp))
    {
        ret = command_config_read_one_line(vty, &cmd, 1);

        switch(ret)
        {
            case CMD_WARNING:
                if(vty->type == VTY_FILE)
                {
                    fprintf(stdout, "Warning...\n");
                }

                break;

            case CMD_ERR_AMBIGUOUS:
                fprintf(stdout, "%% Ambiguous command: %s.\n", vty->buf);
                break;

            case CMD_ERR_NO_MATCH:
                fprintf(stdout, "%% Unknown command: %s", vty->buf);
                break;

            case CMD_ERR_INCOMPLETE:
                fprintf(stdout, "%% Command incomplete: %s\n", vty->buf);
                break;

            case CMD_SUCCESS_DAEMON:
            {
                u_int i;
                int cmd_stat = CMD_SUCCESS;

                if(!strcmp(cmd->string, "login"))
                {
                    cmd_stat = CMD_SUCCESS;
                }
                else
                {
                    if(cmd->order)
                    {
                        struct listnode *node, *nnode;
                        int *deamon_index;
                        vty->cmd_process_lock = 1;

                        for(ALL_LIST_ELEMENTS(cmd->order, node, nnode, deamon_index))
                        {
                            if(cmd->daemon & vtysh_client[*deamon_index - 1].flag)
                            {
                                cmd_stat = vtysh_client_execute(&vtysh_client[*deamon_index - 1], vty->buf, fp, vty, *deamon_index - 1);

                                if(cmd_stat != CMD_SUCCESS)
                                {
                                    break;
                                }
                            }
                        }

                        vty->cmd_process_lock = 0;
                    }
                    else
                    {
                        vty->cmd_process_lock = 1;

                        for(i = 0; i < array_size(vtysh_client); i++)
                        {
                            if(cmd->daemon & vtysh_client[i].flag)
                            {
                                cmd_stat = vtysh_client_execute(&vtysh_client[i], vty->buf, fp, vty, i);

                                if(cmd_stat != CMD_SUCCESS)
                                {
                                    break;
                                }
                            }
                        }

                        vty->cmd_process_lock = 0;
                    }
                }

                if(cmd_stat != CMD_SUCCESS)
                {
                    break;
                }

                if(cmd->func)
                {
                    vline = cmd_make_strvec(vty->buf);
					pthread_mutex_lock(&cmd->daemon_lock);
                    daemon = cmd->daemon;
                    cmd->daemon = 0;
                    ret = cmd_execute_command(vline, vty, &cmd, 1);

                    if(vline != NULL)
                    {
                        cmd_free_strvec(vline);
                    }

                    cmd->daemon = daemon;
					pthread_mutex_unlock(&cmd->daemon_lock);

                    if(ret != CMD_SUCCESS)
                    {
                        (*cmd->func)(cmd, vty, 0, NULL);
                    }
                }

#if 0

                if(cmd->func)
                {
                    (*cmd->func)(cmd, vty, 0, NULL);
                }

#endif
            }
        }
    }

    return CMD_SUCCESS;
}

int
vtysh_config_exec_one_line(struct vty *vty, FILE *fp)
{
    int ret;
    long long daemon;
    vector vline;
    struct cmd_element *cmd;
    zlog_debug(VD_COMM, "vtysh_config_exec_one_line\n");

    ret = command_config_read_one_line(vty, &cmd, 1);

    switch(ret)
    {
        case CMD_WARNING:
            if(vty->type == VTY_FILE)
            {
                fprintf(stdout, "Warning...\n");
            }

            break;

        case CMD_ERR_AMBIGUOUS:
            fprintf(stdout, "%% Ambiguous command.\n");
            break;

        case CMD_ERR_NO_MATCH:
            fprintf(stdout, "%% Unknown command: %s", vty->buf);
            break;

        case CMD_ERR_INCOMPLETE:
            fprintf(stdout, "%% Command incomplete.\n");
            break;

        case CMD_SUCCESS_DAEMON:
        {
            u_int i;
            int cmd_stat = CMD_SUCCESS;

            if(cmd->order)
            {
                struct listnode *node, *nnode;
                int *deamon_index;
                vty->cmd_process_lock = 1;

                for(ALL_LIST_ELEMENTS(cmd->order, node, nnode, deamon_index))
                {
                    if(cmd->daemon & vtysh_client[*deamon_index - 1].flag)
                    {
                        cmd_stat = vtysh_client_execute(&vtysh_client[*deamon_index - 1], vty->buf, fp, vty, *deamon_index - 1);

                        if(cmd_stat != CMD_SUCCESS)
                        {
                            break;
                        }
                    }
                }

                vty->cmd_process_lock = 0;
            }
            else
            {
                vty->cmd_process_lock = 1;

                for(i = 0; i < array_size(vtysh_client); i++)
                {
                    if(cmd->daemon & vtysh_client[i].flag)
                    {
                        cmd_stat = vtysh_client_execute(&vtysh_client[i], vty->buf, fp, vty, i);

                        if(cmd_stat != CMD_SUCCESS)
                        {
                            break;
                        }
                    }
                }

                vty->cmd_process_lock = 0;
            }

            if(cmd_stat != CMD_SUCCESS)
            {
                break;
            }

            if(cmd->func)
            {
                vline = cmd_make_strvec(vty->buf);
				pthread_mutex_lock(&cmd->daemon_lock);
                daemon = cmd->daemon;
                cmd->daemon = 0;
                ret = cmd_execute_command(vline, vty, &cmd, 1);

                if(vline != NULL)
                {
                    cmd_free_strvec(vline);
                }

                cmd->daemon = daemon;
				pthread_mutex_unlock(&cmd->daemon_lock);

                if(ret != CMD_SUCCESS)
                {
                    (*cmd->func)(cmd, vty, 0, NULL);
                }
            }
        }
    }

    return 0;
}


/* Configration make from file. */
int
vtysh_config_from_file_interface(struct vty *vty, FILE *fp, char *ifname)
{
    int   ret = CMD_ERR_NO_MATCH2;
    char *pnt;

//  printf("ifname:%s\t len:%d\n",ifname, strlen(ifname));
    while(fgets(vty->buf, VTY_BUFSIZ, fp))
    {
//        usleep(1000);
        pnt = vty->buf;

        while(*pnt != '\0')
        {
            while(*pnt != '\n' && *pnt != '\r' && *pnt != '\0')
            {
                pnt++;
            }

            *pnt = '\0';
            pnt++;

            if(*(pnt + 1) == '\n' || *(pnt + 1) == '\r')
            {
                *pnt = '\0';
                pnt++;
            }

            if(strcmp(vty->buf, ifname) == 0)
            {
                ret = CMD_SUCCESS;
                zlog_debug(VD_COMM, "exec buf:%s\n", vty->buf);
                vtysh_config_exec_one_line(vty, fp);

                while(fgets(vty->buf, VTY_BUFSIZ, fp))
                {
                    usleep(100);
                    pnt = vty->buf;

                    if(vty->buf[0] == '!')
                    {
                        zlog_debug(VD_COMM, "find !\n");
                        vty_clear_buf(vty);
                        continue;
                    }

                    /*not sub interface node, break*/
                    if(vty->buf[0] != ' ' && strncmp(vty->buf, ifname, strlen(ifname)) != 0)
                    {
                        zlog_debug(VD_COMM, "not sub-interface vty->buf:%s ifname:%s\n", vty->buf, ifname);
                        break;
                    }

                    while(*pnt != '\n' && *pnt != '\r' && *pnt != '\0')
                    {
                        pnt++;
                    }

                    *pnt = '\0';
                    pnt++;

                    if(*(pnt + 1) == '\n' || *(pnt + 1) == '\r')
                    {
                        *pnt = '\0';
                        pnt++;
                    }

                    zlog_debug(VD_COMM, "exec sub_cmd buf:%s\n", vty->buf);
                    vtysh_config_exec_one_line(vty, fp);
                }

                break;
            }
        }
    }

    return ret;
}

/* We don't care about the point of the cursor when '?' is typed. */
static int
vtysh_rl_describe(void)
{
    int ret;
    unsigned int i;
    vector vline;
    vector describe;
    int width;
    struct cmd_token *token;

    vline = cmd_make_strvec(rl_line_buffer);

    /* In case of '> ?'. */
    if(vline == NULL)
    {
        vline = vector_init(1);
        vector_set(vline, NULL);
    }
    else if(rl_end && isspace((int) rl_line_buffer[rl_end - 1]))
    {
        vector_set(vline, NULL);
    }

    describe = cmd_describe_command(vline, vty, &ret);

    fprintf(stdout, "\n");

    /* Ambiguous and no match error. */
    switch(ret)
    {
        case CMD_ERR_AMBIGUOUS:
            cmd_free_strvec(vline);
            fprintf(stdout, "%% Ambiguous command.\n");
            rl_on_new_line();
            return 0;
            break;

        case CMD_ERR_NO_MATCH:
            cmd_free_strvec(vline);
            fprintf(stdout, "%% There is no matched command.\n");
            rl_on_new_line();
            return 0;
            break;
    }

    /* Get width of command string. */
    width = 0;

    for(i = 0; i < vector_active(describe); i++)
        if((token = vector_slot(describe, i)) != NULL)
        {
            int len;

            if(token->cmd[0] == '\0')
            {
                continue;
            }

            len = strlen(token->cmd);

            if(token->cmd[0] == '.')
            {
                len--;
            }

            if(width < len)
            {
                width = len;
            }
        }

    for(i = 0; i < vector_active(describe); i++)
        if((token = vector_slot(describe, i)) != NULL)
        {
            if(token->cmd[0] == '\0')
            {
                continue;
            }

            if(! token->desc)
                fprintf(stdout, "  %-s\n",
                        token->cmd[0] == '.' ? token->cmd + 1 : token->cmd);
            else
                fprintf(stdout, "  %-*s  %s\n",
                        width,
                        token->cmd[0] == '.' ? token->cmd + 1 : token->cmd,
                        token->desc);
        }

    cmd_free_strvec(vline);
    vector_free(describe);

    rl_on_new_line();

    return 0;
}

/* Result of cmd_complete_command() call will be stored here
 * and used in new_completion() in order to put the space in
 * correct places only. */
int complete_status;

static char *
command_generator(const char *text, int state)
{
    vector vline;
    static char **matched = NULL;
    static int index = 0;

    /* First call. */
    if(! state)
    {
        index = 0;

        if(vty->node == AUTH_NODE)
        {
            return NULL;
        }

        vline = cmd_make_strvec(rl_line_buffer);

        if(vline == NULL)
        {
            return NULL;
        }

        if(rl_end && isspace((int) rl_line_buffer[rl_end - 1]))
        {
            vector_set(vline, NULL);
        }

        matched = cmd_complete_command(vline, vty, &complete_status);
    }

    if(matched && matched[index])
    {
        return matched[index++];
    }

    return NULL;
}

static char **
new_completion(char *text, int start, int end)
{
    char **matches;

    matches = rl_completion_matches(text, command_generator);

    if(matches)
    {
        rl_point = rl_end;

        if(complete_status != CMD_COMPLETE_FULL_MATCH)
            /* only append a space on full match */
        {
            rl_completion_append_character = '\0';
        }
    }

    return matches;
}

/* When '^Z' is received from vty, move down to the enable mode. */
static int
vtysh_end(struct vty *vty)
{
    switch(vty->node)
    {
        case CONFIG_NODE:
            /* Nothing to do. */
            break;

        default:
            vty->node = CONFIG_NODE;
            break;
    }

    return CMD_SUCCESS;
}


static struct cmd_node user_line_node =
{
    USER_LINE_NODE,
     "%s(config-line)# ",   
//     1
};


DEFUNSH(VTYSH_ALL,
        vtysh_end_all,
        vtysh_end_all_cmd,
        "end",
        "End current mode and change to config mode\n")
{
    return vtysh_end(vty);
}

DEFUNSH(VTYSH_ALL,
        vtysh_return_all,
        vtysh_return_all_cmd,
        "return",
        "Return current mode and change to config mode\n")
{
    return vtysh_end(vty);
}

DEFUNSH(VTYSH_ALL,
        vtysh_sysrun,
        vtysh_sysrun_cmd,
        "sysrun",
        "system operation\n")
{
    vty->node = SYSRUN_NODE;
    return CMD_SUCCESS;
}

DEFUN(vtysh_sysview,
        vtysh_sysview_cmd,
        "system-view",
        "system-view \n")
{
    vty->node = CONFIG_NODE;
    return CMD_SUCCESS;
}

DEFUNSH(VTYSH_ALL,
        vtysh_config_terminal,
        vtysh_config_terminal_cmd,
        "configure terminal",
        "Configuration from vty interface\n"
        "Configuration terminal\n")
{
    vty->node = CONFIG_NODE;
    return CMD_SUCCESS;
}

int
vtysh_exit(struct vty *vty)
{
    struct login_session *session = (struct login_session *)vty->session;;

    switch(vty->node)
    {
        case CONFIG_NODE:
            if(vty_shell(vty))
            {
                if(session->user_name)
                {
					if(0 == strcmp(session->name_buf, DEF_VTY_USER_NAME))
					{
						gVtyLocalUserLogin = 0;
						vty_consol_close(session);
					}
                    else if(ERRNO_SUCCESS == vty_remote_aaa_logout(session))
                    {
                        vty_consol_close(session);
                    }
                    else
                    {
                        vty_out(vty, "exit failed, try again%s%s", VTY_NEWLINE, VTY_NEWLINE);
                    }
                }

//          if(session->device_name)
//              XFREE(MTYPE_HOST, host.device_name);

//          session->device_name = XSTRDUP (MTYPE_HOST, "Hios");
            }
            else if(vty->type == VTY_TERM || vty->type == VTY_SSH)
            {
				if(session->user_name)
                {
					if(0 == strcmp(session->name_buf, DEF_VTY_USER_NAME))
					{
						gVtyLocalUserLogin = 0;						
					}
				}
                session->session_status = SESSION_CLOSE;
            }

#if 0
            else if(vty->type == VTY_SSH)
            {
//          struct login_session *session = (struct login_session *)vty->session;
                Channel *c = (Channel *)vty->channle;
                channel_close(session->ssh_packet, c);
                c->channel_exit_flag = 1;

                pthread_exit(NULL);
            }

#endif
    		(void)vtysh_line_config_update(session);
		
            break;

        case VSI_NODE:
        case L3VPN_NODE:
        case PW_NODE:
        case MPLSTP_OAM_SESS_NODE:
        case APS_NODE:
        case MPLSTP_OAM_MEG_NODE:
            vty->node = MPLS_NODE;
            break;

        case INTERFACE_NODE:
        case MPLS_NODE:
        case ROUTE_NODE:
        case ACL_NODE:
        case BGP_NODE:
        case RIP_NODE:
        case RIPNG_NODE:
        case OSPF_NODE:
        case OSPF_DCN_NODE:
        case ISIS_NODE:
//        case VTY_NODE:
        case PHYSICAL_IF_NODE:
        case PHYSICAL_SUBIF_NODE:
        case TDM_IF_NODE:
        case TDM_SUBIF_NODE:
        case STM_IF_NODE:
		case SDH_DXC_NODE:	
        case STM_SUBIF_NODE:
        case TUNNEL_IF_NODE:
        case LOOPBACK_IF_NODE:
        case TRUNK_IF_NODE:
        case TRUNK_SUBIF_NODE:
        case VLANIF_NODE:
        case CLOCK_NODE:
		case VCG_NODE:
		case E1_IF_NODE:
        case ARP_NODE:
        case NDP_NODE:
        case LLDP_NODE:
        case MSTP_NODE:
        case POOL_NODE:
        case SYSLOG_NODE:
        case NTP_NODE:
        case VLAN_NODE:
        case DEVM_NODE:
        case QOS_DOMAIN_NODE:
        case QOS_PHB_NODE:
        case QOS_POLICY_NODE:
        case QOS_CAR_NODE:
        case QOS_MIRROR_NODE:
        case QOS_CPCAR_NODE:
        case AAA_NODE:
        case AAA_LOCAL_USER_NODE:
        case ALARM_NODE:
        case STATIS_NODE:
        case RMON_NODE:
        case BFD_SESSION_NODE:
		case BFD_TEMPLATE_NODE:
        case CFM_MD_NODE:
        case CFM_SESSION_NODE:
        case SYNCE_NODE:
        case SNMPD_NODE:
        case SLA_SESSION_NODE:
        case OPENFLOW_NODE:
        case HQOS_WRED_NODE:
        case HQOS_QUEUE_NODE:
        case WEB_NODE:
        case MAC_NODE:
        case ELPS_SESSION_NODE:
        case ERPS_SESSION_NODE:
        case PIM_INSTANCE_NODE:
        case TRUNK_GLOABLE_NODE:
        case IGMP_NODE:
        case OSPF6_NODE:
        case TCP_NODE:
        case DHCPV6_NODE:
        case SYSRUN_NODE:
        case SYSTEM_NODE:
	    case PW_CLASS_NODE:
	    case XCONNECT_GROUP_NODE:
		case ACL_BASIC_NODE:
    	case ACL_ADVANCED_NODE:
		case ACL_MAC_NODE:
		case QOS_CLASSIFIER_NODE:
		case QOS_BEHAVIOR_NODE:
		case QOS_POLICY1_NODE:
		case USER_LINE_NODE:
            //vtysh_execute("end",NULL);
            //vtysh_execute("configure terminal",NULL);
            vty->node = CONFIG_NODE;
            break;

		case ISIS_FAMILY_V4_NODE:
		case ISIS_FAMILY_V6_NODE:
            vty->node = ISIS_NODE;
            break;

//      vty->node = BGP_NODE;
//      break;
//    case KEYCHAIN_KEY_NODE:
//      vty->node = KEYCHAIN_NODE;
//    break;
        case AREA_NODE:
            vty->node = OSPF_NODE;
            break;

        case CFM_MA_NODE:
            vty->node = CFM_MD_NODE;
            break;

        case NEIGHBOR_NODE:
        case NEIGHBOR_NODE_IPV6:
        case BGP_VPN_INSTANCE_NODE:
        case BGP_VPNV4_NODE:
        case BGP_IPV6_NODE:
            vty->node = BGP_NODE;
            break;
    case CONNECTION_NODE:
        vty->node = XCONNECT_GROUP_NODE;
        break;
    case PW_MASTER_NODE:
        vty->node = CONNECTION_NODE;
        break;
    case PW_BACKUP_NODE:
        vty->node = PW_MASTER_NODE;
        break;
    case SERVICE_INSTANCE_NODE:
        vty->node = PHYSICAL_IF_NODE;
        break;
        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUNSH(VTYSH_ALL,
        vtysh_exit_all,
        vtysh_exit_all_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFUN(vtysh_exit_enable,
      vtysh_exit_enable_cmd,
      "exit",
      "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFUNSH(VTYSH_ALL,
        vtysh_quit_node,
        vtysh_quit_node_cmd,
        "quit",
        "quit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


/* Write startup configuration into the terminal. */
DEFUN(show_startup_config,
      show_startup_config_cmd,
      "show startup-config",
      SHOW_STR
      "Contentes of startup configuration\n")
{
    char buf[BUFSIZ];
    FILE *confp;

    confp = fopen(host.config, "r");

    if(confp == NULL)
    {
        vty_out(vty, "Can't open configuration file [%s]%s",
                host.config, VTY_NEWLINE);
        return CMD_WARNING;
    }

    while(fgets(buf, BUFSIZ, confp))
    {
        char *cp = buf;

        while(*cp != '\r' && *cp != '\n' && *cp != '\0')
        {
            cp++;
        }

        *cp = '\0';

        vty_out(vty, "%s%s", buf, VTY_NEWLINE);
    }

    fclose(confp);

    return CMD_SUCCESS;
}

DEFUN(vtysh_terminal_length,
      vtysh_terminal_length_cmd,
      "terminal length <20-512>",
      "Set terminal line parameters\n"
      "Set number of lines on a screen\n"
      "Number of lines on screen\n")
{
    int lines;
    char *endptr = NULL;
//  char default_pager[10];

    lines = strtol(argv[0], &endptr, 10);

    if(lines < 20 || lines > 512 || *endptr != '\0')
    {
        vty_out(vty, "length is malformed%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

#if 0

    if(vty->type == VTY_SHELL)
    {
        if(vtysh_pager_name)
        {
            free(vtysh_pager_name);
            vtysh_pager_name = NULL;
        }

        if(lines != 0)
        {
            snprintf(default_pager, 10, "more -%i", lines);
            vtysh_pager_name = strdup(default_pager);
        }
    }

#endif

    if(vty->type == VTY_TERM)
    {

        vty->lines = lines;
    }

    return CMD_SUCCESS;
}

DEFUN(vtysh_terminal_no_length,
      vtysh_terminal_no_length_cmd,
      "no terminal length",
      "Set terminal line parameters\n"
      NO_STR
      "set terminal line to default\n")
{
#if 0

    if(vty->type == VTY_SHELL)
    {
        if(vtysh_pager_name)
        {
            free(vtysh_pager_name);
            vtysh_pager_name = NULL;
        }

        vtysh_pager_init();
    }

#endif

    if(vty->type == VTY_TERM)
    {
        vty->lines = -1;
    }

    return CMD_SUCCESS;
}

/* Execute command in child process. */
static int
execute_command(const char *command, int argc, const char *arg1,
                const char *arg2)
{
    pid_t pid;
    int status;

    /* Call fork(). */
    pid = fork();

    if(pid < 0)
    {
        /* Failure of fork(). */
        fprintf(stderr, "Can't fork: %s\n", safe_strerror(errno));
        exit(1);
    }
    else if(pid == 0)
    {
        /* This is child process. */
        switch(argc)
        {
            case 0:
                execlp(command, command, (const char *)NULL);
                break;

            case 1:
                execlp(command, command, arg1, (const char *)NULL);
                break;

            case 2:
                execlp(command, command, arg1, arg2, (const char *)NULL);
                break;
        }

        /* When execlp suceed, this part is not executed. */
        fprintf(stderr, "Can't execute %s: %s\n", command, safe_strerror(errno));
        exit(1);
    }
    else
    {
        /* This is parent. */
        execute_flag = 1;
        wait4(pid, &status, 0, NULL);
        execute_flag = 0;
    }

    return 0;
}

DEFUN(vtysh_telnet,
      vtysh_telnet_cmd,
      "telnet WORD",
      "Open a telnet connection\n"
      "IP address or hostname of a remote system\n")
{
    if(vty->type != VTY_SHELL)
    {
        vty_out(vty, "%% Debug command, only support serial port!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    execute_command("telnet", 1, argv[0], NULL);
    return CMD_SUCCESS;
}

DEFUN(vtysh_telnet_port,
      vtysh_telnet_port_cmd,
      "telnet WORD PORT",
      "Open a telnet connection\n"
      "IP address or hostname of a remote system\n"
      "TCP Port number\n")
{
    if(vty->type != VTY_SHELL)
    {
        vty_out(vty, "%% Debug command, only support serial port!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    execute_command("telnet", 2, argv[0], argv[1]);
    return CMD_SUCCESS;
}

DEFUN(vtysh_start_shell,
      vtysh_start_shell_cmd,
      "start-shell",
      "Start UNIX shell\n")
{
    if(vty->type != VTY_SHELL)
    {
        vty_out(vty, "%% Debug command, only support serial port!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

//    VTYSH_CMD_EXEC_UNLOCK;
    execute_command("sh", 0, NULL, NULL);
    return CMD_SUCCESS;
}


DEFUN(vtysh_write_terminal,
      vtysh_write_terminal_cmd,
      "write terminal",
      "Write running configuration to memory, network, or terminal\n"
      "Write to terminal\n")
{
    u_int i;
    char line[] = "write terminal\n";
    FILE *fp = NULL;

    if(vty_shell(vty))
    {
        system("stty cols 128");
        if(vtysh_pager_name)
        {
            fp = popen(vtysh_pager_name, "w");

            if(fp == NULL)
            {
                perror("popen");
                exit(1);
            }
        }
        else
        {
            fp = stdout;
        }
    }

    vty_out(vty, "Building configuration...%s", VTY_NEWLINE);
    vty_out(vty, "%sCurrent configuration:%s", VTY_NEWLINE,
            VTY_NEWLINE);
    vty_out(vty, "!%s", VTY_NEWLINE);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    /* Integrate vtysh specific configuration. */
    vtysh_config_write();
    vtysh_user_line_write();

    if(vty_shell(vty))
    {
        vtysh_config_dump(fp);
    }
    else
    {
        vtysh_vty_config_dump(vty);
    }

    pthread_mutex_unlock(&vtysh_config_lock);

    if(vtysh_pager_name && fp && vty_shell(vty))
    {
        fflush(fp);

        if(pclose(fp) == -1)
        {
            perror("pclose");
            exit(1);
        }

        fp = NULL;
    }

    if(vty_shell(vty))
    {
        system("stty cols 132");
    }
    return CMD_SUCCESS;
}

DEFUN(vtysh_write_terminal_daemon,
      vtysh_write_terminal_daemon_cmd,
      "write terminal (zebra|ripd|ospfd|ospf6d|bgpd|isisd|babeld)",
      "Write running configuration to memory, network, or terminal\n"
      "Write to terminal\n"
      "For the zebra daemon\n"
      "For the rip daemon\n"
      "For the ospf daemon\n"
      "For the ospfv6 daemon\n"
      "For the bgp daemon\n"
      "For the isis daemon\n"
      "For the babel daemon\n")
{
    unsigned int i;
    int ret = CMD_SUCCESS;

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(strcmp(vtysh_client[i].name, argv[0]) == 0)
        {
            break;
        }
    }

    vty->cmd_process_lock = 1;
    ret = vtysh_client_execute(&vtysh_client[i], "show running-config\n", stdout, vty, i);
    vty->cmd_process_lock = 0;

    return ret;
}


#if 0
/* 文件合并，得到合并文件file_new，	 合并顺序file1在前，file2在后					 */
static int vtysh_combine_files(char *file1, char *file2, char *file_new)
{
	FILE *fp_new = NULL;
	FILE *fp = NULL;
	int ch = 0;

	if((NULL == file_new) || (NULL == file1) || (NULL == file2))
	{
		return 1;
	}

	if(0 == (access(file1, F_OK)))
	{
		if(0 == (access(file_new, F_OK)))
		{
			unlink(file_new); 		  
		}

		vtysh_copy_file(file1, file_new);
	}
	else
	{
		if(0 == (access(file2, F_OK)))
		{
			if(0 == (access(file_new, F_OK)))
			{
				unlink(file_new); 		  
			}

			vtysh_copy_file(file2, file_new);
		}

		return 0;
	}

	if((NULL == (fp_new = fopen(file_new, "a"))) || (NULL == (fp = fopen(file2, "r"))))
	{
		return 1;
	}

	while(EOF != (ch = getc(fp)))
	{
		putc(ch, fp_new);
	}

	fclose(fp_new);
	fclose(fp);
	return 0;
}
#endif


/* 文件拷贝 */
void vtysh_copy_file(char *file_src, char *file_dst)
{
	if((NULL == file_src) || (NULL == file_dst) || (0 != (access(file_src, F_OK))))
	{
		return;
	}

	FILE *fp1 = NULL;
	FILE *fp2 = NULL;
	int ch = 0;

	if((NULL == (fp1 = fopen(file_src, "r"))) || (NULL == (fp2 = fopen(file_dst, "w"))))
	{
		return;
	}

	while(EOF != (ch = getc(fp1)))
	{
		putc(ch, fp2);
	}

	fclose(fp1);
	fclose(fp2);
}

static int vtysh_check_file_type(char *filename, char *typename)
{
	if((NULL == filename) || (NULL == typename))
	{
		return 0;
	}

	if(strlen(filename) <= strlen(typename))
	{
		return 0;
	}

	char *pType = filename + (strlen(filename) - strlen(typename));
	if(0 == strcmp(pType, typename))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



static int vtysh_set_reset_from(struct vty *vty, char *filename)
{
	char *file_path = NULL;

	if(0 == strcmp(filename, VTYSH_RESET_FROM_CURRENT))
	{
		/* 将 .current.conf 拷贝为 startup.conf 文件 */
		vtysh_copy_file((char *)VTYSH_CURRENT_CONFIG, (char *)VTYSH_STARTUP_CONFIG);
	}
	else if(0 == strcmp(filename, VTYSH_RESET_FROM_FACTORY))
	{
		/* 将 .startup_addr.conf 拷贝为 startup.conf 文件 */
		vtysh_copy_file((char *)VTYSH_STARTUP_ADDR_CONFIG, (char *)VTYSH_STARTUP_CONFIG);
	}
	else
	{		
		/* 检查文件后缀是否为          .conf */
		if(0 == vtysh_check_file_type(filename, (char *)VTYSH_CONFIG_FILE_TYPE))
		{
			vty_error_out(vty, "File type must be \"%s\" !%s", VTY_NEWLINE, VTYSH_CONFIG_FILE_TYPE);
			return CMD_WARNING;
		}
	   
		file_path = malloc(strlen(filename) + strlen(VTYSH_CFG_FILE_PATH) + 1);
		if(NULL == file_path)
		{
			vty_out(vty, "%s malloc error!%s", filename, VTY_NEWLINE);
			return CMD_WARNING;
		}

		memset(file_path, 0, (strlen(filename) + strlen(VTYSH_CFG_FILE_PATH) + 1));
	    strcpy(file_path, VTYSH_CFG_FILE_PATH);
	    strcat(file_path, filename);
		
		if((file_path != NULL) && (access(file_path, F_OK) < 0))
		{
			vty_error_out(vty, "%s not exist!%s", filename, VTY_NEWLINE);
			free(file_path);
			return CMD_WARNING;
		}

		/* 将 FILE 拷贝为 startup.conf 文件 */
		vtysh_copy_file(file_path, (char *)VTYSH_STARTUP_CONFIG);
	}

	system("sync");
	return CMD_SUCCESS;
}

static void write_config_to_file(struct vty *vty, char *filename)
{
	uint32_t i = 0;
	char line[30] = {'\0'};
    FILE *fp;
    int fd;
    char *filename_sav = NULL;
	
	if(NULL == filename)
	{
		return;
	}
	else if(0 == strcmp(VTYSH_STARTUP_ADDR_CONFIG, filename))	//write to .startup_addr.conf
	{
		
		strcpy(line, "write factory terminal\n");
	}
	else														//write to xxx.conf
	{
		strcpy(line, "write terminal\n");
	}


 	filename_sav = malloc(strlen(filename) + strlen(CONF_BACKUP_EXT) + 1);
    strcpy(filename_sav, filename);
    strcat(filename_sav, CONF_BACKUP_EXT);

    /* Move current configuration file to backup config file. */
    unlink(filename_sav);
    rename(filename, filename_sav);
    free(filename_sav);

	/* 将 .startup_addr.conf 拷贝为 filename */
	//vtysh_copy_file((char *)VTYSH_STARTUP_ADDR_CONFIG, filename);

	/* 再追加业务配置 */
	fp = fopen(filename, "w");

	if(fp == NULL)
    {
        fprintf(stdout, "%% Can't open configuration file %s.\n", filename);
        return;
    }

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_config_write();

	if(0 != strcmp(integrate_addr_default, filename))
	{
		vtysh_user_line_write();
	}

    vtysh_config_dump(fp);
    pthread_mutex_unlock(&vtysh_config_lock);

    /*sync() returns without wait flush finish*/
//  sync();
    fd = fileno(fp);

    if(fd == -1)
    {
        zlog_err("fail to exchange FILE * to fd");
        sync();
    }
    else
    {
        fsync(fd);
    }

    fclose(fp);
    //sleep(1);	
}



static int write_config_integrated(struct vty *vty, const char *fname)
{
	char *fname_new = NULL;

	/* 检查输入为FILE时的文件后缀 */
	if((fname) && 
	   (0 != strcmp("current", fname)) &&
	   (0 != strcmp("factory", fname)) &&
	   (0 != strcmp("clean",   fname)))
	{
		/* 检查文件后缀是否为          .conf */
		if(0 == vtysh_check_file_type((char *)fname, (char *)VTYSH_CONFIG_FILE_TYPE))
		{
			vty_error_out(vty, "File type must be \".conf\" !%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	vty_out(vty, "Building Configuration...\r\n");

	write_config_to_file(vty, (char *)VTYSH_STARTUP_ADDR_CONFIG);

	if(NULL == fname)
    {
		write_config_to_file(vty, (char *)VTYSH_CURRENT_CONFIG);
		vtysh_set_reset_from(vty, (char *)VTYSH_RESET_FROM_CURRENT);
    }
    else if(0 == strcmp("current", fname))
    {
		write_config_to_file(vty, (char *)VTYSH_CURRENT_CONFIG);
    }
    else if(0 == strcmp("factory", fname))
    {
		write_config_to_file(vty, (char *)VTYSH_STARTUP_ADDR_CONFIG);;
	}
	else if(0 == strcmp("clean", fname))
    {
		/* 将config.init文件拷贝为startup.conf */
		vtysh_copy_file((char *)VTYSH_INIT_CONFIG, (char *)VTYSH_STARTUP_CONFIG);
	}
	else
    {		
		fname_new = malloc(strlen(VTYSH_CFG_FILE_PATH) + strlen(fname) + 1);
	    strcpy(fname_new, VTYSH_CFG_FILE_PATH);
	    strcat(fname_new, fname);
		
        write_config_to_file(vty, fname_new);

		free(fname_new);
    }

	vty_out(vty, "Saving Configuration...\r\n");   
    

#if 0
	/* add by lipf, for init factory */
	if(chmod(integrate_addr_default, CONFIGFILE_MASK) != 0)
    {
        vty_out(vty, "%% Can't chmod configuration file %s: %s (%d)\r\n",
                integrate_addr_default, safe_strerror(errno), errno);
        return CMD_WARNING;
    }
	
    if(fname == NULL)
    {		
        if(chmod(integrate_business_default, CONFIGFILE_MASK) != 0)
        {
            vty_out(vty, "%% Can't chmod configuration file %s: %s (%d)\r\n",
                    integrate_business_default, safe_strerror(errno), errno);
            return CMD_WARNING;
        }
    }
    else
    {
        if(chmod(fname, CONFIGFILE_MASK) != 0)
        {
            vty_out(vty, "%% Can't chmod configuration file %s: %s (%d)\r\n",
                    fname, safe_strerror(errno), errno);   
            return CMD_WARNING;
        } 
    }
#endif

//  vty_out(vty,"Integrated configuration saved to %s\r\n",integrate_default);

//  host_config_set (integrate_default);

    vty_out(vty, "[OK]\r\n");

	system("sync");

    return CMD_SUCCESS;
}

DEFUN(vtysh_write_memory,
      vtysh_write_memory_cmd,
      "write memory",
      "Write running configuration to memory, network, or terminal\n"
      "Write configuration to the file (same as write file)\n")
{
    return write_config_integrated(vty, NULL);
}


/* lipf add 2018/11/5 */
DEFUN(vtysh_write_modules,
	vtysh_write_modules_cmd,
	"write module {aaa|acl|bfd|bgp|cfm|devm|e1|interface|isis|lldp|loopback|mpls|mstp|ntp|ospf|physical|qos|rip|rmon|route|snmp|tdm|trunk|tunnel|vlan}",
	"Write running configuration to memory, network, or terminal\n"
	"Module\n"
	"For the aaa daemon\n"
	"For the acl daemon\n"
	"For the bfd daemon\n"
	"For the bgp daemon\n"
	"For the cfm daemon\n"
	"For the devm daemon\n"
	"For the e1 daemon\n"
	"For the interface daemon\n"
	"For the isis daemon\n"
	"For the lldp daemon\n"
	"For the loopback daemon\n"
	"For the mpls daemon\n"
	"For the mstp daemon\n"
	"For the ntp daemon\n"
	"For the ospf daemon\n"
	"For the physical daemon\n"
	"For the qos daemon\n"
	"For the rip daemon\n"
	"For the rmon daemon\n"
	"For the route daemon\n"
	"For the snmp daemon\n"
	"For the tdm daemon\n"
	"For the trunk daemon\n"
	"For the tunnel daemon\n"
	"For the vlan daemon\n")
{
	uint32_t i = 0;
	uint32_t models = 0;
	
	do
	{
		if(argv[i])
		{
			models |= (0x00000001 << i);
		}
	}while(++i < 25);

	if(models)
	{
		struct gpnPortInfo gPortInfo;
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_SOFT_TYPE;

		ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_EMB_POINT_MARK, models);
				
	  	return write_config_integrated(vty, NULL);
	}
	else
	{
		vty_out(vty, "No module has been selected!%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
}

/* lipf add 2018/11/5 */
DEFUN(vtysh_write_module,
	vtysh_write_module_cmd,
	"write-module {e1_base_config|elan|eline|etree|gretunnel|if_vlan_config|interface_info|interface_ip_addr|lagroup|mpls_base_info|mspw|tunnel|tunnel_aps|tunnel_oam|vlan_table}",
	"Write running configuration to memory, network, or terminal\n"
	"For the e1_base_config daemon\n"
	"For the elan daemon\n"
	"For the eline daemon\n"
	"For the etree daemon\n"
	"For the gretunnel daemon\n"
	"For the if_vlan_config daemon\n"
	"For the interface_info daemon\n"
	"For the interface_ip_addr daemon\n"
	"For the lagroup daemon\n"
	"For the mpls_base_info daemon\n"
	"For the mspw daemon\n"
	"For the tunnel daemon\n"
	"For the tunnel_aps daemon\n"
	"For the tunnel_oam daemon\n"
	"For the vlan_table daemon\n")
{
	uint32_t i = 0;
	uint32_t models = 0;
	
	do
	{
		if(argv[i])
		{
			models |= (0x00000001 << i);
		}
	}while(++i < 15);

	if(models)
	{
		struct gpnPortInfo gPortInfo;
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_SOFT_TYPE;

		ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_EMB_POINT_MARK, models);
				
	  	return write_config_integrated(vty, NULL);
	}
	else
	{
		vty_out(vty, "No module has been selected!%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
}


DEFUN(vtysh_reboot_file,
	  vtysh_reboot_file_cmd,
	  "reboot-file FILE",
	  "Reboot form assigned file\n"
	  "File name\n")
{
	char fcfg[VTYSH_H3C_CONFIG_FILE_LEN];
	
	if(strlen(argv[0]) > (VTYSH_H3C_CONFIG_FILE_LEN + strlen(VTYSH_H3C_CONFIG_FILE_TYPE)))
    {
        vty_out(vty, "File name:1-32 characters%s", VTY_NEWLINE);
        return CMD_WARNING;            
    }
    sscanf(argv[0], "%*[^.]%s", fcfg);
    if(strcmp(fcfg, VTYSH_H3C_CONFIG_FILE_TYPE) != 0)
    {
        vty_out(vty, "Invalid file name.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return write_config_integrated(vty, argv[0]);
}


DEFUN(vtysh_test_alarm,
	  vtysh_test_alarm_cmd,
	  "alarm-test",
	  "Alarm test\n")
{
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
	gPortInfo.iMsgPara1 = 100;

	static int i = 0;

	if(0 == i)
	{
		ipran_alarm_port_register(&gPortInfo);
		i = 1;
	}
	else if(1 == i)
	{
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PW_LOC, GPN_SOCK_MSG_OPT_RISE);
		i = 2;
	}
	else if(2 == i)
	{
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PW_LOC, GPN_SOCK_MSG_OPT_CLEAN);
		i = 3;
	}
	else if(3 == i)
	{
		ipran_alarm_port_unregister(&gPortInfo);
		i = 0;
	}
	return CMD_SUCCESS;
}



DEFUN(vtysh_save_memory,
      vtysh_save_file_cmd,
      "save [FILE]",
      "save running configuration to memory, network, or terminal\n"
      "save configuration to the file (1-32 characters)\n")
{
    char fcfg[VTYSH_H3C_CONFIG_FILE_LEN];
    
    if(argv[0] != NULL)
    { 
        if(strlen(argv[0]) > (VTYSH_H3C_CONFIG_FILE_LEN + strlen(VTYSH_H3C_CONFIG_FILE_TYPE)))
        {
            vty_out(vty, "File name:1-32 characters%s", VTY_NEWLINE);
            return CMD_WARNING;            
        }
        sscanf(argv[0], "%*[^.]%s", fcfg);
        if(strcmp(fcfg, VTYSH_H3C_CONFIG_FILE_TYPE) != 0)
        {
            vty_out(vty, "Invalid file name.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return write_config_integrated(vty, argv[0]);
}

ALIAS(vtysh_write_memory,
      vtysh_write_file_cmd,
      "write file",
      "Write running configuration to memory, network, or terminal\n"
      "Write configuration to the file (same as write memory)\n")

ALIAS(vtysh_write_memory,
      vtysh_write_cmd,
      "write",
      "Write running configuration to memory, network, or terminal\n")

/* add by lipf, 2018/12/18, for recoving factory config, replace "write" command */
DEFUN(vtysh_write_config,
	  vtysh_write_config_cmd,
	  "write",
	  "Write running configuration to memory, network, or terminal\n")
{
	write_config_integrated(vty, NULL);
	
	return CMD_SUCCESS;
}

DEFUN(vtysh_write_config2,
	vtysh_write_config2_cmd,
	"write (current|factory|FILE|clean)",
	"Write running configuration to memory, network, or terminal\n"
	"Write configuration to current\n"
	"Write configuration to factory\n"
	"Write configuration to the file\n"
	"Clean all configuration\n")
{
	write_config_integrated(vty, argv[0]);
	
	return CMD_SUCCESS;
}


	  

DEFUN(vtysh_reset_file,
	vtysh_reset_file_cmd,
	"reset-from (current|factory|FILE)",
	"Reset from factory or FILE\n"
	"Reset from current\n"
	"Reset from factory\n"
	"Reset from FILE\n")
{	
	return vtysh_set_reset_from(vty, (char *)argv[0]);	
}


	  

	  

ALIAS(vtysh_write_terminal,
      vtysh_show_running_config_cmd,
      "show running-config",
      SHOW_STR
      "Current operating configuration\n")

ALIAS(vtysh_write_terminal_daemon,
      vtysh_show_running_config_daemon_cmd,
      "show running-config (zebra|ripd|ospfd|ospf6d|bgpd|isisd|babeld)",
      SHOW_STR
      "Current operating configuration\n"
      "For the zebra daemon\n"
      "For the rip daemon\n"
      "For the ospf daemon\n"
      "For the ospfv6 daemon\n"
      "For the bgp daemon\n"
      "For the isis daemon\n"
      "For the babel daemon\n")

DEFUN(vtysh_show_daemons,
      vtysh_show_daemons_cmd,
      "show daemons",
      SHOW_STR
      "Show list of running daemons\n")
{
    u_int i;
    char line[] = "show daemon\n";

    vty->cmd_process_lock = 1;

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        vtysh_client_execute(&vtysh_client[i], line, stdout, vty, i);
    }

    vty->cmd_process_lock = 0;

    for(i = 0; i < array_size(vtysh_client); i++)
        if(vtysh_client[i].fd >= 0)
        {
            vty_out(vty, " %s", vtysh_client[i].name);
        }

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

void
insert_passwd(struct host_users *users, struct vty *vty, int argc, const char *argv[])
{
    if(users->password)
    {
        XFREE(MTYPE_HOST, users->password);
    }

    users->password = NULL;

    if(strcmp(argv[2], "enable") == 0)
    {
        users->encrypt = 1;

        if(users->password_encrypt)
        {
            XFREE(MTYPE_HOST, users->password);
        }

        if(vty->type == VTY_FILE)
        {
            users->password_encrypt = XSTRDUP(MTYPE_HOST, argv[1]);
        }
        else
        {
            users->password_encrypt = XSTRDUP(MTYPE_HOST, zencrypt(argv[1]));
        }
    }
    else if(strcmp(argv[2], "disable") == 0)
    {
        users->encrypt = 0;
        users->password = XSTRDUP(MTYPE_HOST, argv[1]);
    }
}

#if 0
void
check_privilege(int privilege)
{
    struct host_users *self_user;
    struct login_session *session;

    session = (struct login_session *)vty->session;
    self_user = session->user_login;

    /*check user permition*/
    if(self_user->privilege < privilege)
    {
        return FALSE;
    }

    return TRUE;
}
#endif

#if 0
DEFUN(vtysh_login_users,
      vtysh_login_users_cmd,
      "login user NAME password WORD encryption (enable|disable) {privilege <0-15>}",
      "creat new user to login\n"
      "User\n"
      "Input user name\n"
      "Password\n"
      "Input password\n"
      "Encryption\n"
      "Enable encryption\n"
      "Disable encryption\n"
      "Change user privilege\n"
      "Privilege from 0 to 15\n")
{
    struct host_users *users;
    struct host_users *users_loop;
    struct listnode *node, *nnode;
    char input;
    struct login_session *session;
    int privilege = -1;

    /*check parameters*/
    if(strlen(argv[0]) > USER_NAME_MAX - 1)
    {
        vty_out(vty, " %%Username is too long. range <1-31>%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(strlen(argv[1]) > USER_NAME_MAX - 1)
    {
        vty_out(vty, " %%Password is too long. range <1-31>%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(!isalpha((int) *argv[0]))
    {
        vty_out(vty, " %%Please specify username starting with alphabet%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(!isalnum((int) *argv[0]))
    {
        vty_out(vty,
                " %%Please specify username starting with alphanumeric%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL != argv[3])
    {
        privilege = atoi(argv[3]);
    }

    /*lookup whether we have the user*/
    users = NULL;

    if(! host.registe_user)
    {
        host.registe_user = list_new();
        host.registe_user->del = (void (*)(void *))vtysh_user_delete;
        host.registe_user->cmp =    NULL;
    }

    for(ALL_LIST_ELEMENTS(host.registe_user, node, nnode, users_loop))
    {
        if(strcmp(users_loop->name, argv[0]) == 0)
        {
            users = users_loop;
        }
    }

    /*reach max users*/
    if(host.registe_user->count > MAX_USERS_COUNT - 1 && !users)
    {
        vty_out(vty, " %%Reached the maximum number of users%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*user exist, change passwd*/
    if(users)  //user already exist
    {
#if 0

        if(!strcmp(users->name, HOST_NAME))
        {
            vty_out(vty, "Default user admin not allowed to modify%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

#endif

        if(users->login_cnt > 0)
        {
            vty_out(vty, " %%User %s is online%s", users->name, VTY_NEWLINE);
            return CMD_WARNING;
        }

        vty_out(vty, " %%User %s already exist, modify? Y/N:%s", users->name, VTY_NEWLINE);

        if(vty->type == VTY_SHELL)
        {
            struct termios term_new, term_init;
            FILE *cin;
            int err;
            int cin_fileno;

            cin = fopen("/dev/tty", "r");
            cin_fileno = fileno(cin);

            if(tcgetattr(cin_fileno , &term_init) == -1)
            {
                perror("Cannot get standard input description");
                exit(1);
            }

            term_new = term_init;

            term_new.c_lflag &= ~(ICANON | ECHO);
            term_new.c_cc[VMIN] = 1;
            term_new.c_cc[VTIME] = 0;

            err = tcsetattr(cin_fileno, TCSAFLUSH, &term_new);

            if(err == -1 || err == EINTR)
            {
                perror("Failed to change EOF character");
                exit(1);
            }

            while(1)
            {
                input = getc(cin);

                if(input != 'n' && input != 'N' && input != 'y' && input != 'Y')
                {
                    vty_out(vty, " %%Please input y/n:%s", VTY_NEWLINE);
                }
                else
                {
                    break;
                }
            }

            err = tcsetattr(cin_fileno, TCSAFLUSH, &term_init);

            if(err == -1 || err == EINTR)
            {
                perror("Failed to change EOF character");
                exit(1);
            }
        }
        else if(vty->type == VTY_TERM  || vty->type == VTY_SSH)
        {
            vty->telnet_recv_tmp = 1;
            session = (struct login_session *)vty->session;

            while(1)
            {
                usleep(100);

                if(session->tid_w_exit)
                {
                    printf("read thread has exit, write break here\n");
                    session->tid_r_exit = 1;
                    break;
                }

                if(strlen(vty->buf_tmp) != 0)
                {
                    if(*(vty->buf_tmp) != 'n' && *(vty->buf_tmp) != 'N' && *(vty->buf_tmp) != 'y' \
                            && *(vty->buf_tmp) != 'Y')
                    {
                        vty_out(vty, " %%Please input y/n:%s", VTY_NEWLINE);
                        memset(vty->buf_tmp, 0, strlen(vty->buf_tmp));
                        continue;
                    }

                    input = *(vty->buf_tmp);
                    vty->telnet_recv_tmp = 0;
                    memset(vty->buf_tmp, 0, strlen(vty->buf_tmp));
                    break;
                }
            }
        }

        if(input)
        {
            switch(input)
            {
                case 'n':
                case 'N':
                    vty_out(vty, " %%user password unchanged%s", VTY_NEWLINE);
                    return CMD_SUCCESS;

                case 'y':
                case 'Y':
                    insert_passwd(users, vty, argc, argv);

                    if(privilege != -1)
                    {
                        if(privilege != users->privilege)
                        {
                            vty_out(vty, " %%User %s privilege from %d to %d%s",
                                    users->name, users->privilege, privilege, VTY_NEWLINE);
                        }

                        users->privilege = privilege;
                    }

                    vty_out(vty, " %%User changed%s", VTY_NEWLINE);
                    return CMD_SUCCESS;

                default:
                    break;
            }
        }
    }

    if(!users)
    {
        users = vtysh_user_creat();
        users->name = XSTRDUP(MTYPE_HOST, argv[0]);
        users->privilege = (privilege != -1 ? privilege : 0);
        listnode_add(host.registe_user, users);
    }

    insert_passwd(users, vty, argc, argv);

    return CMD_SUCCESS;
}

DEFUN(vtysh_no_login_users,
      vtysh_no_login_users_cmd,
      "no login user NAME",
      NO_STR
      "login\n"
      "user\n"
      "user name to be delete\n")
{
    struct host_users *users;
    struct host_users *users_loop;
    struct listnode *node, *nnode;
    struct login_session *session;
#if 0

    if(vty->node != ENABLE_NODE)
    {
        vty_out(vty, "%% permission denied!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

#endif

    session = (struct login_session *)vty->session;
    users = NULL;

    if(!isalpha((int) *argv[0]))
    {
        vty_out(vty, " Username starting with alphabet%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for(ALL_LIST_ELEMENTS(host.registe_user, node, nnode, users_loop))
    {
        if(strcmp(users_loop->name, argv[0]) == 0)
        {
            users = users_loop;
        }
    }

    if(!users)
    {
        vty_out(vty, " user name: %s not exist.%s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(!strcmp(users->name, HOST_NAME))
    {
        vty_out(vty, " default user: admin is not allowed to delete.%s", VTY_NEWLINE);
    }
    else
    {
#if 0

        for(int i = 0; i < vector_active(vtyvec); i++)
            if((v = vector_slot(vtyvec, i)) != NULL &&    \
                    v->user_name != NULL && \
                    !strcmp(users->name, v->user_name))
            {
                vty_out(vty, " user %s is online.%s", users->name, VTY_NEWLINE);
                return CMD_WARNING;
            }

        listnode_delete(host.registe_user, users);
#endif
        pthread_mutex_lock(&users->login_cnt_lock);

        if(users->login_cnt > 0)
        {
            pthread_mutex_unlock(&users->login_cnt_lock);
            vty_out(vty, " %Warning: user %s is loging on at %d terminal.%s", users->name, users->login_cnt, VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {
            pthread_mutex_unlock(&users->login_cnt_lock);
            listnode_delete(host.registe_user, users);
            vtysh_user_delete(users);
        }

    }

    return CMD_SUCCESS;
}


DEFUN(vtysh_show_login_user,
      vtysh_show_login_user_cmd,
      "show login user [online]",
      "show registered users or online users\n"
      "login\n"
      "user\n"
      "show online user")
{
    struct listnode *node, *nnode;
    struct host_users *users_loop;
    struct login_session *session_loop;

    if(argv[0] == NULL)
    {
        vty_out(vty, "--------------------------------------------%s", VTY_NEWLINE);
        vty_out(vty, "Registered users:%s", VTY_NEWLINE);
        vty_out(vty, " %-20s", "User");
        vty_out(vty, "Privilege%s", VTY_NEWLINE);

        for(ALL_LIST_ELEMENTS(host.registe_user, node, nnode, users_loop))
        {
            vty_out(vty, " %-20s  %d%s",
                    users_loop->name, users_loop->privilege, VTY_NEWLINE);
        }

        vty_out(vty, "--------------------------------------------%s", VTY_NEWLINE);
    }

    else if(argv[0] != NULL)
    {
        vty_out(vty, "--------------------------------------------%s", VTY_NEWLINE);
        vty_out(vty, "Login users: %s", VTY_NEWLINE);
        vty_out(vty, " %-15s", "User");
        vty_out(vty, "%-20s", "Connected_from");
        vty_out(vty, "%-15s", "privilege");
        vty_out(vty, "Auth_mode%s", VTY_NEWLINE);

        for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        {
            char auth_mode[10];

            if(session_loop->user_name != NULL)
            {
                vty_out(vty, " %-15s %-20s %-15d",
                        session_loop->user_name, session_loop->address, session_loop->user_login->privilege);
                memset(auth_mode, 0, sizeof(auth_mode));

                switch(session_loop->auth_mode)
                {
                    case AUTH_LOCAL:
                        memcpy(auth_mode, "local", strlen("local"));
                        break;

                    case AUTH_RADIUS:
                        memcpy(auth_mode, "radius", strlen("radius"));
                        break;

                    case AUTH_TAC_PLUS:
                        memcpy(auth_mode, "tacas", strlen("tacas"));
                        break;
                }

                vty_out(vty, "%s%s", auth_mode, VTY_NEWLINE);
            }
        }

        vty_out(vty, "--------------------------------------------%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
#endif

DEFUN(show_history,
      show_history_cmd,
      "show history",
      SHOW_STR
      "Display the session command history\n")
{
    int index;

//  vty_out (vty, "  %s%s", vty->hist[vty->hindex], VTY_NEWLINE);
    for(index = vty->hindex + 1; index != vty->hindex;)
    {
        if(index == VTY_MAXHIST)
        {
            index = 0;
            continue;
        }

        if(vty->hist[index] != NULL)
        {
            vty_out(vty, "  %s%s", vty->hist[index], VTY_NEWLINE);
        }

        index++;
    }

    return CMD_SUCCESS;
}

DEFUN(login_timeout,
      login_timeout_cmd,
      "login timeout <1-65535>",
      "Set timeout value\n"
      "user logout after timeout\n"
      "Timeout value in minutes\n")
{
    return exec_timeout(vty, argv[0], NULL);
}

DEFUN(no_login_timeout,
      no_login_timeout_cmd,
      "no login timeout",
      NO_STR
      "Set the EXEC timeout\n"
      "Timeout default 30min")
{
    return exec_timeout(vty, NULL, NULL);
}

#if 1
int vytsh_u3_vtys_hostname_refresh(char* hostname)
{
	
	struct login_session *session_tmp;
	struct listnode *node, *nnode;

	for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_tmp))
	{
		if(0 == is_vtysh_u0_session(session_tmp))
		{
			strcpy(session_tmp->device_name,hostname);
			strcpy(session_tmp->vty->hostname,hostname);
		}
	}

	return 0;
}

int vytsh_u0_vtys_hostname_refresh(U0_INFO * p_u0_info,char* hostname)
{
	
	struct login_session *session_tmp;
	struct listnode *node, *nnode;

	for(ALL_LIST_ELEMENTS(&p_u0_info->u0_ses_lst, node, nnode, session_tmp))
	{
		strcpy(session_tmp->device_name,hostname);
		strcpy(session_tmp->vty->hostname,hostname);
	}

	return 0;
}

#endif

#if 0	  
DEFUN(vtysh_hostname,
      vtysh_hostname_cmd,
      "hostname NAMES",
      "Change hostname\n"
      "New host name(1-64 characters)\n")
{
#if 1
	printf("%s %d \n",__FUNCTION__,__LINE__);
	char *hostname;
	struct login_session *session_tmp;
	
    if(strlen(argv[0]) >= VTYSH_HOST_NAME_LEN + 1)
    {
		printf("%s %d \n",__FUNCTION__,__LINE__);
        vty_error_out ( vty, "host name length:1-64 characters %s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    
	hostname = (char *)argv[0];
	printf("%s %d vty->server_ip : %x\n",__FUNCTION__,__LINE__,vty->server_ip);
	#if 0
	pinfo = vtysh_u0_list_get_info_by_ip(vty->server_ip);
	/*在这里对U0 和设备自身作区分
	  如果能查找到表面是U0的VTY，做修改处理
	  如果查找不到表面是设备自身
	*/
	if(pinfo)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		vtysh_u0_list_modify(vty, host_name);
		#if 1
		for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_tmp))
		{
			if(session_tmp->vty->server_ip == vty->server_ip )
			{
				strcpy(session_tmp->device_name,host_name);
				strcpy(session_tmp->vty->hostname,host_name);
			}
		}
		#endif
	}
	else
	{
#if 1
		for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_tmp))
		{
			if(session_tmp->session_type == SESSION_CONSOLE )
			{
				strcpy(session_tmp->device_name,host_name);
				strcpy(session_tmp->vty->hostname,host_name);
			}
		}
#endif
		session_tmp = (	struct login_session *)(vty->session);
		strcpy(session_tmp->device_name,host_name);
		strcpy(vty->hostname,host_name);
		printf("%s %d\n",__FUNCTION__,__LINE__);
		if(host.device_name)
		{
			XFREE(MTYPE_HOST, host.device_name);
		}
    	host.device_name = XSTRDUP(MTYPE_HOST, host_name);
	}

#endif

	printf("%s %d \n",__FUNCTION__,__LINE__);
	if(vty->type == VTY_TERM || vty->type == VTY_SSH)
	{
		session_tmp = ( struct login_session *)(vty->session);
		if(session_tmp->is_u3)
		{
			printf("%s %d \n",__FUNCTION__,__LINE__);
			vytsh_u3_vtys_hostname_refresh(hostname);
			if(host.device_name)
			{
				XFREE(MTYPE_HOST, host.device_name);
			}
			host.device_name = XSTRDUP(MTYPE_HOST, hostname);
		}
		else
		{
			printf("%s %d \n",__FUNCTION__,__LINE__);
			vytsh_u0_vtys_hostname_refresh(vty->server_ip, hostname);
			vtysh_u0_list_modify(vty, hostname);
		}

	}
	else
	{
		printf("%s %d \n",__FUNCTION__,__LINE__);
		vytsh_u3_vtys_hostname_refresh(hostname);
		if(host.device_name)
		{
			XFREE(MTYPE_HOST, host.device_name);
		}
		host.device_name = XSTRDUP(MTYPE_HOST, hostname);
	}
#endif
	printf("%s %d \n",__FUNCTION__,__LINE__);

    return CMD_SUCCESS;
}
#endif	  
DEFUN(vtysh_hostname,
      vtysh_hostname_cmd,
      "hostname NAMES",
      "Change hostname\n"
      "New host name(1-64 characters)\n")
{
	char *hostname;
	struct login_session *session_tmp;
	
	U0_INFO * p_u0_info;
    if(strlen(argv[0]) >= VTYSH_HOST_NAME_LEN + 1)
    {
        vty_error_out ( vty, "host name length:1-64 characters %s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    
	hostname = (char *)argv[0];
//	printf("%s %d vty->server_ip : %x\n",__FUNCTION__,__LINE__,vty->server_ip);

	if(vty->type == VTY_TERM || vty->type == VTY_SSH)
	{
		session_tmp = ( struct login_session *)(vty->session);
		p_u0_info = vtysh_u0_list_get_info_by_ip(session_tmp->server_ip);
		if(p_u0_info)
		{
			vytsh_u0_vtys_hostname_refresh(p_u0_info, hostname);
			vtysh_u0_list_hostname_modify(vty, hostname);
		}
		else
		{
			vytsh_u3_vtys_hostname_refresh(hostname);
			if(host.device_name)
			{
				XFREE(MTYPE_HOST, host.device_name);
			}
			host.device_name = XSTRDUP(MTYPE_HOST, hostname);
		}
	}
	else
	{
		vytsh_u3_vtys_hostname_refresh(hostname);
		if(host.device_name)
		{
			XFREE(MTYPE_HOST, host.device_name);
		}
		host.device_name = XSTRDUP(MTYPE_HOST, hostname);
	}

    return CMD_SUCCESS;
}

ALIAS(vtysh_hostname,
      vtysh_sysname_cmd,
      "sysname TEXT",
      "Specify the host name\n"
      "Host name (1-64 characters)\n")

DEFUN(vtysh_no_hostname,
      vtysh_no_hostname_cmd,
      "no hostname",
      "default hostname\n"
      "hostname\n")
{
	char hostname[100];
	U0_INFO * p_u0_info;

	struct login_session *session_tmp;
 
	memset(hostname,0,sizeof(hostname));
	strcpy(hostname,"Hios");	

	session_tmp = ( struct login_session *)(vty->session);
	if(0 == is_vtysh_u0_session(session_tmp))
	{
		vytsh_u3_vtys_hostname_refresh(hostname);
		
	    if(host.device_name)
	    {
	        XFREE(MTYPE_HOST, host.device_name);
	    }
	    host.device_name = XSTRDUP(MTYPE_HOST, hostname);
	}
	else
	{
		p_u0_info = vtysh_u0_list_get_info_by_ip(session_tmp->server_ip);
		if(p_u0_info)
		{
			vytsh_u0_vtys_hostname_refresh(p_u0_info, hostname);
			vtysh_u0_list_hostname_modify(vty, hostname);
		}
	}
    return CMD_SUCCESS;
}


DEFUN(vtysh_show_hostname,
	vtysh_show_hostname_cmd,
	"show hostname",
	SHOW_STR
	"hostname\n")
{
	vtysh_u0_info_show(vty);

	return CMD_SUCCESS;
}



#if 0
DEFUN(vtysh_user_exit,
      vtysh_user_exit_cmd,
      "exit",
      "exit\n")
{
printf("%s: exit \n", __FUNCTION__);	
    return vtysh_exit (vty);
}
#endif

DEFUN(vtysh_show_slot_type,
      vtysh_show_slot_type_cmd,
      "show vty slot ",
      "show vtysh slot status\n"
      "vtysh\n"
      "slot role\n")
{
    char slot_type[32];
    char vty_status[32];
    memset(slot_type, 0, 32);
    memset(vty_status, 0, 32);

    switch(syncvty.vty_slot)
    {
        case VTY_UNKNOW_SLOT:
            memcpy(slot_type, "BOX_DEVICE", strlen("BOX_DEVICE"));
            break;

        case VTY_MAIN_SLOT:
            memcpy(slot_type, "MAIN_SLOT", strlen("MAIN_SLOT"));
            break;

        case VTY_SLAVE_SLOT:
            memcpy(slot_type, "SLAVE_SLOT", strlen("SLAVE_SLOT"));
            break;

        default:
            memcpy(slot_type, "SLOT_ERR", strlen("SLOT_ERR"));
            break;
    }

    switch(syncvty.self_status)
    {
        case VTYSH_UNREADY:
            memcpy(vty_status, "VTYSH_UNREADY", strlen("VTYSH_UNREADY"));
            break;

        case VTYSH_START:
            memcpy(vty_status, "VTYSH_START", strlen("VTYSH_START"));
            break;

        case VTYSH_BATCH_CONFIG_START:
            memcpy(vty_status, "VTYSH_BATCH_CONFIG_START", strlen("VTYSH_BATCH_CONFIG_START"));
            break;

        case VTYSH_BATCH_CONFIG_FINISH:
            memcpy(vty_status, "VTYSH_BATCH_CONFIG_FINISH", strlen("VTYSH_BATCH_CONFIG_FINISH"));
            break;

        case VTYSH_BATCH_CONFIG_SENT:
            memcpy(vty_status, "VTYSH_BATCH_CONFIG_SENT", strlen("VTYSH_BATCH_CONFIG_SENT"));
            break;

        case VTYSH_BATCH_CONFIG_RECV:
            memcpy(vty_status, "VTYSH_BATCH_CONFIG_RECV", strlen("VTYSH_BATCH_CONFIG_RECV"));
            break;

        case VTYSH_SYNC_CATCH_CMD:
            memcpy(vty_status, "VTYSH_SYNC_CATCH_CMD", strlen("VTYSH_SYNC_CATCH_CMD"));
            break;

        case VTYSH_REALTIME_SYNC_ENABLE:
            memcpy(vty_status, "VTYSH_REALTIME_SYNC_ENABLE", strlen("VTYSH_REALTIME_SYNC_ENABLE"));
            break;

        default:
            memcpy(vty_status, "ERROR", strlen("ERROR"));
            break;

    }

    if(syncvty.vty_slot == VTY_UNKNOW_SLOT)
    {
        vty_out(vty, "vtysh is on %s%s", slot_type, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "slot_type: %s%s", slot_type, VTY_NEWLINE);
        vty_out(vty, "self_slot: %d%s", syncvty.self_slot, VTY_NEWLINE);
        vty_out(vty, "peer_slot: %d%s", syncvty.peer_slot, VTY_NEWLINE);
        vty_out(vty, "status   : %s %d%s", vty_status, syncvty.self_status, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
static int vtysh_check_id_proc(const char *p_name)
{
    unsigned int i;

    if(NULL == p_name)
    {
        return 0;
    }

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if((strncmp(vtysh_client[i].name, p_name, strlen(p_name)) == 0) && (vtysh_client[i].fd >= 0))
        {
            return i;
        }
    }

    return -1;
}
static void
show_separator(struct vty *vty)
{
    vty_out(vty, "-----------------------------\r\n");
}
static int vtysh_show_memory_list(struct vty *vty, struct memory_list *list)
{
    struct memory_list *m;
    int needsep = 0;

    for(m = list; m->index >= 0; m++)
        if(m->index == 0)
        {
            if(needsep)
            {
                show_separator(vty);
                needsep = 0;
            }
        }
        else if(mstat[m->index].alloc)
        {
            vty_out(vty, "%-30s: %10ld\r\n", m->format, mstat[m->index].alloc);
            needsep = 1;
        }

    return needsep;
}

static int vtysh_show_vtysh_proc_memory(struct vty *vty)
{
    struct mlist *ml;
    int needsep = 1;

    for(ml = mlists; ml->list; ml++)
    {
        if(needsep)
        {
            show_separator(vty);
        }

        needsep = vtysh_show_memory_list(vty, ml->list);
    }

    return 0;
}
DEFUN(vtysh_show_memory,
      vtysh_show_memory_cmd,
      "show system memory (all|mpls|snmpd|ripd|isisd|qosd|ifm|dhcp|route|hal|syslog|devm|ftm|filem|ping|ntp|l2|ces|aaa|rmond|ospfd|ospf6d|system|bgpd|alarm|statis|vtysh|webd|openflow|clock|ipmc)",
      SHOW_STR
      "system function\n"
      "Memory statistics\n"
      "All process\n"
      "Mpls process\n"
      "Snmpd process\n"
      "Ripd process\n"
      "Isisd process\n"
      "Qosd process\n"
      "Ifm process\n"
      "Dhcp process\n"
      "Route process\n"
      "Hal process\n"
      "Syslog process\n"
      "Devm process\n"
      "Ftm process\n"
      "Filem process\n"
      "Ping process\n"
      "Ntp process\n"
      "L2 process\n"
      "Ces process\n"
      "AAA process\n"
      "Rmon process\n"
      "Ospfd process\n"
      "Ospf6d process\n"
      "System process\n"
      "Bgpd process\n"
      "Alarm process\n"
      "Statis process\n"
      "Vtysh process\n"
      "Webd process\n"
      "Openflow process\n"
      "Clock process\n"
      "Ipmc process\n"
     )
{
    int  i;
    int ret = CMD_SUCCESS;
    char line[] = "show system memory\n";

    if(strcmp(argv[0], "all") == 0)
    {
        vty->cmd_process_lock = 1;

        for(i = 0; i < (int)array_size(vtysh_client); i++)
        {
            if(vtysh_client[i].fd >= 0)
            {
                vty_out(vty, "Memory statistics for %s:%s", vtysh_client[i].name, VTY_NEWLINE);
                ret = vtysh_client_execute(&vtysh_client[i], line, stdout, vty, i);
            }
        }

        vty->cmd_process_lock = 0;
        vty_out(vty, "Memory statistics for %s:%s", "vtysh", VTY_NEWLINE);
        vtysh_show_vtysh_proc_memory(vty);
    }
    else if(strcmp(argv[0], "vtysh") == 0)
    {
        vtysh_show_vtysh_proc_memory(vty);
    }
    else
    {
        if((i = vtysh_check_id_proc(argv[0])) < 0 || i >= (int)array_size(vtysh_client))
        {
            vty_out(vty, "the proc %s is not exist!%s", argv[0], VTY_NEWLINE);
            return CMD_WARNING;
        }

        if(vtysh_client[i].fd >= 0)
        {
            vty->cmd_process_lock = 1;
            ret = vtysh_client_execute(&vtysh_client[i], line, stdout, vty, i);
            vty->cmd_process_lock = 0;
        }
    }

    return ret;
}
#if 0
extern AUTH_MODE aaa_authmode;
DEFUN(vtysh_authmode,
      vtysh_authmode_cmd,
      "login auth (local | radius | tacas)",
      "Change auth mode\n"
      "Auth\n"
      "Local mode\n"
      "Radius mode\n"
      "Tacas mode\n")
{
    if(argv[0][0] == 'r')
    {
        host.aaa_authmode = AUTH_RADIUS;
    }
    else if(argv[0][0] == 't')
    {
        host.aaa_authmode = AUTH_TAC_PLUS;
    }
    else if(argv[0][0] == 'l')
    {
        host.aaa_authmode = AUTH_LOCAL;
    }

    return CMD_SUCCESS;
}

DEFUN(vtysh_no_authmode,
      vtysh_no_authmode_cmd,
      "no login auth",
      "Change to default\n"
      "Change auth mode\n"
      "Auth\n")
{
    host.aaa_authmode = AUTH_LOCAL;

    return CMD_SUCCESS;
}
#endif
/************************cmd of ifm***************************/

DEFUN(vtysh_write_terminal_interface,
      vtysh_write_terminal_interface_cmd,
      "show running-config interface",
      SHOW_STR
      "running-config\n"
      "interface\n")
{
    u_int i;
    char line[] = "write terminal\n";

    vty_out(vty, "Building interface configuration...%s", VTY_NEWLINE);
    vty_out(vty, "%sCurrent interface configuration:%s", VTY_NEWLINE,
            VTY_NEWLINE);
    vty_out(vty, "!%s", VTY_NEWLINE);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_vty_allif_config_dump(vty, "interface");
    pthread_mutex_unlock(&vtysh_config_lock);

//  vty_out (vty, "end%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(vtysh_write_terminal_interface_ethernet_usp,
      vtysh_write_terminal_interface_ethernet_usp_cmd,
      "show running-config interface ethernet USP",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_ETHERNET_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("ethernet", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface ethernet %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;
    pthread_mutex_unlock(&vtysh_config_lock);


    vtysh_vty_if_config_dump(vty, buf);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_gigabit_ethernet_usp,
      vtysh_write_terminal_interface_gigabit_ethernet_usp_cmd,
      "show running-config interface gigabitethernet USP",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface gigabitethernet %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;
    pthread_mutex_unlock(&vtysh_config_lock);


    vtysh_vty_if_config_dump(vty, buf);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_xgigabit_ethernet_usp,
      vtysh_write_terminal_interface_xgigabit_ethernet_usp_cmd,
      "show running-config interface xgigabitethernet USP",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface xgigabitethernet %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;
    pthread_mutex_unlock(&vtysh_config_lock);


    vtysh_vty_if_config_dump(vty, buf);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}


DEFUN(vtysh_write_terminal_interface_trunk_usp,
      vtysh_write_terminal_interface_trunk_usp_cmd,
      "show running-config interface trunk TRUNK",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_TRUNK_STR
      CLI_INTERFACE_TRUNK_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("trunk", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface trunk %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;


    vtysh_vty_if_config_dump(vty, buf);
    pthread_mutex_unlock(&vtysh_config_lock);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_tunnel_usp,
      vtysh_write_terminal_interface_tunnel_usp_cmd,
      "show running-config interface tunnel USP",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_TUNNEL_STR
      CLI_INTERFACE_TUNNEL_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("tunnel", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface tunnel %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_vty_if_config_dump(vty, buf);
    pthread_mutex_unlock(&vtysh_config_lock);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_vlanif_usp,
      vtysh_write_terminal_interface_vlanif_usp_cmd,
      "show running-config interface vlanif <1-4094>",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_VLANIF_STR
      CLI_INTERFACE_VLANIF_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("vlanif", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface vlanif %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_vty_if_config_dump(vty, buf);
    pthread_mutex_unlock(&vtysh_config_lock);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_loopback_usp,
      vtysh_write_terminal_interface_loopback_usp_cmd,
      "show running-config interface loopback <0-128>",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_LOOPBACK_STR
      CLI_INTERFACE_LOOPBACK_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("loopback", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface loopback %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_vty_if_config_dump(vty, buf);
    pthread_mutex_unlock(&vtysh_config_lock);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_tdm_usp,
      vtysh_write_terminal_interface_tdm_usp_cmd,
      "show running-config interface tdm USP",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_TDM_STR
      CLI_INTERFACE_TDM_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("tdm", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface tdm %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_vty_if_config_dump(vty, buf);
    pthread_mutex_unlock(&vtysh_config_lock);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_write_terminal_interface_clock_usp,
      vtysh_write_terminal_interface_clock_usp_cmd,
      "show running-config interface clock <1-2>",
      SHOW_STR
      "running-config\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_CLOCK_STR
      CLI_INTERFACE_CLOCK_VHELP_STR)
{
    uint32_t ifindex = 0;
    char line[50] = "write terminal\n";
    char buf[50];

    ifindex = ifm_get_ifindex_by_name("clock", (char *) argv[0]);

    if(ifindex == 0)
    {
        vty_out(vty, "%%Wrong format,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sprintf(buf, "interface clock %s", argv[0]);

    pthread_mutex_lock(&vtysh_config_lock);
    vty->cmd_process_lock = 1;

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_vty_if_config_dump(vty, buf);
    pthread_mutex_unlock(&vtysh_config_lock);

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(vtysh_reset_configure,
      vtysh_reset_configure_cmd,
      "protest restore",
      "Reset configuration to factory setting\n"
      "Factory setting\n")
{
    /*remove startup.conf*/
    if(access(VTYSH_STARTUP_CONFIG, F_OK) == 0)
    {
        if(rename(VTYSH_STARTUP_CONFIG, VTYSH_STARTUP_CONFIG_BAK) != 0)
        {
            vty_out(vty, "fail%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    system("sync");
    vty_out(vty, "success%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(vtysh_protest_status,
      vtysh_protest_status_cmd,
      "protest status",
      "Reset configuration to factory setting\n"
      "Factory setting\n")
{
    if(access(config_default, F_OK) < 0)
    {
        vty_out(vty, "factory setting%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    else
    {
        vty_out(vty, "not factory setting%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
}

DEFUN(vtysh_slave_config,
      vtysh_slave_config_cmd,
      "slave_config (enable | disable)",
      "slave_config\n"
      "enable\n"
      "disable\n")
{
    if(argv[0][0] == 'e')
    {
        g_slave_config_enable = 1;
    }
    else
    {
        g_slave_config_enable = 0;
    }

    return CMD_SUCCESS;
}



	
    

	

const struct message vtysh_dbg_name[] = {
	{.key = VD_COMM,    .str = "comm"},
	{.key = VD_MSG,	    .str = "msg"},
	{.key = VD_TELNET,	.str = "telnet"},
	{.key = VD_SYNC, 	.str = "sync"},
	{.key = VD_AUTH, 	.str = "auth"},
	{.key = VD_SSH, 	.str = "ssh"},
	{.key = VD_CMD, 	.str = "cmd"},
	{.key = VD_CONFIG, 	.str = "config"},
	{.key = VD_CLIENT, 	.str = "client"},
	{.key = VD_ALL, 	.str = "all"},
};
DEFUN(vtysh_debug,
		vtysh_debug_cmd,
		"debug vtysh (enable | disable) (comm | msg | telnet | sync | auth | ssh | cmd | config | client| all)",
		"Debug config\n"
		"Vtysh config\n"
		"Vtysh debug enable\n"
		"Vtysh debug disable\n"
		"Comm debug\n"
		"Msg debug\n"
		"Telnet debug\n"
		"Sync debug\n"
		"Auth debug\n"
		"Ssh debug\n"
		"Cmd debug\n"
		"Config debug\n"
		"Client debug\n"
		"All debug\n")
{
	int zlog_num;
	for(zlog_num = 0; zlog_num < array_size(vtysh_dbg_name); zlog_num++)
	{
		if(!strncmp(argv[1], vtysh_dbg_name[zlog_num].str, 2))
		{
			zlog_debug_set( vty, vtysh_dbg_name[zlog_num].key, !strncmp(argv[0], "enable", 1));
			return CMD_SUCCESS;
		}
	}
	vty_out (vty, "No debug type find %s", VTY_NEWLINE);
    return CMD_SUCCESS;
}
DEFUN (show_vtysh_debug,
	show_vtysh_debug_cmd,
	"show vtysh debug",
	SHOW_STR
	"Vtysh"
	"Debug status\n")
{
	int type_num;
	vty_out(vty, "debug type	status %s", VTY_NEWLINE);
	for(type_num = 0; type_num < array_size(vtysh_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s %-10s %s", vtysh_dbg_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}
    zlog_debug(VD_COMM, "debug test com");
    zlog_debug(VD_MSG, "debug test msg");
    zlog_debug(VD_TELNET, "debug test telnet");
	return CMD_SUCCESS;
}

DEFUN(vtysh_exit_user_line,
        vtysh_exit_user_line_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

ALIAS(vtysh_exit_user_line,
        vtysh_quit_user_line_cmd,
        "quit",
        "Exit current mode and down to previous mode\n")


DEFUN(vtysh_user_line_console,
      vtysh_user_line_console_cmd,
      "line console <0>",
      "Configure the line\n"
      "Console line\n"
      "Number of the line\n")
{
	int min = 0;
	char *prompt = NULL;
	
	prompt = vty->change_prompt;
	min = atoi(argv[0]);
	if(min != 0)
	{
		return CMD_ERR_NO_MATCH;
	}

	vty->node = USER_LINE_NODE;
	if(prompt)
	{
		snprintf(prompt, VTY_BUFSIZ, "%%s(config-console0)#");
	}
	
	vty->index = (void *)min;

	return CMD_SUCCESS;
}

// vty 0-63,
DEFUN(vtysh_user_line_vty,
      vtysh_user_line_vty_cmd,
      "line vty <0-63> [<0-63>]",
      "Configure the line\n"
      "Vty line\n"
      "Number of the first line\n"
      "Number of the last line\n")
{
	int min = 0;
	int max = 0;
	char *prompt = NULL;
	
	prompt = vty->change_prompt;
	min = atoi(argv[0]);
	if(argv[1])
	{
		max = atoi(argv[1]);
		if(max <= min)
		{
			return CMD_ERR_NO_MATCH;
		}
		vty->node = USER_LINE_NODE;
		if(prompt)
		{
			snprintf(prompt, VTY_BUFSIZ, "%%s(config-vty%d-%d)#", min, max);
		}

		//高16位存范围值的上限，> 0
		max <<= 16;
		max |= min;
		vty->index = (void *)max;
	}
	else  
	{
		vty->node = USER_LINE_NODE;
		if(prompt)
		{
			snprintf(prompt, VTY_BUFSIZ, "%%s(config-vty%d)#", min);
		}
		//输入单个数值时，高16位用fffff表示，而console的数值高16位是 0
		max = 0xffff0000 | min;
		vty->index = (void *)max;
	}

	return CMD_SUCCESS;
}


#define VYSH_USER_LINE_INDEX_GET(in, index, index_m)\
do{\
	index = in & 0xffff;\
	index_m = (in >> 16) & 0xffff;\
}while(0)

DEFUN(vtysh_auth_mode,
      vtysh_auth_mode_cmd,
      "authentication-mode (password | scheme)",
      "Login authentication mode\n"
//      "Login without authentication\n"
      "Password authentication\n"
      "Authentication use AAA\n")
{
	int index = 0;
	int index_m = 0;
	int user_line = 0;
	int mode = 0;
	int i = 0;

	// ��ȡ��ǰline�������������console ���϶���0�������vty ������Χindex-index_m
	VYSH_USER_LINE_INDEX_GET((int)vty->index, index, index_m);
	VTY_DEBUG(VD_CMD, "index %d - %d, agrv:%s ", index, index_m, argv[0]);
	
	if(argv[0][0] == 'n')
	{
		mode = AUTH_MODE_NONE;
	}
	else if(argv[0][0] == 'p')
	{
		mode = AUTH_MODE_PASSWORD;
	}
	else
	{
		mode = AUTH_MODE_SCHEME;
	}

	if(index_m == 0)
	{
		user_line = USER_LINE_CONSOLE;
	}
	else
	{
		user_line = USER_LINE_VTY;
	}
	
	if(index_m == 0xffff) // ��16λΪFFFFF�� ˵���ǵ���ֵ
	{
		index_m = index;
	}

	//���console ����vty����ֻ��һ����index_m = 0
	for(i = index; i <= index_m; i++)
	{
		vtysh_auth_mode_set(user_line, i, mode);
	}	
		
	return CMD_SUCCESS;
}

DEFUN(vtysh_user_role,
      vtysh_user_role_cmd,
      "user-role (network-admin | network-operator)",
      "Specify user role configuration information\n"
      "Network-admin role\n"
      "Network-operator role\n")
{
	int index = 0;
	int index_m = 0;
	int user_line = 0;
	int role = 0;
	int i = 0;
	
	VYSH_USER_LINE_INDEX_GET((int)vty->index, index, index_m);

	VTY_DEBUG(VD_CMD, "index:%d - %d ", index, index_m);

	if(argv[0][8] == 'a')
	{
		role = USER_ROLE_NETWORK_ADMIN;
	}
	else
	{
		role = USER_ROLE_NETWORK_OPERATOR;
	}

	if(index_m == 0)
	{
		user_line = USER_LINE_CONSOLE;
	}
	else
	{
		user_line = USER_LINE_VTY;
	}
	
	if(index_m == 0xffff) 
	{
		index_m = index;
	}

	for(i = index; i <= index_m; i++)
	{
		vtysh_user_role_set(user_line, i, role);
	}
			
	return CMD_SUCCESS;
}


DEFUN(vtysh_auth_pwd,
      vtysh_auth_pwd_cmd,
      "set authentication password simple WORD",
      "Specify line parameters\n"
      "Specify the authentication parameters for line\n"
      "Specify the password of line\n"
//      "Specify a hashtext password\n"
      "Specify a plaintext password\n"
      "PASSWORD<1-16> Plaintext password string\n")
{
	int index = 0;
	int index_m = 0;
	int user_line = 0;
	int i = 0;	

	VYSH_USER_LINE_INDEX_GET((int)vty->index, index, index_m);

	VTY_DEBUG(VD_CMD, "index:%d - %d ", index, index_m);
    
	if(strlen(argv[0]) > 16)
	{
		vty_out(vty, " %%Password length <1-16>%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(index_m == 0)
	{
		user_line = USER_LINE_CONSOLE;
	}
	else
	{
		user_line = USER_LINE_VTY;
	}
	
	if(index_m == 0xffff) 
	{
		index_m = index;
	}

	for(i = index; i <= index_m; i++)
	{
		vtysh_auth_pwd_set(user_line, i, argv[0]);
	}
			
	return CMD_SUCCESS;
}

DEFUN(vtysh_idle_time,
      vtysh_idle_time_cmd,
      "idle-timeout <0-35791> [<0-59>]",
      "User connection idle timeout\n"
      "Number of minutes for the connection idle timeout\n"
      "Number of seconds for the connection idle timeout\n")
{
	int index = 0;
	int index_m = 0;
	int user_line = 0;
	int i = 0;
	int min = 0;
	int sec = 0;

	VYSH_USER_LINE_INDEX_GET((int)vty->index, index, index_m);

	VTY_DEBUG(VD_CMD, "index:%d - %d ", index, index_m);

	if(index_m == 0)
	{
		user_line = USER_LINE_CONSOLE;
	}
	else
	{
		user_line = USER_LINE_VTY;
	}
	
	if(index_m == 0xffff) 
	{
		index_m = index;
	}

	min = atoi(argv[0]);
	if(argv[1])
	{
		sec = atoi(argv[1]);
	}
	VTY_DEBUG(VD_CMD, "idle time:min %d, sec %d ", min, sec);
	sec = min * 60 + sec;
	if(sec != 0 && sec < 5)
	{
		sec = 5;
	}
	
	for(i = index; i <= index_m; i++)
	{
		vtysh_idle_time_set(user_line, i, sec);
	}
			
	return CMD_SUCCESS;
}

DEFUN(vtysh_idle_time_default,
      vtysh_idle_time_default_cmd,
      "undo idle-timeout",
      "Cancel current setting\n"
      "User connection idle timeout\n")
{
	int index = 0;
	int index_m = 0;
	int user_line = 0;
	int i = 0;
	int sec = 0;

	VYSH_USER_LINE_INDEX_GET((int)vty->index, index, index_m);

	VTY_DEBUG(VD_CMD, "index:%d - %d ", index, index_m);

	if(index_m == 0)
	{
		user_line = USER_LINE_CONSOLE;
	}
	else
	{
		user_line = USER_LINE_VTY;
	}
	
	if(index_m == 0xffff) 
	{
		index_m = index;
	}

	sec = 600;
	
	for(i = index; i <= index_m; i++)
	{
		vtysh_idle_time_set(user_line, i, sec);
	}
			
	return CMD_SUCCESS;
}


DEFUN(vtysh_telnet_enable,
      vtysh_telnet_enable_cmd,
      "telnet server enable",
      "Specify telnet configuration information\n"
      "Telnet server configuration\n"
      "Enable telnet server function\n")
{
	vtysh_telnet_enable_set(1);

	return CMD_SUCCESS;
}

DEFUN(vtysh_telnet_disable,
      vtysh_telnet_disable_cmd,
      "undo telnet server enable",
      "Cancel current setting\n"
      "Specify telnet configuration information\n"
      "Telnet server configuration\n"
      "Enable telnet server function\n")
{
	vtysh_telnet_enable_set(0); 
	
	return CMD_SUCCESS;	
}


DEFUN(vtysh_vpn_config,
	vtysh_vpn_config_cmd,
	"vtysh vpn-instance <1-128>",
	"Vtysh\n"
	"Vpn instance\n"
	"Vpn instance : 1-128\n")
{
	vtysh_vpn_set((uint16_t)atoi(argv[0])); 

	return CMD_SUCCESS; 
}

DEFUN(no_vtysh_vpn_config,
	no_vtysh_vpn_config_cmd,
	"no vtysh vpn-instance",
	"No\n"
	"Vtysh\n"
	"Vpn instance\n")
{
	vtysh_vpn_set(0); 

	return CMD_SUCCESS; 
}

DEFUN(show_vtysh_vpn_config,
	show_vtysh_vpn_config_cmd,
	"show vty vpn-instance",
	SHOW_STR
	"Vtysh\n"
	"Vpn instance\n")
{
	vty_out(vty, "%s : %u %s", "vtysh vpn", vtysh_vpn_get(), VTY_NEWLINE);

	return CMD_SUCCESS; 
}



#if 0
DEFUN(show_ifm_tdm_vtysh,
      show_ifm_tdm_cmd_vtysh,
      "show interface tdm [USP]",
      SHOW_STR
      CLI_INTERFACE_STR
      CLI_INTERFACE_TDM_STR
      CLI_INTERFACE_TDM_VHELP_STR)
{
    u_int i;
    char line[50] = "show interface tdm ";
    FILE *fp = NULL;

    if(argv[0] != NULL)
    {
        strcat(line, argv[0]);
    }

    strcat(line, "\n");

    if(vty_shell(vty))
    {
        if(vtysh_pager_name)
        {
            fp = popen(vtysh_pager_name, "w");

            if(fp == NULL)
            {
                perror("popen");
                exit(1);
            }
        }
        else
        {
            fp = stdout;
        }
    }

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_IFM
                || vtysh_client[i].flag == VTYSH_CES)
        {
            vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
        }
    }

    vtysh_vty_if_config_dump(vty, "interface tdm");

    if(vtysh_pager_name && fp && vty_shell(vty))
    {
        fflush(fp);

        if(pclose(fp) == -1)
        {
            perror("pclose");
            exit(1);
        }

        fp = NULL;
    }

    vty_out(vty, "end%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}

DEFUN(show_ifm_tdm_config_vtysh,
      show_ifm_tdm_config_cmd_vtysh,
      "show interface tdm [USP] config",
      SHOW_STR
      CLI_INTERFACE_STR
      CLI_INTERFACE_TDM_STR
      CLI_INTERFACE_TDM_VHELP_STR)
{
    u_int i;
    char line[50] = "show interface tdm ";
    FILE *fp = NULL;

    if(argv[0] != NULL)
    {
        strcat(line, argv[0]);
    }

    strcat(line, " config\n");

    if(vty_shell(vty))
    {
        if(vtysh_pager_name)
        {
            fp = popen(vtysh_pager_name, "w");

            if(fp == NULL)
            {
                perror("popen");
                exit(1);
            }
        }
        else
        {
            fp = stdout;
        }
    }

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_IFM
                || vtysh_client[i].flag == VTYSH_CES)
        {
            vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
        }
    }

    vtysh_vty_if_config_dump(vty, "interface tdm");

    if(vtysh_pager_name && fp && vty_shell(vty))
    {
        fflush(fp);

        if(pclose(fp) == -1)
        {
            perror("pclose");
            exit(1);
        }

        fp = NULL;
    }

    vty_out(vty, "end%s", VTY_NEWLINE);

    return CMD_SUCCESS;

}
#endif

void
vtysh_install_default(enum node_type node)
{
    install_element_level(node, &config_list_cmd, 0, CMD_LOCAL);
    install_element_level(node, &vtysh_end_all_cmd, 0, CMD_SYNC);    
    install_element_level(node, &vtysh_return_all_cmd, 0, CMD_SYNC);
    install_element_level(node, &vtysh_show_running_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);
}

void vtysh_init_default_cmd(void)
{
//  install_node (&sysrun_node, NULL);
    /* Install default. */
    vtysh_install_default(CONFIG_NODE);
			    
    install_element_level(CONFIG_NODE, &vtysh_exit_all_cmd, 0, CMD_SYNC);
    install_element_level(CONFIG_NODE, &vtysh_quit_node_cmd, 0, CMD_SYNC);
    vtysh_install_default(SYSRUN_NODE);
    install_element_level(SYSRUN_NODE, &vtysh_exit_all_cmd, 0, CMD_SYNC);

    install_element_level(CONFIG_NODE, &vtysh_sysrun_cmd, MANAGE_LEVE, CMD_SYNC);

    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_ethernet_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_gigabit_ethernet_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_xgigabit_ethernet_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_trunk_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_tunnel_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_vlanif_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_loopback_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_tdm_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_write_terminal_interface_clock_usp_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    //install_element_level(CONFIG_NODE, &vtysh_write_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &vtysh_write_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &vtysh_write_config2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &vtysh_reset_file_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &vtysh_write_module_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &show_startup_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	//install_element_level(CONFIG_NODE, &vtysh_test_alarm_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(CONFIG_NODE, &vtysh_terminal_length_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_terminal_no_length_cmd, CONFIG_LEVE_5, CMD_LOCAL);

    install_element_level(CONFIG_NODE, &vtysh_show_daemons_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_show_memory_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level(SYSRUN_NODE, &vtysh_telnet_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(SYSRUN_NODE, &vtysh_telnet_port_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_history_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level(SYSRUN_NODE, &vtysh_slave_config_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(SYSRUN_NODE, &vtysh_start_shell_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_hostname_cmd, MONITOR_LEVE_2, CMD_SYNC);
    
    install_element_level(CONFIG_NODE, &vtysh_no_hostname_cmd, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level(CONFIG_NODE, &vtysh_show_hostname_cmd, MONITOR_LEVE_2, CMD_SYNC);
	
    install_element_level(CONFIG_NODE, &vtysh_reset_configure_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_protest_status_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_show_slot_type_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level(CONFIG_NODE, &vtysh_debug_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_vtysh_debug_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    /*h3c cmd*/
    install_node(&user_line_node, NULL );
	vtysh_install_default(USER_LINE_NODE);
	install_element_level(CONFIG_NODE, &vtysh_user_line_console_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &vtysh_user_line_vty_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_sysview_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_save_file_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &vtysh_sysname_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    
	install_element_level(USER_LINE_NODE, &vtysh_exit_user_line_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(USER_LINE_NODE, &vtysh_quit_user_line_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level(USER_LINE_NODE, &vtysh_auth_mode_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(USER_LINE_NODE, &vtysh_user_role_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(USER_LINE_NODE, &vtysh_auth_pwd_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    
    install_element_level(USER_LINE_NODE, &vtysh_idle_time_cmd, MONITOR_LEVE_2, CMD_LOCAL);    
    install_element_level(USER_LINE_NODE, &vtysh_idle_time_default_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    
    install_element_level(CONFIG_NODE, &vtysh_telnet_disable_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &vtysh_telnet_enable_cmd, MONITOR_LEVE_2, CMD_LOCAL);	

	install_element_level(CONFIG_NODE, &vtysh_vpn_config_cmd, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_vtysh_vpn_config_cmd, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level(CONFIG_NODE, &show_vtysh_vpn_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);
}


void vtysh_install_cmd(void)
{
    /*install default command*/
    vtysh_init_default_cmd();

    /*install command from other apps*/
    vtysh_init_cmd();
    pthread_mutex_init(&vtysh_exec_cmd_lock, NULL);
}

/* Making connection to protocol daemon. */
static int
vtysh_connect(struct vtysh_client *vclient, int *connect_fd, struct vty *vty)
{
    int ret;
    int sock, len;
    struct sockaddr_un addr;
    struct stat s_stat;

    /* Stat socket to see if we have permission to access it. */
    ret = stat(vclient->path, &s_stat);

    if(ret < 0 && errno != ENOENT)
    {
        fprintf(stderr, "vtysh_connect(%s): stat = %s\n",
                vclient->path, safe_strerror(errno));
        exit(1);
    }

    if(ret >= 0)
    {
        if(! S_ISSOCK(s_stat.st_mode))
        {
            fprintf(stderr, "vtysh_connect(%s): Not a socket\n",
                    vclient->path);
            exit(1);
        }

    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if(sock < 0)
    {
#ifdef DEBUG
        fprintf(stderr, "vtysh_connect(%s): socket = %s\n", vclient->path,
                safe_strerror(errno));
#endif /* DEBUG */
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, vclient->path, strlen(vclient->path));
#ifdef HAVE_STRUCT_SOCKADDR_UN_SUN_LEN
    len = addr.sun_len = SUN_LEN(&addr);
#else
    len = sizeof(addr.sun_family) + strlen(addr.sun_path);
#endif /* HAVE_STRUCT_SOCKADDR_UN_SUN_LEN */


    ret = connect(sock, (struct sockaddr *) &addr, len);

    if(ret < 0)
    {
#ifdef DEBUG
        fprintf(stderr, "vtysh_connect(%s): connect = %s\n", vclient->path,
                safe_strerror(errno));
#endif /* DEBUG */
        close(sock);
        return -1;
    }

    if(vclient->fd == -1)
    {
        vclient->fd = sock;
    }

    if(*connect_fd == -1)
    {
        *connect_fd = sock;
    }

    FD_SET(sock, &vty->read_app_fd);


    return 0;
}

int
vtysh_connect_all(struct vty *vty)
{
    u_int i;
    int rc = 0;

    for(i = 0; i < array_size(vtysh_client); i++)
    {

        if(vtysh_connect(&vtysh_client[i], &(vty->connect_fd[i]), vty) == 0)
        {
            rc++;
        }
    }


    return rc;
}

/* To disable readline's filename completion. */
static char *
vtysh_completion_entry_function(const char *ignore, int invoking_key)
{
    return NULL;
}

void
vtysh_readline_init(void)
{
    /* readline related settings. */
    rl_bind_key('?', (rl_command_func_t *) vtysh_rl_describe);
    rl_completion_entry_function = vtysh_completion_entry_function;
    rl_attempted_completion_function = (rl_completion_func_t *)new_completion;
}

char *
vtysh_prompt(void)
{
//  static struct utsname names;
    static char buf[100];
    char *hostname = NULL;
    struct login_session *session = NULL;

    session = (struct login_session *)vty->session;
//  hostname = strdup (session->device_name);
//  hostname = host.device_name;
	hostname = vty->hostname;
    
	VTY_DEBUG(VD_CMD, "session_type %d, node %d, session->auth_prompt %d", session->session_type, vty->node, 
								session->auth_prompt);
    #if 0								
    if(vty->node == AUTH_NODE && session->auth_prompt == 1)
    {
        strcpy(buf, "Username: ");
    }
    #endif
    if((vty->node == AUTH_NODE)) //  && (session->auth_prompt == 1)
    {
		if(session->session_type == SESSION_CONSOLE)
		{
			if(session->ul->auth_mode == AUTH_MODE_PASSWORD)
			{
				strcpy(buf, "Password: ");
				session->auth_prompt = 0; // 
			}
			
			if(session->auth_prompt == 1)
    		{
        		strcpy(buf, "Username: ");
    		}
    		else
    		{
				strcpy(buf, "Password: ");
			}
		}
    }    
    else
    {
        if(cmd_prompt(vty->node) == NULL)
        {
            vty_out(vty, "vtysh_prompt erro node vty->node=%d\n ", vty->node);
//      free(hostname);
            return NULL;
        }

        if((vty->node ==  INTERFACE_NODE)
                || (vty->node == LOOPBACK_IF_NODE)
                || (vty->node == VLANIF_NODE)
                || (vty->node == TRUNK_IF_NODE)
                || (vty->node == TRUNK_SUBIF_NODE)
                || (vty->node == TUNNEL_IF_NODE)
                || (vty->node == TDM_IF_NODE)
                || (vty->node == TDM_SUBIF_NODE)
                || (vty->node == STM_IF_NODE)
                || (vty->node == STM_SUBIF_NODE)
                || (vty->node == PHYSICAL_IF_NODE)
                || (vty->node == PHYSICAL_SUBIF_NODE)
                || (vty->node == CLOCK_NODE)
                || (vty->node == VCG_NODE)  
                || (vty->node == E1_IF_NODE)  
                || (vty->node == QOS_DOMAIN_NODE)
                || (vty->node == QOS_PHB_NODE)
                || (vty->node == BFD_SESSION_NODE)
                || (vty->node == BFD_TEMPLATE_NODE)
                || (vty->node == PW_NODE)
                || (vty->node == L3VPN_NODE)
                || (vty->node == VSI_NODE)
                || (vty->node == MPLSTP_OAM_SESS_NODE)
                || (vty->node == MPLSTP_OAM_MEG_NODE)
                || (vty->node == PIM_INSTANCE_NODE)
                || (vty->node == TRUNK_GLOABLE_NODE)
                || (vty->node == IGMP_NODE)
                || (vty->node == CFM_SESSION_NODE)
                || (vty->node == CFM_MD_NODE)
                || (vty->node == CFM_MA_NODE)
				|| (vty->node == AAA_LOCAL_USER_NODE)
				|| (vty->node == PW_CLASS_NODE)
				|| (vty->node == XCONNECT_GROUP_NODE)
				|| (vty->node == USER_LINE_NODE))
        {
            snprintf(buf, sizeof buf, vty->change_prompt, hostname);
        }
        else
        {
            snprintf(buf, sizeof buf, cmd_prompt(vty->node), hostname);
        }
    }

//  if(!hostname)
//      free(hostname);

    return buf;
}

/* Put out prompt and wait input from user. */
void
vty_prompt(struct vty *vty)
{
//  struct utsname names;
    char *hostname = NULL;
//  struct login_session *session = vty->session;

    if(vty->type == VTY_TERM || vty->type == VTY_SSH)
    {
//      hostname = strdup (session->device_name);
//      hostname = host.device_name;
		hostname = vty->hostname;

        if(cmd_prompt(vty->node) == NULL)
        {
            vty_out(vty, "erro node vty->node=%d\n ", vty->node);
//          free(hostname);
            return;
        }

        if((vty->node ==    INTERFACE_NODE)
                || (vty->node == LOOPBACK_IF_NODE)
                || (vty->node == VLANIF_NODE)
                || (vty->node == TRUNK_IF_NODE)
                || (vty->node == TRUNK_SUBIF_NODE)
                || (vty->node == TUNNEL_IF_NODE)
                || (vty->node == TDM_IF_NODE)
                || (vty->node == TDM_SUBIF_NODE)
                || (vty->node == STM_IF_NODE)
                || (vty->node == STM_SUBIF_NODE)
                || (vty->node == PHYSICAL_IF_NODE)
                || (vty->node == PHYSICAL_SUBIF_NODE)
                || (vty->node == CLOCK_NODE)
                || (vty->node == VCG_NODE)	
                || (vty->node == E1_IF_NODE)
                || (vty->node == QOS_DOMAIN_NODE)
                || (vty->node == QOS_PHB_NODE)
                || (vty->node == PW_NODE)
                || (vty->node == L3VPN_NODE)
                || (vty->node == VSI_NODE)
                || (vty->node == MPLSTP_OAM_SESS_NODE)
                || (vty->node == MPLSTP_OAM_MEG_NODE)
                || (vty->node == TRUNK_GLOABLE_NODE)
                || (vty->node == VLAN_NODE)
                || (vty->node == IGMP_NODE)
                || (vty->node == CFM_SESSION_NODE)
                || (vty->node == CFM_MD_NODE)
                || (vty->node == CFM_MA_NODE)
		  		|| (vty->node == PW_CLASS_NODE)
	    	    || (vty->node == XCONNECT_GROUP_NODE))
        {
            vty_out(vty, vty->change_prompt, hostname);
        }
        else
        {
            vty_out(vty, cmd_prompt(vty->node), hostname);
        }
    }

//  if(!hostname)
//      free(hostname);
}

void *
vtysh_select_all_fd(void *arg)
{
    fd_set readfd_set;
//  fd_set writefd_set;
    struct vty *vty;
    struct timeval timer_val = { .tv_sec = 0, .tv_usec = 100 };
    int num = 0;
    char *pbuf;
    size_t bufsz;
    char *buf;
    int readln;
    int nbytes;
    int err;

    prctl(PR_SET_NAME, " vtysh_select_all_fd");

    if((err = pthread_sigmask(SIG_BLOCK, &sig_mask, NULL)) != 0)
    {
        perror("pthread_sigmask error:");
        exit(1);
    }

    while(1)
    {
        usleep(100000);
        FD_ZERO(&readfd_set);

        for(unsigned int i = 0; i < vtyvec->active; i++)
        {
            if((vty = vector_lookup(vtyvec, i)) == NULL
                    || vty->type == VTY_SNMP
                    || vty->type == VTY_OFP
                    || vty->type == VTY_OSPF
                    || vty->type == VTY_SYNC)
            {
                continue;
            }

            if(vty->cmd_process_lock == 1)
            {
                continue;
            }

            readfd_set = vty->read_app_fd;
#if 0

            for(int i = 0; i < 100; i++)
            {
//              FD_SET(i, &readfd_set);
                if(FD_ISSET(i, &readfd_set))
                {
                    printf("fd:%d is set\n", i);
                }
            }

#endif

            num = select(FD_SETSIZE, &readfd_set, NULL, NULL, &timer_val);

            /* Signals should get quick treatment */
            if(num < 0)
            {
                if(errno == EINTR)
                {
                    continue;    /* signal received - process it */
                }

                zlog_warn("select() error: %s", safe_strerror(errno));
                printf("select() error: %s", safe_strerror(errno));
                return NULL;
            }

            if(num > 0)
            {
                for(int i = 0; i < DAEMON_MAX; i++)
                {
                    if(vty->connect_fd[i] == -1)
                    {
                        continue;
                    }

                    if(FD_ISSET(vty->connect_fd[i], &readfd_set))
                    {
                        /* Allow enough room for buffer to read more than a few pages from socket. */
                        bufsz = 5 * getpagesize() + 1;
                        buf = XMALLOC(MTYPE_TMP, bufsz);
                        memset(buf, 0, bufsz);
                        pbuf = buf;

                        if(pbuf >= ((buf + bufsz) - 1))
                        {
                            fprintf(stderr, "vtysh:vtysh_select_all_fd:" \
                                    "warning - pbuf beyond buffer end.\n");
                            return NULL;
                        }

                        readln = (buf + bufsz) - pbuf - 1;

                        nbytes = read(vty->connect_fd[i], pbuf, readln);

                        if(nbytes <= 0)
                        {
                            if(errno == EINTR)
                            {
                                continue;
                            }

                            fprintf(stderr, "vtysh:vtysh_select_all_fd:" "(%u)", errno);
                            perror("");

                            if(errno == EAGAIN || errno == EIO)
                            {
                                continue;
                            }

                            FD_CLR(vty->connect_fd[i], &vty->read_app_fd);

                            XFREE(MTYPE_TMP, buf);
                            break;
                        }

                        vty_out(vty, "%s", pbuf);

                        XFREE(MTYPE_TMP, buf);
                    }
                }
            }
        }
    }
}


int
vtysh_execute_sigint(struct vty *vty)
{
    int cmd_stat;
    char line[] = "\r\n";

    for(unsigned int i = 0; i < array_size(vtysh_client); i++)
    {
        cmd_stat = vtysh_client_execute(&vtysh_client[i], line, stdout, vty, i);

        if(cmd_stat == CMD_WARNING)
        {
            break;
        }
    }

    return cmd_stat;
}

void *vty_cmd_msg_rcv_snmp(void *arg)
{
    prctl(PR_SET_NAME, " vty_cmd_snmp");

    //vty_cmd_msg_rcv(arg);
    vty_snmp_cmd_msg_rcv(arg);
    
  //  vty_cmd_msg_rcv_execute(arg);
	
    return NULL;
}

void *vty_cmd_msg_rcv_ofp(void *arg)
{
    prctl(PR_SET_NAME, " vty_cmd_ofp");

    vty_cmd_msg_rcv_execute(arg);
    return NULL;
}

void *vty_cmd_msg_rcv_ospf(void *arg)
{
    prctl(PR_SET_NAME, " vty_cmd_ospf");

    vty_cmd_msg_rcv_execute(arg);
    return NULL;
}



void *
vty_snmp_cmd_msg_rcv(void *arg)
{
    int return_no, ret;
    struct login_session *session = NULL;
    struct vty *vty = NULL;
    struct buffer_data *data;
    struct buffer *buffer;
    unsigned char *cmd;
    int module_id;

	//char *delim = "\r\n";
    char *delim = " ";
	char *pbuf = NULL;

    if(pthread_sigmask(SIG_BLOCK, &sig_mask, NULL) != 0)
    {
        perror("pthread_sigmask error:");
        exit(1);
    }

    session = (struct login_session *)arg;
    vty = session->vty;
    buffer = session->buf_ipc_recv;

	if(SESSION_SNMP != session->session_type)
    {
		//ret = ipc_send_noack(return_no, MODULE_ID_SNMPD, MODULE_ID_VTY, IPC_TYPE_VTY, VTY_MSG_CMD, 0);
        return NULL;
    }

	module_id = MODULE_ID_SNMPD;

    while(1)
    {

        if(session->bufdata_cnt == 0 && session->read_bufdata == NULL)
        {
			usleep(100000);
            continue;
        }

		while(session->read_bufdata)
        {
            if(session->recv_copy_busy && \
                    session->read_bufdata == session->recv_bufdata)
            {
                continue;
            }

            data = session->read_bufdata;
            pbuf = data->data;
			while(1)
			{
				cmd = strtok(pbuf, delim);

				if(NULL == cmd)
				{
					break;
				}
			
				//            VTYSH_CMD_EXEC_LOCK;
				return_no = vtysh_execute((char *)cmd, vty);
				//            VTYSH_CMD_EXEC_UNLOCK;
				
				if(return_no)
				{
					/*ret = ipc_send_noack(return_no, module_id, MODULE_ID_VTY,
						IPC_TYPE_VTY, VTY_MSG_CMD, 0);*/
					
					ret = ipc_send_reply_n2(&return_no, sizeof(return_no), 1, module_id, MODULE_ID_VTY,
						IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0, IPC_OPCODE_REPLY);
					break;
				}

				pbuf = NULL;
			}

			//ret = ipc_send_noack(return_no, module_id, MODULE_ID_VTY, IPC_TYPE_VTY, VTY_MSG_CMD, 0);

			ret = ipc_send_reply_n2(&return_no, sizeof(return_no), 1, module_id, MODULE_ID_VTY,
				IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0, IPC_OPCODE_REPLY);

            if(ret < 0)
            {
                zlog_err("ipc send to snmp error\n");
            }

            pthread_mutex_lock(&session->bufdata_delete_lock);

            if(data->next)
            {
                session->read_bufdata = session->read_bufdata->next;
            }
            else
            {
                session->read_bufdata = session->recv_bufdata = NULL;
            }

            buffer_data_delete(buffer, data);
            session->bufdata_cnt--;

            pthread_mutex_unlock(&session->bufdata_delete_lock);
        }
    }
}


void *vty_cmd_msg_rcv_execute(void *arg)
{
    int return_no, ret;
    struct login_session *session = NULL;
    struct vty *vty = NULL;
    struct buffer_data *data;
    struct buffer *buffer;
    unsigned char *cmd;
    int module_id;

    if(pthread_sigmask(SIG_BLOCK, &sig_mask, NULL) != 0)
    {
        perror("pthread_sigmask error:");
        exit(1);
    }

    session = (struct login_session *)arg;
    vty = session->vty;
    buffer = session->buf_ipc_recv;

    switch(session->session_type)
    {
        case SESSION_SNMP:
            module_id = MODULE_ID_SNMPD;
            break;

        case SESSION_OFP:
            module_id = MODULE_ID_OPENFLOW;
            break;

        case SESSION_OSPF:
            module_id = MODULE_ID_OSPF;
            break;

        default:
            zlog_err("vty_cmd_msg_rcv wrong session type:%d\n", session->session_type);
            return NULL;
    }

    while(1)
    {
        usleep(100000);

        if(session->bufdata_cnt == 0 && session->read_bufdata == NULL)
        {
            continue;
        }

        while(session->read_bufdata)
        {
            if(session->recv_copy_busy && \
                    session->read_bufdata == session->recv_bufdata)
            {
                continue;
            }

            data = session->read_bufdata;
            cmd = data->data;

            return_no = vtysh_execute((char *)cmd, vty);
            if(module_id == MODULE_ID_SNMPD)
            {
            if(return_no == 0)
            {
//                ret = ipc_send_ack(module_id, MODULE_ID_VTY,
                ipc_send_reply_n2(NULL, 0, 1, 
                     module_id, MODULE_ID_VTY, IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0, IPC_OPCODE_ACK);                                   
            }                       
            else
            {
//              ret = ipc_send_noack(return_no, module_id, MODULE_ID_VTY,

                ipc_send_reply_n2(&return_no, sizeof(int), 1, 
                     module_id, MODULE_ID_VTY, IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0, IPC_OPCODE_NACK);                                      
            }
                if(return_no < 0)
            {
                    zlog_err("ipc send to %d error\n", module_id);
                }
            }

            pthread_mutex_lock(&session->bufdata_delete_lock);

            if(data->next)
            {
                session->read_bufdata = session->read_bufdata->next;
            }
            else
            {
                session->read_bufdata = session->recv_bufdata = NULL;
            }

            buffer_data_delete(buffer, data);
            session->bufdata_cnt--;

            pthread_mutex_unlock(&session->bufdata_delete_lock);
        }
    }
}



#if 0
void *
vty_ofp_msg_rcv(void *arg)
{
    int return_no, ret, err;
    struct login_session *session = NULL;
    struct vty *vty = NULL;
    struct buffer_data *data, *obuf_data;
    struct buffer *buffer, *obuf;
    unsigned char cmd[512];
    int nbytes;
    char *buf, *begin;
    int i, j;
    struct ipc_mesg mesg;
    struct ipc_msghdr *phdr = NULL;
    struct send_type send_type[10];
    int send_index = 0;
    int data_buf_num;
    u_char header[4] = {0, 0, 0, 0};

    if((err = pthread_sigmask(SIG_BLOCK, &sig_mask, NULL)) != 0)
    {
        perror("pthread_sigmask error:");
        exit(1);
    }

    session = (struct login_session *)arg;
    vty = session->vty;
    buffer = session->buf_ipc_recv;

    while(1)
    {
        usleep(1000);

        if(session->bufdata_cnt == 0 && session->read_bufdata == NULL)
        {
            continue;
        }

        while(session->read_bufdata)
        {

            if(session->recv_copy_busy && \
                    session->read_bufdata == session->recv_bufdata)
            {
                continue;
            }

            data = session->read_bufdata;
            nbytes = data->cp;
            begin = buf = data->data;

            for(i = 0; i < nbytes; i++)
            {
                if(*buf == '\r')
                {
                    *buf = '\0';
                    buf++;
                    /* get cmd*/
                    memcpy(cmd, begin, buf - begin);
                    zlog_debug("##VTY_OFP:cmd:%s\n", cmd);

                    /*execute cmd*/
                    return_no = vtysh_execute(cmd, vty);

                    header[3] = return_no;
                    buffer_put(vty->obuf, header, 4);

                    /* get return value and data */
                    for(obuf_data = vty->obuf->head; obuf_data; obuf_data = obuf_data->next)
                    {
                        size_t cp;
                        int send_buf_num = (obuf_data->cp - obuf_data->sp) / IPC_MSG_LEN + 1;
                        cp = obuf_data->sp;

                        for(j = 0; j < send_buf_num - 1; j++)
                        {
                            send_type[send_index].send_buf = (char *)(obuf_data->data + obuf_data->sp);

                            send_type[send_index].send_len = IPC_MSG_LEN;
                            send_index++;
                            obuf_data->sp += IPC_MSG_LEN;
                        }

                        if(obuf_data->sp < obuf_data->cp)
                        {
                            send_type[send_index].send_buf = (char *)(obuf_data->data + obuf_data->sp);
                            send_type[send_index].send_len = obuf_data->cp - obuf_data->sp;
                            send_index++;
                            obuf_data->sp = obuf_data->cp;
                        }

                        data_buf_num++;
                    }

                    /*replay*/
                    for(int i = 0; i < send_index; i++)
                    {
                        ipc_send_common(send_type[i].send_buf, send_type[i].send_len, 1, MODULE_ID_OPENFLOW,
                                        MODULE_ID_VTY, IPC_TYPE_OFP, VTY_MSG_OFP, 0);
                    }

                    send_index = 0;

                    /*clean vty->obuf*/
                    for(int i = 0; i < data_buf_num; i++)
                    {
                        struct buffer_data *data;

                        if(!(data = vty->obuf->head))
                        {
                            zlog_err("buffer queue empty, ");
                            break;
                        }

                        if(!(vty->obuf->head = data->next))
                        {
                            vty->obuf->tail = NULL;
                        }

                        XFREE(MTYPE_BUFFER, data);
                    }

                    /*continue get next cmd*/
                    begin = buf;
                }
                else
                {
//                  printf("buf[i]:%c\n", buf[i]);
                    buf++;
                }
            }

            /*clean recv buf*/
            pthread_mutex_lock(&session->bufdata_delete_lock);

            if(data->next)
            {
                zlog_debug("####VTY_OFP:we have something in next\n");
                session->read_bufdata = session->read_bufdata->next;
            }
            else
            {
                zlog_debug("####VTY_OFP:read up all bufdata\n");
                session->read_bufdata = session->recv_bufdata = NULL;
            }

            buffer_data_delete(buffer, data);
            session->bufdata_cnt--;
            zlog_debug("##VTY_OFP:after buffer_data_delete bufdata_cnt:%d\n", session->bufdata_cnt);

            pthread_mutex_unlock(&session->bufdata_delete_lock);
        }
    }
}
#endif

struct vty *
vty_console_create(struct login_session *session)
{
    struct vty *vty;
    /* Make vty structure. */
    vty = vty_new();
    vty->type = VTY_SHELL;
    VTY_DEBUG(VD_CMD, "node: %d", vty->node);
//    vty->node = AUTH_NODE;
    vty->v_timeout = USER_IDLE_COUNTER;
    strcpy(vty->ttyname, session->address);
	strcpy(vty->hostname, session->device_name);

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    vty->length = 0;
    memset(vty->hist, 0, sizeof(vty->hist));
    vty->hp = 0;
    vty->hindex = 0;
    vty->session = session;
    /* Initialize commands. */
    cmd_init(0);
#if 0
    pthread_mutex_lock(&session_timer_lock);

    if(vty->v_timeout)
    {
        //vty->t_timeout = thread_add_timer(vtysh_master, vty_timeout, vty, vty->v_timeout);
    }

    pthread_mutex_unlock(&session_timer_lock);
#endif

    if(pthread_mutex_init(&vty->catch_buf_lock, NULL) != 0)
    {
        perror("snmp catch_buf_lock init failed\n");
        exit(1);
    }

    vector_set(vtyvec, vty);

    return vty;
}

struct vty *
vty_snmp_creat(struct login_session *session)
{
    struct vty *vty;
    char ttyname[] = "vty_snmp";

    vty = vty_new();
    vty->type = VTY_SNMP;
    vty->node = CONFIG_NODE;
    memset(vty->ttyname, 0, 20);
    strcpy(vty->ttyname, ttyname);

    vty->length = 0;
    vty->session = session;
    vector_set(vtyvec, vty);

    if(pthread_mutex_init(&vty->catch_buf_lock, NULL) != 0)
    {
        perror("snmp catch_buf_lock init failed\n");
        exit(1);
    }

    return vty;
}

struct vty *
vty_ospf_creat(struct login_session *session)
{
    struct vty *vty;
    char ttyname[] = "vty_ospf";

    vty = vty_new();
    vty->type = VTY_OSPF;
    vty->node = CONFIG_NODE;

    memset(vty->ttyname, 0, 20);
    strcpy(vty->ttyname, ttyname);

    vty->length = 0;
    vty->session = session;

    if(pthread_mutex_init(&vty->catch_buf_lock, NULL) != 0)
    {
        perror("ospf catch_buf_lock init failed\n");
        exit(1);
    }

    return vty;

}


struct vty *
vty_ofp_creat(struct login_session *session)
{
    struct vty *vty;
    char ttyname[] = "vty_ofp";

    vty = vty_new();
    vty->type = VTY_OFP;
    vty->node = CONFIG_NODE;

    memset(vty->ttyname, 0, 20);
    strcpy(vty->ttyname, ttyname);

    vty->length = 0;
    vty->session = session;

    if(pthread_mutex_init(&vty->catch_buf_lock, NULL) != 0)
    {
        perror("snmp catch_buf_lock init failed\n");
        exit(1);
    }

    return vty;

}

struct vty *
vty_sync_creat(struct login_session *session)
{
    struct vty *vty;

    vty = vty_new();
    vty->type = VTY_SYNC;
    vty->node = CONFIG_NODE;

    vty->length = 0;
    vty->session = session;
    vector_set(vtyvec_sync, vty);

    return vty;
}

void vty_cmd_msg_thread_creat()
{
    vty_ospf_session_creat();

    vty_snmp_session_creat();

    vty_ofp_session_creat();

}

static int vtysh_copy_startup_config(FILE *fp)
{
    int flen = 0;
    FILE *stfp = NULL;
    int cnt = 0;
    int *buftmp = NULL;

    if(!fp)
    {
        return -1;
    }

    stfp = fopen(VTYSH_STARTUP_CONFIG, "r");

    if(stfp == NULL)
    {
        zlog_err("%s[%d]:startup.conf not exit !\n", __FUNCTION__, __LINE__);
        return -1;
    }

    fseek(stfp, 0, SEEK_END);
    flen = ftell(stfp);

    buftmp = XMALLOC(MTYPE_VTY, flen);

    if(buftmp == NULL)
    {
        fclose(stfp);
        return -1;
    }

    zlog_debug(VD_COMM, "%s[%d]:size %d\n", __FUNCTION__, __LINE__, flen);

    fseek(stfp, 0, SEEK_SET);
    cnt = fread(buftmp, sizeof(char), flen, stfp);
    zlog_debug(VD_COMM, "%s[%d]:frd size %d\n", __FUNCTION__, __LINE__, cnt);

    cnt = fwrite(buftmp, sizeof(char), cnt, fp);
    zlog_debug(VD_COMM, "%s[%d]:fwr size %d\n", __FUNCTION__, __LINE__, cnt);

    XFREE(MTYPE_VTY, buftmp);
    fclose(stfp);

    return 0;
}

int vtysh_creat_batch_config_file()
{
    u_int i;
    char line[] = "write terminal\n";
    FILE *fp;
    char config_tmp[] = HA_CONFIG_BATCHSYNC_FILE;
    struct vty *vty;
    zlog_debug(VD_COMM, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

    vty =  vty_batch_create();

    /* if config.tmp exist, remove it */
    if(access(config_tmp, F_OK))
    {
        unlink(config_tmp);
    }

    fp = fopen(config_tmp, "w");

    if(fp == NULL)
    {
        zlog_err("Can't open configuration file %s.\n",
                 config_tmp);
        zlog_debug(VD_COMM, "Can't open configuration file %s.\n",
                   config_tmp);
        return -1;
    }

    vty->cmd_process_lock = 1;

    for(i = 0; i < array_size(vtysh_client); i++)
    {
        if(vtysh_client[i].flag == VTYSH_HAL)
        {
            continue;
        }

        vtysh_client_execute(&vtysh_client[i], line, NULL, vty, i);
    }

    vty->cmd_process_lock = 0;

    vtysh_config_write();

    if(g_read_config_err_flag == 0)
    {
        vtysh_config_dump(fp);
    }
    else
    {
        vtysh_copy_startup_config(fp);
    }

    sync();

    fclose(fp);

    vty_batch_delete(vty);

    if(chmod(config_tmp, CONFIGFILE_MASK) != 0)
    {
        zlog_err("%% Can't chmod configuration file %s: %s (%d)\r\n",
                 config_tmp, safe_strerror(errno), errno);
        return -1;
    }

    return 0;

}

void vtysh_close_all_connection()
{
    unsigned int i;
    struct vty *v;
    struct login_session *s;
    zlog_debug(VD_COMM, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

    /*close all term vty connection*/
    for(i = 0; i < vector_active(vtyvec); i++)
    {
        if((v = vector_slot(vtyvec, i)) != NULL
                && (v->type == VTY_TERM || v->type == VTY_SSH || v->type == VTY_SHELL))
        {
            if(vty_shell(v))
            {
                s = (struct login_session *)v->session;
                
                if(s->user_name != NULL)/* console已经登录*/
                {
                    /*user logout, tell aaa*/
                    vty_remote_aaa_logout(s);

                    vty_consol_close(s);
                }

            }
            else
            {
                s = (struct login_session *)v->session;
                s->session_status = SESSION_CLOSE;
            }

            zlog_debug(VD_COMM, "close connect:%s\n", s->address);
        }
    }

}

int vtysh_timer_handle(void *para)
{
    int ret = -1;
    ret = vtysh_session_delete_check();

    vtysh_check_sync_vty_close();

    return ret;
}


static uint16_t g_vtysh_vpn = 0;

void vtysh_vpn_set(uint16_t vpn)
{
	g_vtysh_vpn = vpn;
}


uint16_t vtysh_vpn_get(void)
{
	return g_vtysh_vpn;
}


//U0 info list, including IP and hostname
struct list g_u0_list = 
{
	.head = NULL,
	.tail = NULL,
	.cmp  = NULL,
	.del  = NULL,
	.count = 0
};


//add u0 info 
int vtysh_u0_list_add(int index, uint32_t ipv4, char *hostname,uint32_t ifindex)
{
	if((0 == ipv4) || (NULL == hostname) || (0 == strlen(hostname)))
	{
//		printf("%s[%d] : invalid IP(%x) or hostname\n", __func__, __LINE__, ipv4);
		return 0;
	}

	U0_INFO *pinfo_add = (U0_INFO *)XMALLOC(MTYPE_VTY, sizeof(U0_INFO));

	if(NULL == pinfo_add)
	{
		return 0;
	}

	memset(pinfo_add, 0, sizeof(U0_INFO));
	pinfo_add->index = index;
	pinfo_add->ipv4 = ipv4;
	pinfo_add->ifindex = ifindex;
	memcpy(pinfo_add->hostname, hostname, strlen(hostname));

	zlog_debug("%s[%d] : add u0(%x) success, hostname(%s) ifindex : %x\n", __func__, __LINE__, pinfo_add->ipv4, pinfo_add->hostname,pinfo_add->ifindex);
	
	listnode_add(&g_u0_list, (void *)pinfo_add);

	return 1;
}


//get u0 info by ip
U0_INFO *vtysh_u0_list_get_info_by_ip(uint32_t ipv4)
{
	U0_INFO * p_u0_info;
	struct listnode *node, *nnode;

	for (ALL_LIST_ELEMENTS(&g_u0_list, node,nnode, p_u0_info))
	{
		if (ipv4 == p_u0_info->ipv4)
		{
			return p_u0_info;
		}
	}
	
	return NULL;
}


//modify u0 information
int vtysh_u0_list_hostname_modify(struct vty *vty, char *hostname)
{
	struct u0_device_info info;
	U0_INFO *pinfo = vtysh_u0_list_get_info_by_ip(vty->server_ip);

	if(NULL == pinfo)
	{		
		return 0;
	}

	strcpy(pinfo->hostname, hostname);

	memset(&info, 0, sizeof(struct u0_device_info));
	info.ne_ip.s_addr = htonl(pinfo->ipv4);
	info.ifindex = pinfo->ifindex;
	strcpy(info.hostname,hostname);

	ipc_send_msg_n2((void *)&info, sizeof(struct u0_device_info), 1, MODULE_ID_L2, MODULE_ID_VTY,
		IPC_TYPE_EFM, 0, IPC_OPCODE_CHANGE,0);
	
	zlog_debug("%s[%d] : modify u0(%x) success, hostname(%s) ifindex : %x\n", __func__, __LINE__, pinfo->ipv4, pinfo->hostname, pinfo->ifindex);

	return 1;
}
U0_INFO * vtysh_u0_list_get_info_by_index(int index)
{
	struct listnode *node, *nnode;
	U0_INFO * p_u0_info;

	for (ALL_LIST_ELEMENTS(&g_u0_list, node,nnode, p_u0_info))
	{
		if(p_u0_info->index == index)
		{
			return p_u0_info;
		}

	}
	return NULL;
}
/*
判断该session是否为某个u0会话
如果是返回1，如果不是返回0
*/
int is_vtysh_u0_session(struct login_session *session)
{	
	struct listnode *u0_node_u0, *u0_nnode;
	struct listnode *ses_node, *ses_nnode;
	U0_INFO * p_u0_info;
	struct login_session *p_session;
	for (ALL_LIST_ELEMENTS(&g_u0_list, u0_node_u0, u0_nnode,p_u0_info))
	{
		for (ALL_LIST_ELEMENTS(&p_u0_info->u0_ses_lst, ses_node,ses_nnode, p_session))
		{
			if(session == p_session)
			{
				return 1;
			}

		}

	}

	return 0;
	
}

int vtysh_u0_ses_add(U0_INFO * p_u0_info, struct login_session *session)
{	
	if(NULL == p_u0_info  || NULL == session )
	{
		return 0;
	}
	
//	printf("%s %d session : %p, session->server_ip : %x,session->server_port %d,session->client_ip : %x,session->client_port : %d\n",__FUNCTION__,__LINE__,session,session->server_ip,session->server_port,session->client_ip,session->client_port);
	listnode_add(&p_u0_info->u0_ses_lst, (void *)session);
	return 0;
}

int vtysh_u0_ses_del(U0_INFO * p_u0_info,struct login_session *session )
{
	if(NULL ==p_u0_info || NULL == session )
	{
		return 0;
	}
	
	listnode_delete(&p_u0_info->u0_ses_lst, (void *)session);

	return 0;
}

int vtysh_u0_all_sess_del(U0_INFO * p_u0_info)
{
	struct login_session *p_session;
	struct listnode *node, *nnode;
	if(NULL ==p_u0_info )
	{
		return 0;
	}

	for (ALL_LIST_ELEMENTS(&p_u0_info->u0_ses_lst, node, nnode, p_session))
	{
//		printf("%s %d session : %p, session->server_ip : %x,session->server_port %d,session->client_ip : %x,session->client_port : %d\n",__FUNCTION__,__LINE__,p_session,p_session->server_ip,p_session->server_port,p_session->client_ip,p_session->client_port);
		listnode_delete(&p_u0_info->u0_ses_lst, (void *)p_session);
	}
	

	return 0;
}

int vtysh_u0_all_sess_show(U0_INFO * p_u0_info)
{
	struct login_session *p_session;
	struct listnode *node, *nnode;
	if(NULL ==p_u0_info )
	{
		return 0;
	}

	for (ALL_LIST_ELEMENTS(&p_u0_info->u0_ses_lst, node, nnode, p_session))
	{
		printf("%s %d session : %p, session->server_ip : %x,session->server_port %d,session->client_ip : %x,session->client_port : %d\n",__FUNCTION__,__LINE__,p_session,p_session->server_ip,p_session->server_port,p_session->client_ip,p_session->client_port);
	}
	
	return 0;
}

int vtysh_u0_all_sess_close(U0_INFO * p_u0_info)
{
	struct login_session *p_session;
	struct listnode *node, *nnode;
    pthread_mutex_lock(&session_delete_lock);

	for (ALL_LIST_ELEMENTS(&p_u0_info->u0_ses_lst, node, nnode, p_session))
	{
//		printf("%s %d session : %p, session->server_ip : %x,session->server_port %d,session->client_ip : %x,session->client_port : %d\n",__FUNCTION__,__LINE__,p_session,p_session->server_ip,p_session->server_port,p_session->client_ip,p_session->client_port);
		p_session->session_status = SESSION_CLOSE;
	}
	
    pthread_mutex_unlock(&session_delete_lock);

	return 0;
}

int vtysh_u0_list_del(int      index)
{
	U0_INFO * p_u0_info;
	struct listnode *node, *nnode;

	for(ALL_LIST_ELEMENTS(&g_u0_list, node, nnode, p_u0_info))
	{
		if(p_u0_info->index == index)
		{
			listnode_delete(&g_u0_list, p_u0_info);
			
			XFREE(MTYPE_VTY, p_u0_info);
			
			return 1;
		}

	}

	return 0;
}

int vtysh_u0_list_ip_set(int       index, int ip)
{
	U0_INFO * p_u0_info;
	struct listnode *node, *nnode;

	for(ALL_LIST_ELEMENTS(&g_u0_list, node, nnode, p_u0_info))
	{
		if(p_u0_info->index == index)
		{
			p_u0_info->ipv4 = ip;
			
			break;
		}

	}

	return 0;
}

//show u0 information
void vtysh_u0_info_show(struct vty *vty)
{
	struct listnode *node = NULL;

	U0_INFO *p_u0_info = NULL;

	char str[200] ;

	memset(str,0,sizeof(str));

	vty_out(vty, "------------------------------- U0 infomation ---------------------------------%s", VTY_NEWLINE);
	
	for (node = listhead(&g_u0_list); node; node = listnextnode(node))
	{
		p_u0_info = (U0_INFO *)node->data;

		sprintf(str, "index:%x  ip:%u.%u.%u.%u     hostname:%s ifindex : %x",p_u0_info->index, ((p_u0_info->ipv4 >> 24) & 0xff), ((p_u0_info->ipv4 >> 16) & 0xff),
			((p_u0_info->ipv4 >> 8) & 0xff), ((p_u0_info->ipv4 >> 0) & 0xff),p_u0_info->hostname,p_u0_info->ifindex);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);


	}

	vty_out(vty, "--------------------------------------------------------------------------------%s", VTY_NEWLINE);
}


//handle ospf ipc msg
void vtysh_handle_ospf_msg(struct ipc_mesg_n *pmsg)
{
	U0_INFO * p_u0_info; 
	if(NULL == pmsg)
	{
		return;
	}
	  
	struct ipc_msghdr_n *phdr = (struct ipc_msghdr_n *)&pmsg->msghdr;

	struct u0_device_info *pinfo = NULL;

	if(MODULE_ID_OSPF == phdr->sender_id)
	{
		switch(phdr->opcode)
		{
			case IPC_OPCODE_ADD:
				pinfo = (struct u0_device_info *)pmsg->msg_data;

				if(NULL == pinfo)
				{
					return;
				}
				zlog_debug("%s %d pinfo->ifindex : %x  pinfo->ne_ip.s_addr : %x pinfo->hostname : %s\n",__FUNCTION__,__LINE__,pinfo->ifindex,pinfo->ne_ip.s_addr, pinfo->hostname);

				vtysh_u0_list_add(pinfo->ifindex,ntohl(pinfo->ne_ip.s_addr), pinfo->hostname, pinfo->ifindex);
				
				break;

			case IPC_OPCODE_DELETE:
				pinfo = (struct u0_device_info *)pmsg->msg_data;

				if(NULL == pinfo)
				{
					return;
				}
				
				zlog_debug("%s %d pinfo->ifindex : %x  pinfo->ne_ip.s_addr : %x pinfo->hostname : %s\n",__FUNCTION__,__LINE__,pinfo->ifindex,pinfo->ne_ip.s_addr, pinfo->hostname);
				p_u0_info = vtysh_u0_list_get_info_by_index(pinfo->ifindex);
				if(p_u0_info)
				{
					vtysh_u0_all_sess_close(p_u0_info);
					vtysh_u0_all_sess_del(p_u0_info);
					vtysh_u0_list_del(pinfo->ifindex);

				}
				break;
				
			/*目前认为的u0更新，就是仅指更新ip地址*/	
			case IPC_OPCODE_UPDATE:
				
				pinfo = (struct u0_device_info *)pmsg->msg_data;
				if(NULL == pinfo)
				{
					return;
				}
				
				zlog_debug("%s %d pinfo->ifindex : %x  pinfo->ne_ip.s_addr : %x pinfo->hostname : %s\n",__FUNCTION__,__LINE__,pinfo->ifindex,pinfo->ne_ip.s_addr, pinfo->hostname);
				p_u0_info = vtysh_u0_list_get_info_by_index(pinfo->ifindex);
				/*
				当u0存在并且u0 ip与要更新的ip不同时，执行操作
				*/
				if(p_u0_info)
				{
					if(p_u0_info->ipv4 != ntohl(pinfo->ne_ip.s_addr))
					{
						vtysh_u0_all_sess_close(p_u0_info);
						vtysh_u0_all_sess_del(p_u0_info);
						vtysh_u0_list_ip_set(pinfo->ifindex, ntohl(pinfo->ne_ip.s_addr));
					}
				}
				break;

			default:
				break;
		}
	}
}


