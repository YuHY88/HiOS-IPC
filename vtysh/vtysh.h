/* Virtual terminal interface shell.
 * Copyright (C) 2000 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef VTYSH_H
#define VTYSH_H

#include "log.h"
#include "vtysh_session.h"
#include "errcode.h"
#include "vector.h"
#include <lib/command.h>
#include <lib/vty.h>
#include <lib/msg_ipc.h>
#include "msg_ipc_n.h"

extern pthread_mutex_t vtysh_exec_cmd_lock;
#define VTYSH_CMD_EXEC_LOCK        pthread_mutex_lock(&vtysh_exec_cmd_lock)
#define VTYSH_CMD_EXEC_UNLOCK      pthread_mutex_unlock(&vtysh_exec_cmd_lock) 
#define VTYSH_APP(num)   		(((long long)0x01) << (num))

#define VTYSH_IFM   			VTYSH_APP(0)
#define VTYSH_ROUTE 	        VTYSH_APP(1)
#define VTYSH_RIPD   		 	VTYSH_APP(2)
#define VTYSH_OSPFD 			VTYSH_APP(3)
#define VTYSH_BGPD  	        VTYSH_APP(4)
#define VTYSH_ISISD 	        VTYSH_APP(5)
#define VTYSH_SYSTEM   	 	    VTYSH_APP(6)
#define VTYSH_CES   			VTYSH_APP(7)
#define VTYSH_MPLS 		        VTYSH_APP(8)
#define VTYSH_DHCP 		        VTYSH_APP(9)
#define VTYSH_HAL   	        VTYSH_APP(10)
#define VTYSH_SYSLOG  	        VTYSH_APP(11)
#define VTYSH_DEVM  			VTYSH_APP(12)
#define VTYSH_FTM   	        VTYSH_APP(13)
#define VTYSH_FILEM 	        VTYSH_APP(14)
#define VTYSH_PING  	        VTYSH_APP(15)
#define VTYSH_NTP    	        VTYSH_APP(16)
#define VTYSH_L2     			VTYSH_APP(17)
#define VTYSH_AAA   			VTYSH_APP(18)
#define VTYSH_SNMPD  			VTYSH_APP(19)
#define VTYSH_ALARM				VTYSH_APP(20)
#define VTYSH_WEB  			    VTYSH_APP(21)
#define VTYSH_IPMC				VTYSH_APP(22)
#define VTYSH_CLOCK             VTYSH_APP(23)
#define VTYSH_OSPF6D 			VTYSH_APP(24)
#define VTYSH_DHCPV6			VTYSH_APP(25)
#define VTYSH_QOS   			VTYSH_APP(26)
#define VTYSH_RMON  			VTYSH_APP(27)
#define VTYSH_STATIS  			VTYSH_APP(28)
#define VTYSH_SDHMGT			VTYSH_APP(29)
#define VTYSH_SDHVCG            VTYSH_APP(30)


#define VTYSH_ALL	  VTYSH_RIPD|VTYSH_OSPFD|\
	VTYSH_BGPD|VTYSH_ISISD|VTYSH_QOS| \
	VTYSH_IFM|VTYSH_MPLS|VTYSH_DHCP|VTYSH_ROUTE|VTYSH_HAL|VTYSH_SYSLOG|VTYSH_DEVM|VTYSH_FTM|VTYSH_FILEM| \
	VTYSH_PING|VTYSH_NTP|VTYSH_L2|VTYSH_CES|VTYSH_AAA|VTYSH_SYSTEM|VTYSH_SNMPD|VTYSH_ALARM| \
	VTYSH_IPMC|VTYSH_CLOCK|VTYSH_OSPF6D|VTYSH_SDHVCG|VTYSH_RMON|VTYSH_STATIS

/*attention to l2 interface need defined in VTYSH_INTERFACE_L2,*/
/*attention to l3 interface need defined in VTYSH_INTERFACE_L3,*/
/*if attention to all interface need defined in VTYSH_INTERFACE_L2 and VTYSH_INTERFACE_L3*/

#define VTYSH_INTERFACE_CES  VTYSH_IFM|VTYSH_CES|VTYSH_CLOCK

#define VTYSH_INTERFACE_PORT VTYSH_IFM|VTYSH_L2

#define VTYSH_INTERFACE_L2	 VTYSH_IFM|VTYSH_MPLS|VTYSH_QOS

#define VTYSH_INTERFACE_L3	 VTYSH_IFM|VTYSH_DHCP|VTYSH_ISISD|VTYSH_OSPFD|VTYSH_RIPD| \
                                VTYSH_ROUTE|VTYSH_QOS|VTYSH_FTM|VTYSH_IPMC|VTYSH_OSPF6D

#define VTYSH_INTERFACE_VCG  VTYSH_SDHVCG|VTYSH_HAL
#define VTYSH_INTERFACE_E1   VTYSH_IFM|VTYSH_CLOCK

//#define VTYSH_HOST_NAME_LEN         64
#define VTYSH_IDLE_TIMEOUT_DEF      600


#define VTYSH_RESET_FROM_CURRENT			"current"
#define VTYSH_RESET_FROM_FACTORY			"factory"


/* Integrated configuration file. */
#define VTYSH_CFG_FILE_PATH					"/data/cfg/"

#define VTYSH_STARTUP_CONFIG				"/data/cfg/startup.conf"
#define VTYSH_STARTUP_CONFIG_BAK			"/data/cfg/startup.conf.bak"

#define VTYSH_STARTUP_ADDR_CONFIG			"/data/cfg/.startup_addr.conf"
#define VTYSH_STARTUP_ADDR_CONFIG_BAK		"/data/cfg/.startup_addr.conf.bak"

#define VTYSH_CURRENT_CONFIG				"/data/cfg/.current.conf"
#define VTYSH_CURRENT_CONFIG_BAK			"/data/cfg/.current.conf.bak"

#define VTYSH_CONFIG_FILE_TYPE 				".conf"


#define VTYSH_INIT_CONFIG					"/data/cfg/config.init"

#define VTYSH_H3C_CONFIG_FILE_TYPE ".cfg"
#define VTYSH_H3C_CONFIG_FILE_LEN   32



#define DEF_VTY_USER_NAME		"hios@root"
#define DEF_VTY_USER_PASSWORD	"root@123"


#define MAX_USERS_COUNT 10

#define MAX_PRIVILEGE_LEVEL 15

#define VTYSH_IPC_FAIL        ERRNO_IPC

#define CHECK_IDLE_TIME 5

#define USER_IDLE_COUNTER 0  // 10000000 // xiawl 10000000

extern int vty_slot;

enum config_nodes
{
	/*gloable config*/
	LOGIN_CONFIG,
	SYSLOG_CONFIG,
 	DEVM_CONFIG,
	
	WEB_CONFIG,
	OPENFLOW_CONFIG,
	ALARM_CONFIG,

 	NTP_CONFIG,
 	SDHDXC_CONFIG,
	SYSTEM_CONFIG,
	
	POOL_CONFIG,		  /* dhcp pool */
	/* qos ??? */
	QOS_DOMAIN_CONFIG,
	QOS_PHB_CONFIG,
	QOS_CAR_CONFIG,
	ACL_CONFIG,
	QOS_POLICY_CONFIG,
	QOS_CPCAR_CONFIG,
	HQOS_WRED_CONFIG,
	HQOS_QUEUE_CONFIG,

	VLAN_CONFIG,		  /*vlan node*/

	// CFM
	CFM_MD_CONFIG,
	CFM_MA_CONFIG,
	CFM_SESSION_CONFIG,

	MPLSTP_OAM_SESS_CONFIG, /*MPLSTP_OAM node*/
	BFD_SESSION_CONFIG, /*BFD node*/
	BFD_TEMPLATE_CONFIG,
	/*sla session???*/
	SLA_SESSION_CONFIG,

	ARP_CONFIG,		  /* arp connection node. */
	NDP_CONFIG,
	ROUTE_CONFIG, 	  /* Static ip route node. */

	VRF_CONFIG,		  /* VRF node. */
	RIP_CONFIG,		  /* RIP protocol mode node. */
	RIPNG_CONFIG,
	ISIS_CONFIG,		  /* ISIS protocol mode */
	OSPF_CONFIG,		  /* OSPF protocol mode */
	AREA_CONFIG,		  /* ospf area */
	//DCN_CONFIG,		/*move to end*/
	OSPF6_CONFIG,
	BGP_CONFIG,		  /* BGP protocol mode which includes BGP4+ */
	NEIGHBOR_CONFIG,

	/* mpls ??? */
	MPLS_CONFIG,		  /* Mpls node */
	L3VPN_CONFIG, 	  /* L3vpn node Under the Mpls node */
	VSI_CONFIG,		  /* Vsi node Under the Mpls node */
	LSP_CONFIG,		  /* Staitc-lsp node Under the Mpls node */
	PW_CONFIG,		  /* Pw node Under the Mpls node */

    PW_CLASS_CONFIG,
    SERVICE_INSTANCE_CONFIG,

    MAC_CONFIG,

	/* ????? */
	INTERFACE_CONFIG,		/* ????? */
	LOOPBACK_IF_CONFIG,
	LLDP_CONFIG,			/* lldp connection node */
	VLANIF_CONFIG,
	TRUNK_IF_CONFIG,
	TRUNK_SUBIF_CONFIG,
	QOS_MIRROR_CONFIG,
	TUNNEL_IF_CONFIG,
	TDM_IF_CONFIG,
	TDM_SUBIF_CONFIG,
	STM_IF_CONFIG,
	STM_SUBIF_CONFIG,
	PHYSICAL_IF_CONFIG,
	PHYSICAL_SUBIF_CONFIG,

	MPLS_APS_SESS_CONFIG, /*MPLS_APS node*/
	// ELPS
	ELPS_SESSION_CONFIG,
	ERPS_SESSION_CONFIG,
	CLOCK_CONFIG,
	DHCPV6_CONFIG,
	RMON_CONFIG,

    XCONNECT_GROUP_CONFIG,
    CONNECTION_CONFIG,
    PW_MASTER_CONFIG,
    PW_BACKUP_CONFIG,
	SYNCE_CONFIG,
	VCG_CONFIG,
	//fix by zzl
	DCN_CONFIG,

	SNMPD_CONFIG, 	  /* SNMPD node. */
	AAA_CONFIG,		  /* AAA node. */

	DEBUG_CONFIG,			/* Debug node. *//*debug command transfer to config*/
};

#define VTYSH_DEBUG_SET "/data/cfg/vtysh_debug"

extern int g_vtysh_debug;

#define VD_COMM     (0x1)
#define VD_MSG      (0x2)
#define VD_TELNET	(0x4)
#define VD_SYNC     (0x8)
#define VD_AUTH     (0x10)
#define VD_SSH      (0x20)
#define VD_CMD		(0x40)
#define VD_CONFIG	(0x80)
#define VD_CLIENT	(0x100)
#define VD_ALL  	(VD_COMM | VD_MSG | VD_TELNET | VD_CLIENT | VD_SYNC |VD_AUTH | VD_SSH | VD_CMD)
#define VD_ALL2  	(VD_CMD | VD_CONFIG | VD_TELNET | VD_CLIENT)

#if 0
#define VTY_DEBUG(m, fmt ,args...)	\
{\
	if(g_vtysh_debug & m)\
	{\
		printf( " [[%s-%d]: ", __FUNCTION__, __LINE__);\
		printf(fmt, ##args);\
		printf( " ]\n");\
	}\
}
#else
#define VTY_DEBUG(m, fmt ,args...) zlog_debug(m, fmt ,##args)
#endif


#define TELNET_SERVER_ENABLE		1
#define TELNET_SERVER_DISABLE		0

typedef enum 
{
	AUTH_MODE_NONE,
	AUTH_MODE_PASSWORD,
	AUTH_MODE_SCHEME
}AUTH_MODE_E;

typedef enum 
{
	USER_ROLE_NETWORK_ADMIN,
	USER_ROLE_NETWORK_OPERATOR
}USER_ROLE_E;

#define AUTH_MODE_NONE			0
#define AUTH_MODE_PASSWORD		1
#define AUTH_MODE_SCHEME		2

#define USER_LINE_CONSOLE		1
#define USER_LINE_VTY			0

#define USER_LINE_CONSOLE_NUM	1
#define USER_LINE_VTY_NUM		64
#define USER_LINE_NUM			(USER_LINE_CONSOLE_NUM + USER_LINE_VTY_NUM) 

#define USER_LINE_LOGIN_IN		1
#define USER_LINE_LOGIN_OUT		0

#define USER_LINE_IDLE_EN       1
#define USER_LINE_IDLE_DIS      0

struct user_line
{
	int index;
	int isconsole;
	int islogin;
	int auth_mode;
	int user_role;
	int idle_time_set;   // seconds 
	int idle_cnt_en;     // enable idle time poll
	int pwd_err;
	char password[8];
	int iscfg;
};

struct user_login_trap
{
	char user_name[32];
	char source[32];
	int fail_reason;
};



//u0 data struct
typedef struct _u0_info
{
	int      index;
	uint32_t ipv4;
	char 	 hostname[VTYSH_HOST_NAME_LEN + 1];
	uint32_t ifindex;
	struct list u0_ses_lst;
} U0_INFO;



/* app ????????????? */
extern void vtysh_init_mpls_cmd (void);
extern void vtysh_init_mpls_cmd_h3c (void);
extern void vtysh_init_devm_cmd (void);
extern void vtysh_init_l2_cmd(void);
extern void vtysh_init_lldp_cmd (void);
extern void vtysh_init_route_cmd (void);
extern void vtysh_init_arp_cmd (void);
extern void vtysh_init_ndp_cmd(void);
extern void vtysh_init_ospf_cmd (void);
extern void vtysh_init_ospf6_cmd (void);
extern void vtysh_init_ripd_cmd(void);
extern void vtysh_init_ripngd_cmd(void);
extern void vtysh_init_isis_cmd (void);
extern void vtysh_init_dhcp_cmd ();
extern void vtysh_init_ifm_cmd (void);
extern void vtysh_init_filem_cmd ();
extern void vtysh_init_ntp_cmd (void);
extern void vtysh_init_ipmc_cmd ();
extern void vtysh_init_hal_cmd();
extern void vtysh_init_ping_cmd ();
extern void vtysh_init_ces_cmd(void);
extern void vtysh_init_l2if_efm_cmd();
extern void vtysh_init_aaa_cmd(void);
extern void vtysh_init_statis_cmd(void);
extern void vtysh_init_system_cmd (void);
extern void vtysh_init_bgp_cmd(void);
extern void vtysh_init_clock_cmd (void);
extern void vtysh_init_synce_cmd (void);
extern void vtysh_init_snmpd_cmd(void);
extern void vtysh_init_rmond_cmd();
extern void vtysh_init_alarm_cmd();
extern void vtysh_init_filem_cmd(void);
extern void vtysh_init_web_cmd();
extern void vtysh_init_dhcpv6_cmd ();
extern void vtysh_init_ping_cmd (void);
extern void vtysh_init_ftm_cmd(void);
extern void vtysh_init_dhcp_cmd(void);
extern void vtysh_init_qos_cmd (void);
extern void vtysh_init_syslog_cmd (void);
extern void vtysh_init_syslog_cmd (void);
extern void vtysh_init_hal_cmd (void);
extern void vtysh_init_aaa_cmd (void);
extern void vtysh_init_web_cmd (void);
extern void vtysh_init_alarm_cmd (void);
extern void vtysh_init_snmpd_cmd (void);
extern void vtysh_init_ipmc_cmd (void);
extern void vtysh_init_dhcpv6_cmd (void);
extern void vtysh_init_qos_h3c_cmd ();
extern void vtysh_init_sdhmgt_cmd (void);
extern void vtysh_init_sdhvcg_cmd (void);



/* vtysh ???????? */
void vtysh_init_vty (void);
void vtysh_init_cmd (void);
int vtysh_connect_all (struct vty *);
void vtysh_readline_init (void);
void vtysh_user_init (void);
int vtysh_execute (const char *, struct vty *);
int vtysh_execute_no_pager (const char *, struct vty *);

extern char *vtysh_prompt (void);
extern void vty_prompt (struct vty *vty);;

void vtysh_config_write (void);

int vtysh_config_from_file (struct vty *, FILE *);

int vtysh_read_config (char *, char *);

void vtysh_config_parse (char *, long long );

void vtysh_config_dump (FILE *);

void vtysh_vty_config_dump (struct vty *);


void vtysh_config_init (void);

void vtysh_pager_init (void);

extern int vty_flush (struct thread *thread);
extern void vtysh_install_default (enum node_type node);
extern int vtysh_exit (struct vty *vty);
extern void insert_passwd(struct host_users *, struct vty *, int , const char*[]);

extern void *vty_cmd_msg_rcv(void *arg);
extern void *vty_snmp_cmd_msg_rcv(void *arg);

extern void *vtysh_select_all_fd(void *arg);

extern int vtysh_execute_sigint (struct vty *vty);
extern void vtysh_install_cmd(void);
extern void vtysh_init_default_cmd(void);
extern struct vty * vty_snmp_creat (struct login_session *session);
extern struct vty *vty_console_create (struct login_session *session);
extern int exec_timeout (struct vty *, const char *, const char *);
extern int vty_tcp_close(uint32_t dip, uint16_t dport, uint16_t sport);
extern void *vty_ofp_msg_rcv(void *arg);
extern void *thr_readline(void *arg);
extern int vty_auth (struct vty *vty, char *buf);
extern void vty_hist_add (struct vty *vty);
extern int vtysh_execute_func (const char *line, int pager, struct vty *vty);
extern void vtysh_vty_allif_config_dump (struct vty *vty, const char *buf);
extern int vtysh_config_exec_one_line(struct vty *vty, FILE *fp);
extern void vtysh_read_file (FILE *, char *);
extern int vty_remote_aaa_logout(struct login_session *session);
extern int vty_remote_aaa(struct login_session *session);
extern void *vtysh_get_slave_config(void *);
extern void vty_cmd_msg_thread_creat();
extern void *vty_cmd_msg_rcv_ospf(void *arg);
extern void *vty_cmd_msg_rcv_ofp(void *arg);
extern void *vty_cmd_msg_rcv_snmp(void *arg);
extern int vtysh_get_input(struct vty *vty);
extern void vtysh_add_timer(struct vty *vty);
extern struct vty *vty_ospf_creat (struct login_session *session);
extern struct vty *vty_ofp_creat (struct login_session *session);
extern void sub_config_list_delete(struct list *list);
extern void vtysh_vty_if_config_dump (struct vty *vty, char *buf);
extern void sub_config_list_delete(struct list *list);
extern int vtysh_config_from_file_interface (struct vty *vty, FILE *fp, char *ifname);
extern int vtysh_read_init(char *config_init_dir);
extern void vty_read_file (FILE *confp);
extern void vty_cmd_msg_thread_creat(void);
extern int vtysh_creat_batch_config_file(void);
extern struct vty *vty_sync_creat (struct login_session *session);
extern void vtysh_close_all_connection(void);
extern void vty_consol_close (struct login_session *session);
extern void vtysh_send_start_record_ha(void);



/* Exported variables */
extern char integrate_default[];

/* Child process execution flag. */
extern int execute_flag;

extern struct vty *vty;

extern sigset_t sig_mask;

extern volatile vector vtyvec;

extern struct thread_master *vtysh_master;

extern int config_update;

extern char config_default[];
extern char config_init[];

extern int reading_config_file;
extern int recv_slot_msg_flag;
extern int config_recover_finish;

extern pthread_mutex_t vtysh_config_lock;
extern pthread_mutex_t session_timer_lock;

void vtysh_debug_init(void);
void vtysh_user_line_init(void);
struct user_line *vtysh_user_line_get(int line, int line_index);
struct user_line *vtysh_idle_line_get(int line, int *index);
int vtysh_line_config_update(struct login_session *session);
int vtysh_user_login_trap(struct login_session *session, int login);
int vtysh_is_app_connect(const char *name);

int vtysh_timer_handle(void *para);

void vtysh_copy_file(char *file_src, char *file_dst);

void vtysh_vpn_set(uint16_t vpn);
uint16_t vtysh_vpn_get(void);

void *vty_cmd_msg_rcv_execute(void *arg);


//extern struct vty *vty_snmp;


extern int vtysh_u0_list_add(int index,uint32_t ipv4, char *hostname,uint32_t ifindex );
extern U0_INFO *vtysh_u0_list_get_info_by_ip(uint32_t ipv4);
extern int vtysh_u0_list_hostname_modify(struct vty *vty, char *hostname);
extern int vtysh_u0_list_del(int      index);
extern void vtysh_u0_info_show(struct vty *vty);

extern void vtysh_handle_ospf_msg(struct ipc_mesg_n *pmsg);
extern int vytsh_u3_vtys_hostname_refresh(char* hostname);
extern int vytsh_u0_vtys_hostname_refresh(U0_INFO * p_u0_info,char* hostname);
extern U0_INFO * vtysh_u0_list_get_info_by_index(int index);

extern int is_vtysh_u0_session(struct login_session *session);
extern int vtysh_u0_ses_add(U0_INFO * p_u0_info, struct login_session *session);
extern int vtysh_u0_ses_del(U0_INFO * p_u0_info,struct login_session *session );
extern int vtysh_u0_all_sess_del(U0_INFO * p_u0_info);
extern int vtysh_u0_all_sess_show(U0_INFO * p_u0_info);
extern int vtysh_u0_all_sess_close(U0_INFO * p_u0_info);
extern int vtysh_u0_list_del(int index);
extern int vtysh_u0_list_ip_set(int       index, int ip);



#endif /* VTYSH_H */
