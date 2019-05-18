#ifndef _SERVERPACKET_H
#define _SERVERPACKET_H

#include "packet.h"

int sendOffer(struct dhcpMessage *oldpacket, struct dhcpOfferedAddr *offeredaddr);
int sendNAK(struct dhcpMessage *oldpacket);
int sendACK(struct dhcpMessage *oldpacket, uint32_t yiaddr,struct dhcpOfferedAddr *offeredaddr);
int send_inform(struct dhcpMessage *oldpacket);


#endif
