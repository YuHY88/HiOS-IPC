#ifndef VTYSH_AUTH_H
#define VTYSH_AUTH_H
#include <time.h>


int vty_auth (struct vty *vty, char *buf);
int vty_auth_remote(struct login_session *session);
int vty_remote_aaa(struct login_session *session);
//int vtysh_check_idle_user(struct thread *thread);
int vtysh_check_idle_user(void *para);
int vty_remote_aaa_logout(struct login_session *session);
void vtysh_handle_aaa_force_exit(AAA_FORCE_EXIT *pmsg);
extern void vtysh_handle_aaa_login_response(AAA_LOGIN_RESPONSE *pmsg);
extern time_t vty_get_thread_idle_time(struct login_session *session);
extern int vtysh_auth_remote_ssh(struct login_session *session);

void vtysh_handle_aaa_max_idle_time(AAA_MAX_IDLE_MSG *pdata);


int vty_auth_local(struct login_session *session);

#endif
