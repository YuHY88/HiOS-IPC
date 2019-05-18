/* 
*            define of icmp packet
*
*/
#ifndef HIOS_PKT_ICMP_H
#define HIOS_PKT_ICMP_H

#include "pkt_ip.h"
#include "ftm.h"


#define ICMP_HEADER_SIZE   8   /* ICMP 头长度 */
#define ICMP_RESERVE_SIZE  4


/*
 *  ICMP types define
 */
#define ICMP_TYPE_ECHOREPLY          0       /* Echo Reply                   */
#define ICMP_TYPE_UNREACH            3       /* Destination Unreachable      */
#define ICMP_TYPE_SOURCE_QUENCH      4       /* Source Quench                */
#define ICMP_TYPE_REDIRECT           5       /* Redirect (change route)      */
#define ICMP_TYPE_ECHO               8       /* Echo Request                 */
#define ICMP_TYPE_ROUTER_ADVERT      9
#define ICMP_TYPE_ROUTER_SOLICIT     10
#define ICMP_TYPE_TIMEOUT            11      /* Time Exceeded                */
#define ICMP_TYPE_PARA_PROBLEM       12      /* Parameter Problem            */
#define ICMP_TYPE_TIMESTAMP_REQ      13      /* Timestamp Request            */
#define ICMP_TYPE_TIMESTAMP_REPLY    14      /* Timestamp Reply              */
#define ICMP_TYPE_INFO_REQUEST       15      /* Information Request          */
#define ICMP_TYPE_INFO_REPLY         16      /* Information Reply            */
#define ICMP_TYPE_MASK_REQ           17      /* Address Mask Request         */
#define ICMP_TYPE_MASK_REPLY         18      /* Address Mask Reply           */

/*
 *  ICMP codes define
 */
#define     ICMP_CODE_NET                0
#define     ICMP_CODE_HOST               1
#define     ICMP_CODE_PROTOCOL           2
#define     ICMP_CODE_PORT               3
#define     ICMP_CODE_NEEDFRAG           4
#define     ICMP_CODE_SRCFAIL            5
#define     ICMP_CODE_NET_UNKNOWN        6
#define     ICMP_CODE_HOST_UNKNOWN       7
#define     ICMP_CODE_ISOLATED           8
#define     ICMP_CODE_NET_PROHIB         9
#define     ICMP_CODE_HOST_PROHIB        10
#define     ICMP_CODE_TOSNET             11
#define     ICMP_CODE_TOSHOST            12
#define     ICMP_CODE_FILTER_PROHIB      13
#define     ICMP_CODE_HOST_PRECEDENCE    14
#define     ICMP_CODE_PRECEDENCE_CUTOFF  15

/*ICMPV6 type*/
#define ICMPV6_TYPE_ECHO   128
#define ICMPV6_TYPE_REPLY  129




/* icmp 报文格式 */
struct icmphdr 
{
  uint8_t		type;         /* icmp type */
  uint8_t		code;         /* icmp code */
  uint16_t		checksum;
  uint16_t      id;           /* 唯一标识报文的 ID */
  uint16_t      sequence;     /* 报文发送序列 */
  uint8_t       icmp_data[];  
};

struct icmpv4_hdr
{	
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
};


/*ICMPv6 通用报文头*/
struct icmpv6_hdr
{
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
};


/* icmp 报文接收 */
int icmp_rcv(struct pkt_buffer *pkt);

/* icmp 报文发送 */
int icmp_send_pkt(uint8_t *icmph, int datalen, struct ip_control *ipcb);

/*icmpv6 报文ipc发送到模块*/
void icmpv6_pkt_send_to_ping(struct pkt_buffer *pkt);

/*icmpv6报文接收*/
int icmpv6_rcv(struct pkt_buffer *pkt);

int icmp_send(struct pkt_buffer *pkt, uint8_t type, uint8_t code);

#endif

