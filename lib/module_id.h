/*
 *  define module id
 */

 
#ifndef HIOS_MODULE_H_
#define HIOS_MODULE_H_



#define MODULE_ID_IFM    1    /* 接口管理 */
#define MODULE_ID_DEVM   2    /* 设备管理 */
#define MODULE_ID_VTY    3    /* 配置管理 */
#define MODULE_ID_MPLS   4   
#define MODULE_ID_FTM    5    
#define MODULE_ID_HAL    6    
#define MODULE_ID_HSL    7    
#define MODULE_ID_QOS    8    
#define MODULE_ID_OSPF   9    
#define MODULE_ID_ISIS   10    
#define MODULE_ID_RIP    11    
#define MODULE_ID_BGP    12   
#define MODULE_ID_ROUTE  13   /* 路由管理 */
#define MODULE_ID_ARP    14
#define MODULE_ID_LDP    15
#define MODULE_ID_RSVPTE 16
#define MODULE_ID_VRRP   17
#define MODULE_ID_DHCP   18
#define MODULE_ID_LACP   19
#define MODULE_ID_L2     20
#define MODULE_ID_SYSLOG 21
#define MODULE_ID_FILE   22   /* 文件管理 */
#define MODULE_ID_PING   23   
#define MODULE_ID_CES    24
#define MODULE_ID_AAA    25   /* aaa 模块 */
#define MODULE_ID_SYSTEM 26   /* 系统管理 */
#define MODULE_ID_BFD    27	  /* BFD 模块 */
#define MODULE_ID_ALARM  28	  /* 告警管理 */
#define MODULE_ID_NTP    29   /* NTP */
#define MODULE_ID_SNMPD  30
#define MODULE_ID_SDHMGT 31
#define MODULE_ID_RESV1  32   /* resv */

#define MODULE_ID_WEB        33
#define MODULE_ID_OPENFLOW   34   /* OpenFlow */
#define MODULE_ID_NETCONF    35   /* NETCONFIG */

#define MODULE_ID_IPMC 		 36
#define MODULE_ID_CLOCK      37
#define MODULE_ID_HA         38   /* HA*/
#define MODULE_ID_OSPF6      39   
#define MODULE_ID_DHCPV6     40   /* DHCPV6 */
#define MODULE_ID_RMON       41   /* rmond*/

#define MODULE_ID_REPLY_A    42   /* use for reply message for every module*/

#define MODULE_ID_STAT		 43   /*GPN_STAT*/

#define MODULE_ID_VCG        44

#define MODULE_ID_L3VPN      45
#define MODULE_ID_SLSP       46
#define MODULE_ID_L2VPN      47

#define MODULE_ID_HALHA      48

#define MODULE_ID_SNMPD_TRAP 49

#define MODULE_ID_SNMPD_PACKET  50

#define MODULE_ID_MAXNUM     51

#endif 
