/*
    bgp register to ftm , prepare to receive message from ftm
*/
#include <string.h>
#include <stdlib.h>
#include <lib/log.h>
#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_tcp.h>
#include "bgpd/bgp_msg.h"
/* 注册要接收的协议报文类型 */
void bgp_pkt_register()
{
    union proto_reg proto;
	
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.sport = TCP_PORT_BGP;
    pkt_register(MODULE_ID_BGP, PROTO_TYPE_IPV4, &proto);
		
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_BGP;
    pkt_register(MODULE_ID_BGP, PROTO_TYPE_IPV4, &proto);
		
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.sport = TCP_PORT_BGP;
	proto.ipreg.type = PROTO_TYPE_IPV6;
    pkt_register(MODULE_ID_BGP, PROTO_TYPE_IPV6, &proto);
		
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_BGP;
	proto.ipreg.type = PROTO_TYPE_IPV6;
    pkt_register(MODULE_ID_BGP, PROTO_TYPE_IPV6, &proto);
	
}
void bgp_pkt_unregister()
{
    union proto_reg proto;
	
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.sport = TCP_PORT_BGP;
    pkt_unregister(MODULE_ID_BGP, PROTO_TYPE_IPV4, &proto);
	
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_BGP;
    pkt_unregister(MODULE_ID_BGP, PROTO_TYPE_IPV4, &proto);
	
	memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.sport = TCP_PORT_BGP;
	proto.ipreg.type = PROTO_TYPE_IPV6;
    pkt_unregister(MODULE_ID_BGP, PROTO_TYPE_IPV6, &proto);
	
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_BGP;
	proto.ipreg.type = PROTO_TYPE_IPV6;
    pkt_unregister(MODULE_ID_BGP, PROTO_TYPE_IPV6, &proto);
	
}
