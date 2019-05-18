#ifndef _VTYSH_SSHD_H
#define _VTYSH_SSHD_H

#include <lib/vty.h>
#include <lib/buffer.h>
#include "buffer_ssh.h"
#include "dispatch.h"
#include "kex.h"
#include "cipher.h"
#include "packet.h"
#include <zlib.h>
#include <lib/types.h>
#include "channels.h"
//typedef struct Kex Kex;


struct Newkeys;
struct ssh_packet
{
	Buffer input;

	/* Buffer for raw output data going to the socket. */
	/*output是最终要发送的原始buffer(可能是加密后)*/
	Buffer output;

	/* Buffer for the partial outgoing packet being constructed. */
	/*在packet_send2（）和其后续函数内的临时缓存*/
	Buffer outgoing_packet;

	/* Buffer for the incoming packet currently being processed. */
	/*解密、处理后的报文*/
	Buffer incoming_packet; 

	Buffer compression_buffer;

	char client_version_string[256];

	unsigned char *session_id2;
	unsigned int session_id2_len;	

	Kex *xxx_kex;

	Newkeys *newkeys[MODE_MAX];

	Newkeys *current_keys[MODE_MAX];

	CipherContext send_context;

	CipherContext receive_context;

	int initialized;

	struct packet_state p_read;
	struct packet_state p_send;

	u_char extra_pad;

	int after_authentication;

	int compression_buffer_ready;

	z_stream incoming_stream;
	z_stream outgoing_stream;
	int compress_init_send_called;
	int compress_init_recv_called;
	int inflate_failed;
	int deflate_failed;


	u_int64_t max_blocks_in;
	u_int64_t	max_blocks_out;

	dispatch_fn *dispatch[DISPATCH_MAX];

	unsigned int packet_discard;

	int keep_alive_timeouts;	

	struct login_session *session;

	Channel **channels;

	int channels_alloc;

	unsigned int packet_length;

	int rekeying;
	
};

extern void *vty_ssh_read (void *arg);
extern int sshd_ipc_read(struct login_session *session, char *buf, size_t buf_len);
extern int sshd_ipc_write(struct login_session *session, u_char *buf, int nbytes);
//extern int sshd_ipc_read_input(struct ssh_packet *ssh_packet);
extern void vtysh_sshd_init(void);


struct vty *vty_new_ssh_init(void);
struct vty *vty_ssh_create(struct login_session *session);


#endif

