
 #ifndef HIOS_QOS_CLI_H
#define HIOS_QOS_CLI_H


#include <lib/types.h>
#include <lib/command.h>
#include <lib/vty.h>


#define ACL_IPEX_PROTOCOL_MIN 0x00
#define ACL_IPEX_PROTOCOL_MAX 0xff
#define ACL_MAC_ETHTYPE_MIN 0x0600
#define ACL_MAC_ETHTYPE_MAX 0xffff
#define ACL_MAC_OFFSET_MIN 0x00000000
#define ACL_MAC_OFFSET_MAX 0xffffffff



extern struct cmd_node acl_node;

#define ACL_GET_INTEGER_RANGE_HEX(NAME,V,STR,MAX_LEN,MIN_LEN,MAX)        \
            do {                                                             \
              int retv;                                                      \
              u_int32_t tmp;                            					 \
              tmp = cmd_hexstr2int ((STR),(MAX_LEN),(MIN_LEN), &retv);       \
              if (retv == -1|| (tmp) > (MAX))              \
	          {                                                            \
	              vty_error_out (vty, "Invalid %s value%s", NAME, VTY_NEWLINE);       \
	              return CMD_WARNING;                                                 \
	          }                                                                     \
              *V = tmp;                                                             \
            } while (0)   													          \

u_int32_t cmd_hexstr2int ( char *str, uint8_t max_len,uint8_t min_len, int *ret );
void acl_cli_init(void);
const char* proto_type_parse_str(uint8_t proto_num);
void tcp_type_parse_str(uint8_t tcp_type,struct vty *vty);

#endif
