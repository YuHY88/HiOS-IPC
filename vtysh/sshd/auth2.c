/* $OpenBSD: auth2.c,v 1.120 2008/11/04 08:22:12 djm Exp $ */
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
#include <sys/stat.h>
#include <sys/uio.h>

#include <fcntl.h>
#include <pwd.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>


#include "xmalloc.h"
#include "atomicio.h"
#include "ssh2.h"
#include "packet.h"
#include "log.h"
#include "buffer_ssh.h"
#include "servconf.h"
#include "key.h"
#include "auth.h"
#include "dispatch.h"
#include "pathnames.h"

#ifdef GSSAPI
#include "ssh-gss.h"
#endif
#include <lib/vty.h>
#include "vtysh_user.h"
#include "vtysh_sshd.h"
#include "vtysh_session.h"
#include "ssh_version.h"



/* import */
extern ServerOptions options;
extern u_char *session_id2;
extern u_int session_id2_len;
extern Buffer loginmsg;

/* methods */

extern Authmethod method_none;
extern Authmethod method_pubkey;
extern Authmethod method_passwd;
//extern Authmethod method_kbdint;
extern Authmethod method_hostbased;
#ifdef GSSAPI
extern Authmethod method_gssapi;
#endif
#ifdef JPAKE
extern Authmethod method_jpake;
#endif

Authmethod *authmethods[] = {
	&method_none,
	&method_pubkey,
	&method_passwd,
//	&method_kbdint,
//	&method_hostbased,
	NULL
};

/* protocol */

static void input_service_request(int, u_int32_t, void *, struct ssh_packet *);
static void input_userauth_request(int, u_int32_t, void *, struct ssh_packet *);

/* helper */
static Authmethod *authmethod_lookup(const char *);
static char *authmethods_get(void);

char *
auth2_read_banner(void)
{
	struct stat st;
	char *banner = NULL;
	size_t len, n;
	int fd;

	if ((fd = open(options.banner, O_RDONLY)) == -1)
		return (NULL);
	if (fstat(fd, &st) == -1) {
		close(fd);
		return (NULL);
	}
	if (st.st_size > 1*1024*1024) {
		close(fd);
		return (NULL);
	}

	len = (size_t)st.st_size;		/* truncate */
	banner = xmalloc(len + 1);
	n = atomicio(read, fd, banner, len);
	close(fd);

	if (n != len) {
		xfree(banner);
		return (NULL);
	}
	banner[n] = '\0';

	return (banner);
}

#if 0
void
userauth_send_banner(const char *msg)
{
	if (datafellows & SSH_BUG_BANNER)
		return;

	packet_start(SSH2_MSG_USERAUTH_BANNER);
	packet_put_cstring(msg);
	packet_put_cstring("");		/* language, unused */
	packet_send();
	debug("%s: sent", __func__);
}
#endif

static void
userauth_banner(void)
{
	char *banner = NULL;

	zlog_debug(VD_SSH, "userauth_banner\n");
	if (options.banner == NULL ||
	    strcasecmp(options.banner, "none") == 0)
		return;

	if ((banner = auth2_read_banner()) == NULL)
		goto done;
//	userauth_send_banner(banner);

done:
	if (banner)
		xfree(banner);
}

/*
 * loop until authctxt->success == TRUE
 */
void
do_authentication2(Authctxt *authctxt, struct ssh_packet *ssh_packet)
{
	dispatch_init(&dispatch_protocol_error, ssh_packet);
	dispatch_set(SSH2_MSG_SERVICE_REQUEST, &input_service_request, ssh_packet);
	dispatch_run(DISPATCH_BLOCK, &authctxt->success, authctxt, ssh_packet);
}

/*ARGSUSED*/

static void
input_service_request(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	Authctxt *authctxt = ctxt;
	u_int len;
	int acceptit = 0;
	char *service = packet_get_string(&len, ssh_packet);
	packet_check_eom(ssh_packet);

	zlog_debug(VD_SSH, "input_service_request\n");
	if (authctxt == NULL)
		fatal("input_service_request: no authctxt");

	if (strcmp(service, "ssh-userauth") == 0) {
		if (!authctxt->success) {
			acceptit = 1;
			/* now we can handle user-auth requests */
			/*input_service_request在dispatch_run中被调用，然后在这里添加input_userauth_request的回调。*
			 *dispatch_run中done参数为authctxt->success，此时仍为0，dispatch_run函数不会返回，继续轮询 *
			 *并回调input_userauth_request函数，input_userauth_request函数执行完时，将authctxt->success*
			 *置一，dispatch_run函数才返回 */
			dispatch_set(SSH2_MSG_USERAUTH_REQUEST, &input_userauth_request, ssh_packet);
		}
	}
	/* XXX all other service requests are denied */

	if (acceptit) {
		zlog_debug(VD_SSH, "send SSH2_MSG_SERVICE_ACCEPT here\n");
		packet_start(SSH2_MSG_SERVICE_ACCEPT, ssh_packet);
		packet_put_cstring(service, ssh_packet);
		packet_send(ssh_packet);
		packet_write_wait_ipc(ssh_packet);
	} else {
		packet_disconnect(ssh_packet,"bad service request %s", service);
	}
	xfree(service);
}

/*ARGSUSED*/
static void
input_userauth_request(int type, u_int32_t seq, void *ctxt, struct ssh_packet *ssh_packet)
{
	Authctxt *authctxt = ctxt;
	Authmethod *m = NULL;
	char *user, *service, *method, *style = NULL;
	int authenticated = 0;
	struct login_session *session = ssh_packet->session;
	
	zlog_debug(VD_SSH, "input_userauth_request\n");
	if (authctxt == NULL)
		fatal("input_userauth_request: no authctxt");

	user = packet_get_string(NULL, ssh_packet);
	service = packet_get_string(NULL, ssh_packet);
	method = packet_get_string(NULL, ssh_packet);
	zlog_debug(VD_SSH, "userauth-request for user %s service %s method %s", user, service, method);
	zlog_debug(VD_SSH, "attempt %d failures %d", authctxt->attempt, authctxt->failures);
	
	if ((style = strchr(user, ':')) != NULL)
		{
		*style++ = 0;
		}

	if (authctxt->attempt++ == 0) {
		/* setup auth context */
//		authctxt->login_user = vtysh_user_lookup(user);
		
		authctxt->user = ssh_xstrdup(user);
		strcpy(session->name_buf, user);
#if 0
		if (authctxt->login_user && strcmp(service, "ssh-connection")==0) {
			authctxt->valid = 1;
		zlog_debug(VD_SSH, "input_userauth_request: setting up authctxt for %s", user);
		} else {
			zlog_debug(VD_SSH, "input_userauth_request: invalid user %s", user);  //gl
		}
#endif
		authctxt->service = ssh_xstrdup(service);
		authctxt->style = style ? ssh_xstrdup(style) : NULL;
	} else if (strcmp(user, authctxt->user) != 0 ||
	    strcmp(service, authctxt->service) != 0) {
	    
		packet_disconnect(ssh_packet, "Change of username or service not allowed: "
		    "(%s,%s) -> (%s,%s)",
		    authctxt->user, authctxt->service, user, service);
	}

	authctxt->postponed = 0;

	/* try to authenticate user */
	m = authmethod_lookup(method);
	if (m != NULL && authctxt->failures < options.max_authtries) {
		zlog_debug(VD_SSH, "input_userauth_request: try method %s", method);
		authenticated =	m->userauth(authctxt, ssh_packet);
	}
	zlog_debug(VD_SSH, "authenticated:%d\n", authenticated);
	userauth_finish(authctxt, authenticated, method, ssh_packet);

	xfree(service);
	xfree(user);
	xfree(method);
}

void
userauth_finish(Authctxt *authctxt, int authenticated, char *method, struct ssh_packet *ssh_packet)
{
	char *methods;

	zlog_debug(VD_SSH, "userauth_finish\n");
//	if(!authenticated)
//		fatal("INTERNAL ERROR: authenticated invalid user %s",
//		    authctxt->user);

	if (authctxt->postponed)
	{
		return;
	}

	/* XXX todo: check if multiple auth methods are needed */
	if (authenticated == 1) {
		/* turn off userauth */
		dispatch_set(SSH2_MSG_USERAUTH_REQUEST, &dispatch_protocol_ignore, ssh_packet);
		packet_start(SSH2_MSG_USERAUTH_SUCCESS, ssh_packet);
		zlog_debug(VD_SSH, "SSH2_MSG_USERAUTH_SUCCESS sent\n");
		packet_send(ssh_packet);
		packet_write_wait_ipc(ssh_packet);

		/* now we can break out */
		authctxt->success = 1;
	} else {

		/* Allow initial try of "none" auth without failure penalty */
		if (authctxt->attempt > 1 || strcmp(method, "none") != 0)
			authctxt->failures++;
		if (authctxt->failures >= options.max_authtries) {
			packet_disconnect(ssh_packet, AUTH_FAIL_MSG, authctxt->user);
		}
		methods = authmethods_get();
		zlog_debug(VD_SSH, "methods:%s\n", methods);
		packet_start(SSH2_MSG_USERAUTH_FAILURE, ssh_packet);
		packet_put_cstring(methods, ssh_packet);
		packet_put_char(0, ssh_packet);	/* XXX partial success, unused */
		packet_send(ssh_packet);
		packet_write_wait_ipc(ssh_packet);
		xfree(methods);
	}
}

static char *
authmethods_get(void)
{
	Buffer b;
	char *list;
	int i;
	buffer_init(&b);
	for (i = 0; authmethods[i] != NULL; i++) {
		if (strcmp(authmethods[i]->name, "none") == 0)
			continue;
		if (authmethods[i]->enabled != NULL &&
		    *(authmethods[i]->enabled) != 0) {
			if (buffer_len(&b) > 0)
				buffer_append(&b, ",", 1);
			buffer_append(&b, authmethods[i]->name,
			    strlen(authmethods[i]->name));
		}
	}
	buffer_append(&b, "\0", 1);
//	list = ssh_xstrdup(buffer_ptr(&b));
	list = strdup(buffer_ptr(&b));
	ssh_buffer_free(&b);
	return list;
}

static Authmethod *
authmethod_lookup(const char *name)
{
	int i;

	if (name != NULL)
		for (i = 0; authmethods[i] != NULL; i++)
			if (authmethods[i]->enabled != NULL &&
			    *(authmethods[i]->enabled) != 0 &&
			    strcmp(name, authmethods[i]->name) == 0)
				return authmethods[i];
	zlog_debug(VD_SSH, "Unrecognized authentication method name: %s",
	    name ? name : "NULL");
	return NULL;
}

