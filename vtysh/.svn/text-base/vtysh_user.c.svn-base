/* User authentication for vtysh.
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

#include <zebra.h>
#include <lib/version.h>
#include <pwd.h>
#include "memory.h"
#include "linklist.h"
#include "vtysh_user.h"
#include "aaa_common.h"
#include "vty.h"
#include "vtysh.h"

/* Default motd string. */
static const char *default_motd =
"\r\n\
Hello, this is Huahuan (version IPRAN V001R001 ).\r\n\
";


void
vtysh_host_init(void)
{
//	struct host_users *user;
	/* Default host value settings. */
	host.device_name = XSTRDUP (MTYPE_HOST, "Hios");
	
	host.enable = XSTRDUP (MTYPE_HOST, HOST_ENABLE_PASSWORD);
	host.config = XSTRDUP (MTYPE_HOST, VTYSH_STARTUP_CONFIG);
	host.lines = -1;	

	host.banner = XSTRDUP (MTYPE_HOST, default_motd);

	host.idle_max = AAA_DEF_IDLE_MAX*60;	//Ä¬ÈÏ5·ÖÖÓ
}
#if 0
struct host_users *
vtysh_user_lookup (const char *name)
{
  struct listnode *node, *nnode;
  struct host_users *user;
  char *name_tmp = strdup(name);

  for (ALL_LIST_ELEMENTS (host.registe_user, node, nnode, user))
  {
  if(user != NULL)
    if (strcmp (user->name, name_tmp) == 0)
			return user;
  }
  return NULL;
}

struct host_users *
vtysh_user_get (const char *name)
{
  struct host_users *user;
  user = vtysh_user_lookup (name);
  if (user)
    return user;

  user = vtysh_user_creat ();
  user->name = strdup (name);
  listnode_add (host.registe_user, user);

  return user;
}
#endif

char *
vtysh_get_home (void)
{
  struct passwd *passwd;

  passwd = getpwuid (getuid ());

  return passwd ? passwd->pw_dir : NULL;
}
#if 0
void 
vtysh_user_add()
{
	struct host_users *users;

	users = NULL;
	if(! host.registe_user)
	{
		host.registe_user = list_new ();
		host.registe_user->del = (void (*) (void *))vtysh_user_delete;
		host.registe_user->cmp = 	NULL;
	}

	users->name = XSTRDUP (MTYPE_HOST, HOST_NAME);
	users->password = XSTRDUP (MTYPE_HOST, HOST_PASSWORD); 

	listnode_add (host.registe_user, users);
}
#endif
struct host_users *
vtysh_user_creat()
{
  struct host_users *user;
  user = XCALLOC (MTYPE_HOST, sizeof (struct host_users));
	user->name = NULL;
	user->password = NULL;
	user->password_encrypt = NULL;
	user->enable_password = NULL;
	user->enable_password_encrypt = NULL;
	user->encrypt = 0;
	user->login_cnt =0;

	if(pthread_mutex_init(&user->login_cnt_lock, NULL) != 0)
	{
		zlog_debug(VD_COMM, "obuf_lock init failed\n");
	}

  return user;
}

int 
vtysh_auth_passwd (struct host_users *user, const char *password)
{
	char *passwd = NULL;
	bool fail;

	if(!user)
	{
		return 0;
	}
	if(!password)
	{
		return 0;
	}

	if (strcmp(user->encrypt ? user->password_encrypt : user->password, "") == 0 
		&& strcmp(password, "") == 0)
		return (1);

	if(user->encrypt)
	{
		passwd = user->password_encrypt;
		fail = (strcmp (crypt(password, passwd), passwd) == 0);
	}
	else
	{
		passwd = user->password;
		fail = (strcmp (password, passwd) == 0);
	}

	return fail;
}

void
vtysh_user_delete (struct host_users *users)
{
  if (users->name)
		XFREE (MTYPE_HOST, users->name);
	if (users->password)
		XFREE (MTYPE_HOST, users->password);
	if (users->password_encrypt)
		XFREE (MTYPE_HOST, users->password_encrypt);
	if (users->enable_password)
		XFREE (MTYPE_HOST, users->enable_password);
	if (users->enable_password_encrypt)
		XFREE (MTYPE_HOST, users->enable_password_encrypt);

	XFREE (MTYPE_HOST, users);
}

