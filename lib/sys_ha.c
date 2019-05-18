#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <devm/devm.h>
#include <devm/devm_ha.h>
#include <syslog/syslogd.h>
#include "sys_ha.h"
#include "msg_ipc.h"

extern int eth1_sock;
extern pthread_mutex_t ha_mutex;   /* HA 通道发包互斥信号量 */

extern struct sockaddr_in eth1_myboard_serveraddr;
extern struct sockaddr_in eth1_otherboard_serveraddr;
extern struct sockaddr_in eth1_myslot_serveraddr;
extern struct sockaddr_in eth1_send_serveraddr;

extern struct devm device;


