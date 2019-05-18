/* 
* define of packet header
*/


#ifndef HIOS_PACKET_DEF_H
#define HIOS_PACKET_DEF_H

#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/inet_ip.h>



/* 协议报文类型 */
enum PROTO_TYPE
{
	PROTO_TYPE_INVALID = 0,
	PROTO_TYPE_ETH,
	PROTO_TYPE_MPLS,
	PROTO_TYPE_IPV4,	
	PROTO_TYPE_IPV6,
	PROTO_TYPE_RAW,
	PROTO_TYPE_MAX = 8
};


struct ip_proto
{	
	struct ipv6_addr dipv6;
	struct ipv6_addr sipv6;
	enum PROTO_TYPE type;	 /* ipv6注册时需下发type =PROTO_TYPE_IPV6,ipv4暂不用下发*/
	uint8_t  protocol;      /* IP 层协议号 */
	uint8_t  icmpv6_type;   /* icmpv6_type ,protocol为 IP_P_IPV6_ICMP*/
	uint8_t  dipv6_valid;   /* 1: dipv6 有效 */
	uint8_t  sipv6_valid;   /* 1: sipv6 有效 */
	uint8_t  icmpv6_type_valid;   /*1:  icmpv6_type 有效protocol为 IP_P_IPV6_ICMP 时有用*/
	uint8_t  flag;           	/* 0:lookup  1:add/delete    */
	uint16_t sport;         	/* udp 或 tcp 的源端口 */
	uint16_t dport;         	/* udp 或 tcp 的目的端口 */
	uint32_t dip;
	uint32_t sip;
	uint32_t acl_index;      	/* acl index    */
};


/* 24 bytes */
struct eth_proto
{
	uint8_t  dmac[6];
	uint8_t  smac[6];
	uint8_t  dmac_mask[6]; /* dstmac mask */
	uint8_t  dmac_valid;   /* dmac 有效 */
	uint8_t  smac_valid;   /* smac 有效 */	
	uint8_t  sub_ethtype;  /* 以太层 subtype 或者 oam_level, ethtype 后第一个字节;对 802.3 帧类型, ethtype 后二个字节  */
	uint8_t  oam_opcode;   /* oam opcode, ethtype 后第二个字节 */
	uint8_t  flag;           /* 0:lookup  1:add/delete    */
	uint16_t ethtype;      /* 以太层协议类型, mac 后两个字节 */	
	uint16_t vlan;         	
	uint16_t cvlan;
	uint32_t  acl_index;      /* acl index    */
};


/* 20 bytes */
struct mpls_proto
{
	enum PKT_INIF_TYPE if_type;	/* 入接口类型，只支持 pw 和 lsp */	
	uint8_t  protocol;  			 /* IP 层协议号 */
	uint8_t  oam_opcode; 			/* 控制字后的第二个字节 */
	uint8_t  ttl;        			/* 内层标签的 ttl */
	uint8_t  out_ttl;        		/* 外层标签的 ttl */
	uint8_t  flag;           		/* 0:lookup  1:add/delete    */
	uint16_t chtype;     			/* chanel type, 控制字的最后 2 个字节 */	
	uint16_t dport;      			/* udp 或 tcp 的目的端口 */	
	uint16_t sport;      			/* udp 或 tcp 的目的端口 */	
	uint32_t dip;	     			/* 目的 ip */	
	uint32_t inlabel_value; 		/* 内层标签 */
	uint32_t acl_index;      		/* acl index    */
};


/* 注册协议报文的参数 */
union proto_reg
{
	struct ip_proto ipreg;
	struct eth_proto ethreg;
	struct mpls_proto mplsreg;
};



/* app call this function to register packet type that will be received */
int pkt_register(int module_id, enum PROTO_TYPE type, union proto_reg *proto);

/* app call this function to cancel registerd packet type */
void pkt_unregister(int module_id, enum PROTO_TYPE type, union proto_reg *proto);

extern int pkt_open_tcp(int module_id, struct ip_proto *ptcp); /* 创建 tcp 连接 */
extern int pkt_close_tcp(int module_id, struct ip_proto *ptcp);/* 关闭 tcp 连接 */


#endif








