
#ifndef HIOS_PKT_UDP_H
#define HIOS_PKT_UDP_H

#include <lib/pkt_buffer.h>
#include "lib/msg_ipc.h"

#define UDP_HEADER_SIZE        8  /* udp 头长度 */


/* udp 端口定义 */
#define UDP_PORT_ECHO           7
#define UDP_PORT_DISCARD        9
#define UDP_PORT_NETSTAT        15
#define UDP_PORT_FTP_DATA       20
#define UDP_PORT_FTP_CTRL       21
#define UDP_PORT_SSH            22     /* ssh */
#define UDP_PORT_TELNET         23     /* telnet */
#define UDP_PORT_SMTP           25
#define UDP_PORT_RSFTP          26
#define UDP_PORT_RLP            39
#define UDP_PORT_WINS           42
#define UDP_PORT_TACAS          49     /* tacas */
#define UDP_PORT_DNS            53     /* dns */
#define UDP_PORT_MTP            57
#define UDP_PORT_DHCP_SERVER    67     /* dhcp server */
#define UDP_PORT_DHCP_CLIENT    68     /* dhcp client */
#define UDP_PORT_TFTP           69     /* tftp */
#define UDP_PORT_NTP            123    /* ntp */
#define UDP_PORT_SNMP           161    /* snmp */
#define UDP_PORT_SNMP_TRAP      162    /* snmp trap */
#define UDP_PORT_HTTPS          443
#define UDP_PORT_SYSLOG         514    /* syslog */
#define UDP_PORT_RIP            520
#define UDP_PORT_DHCPV6_CLIENT  546
#define UDP_PORT_DHCPV6_SERVER  547
#define UDP_PORT_RTSP           554
#define UDP_PORT_LDP            646    /* ldp */
#define UDP_PORT_L2TP           1701
#define UDP_PORT_PPTP           1723
#define UDP_PORT_RADIUS_AUTH    1812   /* radius authenticate */
#define UDP_PORT_RADIUS_ACCT    1813   /* radius accounting */
#define UDP_PORT_HSRP           1985
#define UDP_PORT_MGCP           2427
#define UDP_PORT_LSPING_REQ     3503   /* lsp ping packet */
#define UDP_PORT_LSPING_REPLY   3053   /* lsp ping packet */
#define UDP_PORT_BFD            3784   /* bfd packet */
#define UDP_PORT_RTP            5004

#define UDP_PORT_RESERVE        49152   /* 保留 UDP 端口范围的起始值 */
#define UDP_PORT_RFC2544        49184   /* 2544 packet */
#define UDP_PORT_MAX            65535   /* UDP 端口的最大值 */


extern struct hash_table udp_session_table;	 /* tcp session hash table */

/* UDP 头，8 bytes */
struct udphdr 
{
	uint16_t	sport;    /* destination port*/
	uint16_t	dport;    /* source port*/
	uint16_t	len;      /* udp 头长度 + payload 长度*/
	uint16_t	checksum;
};

/* udp session struct*/
struct udp_session
{
	int         module_id;  /* module id*/
	uint32_t    vpn;        /* ip 所属的 l3vpn */	
	uint32_t    sip;        /* 对端设备的 ip */
	uint32_t    dip;	    /* 本机的接口 ip */
	struct ipv6_addr dipv6;
	struct ipv6_addr sipv6;
	uint16_t    sport;      /* 对端的 tcp 发送端口 */
	uint16_t    dport;      /* 本机的 tcp 接收端口 */
	u_char      v6session;
	uint8_t 	timeout; 		
};


void udp_init(void);
int  udp_forward(struct pkt_buffer *pkt);
int  udp_rcv(struct pkt_buffer *pkt);
int  udp_send(void *data, int data_len, struct ip_control *ipcb);
int  udp_port_alloc_for_app(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint16_t sender_id, uint8_t opcode);

#endif

