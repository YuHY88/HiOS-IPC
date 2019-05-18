#include <zebra.h>
#include <lib/vty.h>
#include <lib/command.h>
#include "vtysh.h"
#include <lib/memory.h>
#include <lib/memtypes.h>
#include "vtysh_session.h"
#include "vtysh_user.h"
#include <lib/inet_ip.h>
#include <lib/buffer.h>
#include <pthread.h>
#include "vtysh_telnet.h"
#include "vtysh_msg.h"
#include "sshd/buffer_ssh.h"
#include "sshd/vtysh_sshd.h"
#include "sshd/xmalloc.h"
#include <lib/inet_ip.h>
#include "vtysh_sync.h"

struct list *user_session_list;
pthread_mutex_t session_delete_lock;
struct list *user_session_sync_list;

void
vty_session_init(void)
{
    user_session_list = list_new();
    pthread_mutex_init(&session_delete_lock, NULL);
}


struct login_session *
vty_telnet_session_creat(uint32_t client_ip, uint32_t sport, uint32_t server_ip)
{
    struct login_session *session;
    char address_tmp[50];
	struct user_line *ul = NULL;
	int index = 0;
	U0_INFO * p_u0_info;

    session = vty_session_new();

    session->client_ip = client_ip;
    session->client_port = sport;
	session->server_ip = server_ip;

    inet_ipv4tostr(client_ip, address_tmp);
    sprintf(session->address, "%s:%d", address_tmp, sport);

    session->buf_ipc_recv = buffer_new(VTY_BUFSIZ);

    session->user_login = NULL;
    session->session_type = SESSION_TELNET;
    session->session_status = SESSION_AUTH;
    ul = vtysh_idle_line_get(USER_LINE_VTY, &index);
    if(ul == NULL)
    {
		return NULL;
    }
    else
    {
		VTY_DEBUG(VD_AUTH, "use line index %d, authmode %d", index, ul->auth_mode);
    }
    
    session->ul = ul;
    session->ul->islogin = USER_LINE_LOGIN_IN;
    //  session->device_name = XSTRDUP (MTYPE_HOST, "Hios");
	//session->device_name = host.device_name;
//	printf("%s %d session->server_ip : %x session->client_ip : %x \n",__FUNCTION__,__LINE__,session->server_ip,session->client_ip);

	p_u0_info = vtysh_u0_list_get_info_by_ip(session->server_ip);
	if(p_u0_info)
	{
		vtysh_u0_ses_add(p_u0_info,session);
		strcpy(session->device_name,p_u0_info->hostname);
		
	}
	else
	{
		strcpy(session->device_name,host.device_name);
	}
	
    session->vty =  vty_telnet_create(session);
    session->user_login = NULL;
    session->user_id = 0;
    
    (void)vtysh_line_config_update(session);

    listnode_add(user_session_list, session);

    host.login_cnt++;
 	if(session->ul->auth_mode == AUTH_MODE_NONE)
 	{
		vtysh_user_login_trap(session, 1);
		session->ul->idle_cnt_en = USER_LINE_IDLE_EN;
 	}
 	
    if(pthread_mutex_init(&session->bufdata_delete_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    if(pthread_mutex_init(&session->aaa_replay_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    return session;
}

struct login_session *
vty_ssh_session_creat(uint32_t client_ip, uint32_t sport, uint32_t local_port, uint32_t server_ip)
{
    struct login_session *session;
//  struct in_addr addr;
//  struct vty *vty;
    struct ssh_packet *ssh_packet;
    char address_tmp[50];
	struct user_line *ul = NULL;
	int index = 0;
	U0_INFO * p_u0_info;

    ul = vtysh_idle_line_get(USER_LINE_VTY, &index);
    if(ul == NULL)
    {
		return NULL;
    }
    else
    {
		VTY_DEBUG(VD_CMD, "get line index %d, automode %d", index, ul->auth_mode);
    }	
	
    session = vty_session_new();

    ssh_packet = (struct ssh_packet *)xmalloc(sizeof(struct ssh_packet));
	memset(ssh_packet, 0 ,sizeof(struct ssh_packet));
    ssh_packet->session = session;
    ssh_packet->packet_length = 0;
    ssh_packet->channels_alloc = 0;
    ssh_packet->compression_buffer_ready = 0;
    ssh_packet->p_read.seqnr = 0;
    ssh_packet->after_authentication = 0;
    memset(ssh_packet->client_version_string, 0, strlen(ssh_packet->client_version_string));
    ssh_packet->compression_buffer_ready = 0;
    ssh_packet->compress_init_recv_called = 0;
    ssh_packet->compress_init_send_called = 0;
    ssh_packet->deflate_failed = 0;
    ssh_packet->inflate_failed = 0;
    ssh_packet->keep_alive_timeouts = 0;
    ssh_packet->max_blocks_in = 0;
    ssh_packet->max_blocks_out = 0;
    ssh_packet->packet_discard = 0;
    ssh_packet->packet_length = 0;
    memset(&ssh_packet->p_read, 0, sizeof(struct packet_state));
    memset(&ssh_packet->p_send, 0, sizeof(struct packet_state));
    ssh_packet->rekeying = 0;

    buffer_init(&ssh_packet->input);
    buffer_init(&ssh_packet->output);
    buffer_init(&ssh_packet->outgoing_packet);
    buffer_init(&ssh_packet->incoming_packet);


    session->ssh_packet = ssh_packet;

    session->vty = NULL;
    session->client_ip = client_ip;
    session->client_port = sport;
    session->server_port = local_port;
	session->server_ip = server_ip;
    session->timer_count = 0;
    session->aaa_replay = NULL;
    session->bufdata_cnt = 0;
    session->index = 0;
    memset(session->name_buf, 0, sizeof(session->name_buf));
    memset(session->password_buf, 0, sizeof(session->password_buf));
    session->read_bufdata = session->recv_bufdata = NULL;
    session->read_free_busy = 0;
    session->recv_copy_busy = 0;
    session->user_id = 0;
    session->user_name = NULL;
//  session->device_name = XSTRDUP (MTYPE_HOST, "Hios");

	//session->device_name = host.device_name;
	p_u0_info = vtysh_u0_list_get_info_by_ip(session->server_ip);
	if(p_u0_info)
	{
		vtysh_u0_ses_add(p_u0_info,session);
		strcpy(session->device_name,p_u0_info->hostname);
	}
	else
	{
		strcpy(session->device_name,host.device_name);
	}

    memset(session->address, 0, SU_ADDRSTRLEN);
    inet_ipv4tostr(client_ip, address_tmp);
    sprintf(session->address, "%s:%d", address_tmp, sport);


    session->buf_ipc_recv = buffer_new(VTY_BUFSIZ);
    session->user_login = NULL;

    session->session_type = SESSION_SSH;
    session->tid_ssh_exit = 0;

    session->ul = ul;

    if(pthread_mutex_init(&session->bufdata_delete_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    if(pthread_mutex_init(&session->aaa_replay_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

#if 1
	if(NULL == (session->vty = vty_ssh_create(session)))
	{
		vty_session_delete(session);
		return NULL;
	}
#endif

    listnode_add(user_session_list, session);

    return session;
}

struct login_session *
vty_console_session_creat()
{
    struct login_session *session;
    struct user_line *ul = NULL;
	U0_INFO * p_u0_info;

    session = vty_session_new();
    session->session_status = SESSION_AUTH;
    session->session_type = SESSION_CONSOLE;
    strcpy(session->address, "console");
    session->user_login = NULL;
    session->user_id = 0;
//  session->device_name = XSTRDUP (MTYPE_HOST, "Hios");
//  session->device_name = host.device_name;

	p_u0_info = vtysh_u0_list_get_info_by_ip(session->server_ip);
	if(p_u0_info)
	{
		vtysh_u0_ses_add(p_u0_info,session);
		strcpy(session->device_name,p_u0_info->hostname);
	}
	else
	{
		strcpy(session->device_name,host.device_name);
	}


    
    ul = vtysh_idle_line_get(USER_LINE_CONSOLE, NULL);
    if(ul == NULL)
    {
    	printf("no idle line ! \n");
		return NULL;
    }

    session->ul = ul;    

    vty = vty_console_create(session);

    /* Do not connect until we have passed authentication. */
    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        exit(1);
    }

    session->vty = vty;
    (void)vtysh_line_config_update(session);
    
    listnode_add(user_session_list, session);

    return session;
}

struct login_session *
vty_snmp_session_creat()
{
    struct login_session *session;
    struct vty *vty;
    pthread_t tid;
    int err;

    session = vty_session_new();
    session->session_type = SESSION_SNMP;
    session->session_status = SESSION_NORMAL;
    session->user_login = NULL;

    session->buf_ipc_recv = buffer_new(VTY_BUFSIZ);

    vty = vty_snmp_creat(session);
    session->vty = vty;

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        exit(1);
    }

    if(pthread_mutex_init(&session->bufdata_delete_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    err = pthread_create(&tid, NULL, vty_cmd_msg_rcv_snmp, (void *) session);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    listnode_add(user_session_list, session);

    return session;
}

struct login_session *
vty_ospf_session_creat()
{
    struct login_session *session;
    struct vty *vty;
    pthread_t tid;
    int err;

    session = vty_session_new();
    session->session_type = SESSION_OSPF;
    session->session_status = SESSION_NORMAL;
    session->user_login = NULL;

    session->buf_ipc_recv = buffer_new(VTY_BUFSIZ);

    vty = vty_ospf_creat(session);
    session->vty = vty;

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        exit(1);
    }

    if(pthread_mutex_init(&session->bufdata_delete_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    err = pthread_create(&tid, NULL, vty_cmd_msg_rcv_ospf, (void *) session);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    listnode_add(user_session_list, session);

    return session;
}


struct login_session *
vty_ofp_session_creat()
{
    struct login_session *session;
    struct vty *vty;
    pthread_t tid;
    int err;

    session = vty_session_new();
    session->session_type = SESSION_OFP;
    session->session_status = SESSION_NORMAL;
    session->user_login = NULL;

    session->buf_ipc_recv = buffer_new(VTY_BUFSIZ);

    vty = vty_ofp_creat(session);
    session->vty = vty;

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        exit(1);
    }

    if(pthread_mutex_init(&session->bufdata_delete_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    err = pthread_create(&tid, NULL, vty_cmd_msg_rcv_ofp, (void *) session);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    listnode_add(user_session_list, session);

    return session;
}

struct login_session *
vty_sync_session_creat()
{
    struct login_session *session;
    struct vty *vty;
//  pthread_t tid;
//  int err;

    zlog_debug(VD_COMM, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

    session = vty_session_new();
    session->session_type = SESSION_SYNC;
//  session->session_status = SESSION_NORMAL;
    session->user_login = NULL;

    session->buf_ipc_recv = buffer_new(VTY_BUFSIZ);

    vty = vty_sync_creat(session);
    session->vty = vty;

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        zlog_err("failed to connect to any daemons!\n");
    }

    if(pthread_mutex_init(&session->bufdata_delete_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    /*add session into sync_type session list*/
    listnode_add(user_session_sync_list, session);

    return session;
}

struct login_session *
vty_session_new()
{
    struct login_session *session;
    session = XCALLOC(MTYPE_VTY, sizeof(struct login_session));
	memset(session, 0, sizeof(struct login_session));

    session->auth_prompt = 1;
    return session;
}

struct login_session *
vty_telnet_session_get(uint32_t sip, uint32_t sport)
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_list)
    {
        user_session_list = list_new();
        return NULL;
    }

//  pthread_mutex_lock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        if((session_loop->client_ip == sip) && (session_loop->client_port == sport) && 
			(session_loop->session_type == SESSION_TELNET))
        {
            session = session_loop;
        }

//  pthread_mutex_unlock(&session_delete_lock);

    return session;

}

struct login_session *
vty_ssh_session_get(uint32_t sip, uint32_t sport)
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_list)
    {
        user_session_list = list_new();
        return NULL;
    }

//  pthread_mutex_lock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        if((session_loop->client_ip == sip) && \
                (session_loop->client_port == sport) && \
                (session_loop->session_type == SESSION_SSH))
        {
            session = session_loop;
        }

//  pthread_mutex_unlock(&session_delete_lock);

    return session;

}


struct login_session *
vty_console_session_get()
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_list)
    {
        user_session_list = list_new();
        return NULL;
    }

//  pthread_mutex_lock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        if(session_loop->session_type == SESSION_CONSOLE)
        {
            session = session_loop;
        }

//  pthread_mutex_unlock(&session_delete_lock);

    return session;

}

struct login_session *
vty_snmp_session_get()
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_list)
    {
        user_session_list = list_new();
        return NULL;
    }

//  pthread_mutex_lock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        if(session_loop->session_type == SESSION_SNMP)
        {
            session = session_loop;
        }

//  pthread_mutex_unlock(&session_delete_lock);

    return session;

}

struct login_session *
vty_ospf_session_get()
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_list)
    {
        user_session_list = list_new();
        return NULL;
    }

//  pthread_mutex_unlock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        if(session_loop->session_type == SESSION_OSPF)
        {
            session = session_loop;
        }

//  pthread_mutex_unlock(&session_delete_lock);

    return session;

}

struct login_session *
vty_ofp_session_get()
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_list)
    {
        user_session_list = list_new();
        return NULL;
    }

//  pthread_mutex_unlock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
        if(session_loop->session_type == SESSION_OFP)
        {
            session = session_loop;
        }

//  pthread_mutex_unlock(&session_delete_lock);

    return session;

}

void
vty_session_delete(struct login_session *session)
{
    struct vty *vty;
//  struct host_users *user;

    //assert(session != NULL);
    if(NULL == session)
    {
		return;
	}

    pthread_mutex_destroy(&session->bufdata_delete_lock);

    vty = session->vty;

    if(syncvty.vty_slot == VTY_MAIN_SLOT)
    {
        vtysh_master_sync_exit(vty, VTY_SYNC_EXIT, NULL);
    }

//  if(session->aaa_replay_lock)
    pthread_mutex_destroy(&session->aaa_replay_lock);


    if(session->session_type == SESSION_TELNET
            || session->session_type == SESSION_SSH)
    {
        vty = session->vty;

        /*remote user logout, tell aaa*/
        if(session->user_name)
        {
            vty_remote_aaa_logout(session);
        }

        session->timer_count = 0;

        if(vty != NULL)
        {
            host.login_cnt--;

            if(host.login_cnt < 0)
            {
                host.login_cnt = 0;
            }
        }

        zlog_debug("host.login_cnt:%d\n", host.login_cnt);

//      if(session->session_type == SESSION_TELNET)

        vty_term_close(vty);
		session->vty = NULL;
    }

    if(session->user_name)
    {
        free(session->user_name);
    }

    if(session->buf_ipc_recv)
    {
        buffer_free(session->buf_ipc_recv);
    }

    if(session->session_type == SESSION_SSH)
    {
#if 1
		struct ssh_packet *ssh_packet;

		ssh_packet = (struct ssh_packet *)session->ssh_packet;
		if(ssh_packet)
		{
			channel_close_all(ssh_packet);
		}
#endif
    }
	
    VTY_DEBUG(VD_TELNET, "del session: type %d, host.login_cnt %d", session->session_type, host.login_cnt);
    vtysh_user_login_trap(session, 0);
    session->ul->idle_cnt_en = USER_LINE_IDLE_DIS;
    session->ul->islogin = USER_LINE_LOGIN_OUT;	
    
    listnode_delete(user_session_list, session);

    XFREE(MTYPE_LOGIN_SESSION, session);
}

struct login_session *vty_sync_session_get(struct vty_sync_msg *sync_msg)
{
    struct login_session *session;
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    session = session_loop = NULL;

    if(! user_session_sync_list)
    {
        user_session_sync_list = list_new();
        return NULL;
    }

    for(ALL_LIST_ELEMENTS(user_session_sync_list, node, nnode, session_loop))
        if(strcmp(sync_msg->ttyname, session_loop->address) == 0)
        {
            session = session_loop;
        }

    return session;

}

void vtysh_slave_sync_session_delete(struct login_session *session)
{
    assert(session != NULL);

    pthread_mutex_destroy(&session->bufdata_delete_lock);

    if(session->buf_ipc_recv)
    {
        buffer_free(session->buf_ipc_recv);
    }

    vty_sync_close(session->vty);

    listnode_delete(user_session_sync_list, session);
    XFREE(MTYPE_LOGIN_SESSION, session);

}

int vtysh_session_delete_check(void)
{
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
    {
        if(session_loop->session_status == SESSION_CLOSE)
        {
            if((session_loop->session_type == SESSION_TELNET \
                    && session_loop->tid_r_exit == 1 \
                    && session_loop->tid_w_exit == 1) \
                    || session_loop->session_type == SESSION_SSH) 
            {
                pthread_mutex_lock(&session_delete_lock);
                vty_session_delete(session_loop);
                pthread_mutex_unlock(&session_delete_lock);
                break;
            }
        }
    }
    return 0;
}
