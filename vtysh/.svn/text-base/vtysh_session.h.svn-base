#ifndef _VTYSH_SESSION_H
#define _VTYSH_SESSION_H
#include "vty.h"
#include "linklist.h"
#include <lib/aaa_common.h>
#include "sockunion.h"

#define HOST_ENABLE_PASSWORD "hios@123"
extern struct list *user_session_sync_list;

#define SESSION_DEV_NAME_LEN (VTYSH_HOST_NAME_LEN + 1)

enum SESS_STATUS_E
{
	SESSION_AUTH = 0, 
	SESSION_NORMAL, 
	SESSION_BUSY,
	SESSION_CLOSE, 
};

enum SESS_TYPE_E
{
    SESSION_TELNET = 0,
	SESSION_CONSOLE, 
	SESSION_SSH, 
	SESSION_SNMP,
	SESSION_OFP,
	SESSION_OSPF,
	SESSION_SYNC
};

#define CLIENT                1    
#define SERVER                0  

struct login_session
{
	uint32_t 		 		 client_ip;
	uint32_t 		 		 server_ip;
	uint16_t 				 client_port;
	uint16_t				 server_port;

	uint16_t vpn;


    char address[SU_ADDRSTRLEN];  /* What address is this vty comming from. */
	
	struct vty  		 *vty;
	
	enum SESS_TYPE_E   session_type;  /*type of this session*/
	enum SESS_STATUS_E session_status;

	char device_name[SESSION_DEV_NAME_LEN];

	/*buffer for read thread*/
	struct buffer *buf_ipc_recv;
	
	/* point to the buffer_data we copy to */
	struct buffer_data *recv_bufdata;
	
	/* point to the buffer_data we read from */
	struct buffer_data *read_bufdata;
	
	/* how many buffer_data we alloc now */
	int bufdata_cnt;
	
	/* tell read thread copy have not finished */
	int recv_copy_busy;
	int read_free_busy;
	
	/* lock the buffer while free a buffer_buf or the buffer*/
	pthread_mutex_t bufdata_delete_lock;

	/*used to save input username and password*/
	char name_buf[USER_NAME_MAX + 1];
	char password_buf[USER_PASSWORD_MAX + 1];
	int privilege;

	uint32_t user_id;
	uint32_t user_id_buf;
	char *user_name;
	struct host_users *user_login;

	pthread_mutex_t aaa_replay_lock;
	void *aaa_replay;

	/*auth node:whether we should prompt username or password?*/
	int auth_prompt;

	int timer_count;

	void *ssh_packet;

	/*thread id of read and write*/
	pthread_t tid_w;
	pthread_t tid_r;

	pthread_t tid_consol;

	int index;
	int tid_w_exit;
	int tid_r_exit;
	int tid_ssh_exit;
	int client;     
	struct telnet_client *tc;

	struct user_line *ul;
};
				 

extern void vty_session_init (void);
extern struct login_session *vty_telnet_session_creat(uint32_t sip, uint32_t sport, uint32_t server_ip);
extern struct login_session *vty_console_session_creat(void);
extern struct login_session *vty_snmp_session_creat(void);
extern struct login_session *vty_ssh_session_creat(uint32_t client_ip, uint32_t sport, uint32_t local_port, uint32_t server_ip);
extern struct login_session *vty_ospf_session_creat(void);

extern struct login_session *vty_session_new (void);
extern struct login_session *vty_telnet_session_get(uint32_t sip, uint32_t sport);
extern struct login_session *vty_console_session_get(void);
extern struct login_session *vty_snmp_session_get(void);
extern struct login_session *vty_ofp_session_creat(void);

//extern struct login_session *vty_session_get(int session_type);
extern void vty_session_delete(struct login_session *session);
extern struct login_session *vty_ofp_session_get();
extern struct login_session *vty_ospf_session_get();
extern struct login_session *vty_snmp_session_get();
extern struct login_session *vty_ssh_session_get(uint32_t sip, uint32_t sport);
extern struct login_session *vty_ospf_session_get(void);
extern struct login_session *vty_ofp_session_get(void);
extern struct login_session *vty_sync_session_creat(void);
extern void vtysh_slave_sync_session_delete(struct login_session *session);


extern struct list *user_session_list;
extern pthread_mutex_t session_delete_lock;

int vtysh_session_delete_check(void);
#endif

