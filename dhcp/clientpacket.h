#ifndef _CLIENTPACKET_H
#define _CLIENTPACKET_H

#include "packet.h"
#include "dhcp_if.h"
unsigned long random_xid(void);
int send_discover(struct if_dhcp *pif_client, unsigned long xid, unsigned long requested, struct option124_vlan_ifvalid option124_vlan);
int send_selecting(struct if_dhcp *pif_client, unsigned long xid, unsigned long server, unsigned long requested, struct option124_vlan_ifvalid option124_vlan);
int send_renew(struct if_dhcp *pif_client, unsigned long xid, unsigned long server, unsigned long ciaddr);
int send_release(struct if_dhcp *pif_client, unsigned long server, unsigned long ciaddr);
int send_decline(struct if_dhcp *pif_client, unsigned long server, unsigned long requested);
int get_raw_packet(struct dhcpMessage *payload, int fd);
char *show_dhcp_option60(void);
int dhcp_option60_set(char *str);
int get_object_id(char *value);
#endif
