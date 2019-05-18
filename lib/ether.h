/***********************************************************************
*    define of ethernet protocol
*
***********************************************************************/
#ifndef HIOS_ETHER_H
#define HIOS_ETHER_H


#include <lib/types.h>
#include <lib/inet_ip.h>



#define GET_ONE_COLONOFMAC(s,e) do{\
		int  __len = (e) - (s) + 1;\
		if(__len > 2)\
			return (FALSE);\
}while(0)

#define CHAR_IS_MAC_DIGITAL(c) ((((c) >= '0') && ((c) <= '9')) || \
								(((c) >= 'a') && ((c) <= 'f')) || \
								(((c) >= 'A') && ((c) <= 'F')))


#define NOT_MAC_DIGITAL_CHK(__pc) do{\
		if((__pc == NULL) || !CHAR_IS_MAC_DIGITAL(*(__pc))) return (FALSE);\
}while(0)

uchar *ether_string1_to_mac (char *str, uchar *mac);/* 将字符串HHHH-HHHH-HHHH转换成 mac 地址数组 */
uchar *ether_string_to_mac (const char *str, uchar *mac);/* 将字符串转换成 mac 地址数组 */
uchar *ether_mac_to_string(char *mac, uchar *str); /* 将 mac 地址数组转换成字符串格式 */

uchar *ether_get_broadcast_mac(void);   /* 获取广播 mac */
uchar *ether_get_zero_mac(void);          /* 获取全 0 mac */
int    ether_get_muticast_mac(uint32_t netip, uchar *mac);/* 根据组播 IP 获取组播 mac，rfc1122 */

int ether_get_ipv6_muticast_mac(struct ipv6_addr *paddr, uchar *mac);/*IPv6多播地址映射位以太网地址*/

/* check mac */
int ether_valid_mac(const char *str);           /* 检查 MAC 地址字符串的合法性 */
int ether_is_broadcast_mac(uchar *mac);/* 判断是否广播 MAC */
int ether_is_muticast_mac(uchar *mac);    /* 判断是否为多播 MAC */
int ether_is_zero_mac(uchar *mac);       /* 判断是否全 0 MAC */
int ether_is_unicast_mac(uchar *mac);   /* 判断单播 mac */


#endif
