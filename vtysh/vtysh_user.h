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

#ifndef _VTYSH_USER_H
#define _VTYSH_USER_H

#include "command.h"
#include "aaa/aaa.h"



extern void vtysh_user_add();
extern struct host_users *vtysh_user_lookup (const char *name);
extern struct host_users *vtysh_user_get (const char *name);
extern struct host_users *vtysh_user_creat(void);
extern void vtysh_user_delete (struct host_users *users);
extern void vtysh_host_init(void);
extern int vtysh_auth_passwd (struct host_users *user, const char *password);

extern char *vtysh_get_home (void);

#endif /* _VTYSH_USER_H */
