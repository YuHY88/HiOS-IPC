#include "auth.h"
#include "vtysh_sshd.h"
#include "buffer_ssh.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>


#include <netinet/in.h>

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>

#include "xmalloc.h"
#include "packet.h"
#include "log.h"
#include "servconf.h"
#include "channels.h"
#include "ssh1.h"
#include "ssh2.h"
#include "key.h"
#include "cipher.h"
#include "kex.h"
#include "auth.h"
#include "dispatch.h"
#include "serverloop.h"
#include "misc.h"
#include "vty.h"
#include "command.h"
#include "vtysh_telnet.h"
#include "msg_ipc.h"
#include "msg_ipc_n.h"
#include "pkt_buffer.h"
#include "ftm/pkt_ip.h"
#include "vtysh.h"
#include <sys/prctl.h>


extern int netconf_ipc_id;

static void server_init_dispatch(struct ssh_packet *ssh_packet);

static void
process_buffered_input_packets(struct ssh_packet *ssh_packet)
{
	dispatch_run(DISPATCH_NONBLOCK, NULL, ssh_packet->xxx_kex, ssh_packet);
}

void 
server_loop2(Authctxt *authctxt, struct ssh_packet *ssh_packet)
{
	struct login_session *session = NULL;
	server_init_dispatch(ssh_packet);

	session = ssh_packet->session;
	for(;;)
	{
		usleep(10000);
		process_buffered_input_packets(ssh_packet);
		
		channel_collect_close(ssh_packet);
		/*get data from msg_recv thread, and put into ssh_packet->input*/
		packet_read_poll_ipc(ssh_packet);

		packet_write_poll_ipc(ssh_packet);

		/*send channel data*/
		channel_output_data(ssh_packet);
		
		/**/
		packet_write_poll_ipc(ssh_packet);		

		if(session->session_status == SESSION_CLOSE)
		{
			//printf("%s[%d]: ssh, session delete, call channel_close_all \n", __func__, __LINE__);
			//vty_session_delete(session);

			channel_close_all(ssh_packet);
			pthread_exit(NULL);
		}
	}
}

static Channel *
server_request_session(struct ssh_packet *ssh_packet)
{
	Channel *c;

//	printf("server_request_session\n");

	zlog_debug(VD_SSH, "server_request_session\n");
	packet_check_eom(ssh_packet);
	
	/*
	 * A server session has no fd to read or write until a
	 * CHANNEL_REQUEST for a shell is made, so we set the type to
	 * SSH_CHANNEL_LARVAL.  Additionally, a callback for handling all
	 * CHANNEL_REQUEST messages is registered.
	 */
	c = channel_new("session", SSH_CHANNEL_LARVAL,
	    -1, -1, -1, /*window size*/0, CHAN_SES_PACKET_DEFAULT,
	    0, "server-session", 1, ssh_packet);

	return c;
}

int session_vty_req(struct ssh_packet *ssh_packet, Channel *c)
{
	struct vty *vty;
	unsigned int len;
//	int x, y;
	struct login_session *session = ssh_packet->session;

	c->type = CHANNEL_VTY;
	vty = vty_new();
	c->vty =  vty;
	vty->v_timeout = USER_IDLE_COUNTER;//add out timer
	vty->privilege = session->privilege;
	memset(vty->hostname, 0, sizeof(vty->hostname));
	memcpy(vty->hostname, session->device_name, strlen(session->device_name));

	if(pthread_mutex_init(&vty->obuf_lock, NULL) != 0)
	{
		perror("obuf_lock init failed\n");
		exit(1);
	}

	memcpy(vty->ttyname, packet_get_string(&len, ssh_packet), len);
//	vty->term = packet_get_string(&len, ssh_packet);
	vty->width = packet_get_int(ssh_packet);	//windth	
	vty->height= packet_get_int(ssh_packet);	//height

//	x = packet_get_int(ssh_packet);
//	y = packet_get_int(ssh_packet);
	
  vty->type = VTY_SSH;
  vty->node = CONFIG_NODE;
  vty->cp = 0;
  vty_clear_buf (vty);
  vty->length = 0;
  memset (vty->hist, 0, sizeof (vty->hist));
  vty->hp = 0;
  vty->hindex = 0;
  vty->channle = c;
	extern volatile vector vtyvec; 
	vector_set (vtyvec, vty);
//  vector_set_index (vtyvec, vty_sock, vty);
  vty->status = VTY_NORMAL;
  vty->lines = -1;

	vty->session = ssh_packet->session;
	session->vty = vty;

//	if (host.lines >= 0)
//		vty->lines = host.lines;

	for(int i=0; i < DAEMON_MAX; i++)
		vty->connect_fd[i] = -1;

	if (vtysh_connect_all (vty) <= 0)
	{
		fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
		return 0;
		//exit(1);
	}
#if 0
	pthread_mutex_lock (&session_timer_lock);

	extern struct thread_master *vtysh_master;
	if(vty->v_timeout)
		vty->t_timeout = thread_add_timer (vtysh_master, vty_timeout, vty, vty->v_timeout);
	pthread_mutex_unlock (&session_timer_lock);
#endif
//	vty->xpixel = packet_get_int(ssh_packet);	//windth ����
//	vty->ypixel = packet_get_int(ssh_packet);	//height ����

	if (strcmp(vty->ttyname, "") == 0) 
	{
#if 0
		xfree(vty->ttyname);
		vty->ttyname = NULL;
#endif
	}

//	packet_check_eom(ssh_packet);  (encoded_terminal_modes left)
	return 1;
}

void 
session_thread_req(Channel *c, struct ssh_packet *ssh_packet)
{
//	int err;
	pthread_t tid;
	struct vty *vty = c->vty;
	zlog_debug(VD_SSH, "session_thread_req\n");
	pthread_create(&tid, NULL, vty_ssh_cmd, (void *) c);
	
	vty_prompt(vty);

	c->state = SSH_CHANNEL_OPEN;
	c->local_window = c->local_window_max = CHAN_SES_WINDOW_DEFAULT;

	zlog_debug(VD_SSH, "send SSH2_MSG_CHANNEL_WINDOW_ADJUST\n");
	packet_start(SSH2_MSG_CHANNEL_WINDOW_ADJUST, ssh_packet);
	packet_put_int(c->remote_id, ssh_packet);
	packet_put_int(c->local_window, ssh_packet);
	packet_send(ssh_packet);
	
}

void * 
netconf_msg_handle(void *arg)
{
	Channel *c = (Channel *)arg;
	struct buffer_data *data;
	int nbytes;
	unsigned char *buf;
	struct buffer *buffer = c->buf_ipc_recv;
	struct login_session *session;
	prctl(PR_SET_NAME," netconf_msg_handle");

//	if(netconf_ipc_id == -1)
//		netconf_ipc_id = ipc_connect(IPC_PACKET_NETCONF);

	while(1)
	{
		usleep(500);

		if(c->state == SSH_CHANNEL_CLOSED)
		{
			c->channel_exit_flag = 1;
			break;
		}
		/* channel_input_dataå°†è§£å¯†åŽçš„æ•°æ®æ”¾å…¥c->recv_bufdataä¸­ï¼Œ
		 * ÎÒÃÇÈ¡³öc->recv_bufdataÖÐµÄÊý¾Ý²¢·¢ËÍ¸ønetconf½ø³Ì¡£ÔÚÖ÷Ïß³ÌÖÐ½ÓÊÕÀ´×Ônetconf½ø³ÌµÄ»Ø¸´£¬
		 * ²¢½«netconfÓ¦´ðÊý¾Ý·ÅÈëc->netconf_data, channel_output_dataº¯ÊýÔÚ±¾sshÁ¬½ÓµÄÖ÷loopº¯ÊýÖÐ
		 * ÖÐ±»Ñ­»·µ÷ÓÃ£¬ÂÖÑ¯Á¬½ÓµÄËùÓÐchannel£¬²¢½«channelÖÐµÄÊý¾Ýc->vty->obuf(ptyÀàÐÍchannle)»ò
		 * c->netconf_data(subsystem-netconfÀàÐÍ)¼ÓÃÜ·¢ËÍµ½¿Í»§¶Ë*/
		if(netconf_msg_test_channel(c))
		{		
			while(c->read_bufdata)
			{
				if(c->recv_copy_busy &&	\
					c->read_bufdata == c->recv_bufdata)
						continue;	
			
				data = c->read_bufdata;			
				nbytes = data->cp;
				buf = data->data;	
				session = (struct login_session *)c->session;

				/*send buf to netconf*/
				netconf_pkt_send_n(session, buf, nbytes);
				/*free buff_data*/
				pthread_mutex_lock (&c->bufdata_delete_lock);
				/*ÏÂÒ»¸öbufferdataÖÐÓÐÊý¾ÝÒª¶Á? session->read_bufdataÖ¸ÏòÏÂÒ»¸öbufferdata*/
				if(data->next)
					c->read_bufdata = c->read_bufdata->next;
				else
					c->read_bufdata = c->recv_bufdata = NULL; 	
		
				buffer_data_delete(buffer, data);
				c->bufdata_cnt--;
									
				pthread_mutex_unlock (&c->bufdata_delete_lock);
			}
		}
		
//		recv_msg_from_netconf(c);
	}
	return ((void *)0);
}


int
netconf_msg_test_channel(Channel *c)
{
	if(c->bufdata_cnt == 0 && c->read_bufdata == NULL)
		return 0;

	/* readÖ¸ÏòrecvÕýÔÚ´¦ÀíµÄbufdata£¬µÈ´ýrecv´¦ÀíÍê³É*/
	if(c->recv_copy_busy &&	\
		c->read_bufdata == c->recv_bufdata)
	{
		return 0;
	}
	else
	{
		/*return data len*/
		return c->read_bufdata->cp;
	}
}
#if 0
int netconf_pkt_send(struct login_session *session, unsigned char *data, int len)
{
	int ret = 0;
	char *netconf_send_buf;

	struct ipc_msghdr  msg_head;
	union  pkt_control pkt_ctrl;
	memset(&msg_head, 0, sizeof(msg_head));

	msg_head.module_id   = MODULE_ID_NETCONF;
	msg_head.sender_id   = MODULE_ID_VTY;
	msg_head.msg_type    = IPC_TYPE_PACKET;
	msg_head.data_num    = 1;
	msg_head.data_len    = len + sizeof(pkt_ctrl);

	memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));

	pkt_ctrl.ipcb.protocol = IP_P_TCP;
	pkt_ctrl.ipcb.sip      = session->client_ip;
	pkt_ctrl.ipcb.dip      = session->server_ip;
	pkt_ctrl.ipcb.dport    = session->client_port;
	pkt_ctrl.ipcb.sport    = session->server_port;
	pkt_ctrl.ipcb.tos      = 4;  
	pkt_ctrl.ipcb.is_changed = 1;


	netconf_send_buf = (char *)malloc(len + sizeof(pkt_ctrl));

	memset(netconf_send_buf, 0, len + sizeof(pkt_ctrl));

	memcpy(netconf_send_buf, &pkt_ctrl, sizeof(pkt_ctrl)); 
	memcpy(netconf_send_buf + sizeof(union pkt_control), data, len);

	ret = ipc_send(netconf_ipc_id, &msg_head, netconf_send_buf);
	free(netconf_send_buf);
	
	return(ret);
}
#endif
//xiawl 
int netconf_pkt_send_n(struct login_session *session, unsigned char *data, int len)
{
	int ret = 0;
	struct ipc_mesg_n *pSndMsg = NULL; 
	union  pkt_control pkt_ctrl;

	memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));

	pkt_ctrl.ipcb.protocol = IP_P_TCP;
	pkt_ctrl.ipcb.sip      = session->client_ip;
	pkt_ctrl.ipcb.dip      = session->server_ip;
	pkt_ctrl.ipcb.dport    = session->client_port;
	pkt_ctrl.ipcb.sport    = session->server_port;
	pkt_ctrl.ipcb.tos      = 4;  
	pkt_ctrl.ipcb.is_changed = 1;	

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + sizeof(union pkt_control) + len, MODULE_ID_VTY);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len	= len + sizeof(pkt_ctrl);
		pSndMsg->msghdr.module_id	= MODULE_ID_NETCONF;
		pSndMsg->msghdr.sender_id	= MODULE_ID_VTY;
		pSndMsg->msghdr.msg_type	= IPC_TYPE_PACKET;
		pSndMsg->msghdr.data_num	= 1;
		pSndMsg->msghdr.opcode		= 0;  

		memcpy(pSndMsg->msg_data, &pkt_ctrl, sizeof(pkt_ctrl));
		memcpy(pSndMsg->msg_data + sizeof(pkt_ctrl), data, len);

		ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + len + sizeof(pkt_ctrl));
		if(ret != 0)
		{
            ipc_msg_free(pSndMsg,MODULE_ID_VTY);
		}
	}

    return ret;
}

static int
session_subsystem_req(Channel *c, struct ssh_packet *ssh_packet)
{
//	char *subs;
	u_int len;
	int success = 1;
	pthread_t tid;
	char *subsys = packet_get_string(&len, ssh_packet);
	fprintf(stderr, "subsystem request for %.100s\n", subsys);

	if(!strcmp(subsys, "netconf"))
	{
		c->type = CHANNEL_SUB_NETCONF;
		c->state = SSH_CHANNEL_OPEN;
		c->local_window = c->local_window_max = CHAN_SES_WINDOW_DEFAULT;
		pthread_create(&tid, NULL, netconf_msg_handle, (void *) c);
	}
	packet_check_eom(ssh_packet);

	packet_start(SSH2_MSG_CHANNEL_WINDOW_ADJUST, ssh_packet);
	packet_put_int(c->remote_id, ssh_packet);
	packet_put_int(c->local_window, ssh_packet);
	packet_send(ssh_packet);

	return success;
}

int
session_input_channel_req(Channel *c, const char *rtype, struct ssh_packet *ssh_packet)
{
	int success = 0;
	
	/*
	 * a session is in LARVAL state until a shell, a command
	 * or a subsystem is executed
	 */

	if (c->state == SSH_CHANNEL_LARVAL)
	{
		if (strcmp(rtype, "pty-req") == 0) 
		{		
			success = session_vty_req(ssh_packet, c);
		}
		else if (strcmp(rtype, "shell") == 0)
		{
			success = 1;
			session_thread_req(c, ssh_packet);
		}
		else if (strcmp(rtype, "subsystem") == 0) 
		{
			fprintf(stderr, "type subsystem\n");
			success = session_subsystem_req(c, ssh_packet);
			fprintf(stderr, "success:%d\n", success);
//			success = session_subsystem_req(s);
		}
	}
	return success;
}

static void
server_input_channel_req(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	Channel *c;
	int id, reply, success = 0;
	char *rtype;

	id = packet_get_int(ssh_packet);		//channel id
	rtype = packet_get_string(NULL, ssh_packet);	//request type
	reply = packet_get_char(ssh_packet);		//want reply

	zlog_debug(VD_SSH, "server_input_channel_req: channel %d request %s reply %d\n",
	    id, rtype, reply);

	if ((c = channel_lookup(id, ssh_packet)) == NULL)
	{
		zlog_debug(VD_SSH, "server_input_channel_req: unknown channel %d\n", id);
		packet_disconnect(ssh_packet, "server_input_channel_req: "
		    "unknown channel %d", id);
	}
	if (!strcmp(rtype, "eow@openssh.com")) 
	{
		packet_check_eom(ssh_packet);
	} 
	else if ((c->state == SSH_CHANNEL_LARVAL ||
	    c->state == SSH_CHANNEL_OPEN) && strcmp(c->ctype, "session") == 0)
	{
		success = session_input_channel_req(c, rtype, ssh_packet);
		
	}
	if (reply) {			
		packet_start(success ?
		    SSH2_MSG_CHANNEL_SUCCESS : SSH2_MSG_CHANNEL_FAILURE, ssh_packet);
		packet_put_int(c->remote_id, ssh_packet);
		packet_send(ssh_packet);
	}
	xfree(rtype);
}

static void
server_input_channel_open(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	Channel *c = NULL;
	char *ctype;
	int rchan;
	u_int rmaxpack, rwindow, len;

	ctype = packet_get_string(&len, ssh_packet);	//channel type
	rchan = packet_get_int(ssh_packet);					//sender channel
	rwindow = packet_get_int(ssh_packet);				//window size	
	rmaxpack = packet_get_int(ssh_packet);			//max window size

	zlog_debug(VD_SSH, "server_input_channel_open: ctype %s rchan %d win %d max %d\n",
	    ctype, rchan, rwindow, rmaxpack);	

	if (strcmp(ctype, "session") == 0) {
		c = server_request_session(ssh_packet);
	}
	
	if (c != NULL) {
		zlog_debug(VD_SSH, "server_input_channel_open: confirm %s\n", ctype);
		c->remote_id = rchan;
		c->remote_window = rwindow;
		c->remote_maxpacket = rmaxpack;
		if (c->state != SSH_CHANNEL_CONNECTING) {
			zlog_debug(VD_SSH, "send SSH2_MSG_CHANNEL_OPEN_CONFIRMATION\n");
			packet_start(SSH2_MSG_CHANNEL_OPEN_CONFIRMATION, ssh_packet);
			packet_put_int(c->remote_id, ssh_packet);
			packet_put_int(c->self, ssh_packet);
			packet_put_int(c->local_window, ssh_packet);
			packet_put_int(c->local_maxpacket, ssh_packet);
			packet_send(ssh_packet);
		}
	} else {
		zlog_debug(VD_SSH, "server_input_channel_open: failure %s", ctype);
		packet_start(SSH2_MSG_CHANNEL_OPEN_FAILURE, ssh_packet);
		packet_put_int(rchan, ssh_packet);
		packet_put_int(SSH2_OPEN_ADMINISTRATIVELY_PROHIBITED, ssh_packet);
		packet_put_cstring("open failed", ssh_packet);
		packet_put_cstring("", ssh_packet);
		packet_send(ssh_packet);
	}
	xfree(ctype);
	
}

static void
server_init_dispatch_20(struct ssh_packet *ssh_packet)
{
	dispatch_init(&dispatch_protocol_error, ssh_packet);
	dispatch_set(SSH2_MSG_CHANNEL_CLOSE, &channel_input_oclose, ssh_packet);
	dispatch_set(SSH2_MSG_CHANNEL_DATA, &channel_input_data, ssh_packet);
//	dispatch_set(SSH2_MSG_CHANNEL_EOF, &channel_input_ieof);
//	dispatch_set(SSH2_MSG_CHANNEL_EXTENDED_DATA, &channel_input_extended_data);
	dispatch_set(SSH2_MSG_CHANNEL_OPEN, &server_input_channel_open, ssh_packet);
//	dispatch_set(SSH2_MSG_CHANNEL_OPEN_CONFIRMATION, &channel_input_open_confirmation);
//	dispatch_set(SSH2_MSG_CHANNEL_OPEN_FAILURE, &channel_input_open_failure);
	dispatch_set(SSH2_MSG_CHANNEL_REQUEST, &server_input_channel_req, ssh_packet);
	dispatch_set(SSH2_MSG_CHANNEL_WINDOW_ADJUST, &channel_input_window_adjust, ssh_packet);
//	dispatch_set(SSH2_MSG_GLOBAL_REQUEST, &server_input_global_request);
	/* client_alive */
//	dispatch_set(SSH2_MSG_CHANNEL_SUCCESS, &server_input_keep_alive);
//	dispatch_set(SSH2_MSG_CHANNEL_FAILURE, &server_input_keep_alive);
//	dispatch_set(SSH2_MSG_REQUEST_SUCCESS, &server_input_keep_alive);
//	dispatch_set(SSH2_MSG_REQUEST_FAILURE, &server_input_keep_alive);
	/* rekeying */
	dispatch_set(SSH2_MSG_KEXINIT, &kex_input_kexinit, ssh_packet);
}


static void
server_init_dispatch(struct ssh_packet *ssh_packet)
{
	server_init_dispatch_20(ssh_packet);
}


