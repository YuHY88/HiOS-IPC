
#ifndef HIOS_PKT_ETH_H
#define HIOS_PKT_ETH_H

#include <lib/pkt_buffer.h>
#include <lib/ether.h>


#define ETH_HLEN	        14		/* ethernet header len */
#define ETH_ALEN   	        6		/* mac len */
#define VLAN_HLEN	        4		/* vlan + tpid len */
#define VLAN_ETH_HLEN	    18		/* vlan + ethernet header len */
#define QINQ_ETH_HLEN	    22		/* vlan + ethernet header len */

#define ETH_ZLEN	        60		/* Min. octets in eth frame sans FCS */
#define VLAN_ETH_ZLEN	    64		/* Min. octets in vlan frame sans FCS */

#define ETH_DATA_LEN	    1500	/* Max. octets in eth payload	 */
#define ETH_FRAME_LEN	    1514	/* Max. octets in eth frame sans FCS */
#define VLAN_ETH_DATA_LEN	1500	/* Max. octets in vlan payload	 */
#define VLAN_ETH_FRAME_LEN	1518	/* Max. octets in vlan frame sans FCS */


#define VLAN_CFI_MASK	    0x1000  /* Canonical Format Indicator */
#define VLAN_VID_MASK	    0x0fff  /* VLAN Identifier */
#define VLAN_PRIO_MASK	    0xe000  /* Priority Code Point */
#define VLAN_PRIO_SHIFT	    13


/*
 *	ether type 定义
 */ 
#define ETH_P_ALL	     0x0003		/* all Ethernet packet	*/
#define ETH_P_LOOP	     0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_LEN	     0x0600		/* 小于 1536 表示报文长度，大于等于 1536 表示协议类型 */
#define ETH_P_IP	     0x0800		/* Internet Protocol packet	*/
#define ETH_P_ARP	     0x0806		/* Address Resolution packet	*/
#define ETH_P_IS_IS      0x8000     /* is-is */
#define ETH_P_RARP       0x8035		/* Reverse arp	*/
#define ETH_P_8021Q      0x8100     /* 802.1q vlan */
#define ETH_P_EAPS       0x8100     /* eaps */
#define ETH_P_SNMP       0x814C     /* snmp over eth */
#define ETH_P_IPV6	     0x86DD		/* IPv6 */
#define ETH_P_EPON       0x8808     /* 802.3, epon */
#define ETH_P_EFM        0x8809     /* 802.3ah，慢协议, 需要通过 subtype 区分 */
#define ETH_P_LACP       0x8809     /* lacp ，慢协议, 需要通过 subtype 区分 */
#define ETH_P_SYNCE      0x8809     /* synce ssm 报文，慢协议, 需要通过 subtype 区分 */
#define ETH_P_PPP        0x880B     /* ppp */
#define ETH_P_GSMP       0x880C     /* GSMP */
#define ETH_P_MPLS_UC	 0x8847		/* MPLS Unicast    */
#define ETH_P_MPLS_MC	 0x8848		/* MPLS Multicast  */
#define ETH_P_PPPOE_DISC 0x8863		/* PPPoE discovery */
#define ETH_P_PPPOE_SES	 0x8864		/* PPPoE session   */
#define ETH_P_8021X      0x888E     /* 802.1x */
#define ETH_P_8021Q_STAG 0x88a8     /* qinq svlan */
#define ETH_P_LLDP       0x88CC     /* lldp */
#define ETH_P_MVRP       0x88F5     /* mvrp */
#define ETH_P_MMRP       0x88F6     /* mmrp */
#define ETH_P_PTP        0x88F7     /* 1588v2 */
#define ETH_P_1588       0x88F7     /* 1588 报文 */
#define ETH_P_ELMI       0x88EE     /* elmi */
#define ETH_P_CFM        0x8902     /* 802.1ag */
#define ETH_P_ECP        0x8940
#define ETH_P_VLAN1      0x9100     /* VLAN tag */
#define ETH_P_VLAN2      0x9200     /* VLAN tag */
#define ETH_P_LOOPD      0x9966     /* loopdetect 报文*/
#define ETH_P_ISIS       0xFEFE     /* isis 报文的 llc 头，用作 ethtype */
#define ETH_P_STP        0x4242     /* stp 报文的 llc 头，用作 ethtype */

#define ETH_P_MAX        0xFFFF     /* reserved */


/* 
 * ether subtype define 
 * 位于 ethtype 之后的 1 个字节
 */
#define ETH_SUBTYPE_LACP       0x01   /* lacp 的 subtype */
#define ETH_SUBTYPE_EFM        0x03   /* 802.3ah 的 subtype */
#define ETH_SUBTYPE_SYNCE      0x0A   /* synce ssm 的 subtype */
#define ETH_SUBTYPE_ISIS       0x03   /* isis 的 subtype，使用 llc 头的第 3 个字节 */
#define ETH_SUBTYPE_STP        0x03   /* stp 的 subtype，使用 llc 头的第 3 个字节 */


/* 
 * logical link control (LLC)  for 802.3 frame
 * 位于 ethtype 之后的两个字节
 */
#define ETH_LLC_ISIS       0xFEFE     /* isis 报文的 llc 头 */
#define ETH_LLC_STP        0x4242     /* stp 报文的 llc 头 */


/* 
 * 保留组播 mac
 */
#define ETH_MAC_STP        0x0180C2000000    /* stp 报文的组播 mac */


/*
 *	以太报文头
 */ 
struct ethhdr 
{
	uint8_t	    h_dest[ETH_ALEN];	/* destination eth addr	*/
	uint8_t	    h_source[ETH_ALEN];	/* source ether addr	*/
	uint16_t	h_ethtype;          /* ethtype */
} __attribute__((packed));


/* vlan 头 */
struct vlanhdr 
{
	uint16_t	h_tpid;
	uint16_t	h_vlan_tci;
};


/* 802.1q 报文格式 */
struct vlan_ethhdr 
{
	uint8_t	    h_dest[ETH_ALEN];	/* destination eth addr	*/
	uint8_t	    h_source[ETH_ALEN];	/* source ether addr	*/
	uint16_t	h_tpid;             /* tpid of vlan */
	uint16_t	h_vlan_tci;         /* vlan and cos */
	uint16_t	h_ethtype;          /* ethtype */
}__attribute__((packed));


/* qinq 报文格式 */
struct qinq_ethhdr 
{
	uint8_t	    h_dest[ETH_ALEN];	/* destination eth addr	*/
	uint8_t	    h_source[ETH_ALEN];	/* source ether addr	*/
	uint16_t	h_svlan_tpid;       /* tpid of svlan */
	uint16_t	h_svlan_tci;        /* svlan and cos */
	uint16_t	h_cvlan_tpid;       /* tpid of cvlan */
	uint16_t	h_cvlan_tci;        /* cvlan and cos */
	uint16_t	h_ethtype;          /* ethtype */
}__attribute__((packed));


int eth_rcv_local(struct pkt_buffer *pkt);
int eth_rcv(struct pkt_buffer *pkt);
int eth_encap(struct pkt_buffer *pkt);
int eth_decap(struct pkt_buffer *pkt, uint16_t tpid);
int eth_forward(struct pkt_buffer *pkt);


#endif



