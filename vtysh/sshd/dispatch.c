/* $OpenBSD: dispatch.c,v 1.22 2008/10/31 15:05:34 stevesk Exp $ */
/*
 * Copyright (c) 2000 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include "includes.h"

#include <sys/types.h>

#include <signal.h>
#include <stdarg.h>

#include "ssh1.h"
#include "ssh2.h"
#include "log.h"
#include "dispatch.h"
#include "packet.h"
#include "ssh_version.h"
#include "vtysh_sshd.h"
//#include "compat.h"
#include "vtysh_session.h"

dispatch_fn *dispatch[DISPATCH_MAX];

void
dispatch_protocol_error(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	zlog_debug(VD_SSH, "dispatch_protocol_error: type %d seq %u\n", type, seq);
//	if (!compat20)
//		fatal("protocol error");
	packet_start(SSH2_MSG_UNIMPLEMENTED, ssh_packet);
	packet_put_int(seq, ssh_packet);
	packet_send(ssh_packet);
	packet_write_poll_ipc(ssh_packet);
//	packet_write_wait();
}

void
dispatch_protocol_ignore(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	zlog_debug(VD_SSH, "dispatch_protocol_ignore: type %d seq %u", type, seq);
}

/* 调用dispatch_protocol_error初始化dispatch函数指针数组，如果访问错误元素，
 * dispatch_protocol_error函数会进行出错处理而不是抛出段错误*/
void
dispatch_init(dispatch_fn *dflt, struct ssh_packet *ssh_packet)
{
	u_int i;
	for (i = 0; i < DISPATCH_MAX; i++)
		ssh_packet->dispatch[i] = dflt;
}
void
dispatch_range(u_int from, u_int to, dispatch_fn *fn, struct ssh_packet *ssh_packet)
{
	u_int i;

	for (i = from; i <= to; i++) {
		if (i >= DISPATCH_MAX)
			break;
		ssh_packet->dispatch[i] = fn;
	}
}

/* dispatch_set函数对各种事件注册回调函数，各种ssh消息的类型作为数组的索引，元素为
 * 函数指针*/
void
dispatch_set(int type, dispatch_fn *fn, struct ssh_packet *ssh_packet)
{
	ssh_packet->dispatch[type] = fn;
}

/* 在dispatch_run中调用相应的回调函数，只有当用户认证方法成功时，才会留下循环*/
void
dispatch_run(int mode, volatile sig_atomic_t *done, void *ctxt, struct ssh_packet *ssh_packet)
{
	struct login_session *session_tmp = NULL;
	session_tmp = ssh_packet->session;
	for (;;) {
		int type;
		u_int32_t seqnr;

		if (mode == DISPATCH_BLOCK) {
			type = packet_read_seqnr_ipc(&seqnr, ssh_packet);
		} else {
			type = packet_read_poll_seqnr(&seqnr, ssh_packet);
			if (type == SSH_MSG_NONE)
				{
				return;
				}
		}
		//调用回调函数
		if (type > 0 && type < DISPATCH_MAX && ssh_packet->dispatch[type] != NULL)
			{
			(*ssh_packet->dispatch[type])(type, seqnr, ctxt, ssh_packet);
			}
		else
			{
			packet_disconnect(ssh_packet, "protocol error: rcvd type %d", type);
			}
		/*done为1时return*/
		if (done != NULL && *done)
			{
			return;
			}
	}
	
}
