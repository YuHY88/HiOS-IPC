#ifndef _ZEBRA_RIP_IPC_H
#define _ZEBRA_RIP_IPC_H
#define INADDR_RIP_GROUP        0xe0000009    /* 224.0.0.9 */
#define INADDR_BROAD_GROUP        0xffffffff    /* 255.255.255.255 */

extern void rip_pkt_register(void);

#endif

