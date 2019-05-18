#ifndef _PACKET_H
#define _PACKET_H

//#include <netinet/udp.h>
#include <netinet/ip.h>

#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>

/* Octets in one Ethernet addr, from <linux/if_ether.h> */
//#define ETH_ALEN	                        6
#define VLAN_NONE				            0
#define OPTION_VERDOR_IDENTIFY_LEN 			 10/*Huahuan option 124 length*/

struct dhcpMessage {
	uint8_t op;/* 消息类别 BOOTREQUEST ＝ 1 BOOTREPLY ＝ 2 */
	uint8_t htype;/* 硬件类型代码 */
	uint8_t hlen;/* 硬件地址长度 */
	uint8_t hops;/* 客户端清0,DHCP中继服务器在提供中继服务的时候使用 */
	uint32_t xid;
	uint16_t secs;/* 客户进入IP地址申请进程的时间或者更新IP地址进程的时间 */
	uint16_t flags;/* 标志字段 */
	uint32_t ciaddr;/* 客户的IP地址 */
	uint32_t yiaddr;/* DHCP服务器分配给客户端的IP地址 */
	uint32_t siaddr;/* 服务器的地址 */
	uint32_t giaddr;/* DHCP中继器地址 */
	uint8_t chaddr[16];/* 客户端的硬件地址 */
	uint8_t sname[64];/* 服务器的主机名 */
	uint8_t file[128];/* 启动文件名字 */
	uint32_t cookie;
	uint8_t options[308]; /* 312 - cookie */
};

struct vendorsub_message{
	uint8_t *optionptr;
	uint8_t length;
};

struct udp_dhcp_packet {
	struct iphdr ip;
	struct udphdr udp;
	struct dhcpMessage data;
};
struct udp_dhcp_packet_vlan {
       u_int16_t tci; 
       uint16_t ether_type;
	struct udp_dhcp_packet dhcp_packet;
};

struct vendor_124
{
	char vendor, length;
	char str[OPTION_VERDOR_IDENTIFY_LEN];
};

struct option124_vlan_ifvalid
{
	int vlanid;
	int ifvalid;/*ifvalid 0 mean tag, 1 mean utag*/
};
extern char *inet_ntoa(struct in_addr in);
void init_header(struct dhcpMessage *packet, char type);
uint16_t checksum(void *addr, int count);
int raw_packet(struct dhcpMessage *payload, uint32_t source_ip, int source_port,
		   uint32_t dest_ip, int dest_port, uint8_t *dest_arp, int ifindex, struct option124_vlan_ifvalid option124_vlan, int zero);
int kernel_packet(struct dhcpMessage *payload, uint32_t source_ip, int source_port,
		   uint32_t dest_ip, int dest_port);
uint16_t dhcp_vlan_get(void);
int check_packet(struct dhcpMessage *packet);

#endif
