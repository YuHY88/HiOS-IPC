#ifndef HIOS_DHCP6_OPTIONS_H
#define HIOS_DHCP6_OPTIONS_H

//#include "dhcp6.h"
struct dhcpv6_option {
	uint16_t code;
	char flags;
};

#define OPTION_REQ      0x10
#define OPTION_UNREQ    0x00
#define DHCP_END		0xFF
#define DHCP_PADDING	0x00
#define OPTV6_LEN       2
#define OPTV6_CODE      2
#define OPTV6_LEN_IA_NA 12
#define OPTV6_LEN_IA_PD 12
#define OPTV6_LEN_IA_AD 24
#define OPTV6_LEN_IA_PREFIX 25
#define OPTV6_DATA      2
#define OPTV6_IAID      4
#define OPTV6_IAT1      4
#define OPTV6_IAT2      4
#define OPTV6_IANA_AD   16
#define OPTV6_IAPD_PREFIX_AD   1
#define OPTV6_IANA_TIME 4
#define OPTV6_MAX 160

#define OPTV6_LEN_IANA_VALID   OPTV6_IAID + OPTV6_IAT1 + OPTV6_IAT2
#define OPTV6_LEN_IANA_AD      OPTV6_IANA_AD + OPTV6_IANA_TIME + OPTV6_IANA_TIME
#define OPTV6_LEN_IANA_STATUS_CODE 2

//extern struct dhcp6_list reqopt_list;

int dhcpv6_add_option(uint8_t *optionptr, uint32_t start, uint16_t code, uint16_t len, uint8_t *data);
int get_option_string(uint8_t *optionptr, uint16_t code, uint16_t subcode, uint16_t sub2code, uint8_t *string);
uint32_t dhcpv6_parse_options(struct dhcpv6_if *ifp, uint8_t *optionptr);
int dhcpv6_add_request(uint8_t *optionptr, uint32_t start);

#endif

