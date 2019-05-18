/*
    RIP register to ftm , prepare to recive message from ftm

*/

#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include <string.h>
//#include <arpa/inet.h>
#include "rip_ipc.h"

void rip_pkt_register(void)
{
    union proto_reg proto;
    uint8_t ipv6[16] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09};

    /* RIP 组播报文 */
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.dip = INADDR_RIP_GROUP;
//  proto.ipreg.type = PROTO_TYPE_IPV4;
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_RIP;
    pkt_register(MODULE_ID_RIP, PROTO_TYPE_IPV4, &proto);

    /* RIP 广播报文 */
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.dip = INADDR_BROAD_GROUP;
//  proto.ipreg.type = PROTO_TYPE_IPV4;
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_RIP;
    pkt_register(MODULE_ID_RIP, PROTO_TYPE_IPV4, &proto);

    /* RIP 单播报文 */
    memset(&proto, 0, sizeof(union proto_reg));
//  proto.ipreg.type = PROTO_TYPE_IPV4;
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_RIP;
    pkt_register(MODULE_ID_RIP, PROTO_TYPE_IPV4, &proto);


    /* RIPNG 组播报文 */
    memset(&proto, 0, sizeof(union proto_reg));
    IPV6_ADDR_COPY(&proto.ipreg.dipv6, &ipv6);
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.type = PROTO_TYPE_IPV6;
    proto.ipreg.dport = 521;
    proto.ipreg.sport = 521;
    proto.ipreg.dipv6_valid = 1;
    pkt_register(MODULE_ID_RIP, PROTO_TYPE_IPV6, &proto);
}


