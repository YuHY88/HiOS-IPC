#ifndef _VTYSH_TELNET_H
#define _VTYSH_TELNET_H

#include "vty.h"
#include "vtysh_session.h"

#define CONTROL(X)  ((X) - '@')
#define VTY_NORMAL     0
#define VTY_PRE_ESCAPE 1
#define VTY_ESCAPE     2

extern void *vty_telnet_write (void *arg);
extern void *vty_telnet_read (void *arg);
extern struct vty *vty_telnet_create (struct login_session *session);


extern void vty_hist_add (struct vty *vty);
extern void vty_backward_char (struct vty *vty);
extern void vty_term_close (struct vty *vty);
extern void vty_will_echo (struct vty *vty);
extern void vty_will_suppress_go_ahead (struct vty *vty);
extern void vty_escape_map (unsigned char c, struct vty *vty);
extern void *vty_ssh_cmd(void *arg);
extern void vty_end_config (struct vty *vty);
extern void vty_stop_input (struct vty *vty);
extern void vty_sync_close (struct vty *vty);
struct login_session *
vty_telnet_thread_creat_connect(uint32_t client_ip, uint16_t client_port, uint32_t server_ip);
void vtysh_telnet_enable_set(int val);


#endif

