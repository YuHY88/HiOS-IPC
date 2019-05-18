/* 动态 arp 的定义 */
#ifndef HIOS_PKT_ARP_H
#define HIOS_PKT_ARP_H


#include <lib/types.h>
#include <lib/thread.h>
#include <lib/pkt_buffer.h>
#include <route/arp.h>

#include "ftm_ifm.h"


#define ARP_RETRY_NUM    3    /* 免费 ARP 重传次数 */
#define ARP_IF_IP_EXIST(ip)      ((ip)!=0)


/* ARP 报文 opcodes. */
enum ARP_OPCODE
{
	ARP_OPCODE_REQUEST = 1,		/* ARP request	*/
	ARP_OPCODE_REPLY,       	/* ARP reply	*/
	RARP_OPCODE_REQUEST, 	    /* RARP request	*/
	RARP_OPCODE_REPLY,			/* RARP reply	*/
	ARP_OPCODE_GRATUITOUS_REPLY,		/* ARP request	*/
	RARP_OPCODE_MAX = 6
};


/* ARP 协议状态机事件 */
enum ARP_EVENT
{
	ARP_EVENT_INVALID = 0,
    ARP_RCV_REPLY,
    ARP_RCV_REQUEST, 
    ARP_RCV_GRATUITOUS,
    ARP_EVENT_MISS,
    ARP_EVENT_UPDATE,
    ARP_EVENT_MAX = 8
};


/* arp 报文格式，28 bytes */
struct arphdr
{
    uint16_t hw_type;   /* hardware type, 1: ethernet */
    uint16_t pro_type;  /* protocol type, 0x0800: IP protocol */
    uint8_t  hw_len;    /* length of hardware address, 6 for MAC */
    uint8_t  pro_len;   /* length of protocol address, 4 for IPV4 */
    uint16_t opcode;    /* ARP/RARP operation */
    uchar    smac[6];   /* sender hardware address */
    uint32_t sip;       /* sender protocol address */
    uchar    dmac[6];   /* target hardware address */
    uint32_t dip;       /* target protocol address */
}__attribute__((packed));



/*发送免费arp 参数*/
struct arp_gratuitous
{
	struct arp_entry parp;
	uint8_t  opcode;
	uint32_t sip;
	uchar    smac[6];
	uint8_t  retry_cnt; /* 重传次数 */
}__attribute__((packed));


struct arp_anti_key
{
    uint32_t sip_vlan_ifidx;
    uint8_t  type;
}__attribute__((packed));

struct arp_miss_anti
{
    struct arp_anti_key key;
    uint32_t cnt;
    uint32_t num_limit;
    uint32_t timer;
}__attribute__((packed));


struct garp_miss_anti
{
    uint32_t gsip_limit;
    uint8_t sip_flag;
    uint8_t vlan_flag;
    uint8_t ifidx_flag;
}__attribute__((packed));

struct hash_table amiss_anti;
struct garp_miss_anti gmiss_anti;


void arp_miss_anti_cnt(struct pkt_buffer *pkt);
void arp_miss_anti_cnt_del(struct arp_anti_key *key);
uint8_t arp_miss_anti_cnt_add(struct arp_anti_key* sip_key, uint32_t maxnum);
int  arp_send(struct arp_entry *parp, uint8_t opcode, uint32_t sip, uchar *smac);
int arp_send_gratuitous(void *arg);
void arp_send_if_linkup(struct ftm_ifm *pifm);
void arp_send_if_addr(uint8_t flag, struct ftm_ifm *pifm);
int arp_miss(uint32_t ipaddr, uint16_t vpn);
int  arp_rcv(struct pkt_buffer *pkt, struct ftm_ifm *pifm);
uint8_t arp_rcv_gratuitous(struct ftm_ifm * pif,struct arp_entry * psrc_arp);
uint8_t arp_ip_conflict(struct ftm_ifm *pif,struct arp_entry *parp);
int arp_ip_conflict_process(void *arg);
int  arp_fsm(struct arp_entry *psrc_arp, uint32_t dip, uint8_t event_type, struct ftm_ifm *pif);
uint8_t pkt_arp_init(void);
uint8_t arp_to_static_check( struct arp_entry *psrc_arp );



#endif
