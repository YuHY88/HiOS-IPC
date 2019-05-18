#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xmalloc.h"
#include "buffer_ssh.h"
#include "cipher.h"
#include "auth.h"
#include "key.h"
#include "kex.h"
#include "servconf.h"
#include "vtysh_session.h"
#include "vtysh_sshd.h"
#include "ssh_version.h"
#include "myproposal.h"
#include "buffer_ssh.h"
#include <lib/vty.h>
#include "sha2.h"
#include <lib/buffer.h>
#include "serverloop.h"
#include "authfile.h"
#include "misc.h"
#include <ftm/pkt_tcp.h>
#include <sys/prctl.h>
#include <lib/log.h>
#include "vtysh.h"
#include "vtysh_sync.h"


#define PROTOCOL_MAJOR_2	2
#define PROTOCOL_MINOR_2	0

/* same for ssh2 */
u_char *session_id2 = NULL;
u_int session_id2_len = 0;

//char *client_version_string = NULL;
char *server_version_string = NULL;

/* Server configuration options. */
ServerOptions options;

int compat20 = 0;

/*
 * Any really sensitive data in the application is contained in this
 * structure. The idea is that this structure could be locked into memory so
 * that the pages do not get written into swap.  However, there are some
 * problems. The private key contains BIGNUMs, and we do not (in principle)
 * have access to the internals of them, and locking just the structure is
 * not very useful.  Currently, memory locking is not implemented.
 */
struct {
	Key	*server_key;		/* ephemeral server key */
	Key	**host_keys;		/* all private host keys */
	int	have_ssh2_key;
} sensitive_data;

static void do_ssh2_kex(struct login_session *);


int 
sshd_ipc_write(struct login_session *session, u_char *buf, int nbytes)
{
//	struct send_type send[100];	
	struct send_type send;
	uint32_t ip = session->client_ip;
	uint16_t port = session->client_port;
	uint16_t local_port = session->server_port;
	uint16_t vpn = vtysh_vpn_get();
//	int send_index = 0;
//	int j;
#if 0
	if(nbytes > 1000)
	{
		for(; nbytes >=1000; nbytes = nbytes -1000)
		{
			send[send_index].send_buf = (char *)(buf + (strlen((char *)buf) - nbytes));
			send[send_index].send_len = 1000;
			send_index ++;
		}
	}
//	printf("strlen buf:%d nbytes:%d strlen-nbytes:%d\n", strlen(buf), nbytes, strlen(buf) - nbytes);
//	send[send_index].send_buf = buf + (strlen(buf) - nbytes);
	send[send_index].send_buf = (char *)buf;
//printf("(strlen(buf) - nbytes):%d\n", (strlen(buf) - nbytes));
	send[send_index].send_len = nbytes;
	printf("send[send_index].send_len:%d\n", send[send_index].send_len);
	send_index ++;
#endif

	send.send_buf = (char *)buf;
	send.send_len = nbytes;
	zlog_debug(VD_SSH, "send.send_len:%d\n", send.send_len);
	if(buffer_pkt_send(ip, port, local_port, vpn, send) == -1)
	{
		fprintf(stderr, "Could not write ident string to %s\n", session->address);
		return -1;
	}


#if 0	
	printf("send len:%d\n", nbytes);
	for(unsigned int i = 0; i < send_index; i++)
	{
//			for(j = 0; j < send[i].send_len; j++);
//				{
//				printf("%x ", send[i].send_buf[j]);
//			}
		if(buffer_pkt_send(ip, port, 22, send[i]) == -1)
		{
			fprintf(stderr, "Could not write ident string to %s", session->address);
			return -1;
		}
	}
#endif
	return 0;
}



int sshd_ipc_read(struct login_session *session, char *buf, size_t buf_len)
{
	int    data_available_len;
	int    data_used_len;
	char * buf_tmp;
	int    buf_len_tmp;
	
	struct buffer *buffer;
	struct buffer_data *data;
	struct buffer_data *data_tmp;
	
//	printf("thread_id %d %s %d \n",pthread_self(),__FUNCTION__,__LINE__);
	buf_tmp = buf;
	buf_len_tmp = buf_len;

	zlog_debug(VD_SSH, "sshd_ipc_read\n");
//	printf("thread_id %d %s %d \n",pthread_self(),__FUNCTION__,__LINE__);
	while(1)
	{
		if(session->bufdata_cnt == 0 && session->read_bufdata == NULL)
		{
			usleep(1000);
			continue;	
		}
		
		if(session->recv_copy_busy && \
			 session->read_bufdata == session->recv_bufdata)
		{
				continue;
		}
		break;
	}

	pthread_mutex_lock (&session->bufdata_delete_lock);
	zlog_debug(VD_SSH, "sshd_ipc_read  get bufdata_delete_lock\n");
	buffer = session->buf_ipc_recv;
	data = session->read_bufdata; 	

	while(NULL != data)
	{
		data_available_len = data->cp - data->sp;
		/*
		æ•°æ®ç¬¬ä¸€ä¸ªdataä¾æ¬¡å­˜å‚¨ï¼Œå½“data_available_lenä¸º0ï¼Œè¡¨é¢å½“å‰dataæ²¡æœ‰æ•°æ®å¯è¯»ï¼ŒåŽç»­dataä¹Ÿæ— æ•°æ®å¯è¯»
		*/
		if(0 == data_available_len)
		{
			break;
		}
		
		if(data_available_len > buf_len_tmp)
		{
			data_used_len  = buf_len_tmp;
		}
		else
		{	
			data_used_len  = data_available_len;
		}
		
		memcpy(buf_tmp, &data->data[data->sp], data_used_len);
		
		buf_tmp     += data_used_len;
		buf_len_tmp -= data_used_len;
		data->sp    += data_used_len;

		//å½“data->sp == buffer->size è¡¨é¢è¯¥data->spå·²ç»æŒ‡å‘çš„æœ€å¤§å€¼ï¼Œè¯¥dataå•å…ƒä»¥æ•°æ®å·²ç©º
		if(data->sp == buffer->size)
		{
			data_tmp = data;
			data = data->next;
			//æ›´æ–°read_bufdata
			session->read_bufdata = data;
			buffer_data_delete(buffer, data_tmp);
			session->bufdata_cnt--;
		}

		if(0 == buf_len_tmp )
		{			
			break;
		}

	}

	//session->read_bufdataä¸ºç©ºè¡¨é¢æ•°æ®è¯»å®Œï¼Œrecv_bufdataç›¸åº”æ›´æ–°
	if(NULL == session->read_bufdata)
	{
		session->recv_bufdata = session->read_bufdata;	
	}
	
//	printf("thread_id %d %s %d \n",pthread_self(),__FUNCTION__,__LINE__);
	
	pthread_mutex_unlock (&session->bufdata_delete_lock);

	
	return (buf_tmp - buf);
}

static void init_rng(void)
{
	/*
	 * OpenSSL version numbers: MNNFFPPS: major minor fix patch status
	 * We match major, minor, fix and status (not patch)
	 */
	if ((SSLeay() ^ OPENSSL_VERSION_NUMBER) & ~0xff0L)
		fatal("OpenSSL version mismatch. Built against %lx, you "
		    "have %lx", OPENSSL_VERSION_NUMBER, SSLeay());

}

static void
sshd_exchange_identification(struct login_session *session)
{
	int i;
	int mismatch;
	int remote_major, remote_minor;
	const char *s;
	char buf[256];			/* Must not be larger than remote_version. */
	char remote_version[256];	/* Must be at least as big as buf. */
//    int nbytes;
	struct ssh_packet *ssh_packet;

	zlog_debug(VD_SSH, "sshd_exchange_identification\n");

	/* Send our protocol version identification. */
	if(sshd_ipc_write(session, (u_char *)server_version_string, strlen(server_version_string)) == -1)
	{
		fprintf(stderr, "Write to IPC error\n");
	}
#if 0	

	buf = (unsigned char *)malloc(buf_len); 
	if(buf == NULL)
	{
		zlog_err("malloc error, buf == NULL\n");
		exit(1);
	}
	printf("%s %d thread id: %lu\n",__FUNCTION__,__LINE__, pthread_self());
	printf("%s %d server_ip : %x, server_port : %d,client_ip : %x,client_port : %d \n",__FUNCTION__,__LINE__,session->server_ip, session->server_port, session->client_ip,session->client_port);
//	nbytes = sshd_ipc_read(session, &buf, buf_len);
	printf("%s %d thread id: %lu\n",__FUNCTION__,__LINE__, pthread_self());
//	printf("%s\n",pkt_dump(buf, buf_len));
	printf("%s %d thread id: %lu\n",__FUNCTION__,__LINE__, pthread_self());
	for(i = 0; i < nbytes; i ++)
	{
		printf("%c",buf[i]);
	}
	printf("\n");
#endif	
	
	memset(buf, 0, sizeof(buf));

	for (i = 0; i < sizeof(buf) - 1; i++) {
		if (sshd_ipc_read(session, &buf[i], 1)!= 1) {
			packet_close(ssh_packet);
			session->session_status = SESSION_CLOSE;
			vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);
			pthread_exit(NULL);
		}
		
//		printf("%d",buf[i]);
		if (buf[i] == '\r') {
			buf[i] = 0;
			/* Kludge for F-Secure Macintosh < 1.0.2 */
			if (i == 12 &&
			    strncmp(buf, "SSH-1.5-W1.0", 12) == 0)
				break;
			continue;
		}
		if (buf[i] == '\n') {
			buf[i] = 0;
			break;
		}
	}

#if 0
	for (i = 0; i < nbytes; i++)
	{
		if (buf[i] == '\r') {
			buf[i] = 0;
			/* Kludge for F-Secure Macintosh < 1.0.2 */
			if (i == 12 &&
			    strncmp((char *)buf, "SSH-1.5-W1.0", 12) == 0)
				break;
			continue;
		}
		if (buf[i] == '\n') {
			buf[i] = 0;
			break;
		}		
	}
#endif
	ssh_packet = (struct ssh_packet *)(session->ssh_packet);
	memset(ssh_packet->client_version_string, 0, sizeof(ssh_packet->client_version_string));
	memcpy(ssh_packet->client_version_string, buf, strlen((char *)buf));


	/*
	 * Check that the versions match.  In future this might accept
	 * several versions and set appropriate flags to handle them.
	 * sscanf´Ó¸ø¶¨×Ö·û´®ÖÐÕÒµ½Ö¸¶¨¸ñÊ½¡£ÅÐ¶ÏÊÕµ½µÄclient°æ±¾ÊÇ·ñºÏ·¨
	 */
	zlog_debug(VD_SSH, "client_version_string:%s\n", ssh_packet->client_version_string);
	if (sscanf(ssh_packet->client_version_string, "SSH-%d.%d-%[^\n]\n",
	    &remote_major, &remote_minor, remote_version) != 3) {
		s = "Protocol mismatch.\n";
		if(sshd_ipc_write(session, (u_char *)s, strlen(s)))
		{
			fprintf(stderr, "Write to IPC error\n");
		}

		packet_close(ssh_packet);
		session->session_status = SESSION_CLOSE;
		vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);

		pthread_exit(NULL);
	}
	zlog_debug(VD_SSH, "Client protocol version %d.%d; client software version %.100s\n",
	    remote_major, remote_minor, remote_version);

	mismatch = 0;

	switch (remote_major) {
	case 1:
		if (remote_minor == 99) 
		{
			break;
		}
		else
		{
			mismatch = 1;
			fprintf(stderr,"Your ssh version is too old and "
			    "is no longer supported.  Please install a newer version.");
			break;
		}
	case 2:
		break;
		/* FALLTHROUGH */
	default:
		mismatch = 1;
		break;
	}

	chop(server_version_string);
//	debug("Local version string %.200s", server_version_string);

	if (mismatch) {
		s = "Protocol major versions differ.\n";
		if(sshd_ipc_write(session, (u_char *)s, strlen(s)))
		{
			fprintf(stderr, "Write to IPC error\n");
		}
		
		packet_close(ssh_packet);
		session->session_status = SESSION_CLOSE;
		vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);

		pthread_exit(NULL);

	}
}

void vtysh_sshd_init()
{
	extern char *optarg;
	extern int optind;
	int i;
	int major, minor;
	const char *newline = "\r\n";;
	char buf[256];
	Key *key;

	init_rng();

	SSLeay_add_all_algorithms();

	sensitive_data.server_key = NULL;
	sensitive_data.have_ssh2_key = 0;
	
	initialize_server_options(&options);

	/* Fill in default values for those options not explicitly set. */
	fill_default_server_options(&options);	

	if (!RAND_status())
		fprintf(stderr, "Not enough entropy in RNG");

	sensitive_data.host_keys = xcalloc(options.num_host_key_files, sizeof(Key *));	
	if(!sensitive_data.host_keys)
		fprintf(stderr, "calloc error\n");
	
	for (i = 0; i < options.num_host_key_files; i++)
		sensitive_data.host_keys[i] = NULL;

	for (i = 0; i < options.num_host_key_files; i++) {
		zlog_debug(VD_SSH, "options.host_key_files[%d]:%s\n", i, options.host_key_files[i]);
		key = key_load_private(options.host_key_files[i], "", NULL);
		sensitive_data.host_keys[i] = key;
		if (key == NULL) {
			fprintf(stderr, "Could not load host key: %s\n",
					options.host_key_files[i]);
			sensitive_data.host_keys[i] = NULL;
			continue;
		}
		switch (key->type) {
		case KEY_RSA:
		case KEY_DSA:
			sensitive_data.have_ssh2_key = 1;
			break;
		}
	}

	if (options.protocol & SSH_PROTO_1) {
		fprintf(stderr, "Disabling protocol version 1. Too old\n");
		options.protocol &= ~SSH_PROTO_1;
	}
	if ((options.protocol & SSH_PROTO_2) && !sensitive_data.have_ssh2_key) {
		options.protocol &= ~SSH_PROTO_2;
	}

	arc4random_stir();

	major = PROTOCOL_MAJOR_2;
	minor = PROTOCOL_MINOR_2;
//	newline = "\r\n";

	snprintf(buf, sizeof buf, "SSH-%d.%d-%.100s%s", major, minor,
	    SSH_VERSION, newline);
	server_version_string = strdup(buf);

}

void *
vty_ssh_read (void *arg)
{
	struct login_session *session;
//	struct buffer *buffer;
	struct ssh_packet *ssh_packet;
	Authctxt *authctxt;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME," vty_ssh_read");

	session = (struct login_session *) arg;
//	buffer = session->buf_ipc_recv;	
	ssh_packet = (struct ssh_packet *)session->ssh_packet;
    if(ssh_packet == NULL)
    {
        return NULL;
    }

	packet_set_connection(ssh_packet);

	server_version_string = strdup("SSH-2.0-Huahuan\r\n");

	
	sshd_exchange_identification(session);

	/* allocate authentication context */
	authctxt = xcalloc(1, sizeof(*authctxt));

	do_ssh2_kex(session);

	do_authentication2(authctxt, ssh_packet);

	authctxt->authenticated = 1;

	server_loop2(authctxt, ssh_packet);
	
	return NULL;
}

static char *
list_hostkey_types(void)
{
	Buffer b;
	const char *p;
	char *ret;
	int i;
	buffer_init(&b);
	for (i = 0; i < options.num_host_key_files; i++) {
		Key *key = sensitive_data.host_keys[i];
		if (key == NULL)
			{
			continue;
			}
		switch (key->type) {
		case KEY_RSA:
		case KEY_DSA:		
			if (buffer_len(&b) > 0)
				buffer_append(&b, ",", 1);
			p = key_ssh_name(key);
			buffer_append(&b, p, strlen(p));
			break;
		}
	}
	buffer_append(&b, "\0", 1);
	ret = ssh_xstrdup(buffer_ptr(&b));
	ssh_buffer_free(&b);
	return ret;
}

Key *
get_hostkey_by_type(int type)
{
	int i;

	for (i = 0; i < options.num_host_key_files; i++) {
		Key *key = sensitive_data.host_keys[i];
		if (key != NULL && key->type == type)
			return key;
	}
	return NULL;
}

int
get_hostkey_index(Key *key)
{
	int i;
	fprintf(stderr,"get_hostkey_index\n");

	for (i = 0; i < options.num_host_key_files; i++) {
		if (key == sensitive_data.host_keys[i])
			return (i);
	}
	return (-1);
}

/*
 * SSH2 key exchange: diffie-hellman-group1-sha1
 */
static void
do_ssh2_kex(struct login_session *session)
{
	Kex *kex;
	struct ssh_packet *ssh_packet;

	/*options.ciphersÖ¸¶¨ssh2Ö§³ÖµÄ¼ÓÃÜËã·¨*/
	if (options.ciphers != NULL) {
		myproposal[PROPOSAL_ENC_ALGS_CTOS] =
		myproposal[PROPOSAL_ENC_ALGS_STOC] = options.ciphers;
	}

	if (options.macs != NULL) {
		myproposal[PROPOSAL_MAC_ALGS_CTOS] =
		myproposal[PROPOSAL_MAC_ALGS_STOC] = options.macs;
	}
	
	if (options.compression == COMP_NONE) {
		myproposal[PROPOSAL_COMP_ALGS_CTOS] =
		myproposal[PROPOSAL_COMP_ALGS_STOC] = "none";
	} else if (options.compression == COMP_DELAYED) {
		myproposal[PROPOSAL_COMP_ALGS_CTOS] =
		myproposal[PROPOSAL_COMP_ALGS_STOC] = "none,zlib@openssh.com";
	}

	myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] = list_hostkey_types();

	ssh_packet = (struct ssh_packet *)(session->ssh_packet);
	/* start key exchange */
	kex = kex_setup(myproposal, ssh_packet);
	
	kex->kex[KEX_DH_GRP1_SHA1] = kexdh_server;
	kex->kex[KEX_DH_GRP14_SHA1] = kexdh_server;
	kex->kex[KEX_DH_GEX_SHA1] = kexgex_server;
	kex->kex[KEX_DH_GEX_SHA256] = kexgex_server;
	kex->server = 1;
	kex->client_version_string = ssh_packet->client_version_string;
	kex->server_version_string = server_version_string;
	kex->load_host_key=&get_hostkey_by_type;
	kex->host_key_index=&get_hostkey_index;
	
	ssh_packet->xxx_kex = kex;

	/*dispatch_runÖÐ·¢ËÍËã·¨Ð­ÉÌÊý¾Ý£¬²¢ÔÚÊÕµ½»Ø¸´ºóµ÷ÓÃËã·¨Ð­ÉÌµÄ»Øµ÷º¯Êý kex_input_kexinit*/
	dispatch_run(DISPATCH_BLOCK, &kex->done, kex, ssh_packet);

	ssh_packet->session_id2 = kex->session_id;
	ssh_packet->session_id2_len = kex->session_id_len;
}





/**
 * @brief      : æ–°å»ºssh vty
 * @param[in ] : 
 * @param[out] :
 * @return     : æˆåŠŸè¿”å›ž vtyï¼Œå¦åˆ™è¿”å›žNULL
 * @author     : Lipf
 * @date       : 2019å¹´2æœˆ21æ—¥
 * @note       : 
 */

struct vty *vty_new_ssh_init(void)
{
    struct vty *vty;

    vty = vty_new();
//  vty->fd = vty_sock;
//  vty->wfd = vty_sock;
    vty->type = VTY_SSH;
    vty->node = AUTH_NODE;
    vty->fail = 0;
    vty->cp = 0;
    vty_clear_buf(vty);
    vty->length = 0;
    memset(vty->hist, 0, sizeof(vty->hist));
    vty->hp = 0;
    vty->hindex = 0;
    vector_set(vtyvec, vty);
//  vector_set_index (vtyvec, vty_sock, vty);
    vty->status = VTY_NORMAL;
    vty->lines = -1;
    vty->iac = 0;
    vty->iac_sb_in_progress = 0;
    vty->sb_len = 0;

    return vty;
}


/**
 * @brief      : åˆå§‹åŒ–ssh vty
 * @param[in ] : sessionï¼Œä¼šè¯
 * @param[out] :
 * @return     : æˆåŠŸè¿”å›ž vtyï¼Œå¦åˆ™è¿”å›žNULL
 * @author     : Lipf
 * @date       : 2019å¹´2æœˆ21æ—¥
 * @note       : 
 */

struct vty *vty_ssh_create(struct login_session *session)
{
    struct vty *vty;
    vty = vty_new_ssh_init();

    memset(vty->ttyname, 0, 20);
    strcpy(vty->ttyname, session->address);

    vty->session = session;
    vty->v_timeout = USER_IDLE_COUNTER;//add out timer

    if(pthread_mutex_init(&vty->obuf_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        vty_close(vty);
		return NULL;
    }

    if(host.lines >= 0)
    {
        vty->lines = host.lines;
    }

    if(pthread_mutex_init(&vty->catch_buf_lock, NULL) != 0)
    {
        perror("snmp catch_buf_lock init failed\n");
        //exit(1);

		vty_close(vty);
		return NULL;
    }

#if 0
    vty_will_echo(vty);
    vty_will_suppress_go_ahead(vty);
    vty_dont_linemode(vty);
    vty_do_window_size(vty);
#endif
    /* Say hello to the world. */
    vty_hello(vty);


    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    vty_out(vty, "%s%% User Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    /* Setting up terminal. */

    /* vty_dont_lflow_ahead (vty); */

	if((session->ul->auth_mode == AUTH_MODE_SCHEME) && (host.login_cnt <= USER_LOGIN_MAX))
    {
		vty_out(vty, "Username:");
	}
	else if(session->ul->auth_mode == AUTH_MODE_PASSWORD)
    {
		vty_out(vty, "Password:");
    }

    /* Do not connect until we have passed authentication. */
    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        //exit(1);

		vty_close(vty);
		return NULL;
    }

#if 0
    pthread_mutex_lock(&session_timer_lock);

    if(vty->v_timeout)
    {
        vty->t_timeout = thread_add_timer(vtysh_master, vty_timeout, vty, vty->v_timeout);
    }

    pthread_mutex_unlock(&session_timer_lock);
#endif

    /*sync vty if master is at realtime_sync status*/
#if 0
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            && syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE)
    {
        vtysh_master_sync_vty(vty, VTY_SYNC_CREAT, NULL);
    }
#endif

    return vty;

}


