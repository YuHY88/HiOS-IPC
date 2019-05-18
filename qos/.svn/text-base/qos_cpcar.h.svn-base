
#ifndef HIOS_QOS_CPCAR_H
#define HIOS_QOS_CPCAR_H

#include <lib/types.h>

#define QOS_CPCAR_QUEUE_GLOBAL   	 16


/* cpcar 的类型 */
enum CPCAR_TYPE
{
	CPCAR_TYPE_INVALID = 5,
	CPCAR_TYPE_DHCP,
	CPCAR_TYPE_IGMP,
	CPCAR_TYPE_ARP,
	CPCAR_TYPE_ICMP,
	CPCAR_TYPE_TCP,
	CPCAR_TYPE_OUTBAND_ARP = 16,
	CPCAR_TYPE_OUTBAND_IPV6,
	CPCAR_TYPE_OUTBAND_DHCP,
	CPCAR_TYPE_OUTBAND_TFTP,
	CPCAR_TYPE_OUTBAND_SSH,
	CPCAR_TYPE_OUTBAND_TELNET,
	CPCAR_TYPE_OUTBAND_SNMP,
	CPCAR_TYPE_OUTBAND_ICMP,
	CPCAR_TYPE_OUTBAND_IP_OTHER,
	CPCAR_TYPE_ALL = 32
};

/* cpcar 的数据结构 */
struct qos_cpcar
{
	uint16_t arp_pps;
	uint16_t dhcp_pps;
	uint16_t icmp_pps;
	uint16_t tcp_pps;
	uint16_t igmp_pps;
	uint16_t all_pps;   /* 整机所有报文的限速 */
};


/* cpu 队列配置及packet统计 */
struct qos_cpcar_queue
{
	uint8_t 	cosq_id;		/* cpu queue id  */
	uint16_t 	cosq_pps;		/* cpu queue  pps */
	uint16_t 	cosq_pps_config;/* cpu queue 配置*/
    uint32_t 	rx_packets;  	/* cpu queue packets received  */
};

/* 管理口配置及packet统计 */
struct qos_cpcar_outband
{
	uint16_t 	arp_pps;			
	uint16_t 	ipv6_pps;	
	uint16_t 	tftp_pps;
	uint16_t 	telnet_pps;
	uint16_t 	ssh_pps;
	uint16_t 	dhcp_pps;
	uint16_t 	snmp_pps;
	uint16_t 	icmp_pps;
	uint16_t 	ipother_pps;
};


extern struct qos_cpcar_outband outband_config;
extern struct qos_cpcar  gcpcar; 					/* 全局 cpcar */
extern struct qos_cpcar_queue  cpcar_queue[17]; 	/*id 0-15 cpu 单个队列，id 16获取整机参数*/


void qos_cpcar_init(void);             										/* 初始化 gcpcar */
void qos_cpcar_cmd_init(void);
int qos_cpcar_set(uint32_t ifindex, uint16_t pps, enum CPCAR_TYPE type);	/* 设置 cpu car 的参数,queue<6-10>*/
int qos_cpcar_set_queue(uint8_t id, uint16_t pps);						/* 设置 cpu car 的参数,queue<1-15> except queue <6-10>*/


#endif



