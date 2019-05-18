/*
*       manage the static arp table
*
*/

#ifndef HIOS_ARP_CMD_H
#define HIOS_ARP_CMD_H

#include <route/arp_static.h>

/*set garp.num backup ,then set garp.num=0,and then restore garp.num to backup*/
#define ROUTE_ARP_DELETE_ALL_DYNAMIC()\
    backup_numlimit = g_arp.num_limit;\
    g_arp.num_limit = 0;\
    ipc_send_msg_n2(&g_arp, sizeof(struct arp_global), 1, MODULE_ID_FTM,MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_NUM_LIMIT, 0, 0);\
    g_arp.num_limit = backup_numlimit;\
    ipc_send_msg_n2(&g_arp, sizeof(struct arp_global), 1, MODULE_ID_FTM,MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_NUM_LIMIT, 0, 0);

#define ARP_DEBUG_LOG  		0x01    //for tdm interface configuration  
#define ARP_DEBUG_OTHER     0x02    //reserve
#define ARP_DEBUG_ALL       0x07    //for all debug info

void arp_static_cmd_init(void);
void arp_cmd_init(void);

#endif

