
#ifndef HIOS_H3C_QOS_CLI_H
#define HIOS_H3C_QOS_CLI_H


#include <lib/types.h>
#include <lib/command.h>
#include <lib/vty.h>




#define ACL_TYPE_PROMPT_GET(_type,_str)\
do{                                       \
switch(_type){                        \
    case ACL_BASIC_NODE:                \
        _str = "basic";                 \
        break;                        \
    case ACL_ADVANCED_NODE:                \
        _str = "advanced";                 \
        break;                        \
    case ACL_MAC_NODE:                \
        _str = "mac";                 \
        break;                        \
    default:                          \
        break;                        \
}                                     \
}while(0) 


void acl_h3c_cli_init(void);
uint32_t wildcard_to_mask(uint32_t wildcard);
uint32_t mask_to_wildcard(uint32_t n);
#endif
