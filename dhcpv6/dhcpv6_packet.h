#ifndef HIOS_DHCP6_PACKET_H
#define HIOS_DHCP6_PACKET_H

struct option_ia_na{
	uint32_t iaid;
	uint32_t t1;
	uint32_t t2;
};

struct option_ia_address{
	uint8_t ipv6_address[16];
	uint32_t pre_time;
	uint32_t valid_time;
};

struct option_ia_prefix_address{
	uint32_t pre_time;
	uint32_t valid_time;
	uint8_t prefix_len;
	uint8_t prefix_address[16];
};
struct option_ia{
	struct option_ia_na ia_na;
	uint16_t code;
	uint16_t len;
	union{
		struct option_ia_address ia_ad;
		struct option_ia_prefix_address ia_pd;
	}ia_suboption;
	//uint16_t code2;
	//uint16_t len2;
	//uint16_t status_code;
};

struct dhcpv6_message{
	uint8_t msgtype;/* 消息类别 */
	uint32_t xid;
	uint8_t options[160];
};
struct udp_dhcpv6_packet
{
	struct ipv6_hdr ip;
	struct udphdr udp;
	//uint8_t option[236];//sizeof(struct dhcp6opt) + sizeof(struct dhcp6_optinfo)
	uint8_t data[254];
};

int check_dhcpv6_packet(struct pkt_buffer *pkt);
uint16_t in_checksum (uint16_t *ptr, int nbytes);
int send_multicast_packet(uint8_t *payload, uint32_t ifindex, int len);
int create_ipv6_addr(int ifindex, char *addr);
int dhcp_if_get_linklocal(uint32_t ifindex, uint8_t *addr);

#endif
