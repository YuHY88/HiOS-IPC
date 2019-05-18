/*
*       manage the static ping table
*
*/

#ifndef HIOS_PING_CMD_H
#define HIOS_PING_CMD_H

enum PARSE_CLI_RET
{
	PARSE_SUCCEED,
	PARSE_INVALID_CHAR,
	PARSE_IPLEN_SHORT,
	PARSE_IPLEN_LONG,
	PARSE_IPMASK_LONG,
	PARSE_MACLEN_SHORT,
	PARSE_MACLEN_LONG,
	PARSE_MALFORMED,
	PARSE_COMMON_ERROR
};

uint8_t ipv6_sip_check(struct ipv6_addr *sip6,struct vty *vty);
void ping_CmdInit(void);

#endif


