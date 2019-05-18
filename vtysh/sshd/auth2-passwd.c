/* $OpenBSD: auth2-passwd.c,v 1.9 2006/08/03 03:34:41 deraadt Exp $ */
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

#include <string.h>
#include <stdarg.h>

#include "xmalloc.h"
#include "packet.h"
#include "log.h"
#include "key.h"
//#include "hostfile.h"
#include "auth.h"
#include "buffer_ssh.h"
#ifdef GSSAPI
#include "ssh-gss.h"
#endif
//#include "monitor_wrap.h"
#include "servconf.h"
#include "memtypes.h"
#include "vtysh_session.h"
#include "memory.h"
#include "vtysh_sshd.h"
#include "vtysh_auth.h"
#include "vtysh.h"
/* import */
extern ServerOptions options;

int
auth_password(Authctxt *authctxt, const char *password)
{
	int result, ok = authctxt->valid;

	/*do not support none user*/
	return 0;
	zlog_debug(VD_SSH, "auth_password\n");
	if (*password == '\0')
	{
	return 0;
	}

	zlog_debug(VD_SSH, "result:%d ok:%d\n", result, ok);
	return (result && ok);
}

static int
userauth_passwd(Authctxt *authctxt, struct ssh_packet *ssh_packet)
{
	char *password, *newpass;
	int authenticated = 0;
	int change;
	u_int len, newlen;
	struct login_session *session = ssh_packet->session;

	zlog_debug(VD_SSH, "userauth_passwd\n");
	change = packet_get_char(ssh_packet);
	password = packet_get_string(&len, ssh_packet);
	strcpy(session->password_buf, password);
	zlog_debug(VD_SSH, "session->password_buf:%s\n", session->password_buf);
	if (change) {
		/* discard new password from packet */
		newpass = packet_get_string(&newlen, ssh_packet);
		memset(newpass, 0, newlen);
		xfree(newpass);
	}
	packet_check_eom(ssh_packet);

	if (change)
		zlog_debug(VD_SSH, "password change not supported");
	else if (vtysh_auth_remote_ssh(session) == 1)
	{		
		authenticated = 1;
	zlog_debug(VD_SSH, "authenticated:%d\n", authenticated);
		session->session_status = SESSION_NORMAL;
		session->user_name = XSTRDUP (MTYPE_VTY, authctxt->user);
	}
#ifdef HAVE_CYGWIN
	if (check_nt_auth(1, authctxt->pw) == 0)
		authenticated = 0;
#endif
	memset(password, 0, len);
	xfree(password);
	return authenticated;
}

Authmethod method_passwd = {
	"password",
	userauth_passwd,
	&options.password_authentication
};
