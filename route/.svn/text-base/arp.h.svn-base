/* 
* arp 的公共头文件 
*/


#ifndef HIOS_ARP_H
#define HIOS_ARP_H

#include <lib/types.h>
#include <lib/route_com.h>



#define ARP_CLI_INFO  "Prompt information display in vtysh_route_cmd.c \n"

#define ARP_AGED_TIME 30*60
#define ARP_KEEPALIVE_TIME 15*60
#define ARP_TIMER 1*60
#define VPN_SUPPORT 128
#define ARP_FAKE_TIME 3

/* arp errno */
#define ARP_ENTRY_IS_NULL    1
#define ARP_OVER_STATIC_NUM  2
#define ARP_OVER_TOTAL_SUM   3
#define ARP_HASH_BUCKET_FULL 4


/*arp 防御类型*/
enum AANTI_TYPE
{
    ARP_ANTI_SMAC = 0,
    ARP_ANTI_SIP,
    ARP_ANTI_VLAN,
    ARP_ANTI_IFIDX,
    ARP_ANTI_GLOBAL,
    ARP_ANTI_MAX = 5,  
};

enum ARP_FIXED_TYPE
{
    ARP_FIXED_ALL = 1,
    ARP_FIXED_MAC,
    ARP_FIXED_SENDACK,
    ARP_FIXED_MAX = 5,
};

#endif
