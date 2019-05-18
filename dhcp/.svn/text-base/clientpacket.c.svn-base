
#include <string.h>
#include <features.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
//#include <netinet/in.h>
#include <fcntl.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/devm_com.h>

#include "dhcp_server.h"
#include "options.h"
#include "dhcp_client.h"
#include "dhcp_if.h"
#include "clientpacket.h"

#define DHCP_VERSION "0.9.9-pre"


/* Create a random xid */
unsigned long random_xid(void)
{
	static int initialized;
	if (!initialized)
	{
		int fd;
		unsigned long seed;

		fd = open("/dev/urandom", 0);
		if (fd < 0 || read(fd, &seed, sizeof(seed)) < 0)
		{
			DHCP_LOG_ERROR("Could not load seed from /dev/urandom: %m");
			seed = time(0);
		}
		if (fd >= 0) close(fd);
		srand(seed);
		initialized++;
	}
	return rand();
}

/* set option 60 value is devid*/
int get_object_id(char *value)
{
	int devid_tmp = 0;
	int devid[11];
	uint32_t i, len;
	char option60_value[30]={0};

	//get object id
	if(0 == devm_comm_get_id(1, 0, MODULE_ID_DEVM, (unsigned int *)&devid_tmp))
	{
		devid[0] = 1;
		devid[1] = 3;
		devid[2] = 6;
		devid[3] = 1;
		devid[4] = 4;
		devid[5] = 1;
		devid[6] = 9966;
		devid[7] = 2;
		devid[8] = 20;
		devid[9] = 201;
		devid[10] = devid_tmp;
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "in function '%s'-> dhcp get devid is 0X%d ok\n", __func__, devid_tmp);
	}
	else
	{
		zlog_err("%s[%d]: in function '%s'-> dhcp get devid failed\n", __FILE__, __LINE__, __func__);
		devid[0] = 0;
	}
	for(i = 0; i < (sizeof(devid)/4); i++)
	{
		len = strlen(option60_value);
		if (i != ((sizeof(devid)/4) - 1))
		{
			sprintf(option60_value + len, "%d.",devid[i]);
		} else {
			sprintf(option60_value + len, "%d",devid[i]);
		}
	}
	if (option60_value != NULL)
	{
		memcpy(value, &option60_value, 30);
	}
	return 0;
}

#ifdef HUAHUAN_DHCP
	//TODO: here must confirm where to get the verndor ID
		struct vendor  {
			char vendor, length;
			char str[sizeof(HUAHUAN_VENDOR_ID_STR)];
		} vendor_id = { DHCP_VENDOR,  sizeof(HUAHUAN_VENDOR_ID_STR) - 1, HUAHUAN_VENDOR_ID_STR};
#else
		struct vendor  {
			char vendor, length;
			char str[sizeof("udhcp "DHCP_VERSION)];
		} vendor_id = { DHCP_VENDOR,  sizeof("udhcp "DHCP_VERSION) - 1, "udhcp "DHCP_VERSION};
#endif

int dhcp_option60_set(char *str)
{
	if (str == NULL) {
		return 1;
	}
	vendor_id.length = strlen(str);
	memset(vendor_id.str, 0, strlen(str) + 1);
	memcpy(vendor_id.str, str, strlen(str));
	return 0;
}

char *show_dhcp_option60(void)
{
	char *option60;
	if(vendor_id.length == 0)
	{
		DHCP_LOG_ERROR("vendor_id.length is 0!");
		return NULL;
	}
	option60 = XCALLOC(MTYPE_IF, vendor_id.length + 1);
	if (option60 == NULL)
	{
		DHCP_LOG_ERROR("XCALLOC option60 is failed!");
		return NULL;
	}

	memcpy(option60, vendor_id.str, vendor_id.length);
	return option60;
}

/* initialize a packet with the proper defaults */
static int init_packet(struct if_dhcp *pif_client ,struct dhcpMessage *packet, char type)
{
	uchar    pmac[6];
	uchar 	 mac[6];
	/*
#ifdef HUAHUAN_DHCP
	//TODO: here must confirm where to get the verndor ID
		struct vendor  {
			char vendor, length;
			char str[sizeof(HUAHUAN_VENDOR_ID_STR)];
		} vendor_id = { DHCP_VENDOR,  sizeof(HUAHUAN_VENDOR_ID_STR) - 1, devid};
#else
		struct vendor  {
			char vendor, length;
			char str[sizeof("udhcp "DHCP_VERSION)];
		} vendor_id = { DHCP_VENDOR,  sizeof("udhcp "DHCP_VERSION) - 1, "udhcp "DHCP_VERSION};
#endif
	*/
	/*
	char 	 option60_vendor[32]={'\0'};
	char	 option60_value[30]={'\0'};
	get_object_id(option60_value);
	LOG (LOG_DEBUG,"init_packet() get option60 value is %s\n", option60_value);	
	option60_vendor[OPT_CODE] = DHCP_VENDOR;
	option60_vendor[OPT_LEN] = sizeof(option60_value);
	memcpy(option60_vendor+2, &option60_value, sizeof(option60_value));
	*/
	if(pif_client == NULL || packet == NULL || type == 0)
	{
		DHCP_LOG_ERROR("Input valid is error!\n");
		return 1;
	}
	memset(mac,0,6);
	if(!memcmp(mac,pif_client->mac,6))
	{
		if(ifm_get_mac(pif_client->ifindex, MODULE_ID_DHCP, pmac) == 0)
		{
			memcpy(pif_client->mac,pmac,6);
		}
		else
		{
			DHCP_LOG_ERROR("init_packet(), cannot get mac on the interface %08X, return\n",pif_client->ifindex);	
			return 1;		
		}
	}
	
	memcpy(pif_client->c_ctrl->clientid+3,pif_client->mac,6);


	init_header(packet, type);
	memcpy(packet->chaddr, pif_client->mac, 6);
	
	add_option_string(packet->options, pif_client->c_ctrl->clientid);
	
	add_option_string(packet->options, pif_client->c_ctrl->hostname);
	add_option_string(packet->options, (uint8_t *) &vendor_id);
	//add_option_string(packet->options, (uint8_t *)&option60_vendor);
	return 0;
}

/* Add a parameter request list for stubborn DHCP servers. Pull the data
 * from the struct in options.c. Don't do bounds checking here because it
 * goes towards the head of the packet. */
static void add_requests(struct dhcpMessage *packet)
{
	int end = end_option(packet->options);
	int i, len = 0;

	packet->options[end + OPT_CODE] = DHCP_PARAM_REQ;
	for(i = 0; dhcp_options[i].code; i++)
	{
		if(dhcp_options[i].flags & OPTION_REQ)
		{
			packet->options[end + OPT_DATA + len++] = dhcp_options[i].code;
		}
	}
	packet->options[end + OPT_LEN] = len;
	packet->options[end + OPT_DATA + len] = DHCP_END;
}

/* initialize a packet with the proper defaults */
static int init_packet_by_ifindex(uint32_t ifindex ,struct dhcpMessage *packet, char type)
{
	uchar     pmac[6];
	
	struct if_dhcp *pif_client = NULL;
/*	
#ifdef HUAHUAN_DHCP
	//TODO: here must confirm where to get the verndor ID
		struct vendor  {
			char vendor, length;
			char str[sizeof(HUAHUAN_VENDOR_ID_STR)];
		} vendor_id = { DHCP_VENDOR,  sizeof(HUAHUAN_VENDOR_ID_STR) - 1, HUAHUAN_VENDOR_ID_STR};
#else
		struct vendor  {
			char vendor, length;
			char str[sizeof("udhcp "DHCP_VERSION)];
		} vendor_id = { DHCP_VENDOR,  sizeof("udhcp "DHCP_VERSION) - 1, "udhcp "DHCP_VERSION};
#endif
*/	
	if(packet == NULL || ifindex == 0 || type == 0)
	{
		DHCP_LOG_ERROR("Input valid is error!\n");
		return 1;
	}

	pif_client = dhcp_if_lookup(ifindex);
	#if 0
	char 	 option60_vendor[32]={'\0'};
	char	 option60_value[30]={'\0'};
	get_object_id(option60_value);
	LOG (LOG_DEBUG,"init_packet() get option60 value is %s\n", option60_value);	
	option60_vendor[OPT_CODE] = DHCP_VENDOR;
	option60_vendor[OPT_LEN] = sizeof(option60_value);
	memcpy(option60_vendor+2, &option60_value, sizeof(option60_value));
	#endif

	if(pif_client == NULL)
	{
		DHCP_LOG_ERROR("get ifindex 0x%x dhcp_if is NULL!\n", ifindex);
		return 1;
	}
	if(dhcp_mac_is_empty(pif_client->mac))
	{
		if(ifm_get_mac(pif_client->ifindex, MODULE_ID_DHCP, pmac) == 0)
		{
			memcpy(pif_client->mac,pmac,6);
		}
		else
		{
			DHCP_LOG_ERROR("cannot get mac on the interface %08X, return\n",pif_client->ifindex);	
			return 1;		
		}
	}
	
	memcpy(pif_client->c_ctrl->clientid+3,pif_client->mac,6);

	init_header(packet, type);
	memcpy(packet->chaddr, pif_client->mac, 6);
	
	add_option_string(packet->options, pif_client->c_ctrl->clientid);
	
	add_option_string(packet->options, pif_client->c_ctrl->hostname);
	add_option_string(packet->options, (uint8_t *) &vendor_id);
	//add_option_string(packet->options, (uint8_t *)&option60_vendor);
	return 0;
}
/* Broadcast a DHCP discover packet to the network, with an optionally requested IP */
int send_discover(struct if_dhcp *pif_client, unsigned long xid, unsigned long requested, struct option124_vlan_ifvalid option124_vlan)
{
	struct dhcpMessage packet;
	int ret=0;
	ret = init_packet_by_ifindex(pif_client->ifindex, &packet, DHCPDISCOVER);
	if(ret)
	{
		return 0;
	}
	packet.xid = xid;
	packet.flags = htons(0x8000);
	if (requested)
	{
		add_simple_option(packet.options, DHCP_REQUESTED_IP, requested);
	}
	add_requests(&packet);
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Sending discover...");
	return raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST,
				SERVER_PORT, MAC_BCAST_ADDR, pif_client->ifindex, option124_vlan, pif_client->zero);
	//return probe_raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST,
	//			SERVER_PORT, MAC_BCAST_ADDR, client_config.ifindex);
}


/* Broadcasts a DHCP request message */
int send_selecting(struct if_dhcp *pif_client, unsigned long xid, unsigned long server, unsigned long requested, struct option124_vlan_ifvalid option124_vlan)
{
	struct dhcpMessage packet;
	int ret=0;
	struct in_addr addr;

	ret = init_packet(pif_client, &packet, DHCPREQUEST);
	if(ret)
	{
		return 0;
	}
	packet.xid = xid;
	packet.flags = htons(0x8000);

	add_simple_option(packet.options, DHCP_REQUESTED_IP, requested);
	add_simple_option(packet.options, DHCP_SERVER_ID, server);

	add_requests(&packet);

	addr.s_addr		= requested;
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Sending request for %s...", inet_ntoa(addr));
	
	return raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST,
				SERVER_PORT, MAC_BCAST_ADDR, pif_client->ifindex, option124_vlan, pif_client->zero);
}


/* Unicasts or broadcasts a DHCP renew message */
int send_renew(struct if_dhcp *pif_client, unsigned long xid, unsigned long server, unsigned long ciaddr)
{
	struct dhcpMessage packet;
	int ret = 0;
	struct option124_vlan_ifvalid option124_vlan;

	option124_vlan.vlanid = 0;
	option124_vlan.ifvalid = 0;
	ret = init_packet(pif_client, &packet, DHCPREQUEST);
	if(ret)
	{
		return 0;
	}
	packet.xid = xid;
	packet.ciaddr = ciaddr;

	add_requests(&packet);	
	if (server)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Sending renew unicast...");
		ret = kernel_packet(&packet, ciaddr, CLIENT_PORT, server, SERVER_PORT);
	}
	else 
	{	
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Sending renew broadcast...");
		ret = raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST,
					SERVER_PORT, MAC_BCAST_ADDR, pif_client->ifindex, option124_vlan, pif_client->zero);
	}
	return ret;
}


/* Unicasts a DHCP release message */
int send_release(struct if_dhcp *pif_client, unsigned long server, unsigned long ciaddr)
{
	struct dhcpMessage packet;
	int ret = 0;	
	#if 0
	struct option124_vlan_ifvalid option124_vlan;

	if (pif_client->zero && pif_client->c_ctrl) {
		option124_vlan.vlanid = pif_client->c_ctrl->vlanid;
		option124_vlan.ifvalid =  pif_client->c_ctrl->ifvalid;
	} else {
		option124_vlan.vlanid = 0;
		option124_vlan.ifvalid = 0;
	}
	#endif

	ret = init_packet(pif_client, &packet, DHCPRELEASE);

	if(ret)
	{
		return 0;
	}
	packet.xid = random_xid();
	packet.ciaddr = ciaddr;

	add_simple_option(packet.options, DHCP_REQUESTED_IP, ciaddr);
	add_simple_option(packet.options, DHCP_SERVER_ID, server);


	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Sending release,we use unicast...");
	ret = kernel_packet(&packet, ciaddr, CLIENT_PORT, server, SERVER_PORT);
	
	//ret = raw_packet(&packet, ciaddr, CLIENT_PORT, 	INADDR_BROADCAST,SERVER_PORT, 
	//MAC_BCAST_ADDR, pif_client->ifindex, option124_vlan, pif_client->zero);
	
	return ret;
	
}

/* Broadcast a DHCP decline message */
int send_decline(struct if_dhcp *pif_client, unsigned long server, unsigned long requested)
{
	struct dhcpMessage packet;
	int ret = 0;	
	struct option124_vlan_ifvalid option124_vlan;

	option124_vlan.vlanid = 0;
	option124_vlan.ifvalid = 0;

	ret = init_packet(pif_client, &packet, DHCPDECLINE);

	if(ret)
	{
		return 0;
	}
	packet.xid = random_xid();

	add_simple_option(packet.options, DHCP_REQUESTED_IP, requested);
	add_simple_option(packet.options, DHCP_SERVER_ID, server);


	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Sending decline,broadcast...");
	
	ret = raw_packet(&packet, INADDR_ANY, CLIENT_PORT, 	INADDR_BROADCAST,SERVER_PORT, 
	MAC_BCAST_ADDR, pif_client->ifindex, option124_vlan, pif_client->zero);
	
	return ret;
	
}

#if 0
/* return -1 on errors that are fatal for the socket, -2 for those that aren't */
int get_raw_packet(struct dhcpMessage *payload, int fd)
{
	int bytes;
	struct udp_dhcp_packet packet;
	uint32_t source, dest;
	uint16_t check;
	int i=0;
	unsigned char *aa;

	memset(&packet, 0, sizeof(struct udp_dhcp_packet));
	bytes = read(fd, &packet, sizeof(struct udp_dhcp_packet));


	if (bytes < 0) {
		DEBUG(LOG_INFO, "couldn't read on raw listening socket -- ignoring");
		usleep(500000); /* possible down interface, looping condition */
		return -1;
	}

	if (bytes < (int) (sizeof(struct iphdr) + sizeof(struct udphdr))) {
		DEBUG(LOG_INFO, "message too short, ignoring");
		return -2;
	}

	if (bytes < ntohs(packet.ip.tot_len)) {
		DEBUG(LOG_INFO, "Truncated packet");
		return -2;
	}

	/* ignore any extra garbage bytes */
	bytes = ntohs(packet.ip.tot_len);

	/*if(bytes ==337  )
	{
		for(i,aa=&packet;i<bytes;i++)
		{
			printf("%02X ",aa[i]);
			if(i%8==0)
				printf("\n");
		}
		printf("\n");

	}*/




	/* Make sure its the right packet for us, and that it passes sanity checks */
	if (packet.ip.protocol != IPPROTO_UDP || packet.ip.version != IPVERSION ||
	    packet.ip.ihl != sizeof(packet.ip) >> 2 || packet.udp.dest != htons(CLIENT_PORT) ||
	    bytes > (int) sizeof(struct udp_dhcp_packet) ||
	    ntohs(packet.udp.len) != (uint16_t) (bytes - sizeof(packet.ip))) {
	    	DEBUG(LOG_INFO, "unrelated/bogus packet");
	    	return -2;
	}

	/* check IP checksum */
	check = packet.ip.check;
	packet.ip.check = 0;
	if (check != checksum(&(packet.ip), sizeof(packet.ip))) {
		DEBUG(LOG_INFO, "bad IP header checksum, ignoring");
		return -1;
	}

	/* verify the UDP checksum by replacing the header with a psuedo header */
	source = packet.ip.saddr;
	dest = packet.ip.daddr;
	check = packet.udp.check;
	packet.udp.check = 0;
	memset(&packet.ip, 0, sizeof(packet.ip));
//printf("source:%02X,%02X,%02X,%02X\n",(unsigned char)(source>>24),(unsigned char)(source>>16),(unsigned char)(source>>8),(unsigned char)source);
	packet.ip.protocol = IPPROTO_UDP;
	packet.ip.saddr = source;
	packet.ip.daddr = dest;
	packet.ip.tot_len = packet.udp.len; /* cheat on the psuedo-header */
	if (check && check != checksum(&packet, bytes)) {
		DEBUG(LOG_ERR, "packet with bad UDP checksum received, ignoring");
		return -2;
	}

	memcpy(payload, &(packet.data), bytes - (sizeof(packet.ip) + sizeof(packet.udp)));

	if (ntohl(payload->cookie) != DHCP_MAGIC) {
		LOG(LOG_ERR, "received bogus message (bad magic) -- ignoring");
		return -2;
	}
	DEBUG(LOG_INFO, "oooooh!!! got some!");
	return bytes - (sizeof(packet.ip) + sizeof(packet.udp));

}
#endif
