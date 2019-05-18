

#ifndef HIOS_NDP_CMD_H
#define HIOS_NDP_CMD_H

#include "ndp.h"
#include "ndp_static.h"


/*master ip exist or linklocal exist*/
#define NDP_IF_IPV6_EXIST(l3if)\
    ((!ipv6_is_zero((struct ipv6_addr *)(l3if->intf.ipv6[0].addr))) || (!ipv6_is_zero((struct ipv6_addr *)(l3if->intf.ipv6_link_local.addr))))
    
#define NDP_DEBUG_LOG  		0x01    //for tdm interface configuration  
#define NDP_DEBUG_OTHER     0x02    //reserve
#define NDP_DEBUG_ALL       0x07    //for all debug info

int ndp_ipv6_str_check(const char *src);
void ndp_cmd_init(void);
void ndp_static_cmd_init(void);




#endif

