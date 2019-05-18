/*
*    define of ipv6 packet
*/


#ifndef HIOS_PKT_IPV6_H
#define HIOS_PKT_IPV6_H

#include <lib/pkt_buffer.h>
#include <lib/types.h>


#define IPV6_HEADER_SIZE            40    /* IPV6 头长度 */
#define IPV6_PSEUDO_HDR_SIZE          36    /* IPv6 伪头长度，用于计算 icmpv6/udp/tcp checksum */

#define IPV6_MIN_MTU	            1280
#define IPV6_DEFAULT_HOPLIMIT       64
#define IPV6_DEFAULT_MCASTHOPS	    1


/* ipv6 报文头格式, 40 bytes */
struct ipv6_hdr
{
#if BIG_ENDIAN_ON
	uint8_t		version:4, /* 版本号，固定为 6 */
				tos1:4;    /* 优先级的低 4 位 */
#else                      
	uint8_t		tos1:4,
				version:4;
#endif

#if BIG_ENDIAN_ON
	uint8_t 	tos2:4,    /* 优先级的高 4 位 */
				flabel1:4; /* 流标签的低 4 位 */
#else                      
	uint8_t 	flabel1:4,
				tos2:4;
#endif

	uint16_t	flabel2;    /* 流标签的高 16 位 */
	uint16_t	data_len;   /* payload 部分的长度，包括扩展头长度，以字节为单位 */
	uint8_t		protocol;   /* ip 层协议号,跟 ipv4 protocol 定义相同, 有扩展头时，则标志第一个扩展头的类型 */
	uint8_t		ttl;
	struct	ipv6_addr	saddr; /* 源 ip */
	struct	ipv6_addr	daddr; /* 目的 ip */
};


/*
 *	routing header
 */
struct ipv6hdr_rt
{
	uint8_t 	next_hdr;
	uint8_t 	hlen;
	uint8_t		type;
	uint8_t		seg_left;

	/*
	 *	type specific data
	 *	variable length field
	 */
};


/* ipv6 选项 */
struct ipv6hdr_opt
{
	uint8_t 		next_hdr;
	uint8_t 		hlen;
	
	/* 
	 * TLV encoded option data follows.
	 */
};

/*ipv6 icmpv6, tcp, udp 计算校验和使用的伪首部结构，36 字节 */
struct pseudo_ipv6hdr
 {
	struct	ipv6_addr	saddr; /* 源 ip */
	struct	ipv6_addr	daddr; /* 目的 ip */
	uint8_t    placeholder; /* 占位符 */
	uint8_t    protocol;    /* ipv6  协议 protocol*/
	uint16_t   length;      /* icmpv6/udp/tcp 报文长度 */
 };



/* ipv6 报文处理函数 */
int  ipv6_rcv(struct pkt_buffer *pkt);
int  ipv6_xmit(struct pkt_buffer *pkt);
int  ipv6_xmit_unicast(struct pkt_buffer *pkt);  /* 单播报文查 nd 发送 */
int  ipv6_encap(struct pkt_buffer *pkt);  /* 封装 ip 头 */
int  ipv6_decap(struct pkt_buffer *pkt);  /* 解封装 ip 头 */

#endif

