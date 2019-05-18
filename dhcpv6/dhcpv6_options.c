/******************************************************************************
 * Filename: dhcp6_packet.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.23  wumingming created
 *
******************************************************************************/
#include <sys/queue.h>
#include <string.h>
#include <lib/pkt_buffer.h>
#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/memory.h>
#include <lib/memtypes.h>


#include "dhcp6.h"
#include "common.h"
#include "dhcpv6_if.h"
#include "dhcpv6_options.h"

struct dhcpv6_option dhcpv6_options[] = {
	/* code						flags	    */
	{DH6OPT_CLIENTID,			OPTION_UNREQ},
	{DH6OPT_SERVERID,			OPTION_UNREQ},
	{DH6OPT_IA_NA,				OPTION_UNREQ},
	{DH6OPT_IA_TA,				OPTION_UNREQ},
	{DH6OPT_IAADDR,				OPTION_UNREQ},
	{DH6OPT_ORO,				OPTION_UNREQ},
	{DH6OPT_PREFERENCE,			OPTION_UNREQ},
	{DH6OPT_ELAPSED_TIME,		OPTION_UNREQ},
	{DH6OPT_RELAY_MSG,			OPTION_UNREQ},
	{DH6OPT_AUTH,				OPTION_UNREQ},
	{DH6OPT_UNICAST,			OPTION_UNREQ},
	{DH6OPT_STATUS_CODE,		OPTION_UNREQ},
	{DH6OPT_RAPID_COMMIT,		OPTION_UNREQ},
	{DH6OPT_USER_CLASS,			OPTION_UNREQ},
	{DH6OPT_VENDOR_CLASS,		OPTION_UNREQ},
	{DH6OPT_VENDOR_OPTS,		OPTION_UNREQ},
	{DH6OPT_INTERFACE_ID,		OPTION_UNREQ},
	{DH6OPT_RECONF_MSG,			OPTION_UNREQ},
	{DH6OPT_RECONF_ACCEPT,		OPTION_UNREQ},
	{DH6OPT_SIP_SERVER_D,		OPTION_UNREQ},
	{DH6OPT_SIP_SERVER_A,		OPTION_UNREQ},
	{DH6OPT_DNS,				OPTION_REQ},
	{DH6OPT_DNSNAME,			OPTION_REQ},
	{DH6OPT_IA_PD,				OPTION_UNREQ},
	{DH6OPT_IA_PD_PREFIX,		OPTION_UNREQ},
	{DH6OPT_NIS_SERVERS,		OPTION_UNREQ},
	{DH6OPT_NISP_SERVERS,		OPTION_UNREQ},
	{DH6OPT_NIS_DOMAIN_NAME,	OPTION_UNREQ},
	{DH6OPT_NISP_DOMAIN_NAME,	OPTION_UNREQ},
	{DH6OPT_NTP,				OPTION_UNREQ},
	{DH6OPT_REFRESHTIME,		OPTION_UNREQ},
	{DH6OPT_REFRESHTIME_UNDEF,	OPTION_UNREQ},
	{DH6OPT_BCMCS_SERVER_D,		OPTION_UNREQ},
	{DH6OPT_BCMCS_SERVER_A,		OPTION_UNREQ},
	{DH6OPT_GEOCONF_CIVIC,		OPTION_UNREQ},
	{DH6OPT_REMOTE_ID,			OPTION_UNREQ},
	{DH6OPT_SUBSCRIBER_ID,		OPTION_UNREQ},
	{DH6OPT_CLIENT_FQDN,		OPTION_UNREQ},
	{0x00,						0x00}
};

int dhcpv6_add_option(uint8_t *optionptr, uint32_t start, uint16_t code, uint16_t len, uint8_t *data)
{
	uint32_t end = 0;
	if(len == 0 || code == 0)
	{
		DHCPV6_LOG_ERR("Input option is error!");
		return 0;
	}
	DHCPV6_LOG_DEBUG("option start is %d\n", start);

	memcpy(optionptr + start, &code, OPTV6_CODE);
	
	memcpy(optionptr + start + OPTV6_CODE, &len, OPTV6_LEN);
	
	end = OPTV6_CODE + OPTV6_LEN + len;
	if(data != NULL)
	{
		DHCPV6_LOG_DEBUG("Input data is:\n");
		u_char* pktdata;
		uint32_t k = 0;
		for ( k = 0; k < 10; k++ )
	    {
			pktdata = (unsigned char *)data + k*16;
	        zlog_debug("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
						pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7],
						pktdata[8], pktdata[9],pktdata[10],pktdata[11],pktdata[12],pktdata[13],pktdata[14],pktdata[15]);
	    }
		if(code == DH6OPT_IAADDR)
		{
			memcpy(optionptr + start + OPTV6_CODE + OPTV6_LEN, data + 1, len);
		}else{
			memcpy(optionptr + start + OPTV6_CODE + OPTV6_LEN, data + 1, len);
		}
	}else{
		int i = 0;
		for(i = 0; i < len; i++)
		{
			optionptr[start+2+i+1] = 0;
		}
	}
	
	return end;
}

int dhcpv6_add_request(uint8_t *optionptr, uint32_t start)
{
	
	int i, len = 0;
	uint8_t *request = NULL;
	uint32_t count = 0;
	
	request = XMALLOC(MTYPE_IF, sizeof(dhcpv6_options));
	if(request == NULL)
	{
		DHCPV6_LOG_ERR("XMALLOC request is failed!\n");
		return 0;
	}
	for(i = 0; dhcpv6_options[i].code; i++)
	{
		if(dhcpv6_options[i].flags & OPTION_REQ)
		{
			uint16_t code = 0;
			code = htons(dhcpv6_options[i].code);
			
			memcpy(request + (count * OPTV6_CODE), &code, OPTV6_CODE);
			len = len + OPTV6_DATA;			
			count++;
		}
	}
	
	return dhcpv6_add_option(optionptr, start, DH6OPT_ORO, len, request);
}

/*
optionptr:dhcpv6_message->options

*/

/* 只能返回option一级列表，不能返回子option */
int get_option_string(uint8_t *optionptr, uint16_t code, uint16_t subcode, uint16_t sub2code, uint8_t *string)
{
	uint8_t done = 0;
	uint32_t length = 308;
	uint32_t i = 0;
//	uint16_t code_ad = 0;
//	uint16_t option_len = 0;

	while(!done)
	{
		if(optionptr[i + 1] > length)
		{
			DHCPV6_LOG_ERR("This option's lengths is over max length!\n");
			return -1;
		}
		if(code != 0){
			/* option 1st is option's code */
			if(optionptr[i] == code)
			{
				memcpy(string, optionptr + i + 2, optionptr[i + 3] + 2);
				return 0;
			}else{
				i = i + 2 + optionptr[i + 1];/* option 2nd is option's length */
			}
		}else if(subcode != 0)
		{
			if(subcode == DH6OPT_IAADDR)
			{
				if(optionptr[i] == DH6OPT_IA_NA)
				{
					memcpy(string, optionptr + i + 18, optionptr[i + 19] + 2);
				}else if(optionptr[i] == DH6OPT_IA_TA)
				{
					memcpy(string, optionptr + i + 10, optionptr[i + 11] + 2);
				}
			}
		}
		else if(sub2code != 0)
		{
			if(sub2code == DH6OPT_STATUS_CODE)
			{
				if(optionptr[i] == DH6OPT_IA_NA)
				{
					memcpy(string, optionptr + i + 48, optionptr[i + 49] + 2);
				}else if(optionptr[i] == DH6OPT_IA_TA)
				{
					memcpy(string, optionptr + i + 38, optionptr[i + 39] + 2);
				}
			}
		}
	}
	return 0;
}

uint32_t dhcpv6_parse_options(struct dhcpv6_if *ifp, uint8_t *optionptr)
{
	uint8_t done = 0;
	uint32_t i = 0;
	uint16_t code = 0;
	if(ifp == NULL)
	{
		DHCPV6_LOG_ERR("Input ifp is null!\n");
		return -1;
	}
	u_char* pktdata1;
	uint32_t k1 = 0;
	u_char* pktdata;
	uint32_t k = 0;
	for ( k = 0; k < 10; k++ )
	{
		pktdata = (unsigned char *)optionptr + k*16;
		zlog_debug("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7],
					pktdata[8], pktdata[9],pktdata[10],pktdata[11],pktdata[12],pktdata[13],pktdata[14],pktdata[15]);
	}
	while(!done)
	{
		code = optionptr[i + 1];
		DHCPV6_LOG_DEBUG("code = %d, i = %d", code, i);
		switch(code)
		{
			case DH6OPT_CLIENTID:
				
				ifp->client_id.duid_len = optionptr[i + OPTV6_CODE + 1];
				
				uint8_t *client_id = NULL;
				
				client_id = XMALLOC(MTYPE_IF, ifp->client_id.duid_len);
				if(client_id == NULL)
				{
					DHCPV6_LOG_ERR("XMALLOC client duid_id is failed!\n");
					return -1;
				}
				memset(client_id, 0, ifp->client_id.duid_len);
				memcpy(client_id, optionptr + i + OPTV6_CODE + OPTV6_LEN, ifp->client_id.duid_len);
				ifp->client_id.duid_id = client_id;
				break;
			case DH6OPT_SERVERID:
				DHCPV6_LOG_DEBUG("server_id's len is %d!\n", optionptr[i + OPTV6_CODE + 1]);
				
				ifp->server_id.duid_len = optionptr[i + OPTV6_CODE + 1];
				uint8_t *server_id = NULL;

				server_id = XMALLOC(MTYPE_IF, ifp->server_id.duid_len);
				if(server_id == NULL)
				{
					DHCPV6_LOG_ERR("XMALLOC server duid_id is failed!\n");
					return -1;
				}
				memset(server_id, 0, ifp->server_id.duid_len);
				memcpy(server_id, optionptr + i + OPTV6_CODE + OPTV6_LEN, ifp->server_id.duid_len);
				ifp->server_id.duid_id = server_id;
				break;
			case DH6OPT_IA_NA:
				ifp->ia.ia_type = IATYPE_NA;
				memcpy(&(ifp->ia.iaid), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN), OPTV6_IAID);
				ifp->ia.iaid = ntohl(ifp->ia.iaid);
				memcpy(&(ifp->ia.t1), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN + OPTV6_IAID), OPTV6_IAT1);
				ifp->ia.t1 = ntohl(ifp->ia.t1);
				memcpy(&(ifp->ia.t2), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN + OPTV6_IAID + OPTV6_IAT1), OPTV6_IAT2);
				ifp->ia.t2 = ntohl(ifp->ia.t2);

				DHCPV6_LOG_DEBUG("ifp->ia.ia_type is %d, ifp->ia.iaid is %d, ifp->ia.t1 is %d, ifp->ia.t2 is %d!\n", ifp->ia.ia_type, ifp->ia.iaid, ifp->ia.t1, ifp->ia.t2);

				if(optionptr[i + OPTV6_CODE + OPTV6_LEN + OPTV6_LEN_IANA_VALID + 1] == DH6OPT_IAADDR)
				{
					memcpy(ifp->ia.ia.iana.addr, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN, OPTV6_IANA_AD);
					memcpy(&(ifp->ia.ia.iana.pre_time), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_AD), OPTV6_IANA_TIME);
					ifp->ia.ia.iana.pre_time = ntohl(ifp->ia.ia.iana.pre_time);
					memcpy(&(ifp->ia.ia.iana.valid_time), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_AD + OPTV6_IANA_TIME), OPTV6_IANA_TIME);
					ifp->ia.ia.iana.valid_time = ntohl(ifp->ia.ia.iana.valid_time);
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iana.addr is %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x!\n", 
						ifp->ia.ia.iana.addr[0], ifp->ia.ia.iana.addr[1], ifp->ia.ia.iana.addr[2], ifp->ia.ia.iana.addr[3], 
						ifp->ia.ia.iana.addr[4], ifp->ia.ia.iana.addr[5], ifp->ia.ia.iana.addr[6], ifp->ia.ia.iana.addr[7],
						ifp->ia.ia.iana.addr[8], ifp->ia.ia.iana.addr[9], ifp->ia.ia.iana.addr[10], ifp->ia.ia.iana.addr[11], 
						ifp->ia.ia.iana.addr[12], ifp->ia.ia.iana.addr[13], ifp->ia.ia.iana.addr[14], ifp->ia.ia.iana.addr[15]);
					
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iana.pre_time is %d, ifp->ia.ia.iana.valid_time is %d!\n", ifp->ia.ia.iana.pre_time, ifp->ia.ia.iana.valid_time);
				}
				if(optionptr[i + OPTV6_CODE + OPTV6_LEN + OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + 1] == DH6OPT_STATUS_CODE)
				{
					memcpy(&(ifp->ia.ia.iana.status_code.code), (uint16_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN), OPTV6_CODE);
					ifp->ia.ia.iana.status_code.code = ntohs(ifp->ia.ia.iana.status_code.code);
					memcpy(ifp->ia.ia.iana.status_code.msg, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN + OPTV6_CODE, optionptr[i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN + OPTV6_CODE + 1]);
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iana.status_code.code is %d, ifp->ia.ia.iana.status_code.msg is %s!\n", ifp->ia.ia.iana.status_code.code, ifp->ia.ia.iana.status_code.msg);
				}
				break;
			case DH6OPT_IA_PD:
				
				for ( k1 = 0; k1 < 10; k1++ )
				{
					pktdata1 = (unsigned char *)optionptr + i + OPTV6_CODE + OPTV6_LEN+ k1*16;
					zlog_debug("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
								pktdata1[0], pktdata1[1],pktdata1[2],pktdata1[3],pktdata1[4],pktdata1[5],pktdata1[6],pktdata1[7],
								pktdata1[8], pktdata1[9],pktdata1[10],pktdata1[11],pktdata1[12],pktdata1[13],pktdata1[14],pktdata1[15]);
				}
				ifp->ia.ia_type = IATYPE_PD;
				//zlog_debug("iaid %02x", (unsigned char *)(optionptr + i + OPTV6_CODE + OPTV6_LEN));
				memcpy(&(ifp->ia.iaid), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN), OPTV6_IAID);
				ifp->ia.iaid = ntohl(ifp->ia.iaid);
				memcpy(&(ifp->ia.t1), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN + OPTV6_IAID), OPTV6_IAT1);
				ifp->ia.t1 = ntohl(ifp->ia.t1);
				memcpy(&(ifp->ia.t2), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN + OPTV6_IAID + OPTV6_IAT1), OPTV6_IAT2);
				ifp->ia.t2 = ntohl(ifp->ia.t2);
			
				DHCPV6_LOG_DEBUG("ifp->ia.ia_type is %d, ifp->ia.iaid is %x, ifp->ia.t1 is %d, ifp->ia.t2 is %d!\n", ifp->ia.ia_type, ifp->ia.iaid, ifp->ia.t1, ifp->ia.t2);
			
				if(optionptr[i + OPTV6_CODE + OPTV6_LEN + OPTV6_LEN_IANA_VALID + 1] == DH6OPT_IAPREFIX)
				{
					memcpy(&(ifp->ia.ia.iapd.pre_time), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN), OPTV6_IANA_TIME);
					ifp->ia.ia.iapd.pre_time = ntohl(ifp->ia.ia.iapd.pre_time);
					memcpy(&(ifp->ia.ia.iapd.valid_time), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_TIME), OPTV6_IANA_TIME);
					ifp->ia.ia.iapd.valid_time = ntohl(ifp->ia.ia.iapd.valid_time);
					memcpy(&(ifp->ia.ia.iapd.prefix_len), (uint32_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_TIME + OPTV6_IANA_TIME), OPTV6_IAPD_PREFIX_AD);
					memcpy(ifp->ia.ia.iapd.prefix_addr, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_TIME + OPTV6_IANA_TIME + OPTV6_IAPD_PREFIX_AD, OPTV6_IANA_AD);
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iapd.addr is %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x!\n", 
						ifp->ia.ia.iapd.prefix_addr[0], ifp->ia.ia.iapd.prefix_addr[1], ifp->ia.ia.iapd.prefix_addr[2], ifp->ia.ia.iapd.prefix_addr[3], 
						ifp->ia.ia.iapd.prefix_addr[4], ifp->ia.ia.iapd.prefix_addr[5], ifp->ia.ia.iapd.prefix_addr[6], ifp->ia.ia.iapd.prefix_addr[7],
						ifp->ia.ia.iapd.prefix_addr[8], ifp->ia.ia.iapd.prefix_addr[9], ifp->ia.ia.iapd.prefix_addr[10], ifp->ia.ia.iapd.prefix_addr[11], 
						ifp->ia.ia.iapd.prefix_addr[12], ifp->ia.ia.iapd.prefix_addr[13], ifp->ia.ia.iapd.prefix_addr[14], ifp->ia.ia.iapd.prefix_addr[15]);
					
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iapd.pre_time is %d, ifp->ia.ia.iapd.valid_time is %d!\n", ifp->ia.ia.iapd.pre_time, ifp->ia.ia.iapd.valid_time);
				}
				if(optionptr[i + OPTV6_CODE + OPTV6_LEN + OPTV6_LEN_IANA_VALID + OPTV6_LEN_IA_PREFIX + 1] == DH6OPT_STATUS_CODE)
				{
					memcpy(&(ifp->ia.ia.iapd.status_code.code), (uint16_t *)(optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN), OPTV6_CODE);
					ifp->ia.ia.iapd.status_code.code = ntohs(ifp->ia.ia.iapd.status_code.code);
					memcpy(ifp->ia.ia.iapd.status_code.msg, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN + OPTV6_CODE, optionptr[i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_LEN_IANA_VALID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN + OPTV6_CODE + 1]);
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iapd.status_code.code is %d, ifp->ia.ia.iapd.status_code.msg is %s!\n", ifp->ia.ia.iapd.status_code.code, ifp->ia.ia.iapd.status_code.msg);
				}
				break;
			case DH6OPT_IA_TA:
				ifp->ia.ia_type = IATYPE_TA;
				memcpy(&ifp->ia.iaid, optionptr + i + OPTV6_CODE + OPTV6_LEN, OPTV6_IAID);
				ifp->ia.iaid = ntohl(ifp->ia.iaid);
				DHCPV6_LOG_DEBUG("ifp->ia.ia_type is %d, ifp->ia.iaid is %d!\n", ifp->ia.ia_type, ifp->ia.iaid);
				if(optionptr[i + OPTV6_CODE + OPTV6_LEN + OPTV6_IAID + 1] == DH6OPT_IAADDR)
				{
					memcpy(ifp->ia.ia.iana.addr, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_IAID + OPTV6_CODE + OPTV6_LEN, OPTV6_IANA_AD);
					memcpy(&ifp->ia.ia.iana.pre_time, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_IAID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_AD, OPTV6_IANA_TIME);
					ifp->ia.ia.iana.pre_time = ntohl(ifp->ia.ia.iana.pre_time);
					memcpy(&ifp->ia.ia.iana.valid_time, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_IAID + OPTV6_CODE + OPTV6_LEN + OPTV6_IANA_AD + OPTV6_IANA_TIME, OPTV6_IANA_TIME);
					ifp->ia.ia.iana.valid_time = ntohl(ifp->ia.ia.iana.valid_time);
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iana.addr is %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x!\n", 
						ifp->ia.ia.iana.addr[0], ifp->ia.ia.iana.addr[1], ifp->ia.ia.iana.addr[2], ifp->ia.ia.iana.addr[3], 
						ifp->ia.ia.iana.addr[4], ifp->ia.ia.iana.addr[5], ifp->ia.ia.iana.addr[6], ifp->ia.ia.iana.addr[7],
						ifp->ia.ia.iana.addr[8], ifp->ia.ia.iana.addr[9], ifp->ia.ia.iana.addr[10], ifp->ia.ia.iana.addr[11], 
						ifp->ia.ia.iana.addr[12], ifp->ia.ia.iana.addr[13], ifp->ia.ia.iana.addr[14], ifp->ia.ia.iana.addr[15]);
					

					DHCPV6_LOG_DEBUG("ifp->ia.ia.iana.pre_time is %d, ifp->ia.ia.iana.valid_time is %d!\n", ifp->ia.ia.iana.pre_time, ifp->ia.ia.iana.valid_time);
				}
				if(optionptr[i + OPTV6_CODE + OPTV6_LEN + OPTV6_IAID + OPTV6_LEN_IANA_AD + 1] == DH6OPT_STATUS_CODE)
				{
					memcpy(&ifp->ia.ia.iana.status_code.code, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_IAID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN, OPTV6_CODE);
					memcpy(ifp->ia.ia.iana.status_code.msg, optionptr + i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_IAID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN + OPTV6_CODE, optionptr[i + OPTV6_CODE + OPTV6_LEN 
						+ OPTV6_IAID + OPTV6_LEN_IANA_AD + OPTV6_CODE + OPTV6_LEN + OPTV6_CODE + 1]);
					DHCPV6_LOG_DEBUG("ifp->ia.ia.iana.status_code.code is %d, ifp->ia.ia.iana.status_code.msg is %s!\n", ifp->ia.ia.iana.status_code.code, ifp->ia.ia.iana.status_code.msg);
				}
				break;
				
			case DH6OPT_ELAPSED_TIME:
				memcpy(&(ifp->event.timeouts), optionptr + i + OPTV6_CODE, optionptr[i + OPTV6_CODE + 1]);
				DHCPV6_LOG_DEBUG("ifp->event.timeouts is %d!\n", ifp->event.timeouts);
				break;
			case DH6OPT_REFRESHTIME:
				//memcpy(ifp->server_id, optionptr + i + OPTV6_CODE, optionptr[i + OPTV6_CODE + 1]);
				break;
		}
		i = i + optionptr[i + OPTV6_CODE + 1] + OPTV6_CODE + OPTV6_LEN;
		/* 在code后面为len，但len一般为00 xx，跳过00需+1 */
		if(optionptr[i + OPTV6_CODE + 1] == '\0')
		{
			done =1;
		}
	}
	return 0;
}


