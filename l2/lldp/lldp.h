/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp.h
*
*  date: 2016.7.28
*
*  modify:	2018.3.10 modified by liufuying to make LLDP module code beautiful
*
*/



#ifndef _LLDP_H_
#define _LLDP_H_

#include <time.h>
#include "lldp_tlv.h"


#define min(A,B) ((A) < (B) ? (A) : (B))



#define			LLDP_DEST_MAC_0				0x01
#define			LLDP_DEST_MAC_1				0x80
#define			LLDP_DEST_MAC_2				0xC2
#define			LLDP_DEST_MAC_3				0x00
#define			LLDP_DEST_MAC_4				0x00
#define			LLDP_DEST_MAC_5				0x0E

#define			LLDP_ETH_TYPE				0x88CC

#define 		LLDP_802_3_PROTO_ID_LEN			8
#define 		LLDP_MRP_PROTO_ID_LEN			2
#define 		LLDP_DOT1X_PROTO_ID_LEN			2
#define 		LLDP_SLOW_PROTO_ID_LEN			3
#define 		LLDP_UNKNOWN_PROTO_ID_LEN		0


#define			INTERFACE_NAMSIZ				32
#define			VLAN_NAMSIZ						32
#define			SYSTEM_NAMSIZ					32
#define			MSAP_NAMSIZ						64	/*the value is 128 in before version*/
#define			OID_STRING_LEN					128

#define			MAX_SIZE						128	/*the value is 512 in before version */


#define			SYSTEM_DESCR_MAX_SIZE	 256



#ifndef NULL
#define			NULL		0
#endif

#ifndef LLDP_TRUE
#define			LLDP_TRUE		1
#endif

#ifndef LLDP_FALSE
#define			LLDP_FALSE		0
#endif



enum portAdminStatus 
{
	disabled = 0,
#if 0
    enabledTxOnly,			/*do not support*/
    enabledRxOnly,			/*do not support*/
#endif
    enabledRxTx,
};

enum lldp_protocol
{
  LLDP_PROTO_STP,
  LLDP_PROTO_RSTP,
  LLDP_PROTO_MSTP,
  LLDP_PROTO_GMRP,
  LLDP_PROTO_MMRP,
  LLDP_PROTO_GVRP,
  LLDP_PROTO_MVRP,
  LLDP_PROTO_LACP,
  LLDP_PROTO_DOT1X,
  LLDP_PROTO_EFM_OAM,
  LLDP_PROTO_UNKNOWN,
  LLDP_PROTO_MAX,
};

#define LLDP_LEARNING_ARP					(1 << 0)	//ipv4
#define LLDP_LEARNING_ND					(1 << 1)	//ipv6

struct lldp_arp_key
{
	uint32_t ipaddr;
	uint32_t vpnid;
};

enum LLDP_ARP_STATUS
{
    LLDP_ARP_STATUS_INCOMPLETE = 0,	// 动态 arp 半连接状态
    LLDP_ARP_STATUS_COMPLETE,        // 动态 ARP
    LLDP_ARP_STATUS_STATIC,          // 静态 ARP
    LLDP_ARP_STATUS_AGED,            // 老化状态
    LLDP_ARP_STATUS_LLDP, 			//lldp learn
    LLDP_ARP_STATUS_MAX = 8
};

struct lldp_arp_entry
{
    struct lldp_arp_key   key;
	enum LLDP_ARP_STATUS  status;   // arp 状态
	uint32_t         ifindex;  // output interface
	uint32_t         port;     // 用于 vlanif 接口的物理成员口
	uint32_t         arpindex; // arp 索引
	uint8_t          mac[6];
	uint16_t         time;     // 剩余老化时间
    uint8_t          count;    // 发送 arp 请求的次数
	uint8_t          pad[3];
};


#if 0
struct lldp_protocol_identity
{
	unsigned char		*protocol_payload;
	unsigned char		 protocol_id_len;
	unsigned char		 protocol_id;
	unsigned char		 *protocol_str;
};
#endif

/*************************************************/




/**************************************************/


/*used for autonego_cap*/
#define AUTONEGO_BOTHER                           0
#define AUTONEGO_B10BASET                         1
#define AUTONEGO_B10BASETFD                       2
#define AUTONEGO_B100BASET4                       3
#define AUTONEGO_B100BASETX                       4
#define AUTONEGO_B100BASETXFD                     5
#define AUTONEGO_B100BASET2                       6
#define AUTONEGO_B100BASET2FD                     7
#define AUTONEGO_BFDXPAUSE                        8
#define AUTONEGO_BFDXAPAUSE                       9
#define AUTONEGO_BFDXSPAUSE                       10
#define AUTONEGO_BFDXBPAUSE                       11
#define AUTONEGO_B1000BASEX                       12
#define AUTONEGO_B1000BASEXFD                     13
#define AUTONEGO_B1000BASET                       14
#define AUTONEGO_B1000BASETFD                     15

/*used for oper_mau_type*/
#define DOT3MAUTYPEAUI                            1
#define DOT3MAUTYPE10BASE5                        2
#define DOT3MAUTYPEFOIRL                          3
#define DOT3MAUTYPE10BASE2                        4
#define DOT3MAUTYPE10BASET                        5
#define DOT3MAUTYPE10BASEFP                       6
#define DOT3MAUTYPE10BASEFB                       7
#define DOT3MAUTYPE10BASEFL                       8
#define DOT3MAUTYPE10BROAD36                      9
#define DOT3MAUTYPE10BASETHD                      10
#define DOT3MAUTYPE10BASETFD                      11
#define DOT3MAUTYPE10BASEFLHD                     12
#define DOT3MAUTYPE10BASEFLFD                     13
#define DOT3MAUTYPE100BASET4                      14
#define DOT3MAUTYPE100BASETXHD                    15
#define DOT3MAUTYPE100BASETXFD                    16
#define DOT3MAUTYPE100BASEFXHD                    17
#define DOT3MAUTYPE100BASEFXFD                    18
#define DOT3MAUTYPE100BASET2HD                    19
#define DOT3MAUTYPE100BASET2FD                    20
#define DOT3MAUTYPE1000BASEXHD                    21
#define DOT3MAUTYPE1000BASEXFD                    22
#define DOT3MAUTYPE1000BASELXHD                   23
#define DOT3MAUTYPE1000BASELXFD                   24
#define DOT3MAUTYPE1000BASESXHD                   25
#define DOT3MAUTYPE1000BASESXFD                   26
#define DOT3MAUTYPE1000BASECXHD                   27
#define DOT3MAUTYPE1000BASECXFD                   28
#define DOT3MAUTYPE1000BASETHD                    29
#define DOT3MAUTYPE1000BASETFD                    30
#define DOT3MAUTYPE10GIGBASEX                     31
#define DOT3MAUTYPE10GIGBASELX4                   32
#define DOT3MAUTYPE10GIGBASER                     33
#define DOT3MAUTYPE10GIGBASEER                    34
#define DOT3MAUTYPE10GIGBASELR                    35
#define DOT3MAUTYPE10GIGBASESR                    36
#define DOT3MAUTYPE10GIGBASEW                     37
#define DOT3MAUTYPE10GIGBASEEW                    38
#define DOT3MAUTYPE10GIGBASELW                    39
#define DOT3MAUTYPE10GIGBASESW                    40

/***************************************************************/

/*sys_cap
**sys_cap_enable
*/
#define LLDP_SYS_CAP_L2_OTHER						(1 << 0)
#define LLDP_SYS_CAP_L2_REPEATER					(1 << 1)
#define LLDP_SYS_CAP_L2_SWITCHING					(1 << 2)
#define LLDP_SYS_CAP_L2_WAN							(1 << 3)
#define LLDP_SYS_CAP_ROUTING						(1 << 4)
#define LLDP_SYS_CAP_TEL							(1 << 5)
#define LLDP_SYS_CAP_CABLE_DEV						(1 << 6)
#define LLDP_SYS_CAP_STATION						(1 << 7)
#define LLDP_SYS_CAP_CVLAN							(1 << 8)
#define LLDP_SYS_CAP_SVLAN							(1 << 9)
#define LLDP_SYS_CAP_TPMR							(1 << 10)


/*	basic_tlv_tx_enable */
/*basic  */
#define PORT_DESCRIPTION_TLV_TX_ENABLE				(1 << 0)
#define SYSTEM_NAME_TLV_TX_ENABLE					(1 << 1)
#define SYSTEM_DESCRIPTION_TLV_TX_ENABLE			(1 << 2)
#define SYSTEM_CAPABILITIES_TLV_TX_ENABLE			(1 << 3)
#define MANAGEMENT_ADDRESS_TLV_TX_ENABLE			(1 << 4)


/*org specific	sub_tlv_tx_enable */
/*802.1*/
#define PORT_VLAN_ID_TX_ENABLE						(1 << 0)
#define PROTO_VLAN_ID_TX_ENABLE						(1 << 1)
#define PORT_VLAN_NAME_TX_ENABLE					(1 << 2)
#define PROTOCOL_ID_TX_ENABLE						(1 << 3)
/*802.3*/
#define MAC_PHY_CONFIG_STATUS_TX_ENABLE				(1 << 4)
#define POWER_TX_ENABLE								(1 << 5)
#define LINK_AGG_TX_ENABLE							(1 << 6)
#define MAX_FRAME_SIZE_TX_ENABLE					(1 << 7)


#define TIME_MARK_INDEX_MAX        10

/*************************************************************/
struct lldp_tx_port_statistics {
    unsigned long long		statsFramesOutTotal;
};

struct lldp_tx_port 
{
	/*need to be malloc*/
	unsigned char			*frame;    	/*The tx frame buffer*/
	unsigned int			sendsize; 	/*tx frame size*/
	unsigned char			state;    	/*The tx state for this interface*/
	unsigned char			somethingChangedLocal;/*TRUE or FALSE, local msg change(Interval), into tx_fast*/
	unsigned short			txTTL;
	unsigned short			txTTR;

	struct lldp_tx_port_statistics statistics;
};

struct lldp_rx_port_timers {
#if 0	/*do not support tooManyNeighbors*/
	unsigned short			tooManyNeighborsTimer;
#endif
	unsigned short			rxTTL;
};

struct lldp_rx_port_statistics {
    unsigned long long 		statsAgeoutsTotal;
    unsigned long long 		statsFramesDiscardedTotal;
    unsigned long long 		statsFramesInErrorsTotal;
    unsigned long long 		statsFramesInTotal;			/*good frame*/
    unsigned long long 		statsTLVsDiscardedTotal;
    unsigned long long 		statsTLVsUnrecognizedTotal;
};

struct lldp_rx_port 
{
    unsigned char 			*frame;
    unsigned long long		recvsize;
    unsigned char 			state;
    unsigned char 			badFrame;
    unsigned char 			rcvFrame;		/*when recv one frame is TRUE*/
    unsigned char 			rxInfoAge;
    unsigned char 			somethingChangedRemote;	/*when add new neighbor*/

	struct lldp_rx_port_timers 		timers;
	struct lldp_rx_port_statistics	statistics;
};

struct lldp_neighbor_msg 
{
	/*basic message*/
	unsigned char			chassis_id_subtype;
	unsigned char			chassis_id_mac_addr[MAC_LEN];	/*subtype:4*/
	unsigned char			chassis_id_net_addr[IPV4_LEN]; /*subtype:5*/

	unsigned char			port_id_subtype;
	unsigned char			if_name[INTERFACE_NAMSIZ];/*subtype:5*/
	unsigned char			network_addr[IPV4_LEN];	/*subtype:4*/

	unsigned short			rx_ttl;

	unsigned char			pt_descr[IFM_ALIAS_STRING_LEN+1];

	unsigned char			sys_name[MAX_SIZE];

	unsigned char			sys_descr[SYSTEM_DESCR_MAX_SIZE];

	unsigned short			sys_cap;
	unsigned short			sys_cap_enabled;

	unsigned char			mgmt_addr_sub_type;
	unsigned char			mgmt_addr[31];
	unsigned char			if_numbering;
	unsigned int			if_number;
	unsigned char			oid_string_len;
	unsigned char			obj_oid[OID_STRING_LEN];

	time_t time_mark[TIME_MARK_INDEX_MAX];
	uint8_t time_mark_index;

#if 0	/*No support*/
	/*802.1*/
	/*subtype:1*/
	unsigned short			port_vlan_id;

	/*subtype:2*/
	unsigned char			pt_protocol_flag;
	unsigned short			pp_vlan_id;

	/*subtype:3*/
	unsigned short			vlan_id;
	unsigned char			vlan_name_len;
	unsigned char			vlan_name[32];

	/*subtype:4*/
	unsigned short			protocol;

	/*subtype:7*/
	unsigned char			link_aggr_status_8021;
	unsigned int			link_aggr_id_8021;


	/*802.3*/
	/*subtype:1*/
	unsigned char			autonego_support_status;
	unsigned short			autonego_cap;
	unsigned short			oper_mau_type;

	/*subtype:2*/
	unsigned char			mdi_power_support;
	unsigned char			pse_power_pair;
	unsigned char			power_class;

	/*subtype:3*/
	unsigned char			link_aggr_status_8023;
	unsigned int			link_aggr_id_8023;

	/*subtype:4*/
	unsigned short			max_frame_size;
#endif	
};

struct lldp_msap 
{
	struct lldp_msap 			*next;
	unsigned char 				msap_id[MSAP_NAMSIZ];	/*smac+vlan+portid*/
	unsigned char 				msap_id_length;

	unsigned short				svlan_tpid;		
	unsigned short				svlan;					/*COS CFI VLANID*/
	unsigned short				cvlan_tpid;
	unsigned short				cvlan;					/*COS CFI VLANID*/

	struct lldp_neighbor_msg	neighbor_msg;

	unsigned short 				rxInfoTTL;				/*ageTime*/
	uint32_t vpn;
	uint32_t pkt_ifindex;
	uint8_t arp_notice_flag;
};


struct lldp_tlv_cfg
{
	unsigned char			basic_tlv_tx_enable;
	unsigned char			sub_tlv_tx_enable;
};

struct lldp_port 
{
  	unsigned char			portEnabled;		/*link up:1, link down:0*/		
  	unsigned char			adminStatus;		/*enabledRxTx:1, disabled:0*/
	
	char					if_name[INTERFACE_NAMSIZ];	/*The interface name*/
  	unsigned int			if_index;			/*The interface index*/ 

  	unsigned short			sys_cap;			/*Capabilities*/
  	unsigned short			sys_cap_enable;		/*Capabilities*/

	unsigned char			rxChanges;			/*add/delete neighbor, trap info*/

	struct lldp_tlv_cfg 	tlv_cfg;			/*local port TLV cfg (enable / disable)*/

#if 0	/*No support*/
	/*802.1 subtype:1*/
	unsigned short			pvid;				/*default port vlan*/

	/*802.1 subtype:2*/	
	unsigned char			pt_protocol_flag;	/*we do not support, value is 0*/
	unsigned short			pp_vlan_id;			/*we do not support, value is 0*/

	/*802.1 subtype:3*/
	unsigned short			vlan_id;
	unsigned char			vlan_name_len;
	unsigned char			vlan_name[VLAN_NAMSIZ];

	/*802.1 subtype:4*/
	unsigned short			protocol;			//FIXME

	/*802.3 subtype = 1, MAC/PHY configuration/status TLV */
	unsigned char			autonego_support;	
	unsigned short			autonego_cap;		
	unsigned short			oper_mau_type;		/*base of interface work(ifp->duplex,ifp->bandwidth), set value*/

	/*802.3 subtype = 2 , Power Via MDI TLV */
	unsigned char			mdi_power_support;
	unsigned char			pse_power_pair;
	unsigned char			power_class;	

	/*802.3 subtype = 3*/
	unsigned char			link_aggr_status;	/*we do not support, value is 0*/
	unsigned int			link_aggr_id;		/*we do not support, value is 0*/
#endif

	/*802.3 subtype = 4*/
	unsigned short			max_frame_size;			 
	
	struct lldp_rx_port		rx;				/*rx msg*/
	struct lldp_tx_port		tx;				/*tx msg*/

  	unsigned char			msap_num;		/*have neighbor num*/
	struct lldp_msap 		*msap_cache;	/*store neighbor msg*/

	uint8_t arp_nd_learning;
	uint16_t arp_nd_learning_vlan;
	uint32_t l3_vpnid;
	uint32_t pkt_ifindex;
	uint8_t if_mac[6];
	char alias[IFM_ALIAS_STRING_LEN+1];
};

typedef struct _lldp_global
{
	unsigned short			msgTxInterval;	/*default: 30s, range: <1-3600>*/
	unsigned char			msgTxHold;		/*default: 4, range: <2-10>*/
	unsigned char			trapEnable;		/*TRUE(1), FALSE(0),default is TRUE*/
	unsigned short			trapInterval;	/*default: 5s, range: <5-3600>*/

//unsigned char 			system_name[SYSTEM_NAMSIZ];
	unsigned char			system_name[SYSTEM_NAMSIZ];
	unsigned char 			system_desc[MAX_SIZE];

	unsigned char			mng_addr_sub;		/*6:MAC, 1:ip*/
  	unsigned char			source_mac[MAC_LEN];
  	unsigned char			source_ipaddr[IPV4_LEN];	/*management IP addr*/

	unsigned char			timer;				/*tx/rx state machine interval, default 1s*/
	unsigned char			global_enable;	/*Global enable flag,default 0(disable)*/

        unsigned char                           arp_learn_enable;              /*Global  dcn_enable flag */
        unsigned short                           dcn_vlan ;  /*Global  dcn_vlan , default */
    
}lldp_global;

struct eth_hdr 
{
	unsigned char 			dmac[MAC_LEN];
	unsigned char 			smac[MAC_LEN];

	unsigned short			svlan_tpid;	
	unsigned short			svlan;
	unsigned short			cvlan_tpid;
	unsigned short			cvlan;

	unsigned short 			ethertype;
};





struct lldp_neighbor_trap_info
{
	/*basic message*/
	unsigned char			chassis_id_subtype;
	unsigned char			chassis_id_mac_addr[MAC_LEN];	/*subtype:4*/
	unsigned char			chassis_id_net_addr[IPV4_LEN]; /*subtype:5*/
	
	unsigned char			port_id_subtype;
	unsigned char			port_if_name[INTERFACE_NAMSIZ];/*subtype:5*/
	unsigned char			network_addr[IPV4_LEN];	/*subtype:4*/
	
	unsigned char			pt_descr[IFM_ALIAS_STRING_LEN+1];
	unsigned char			sys_name[MAX_SIZE];
	unsigned char			sys_descr[SYSTEM_DESCR_MAX_SIZE];
};

struct lldp_msap_trap
{
	unsigned char           msap_num;
	unsigned char           if_name[INTERFACE_NAMSIZ];	/*The interface name*/

	/*basic message*/
	unsigned char			chassis_id_subtype;
	unsigned char			chassis_id_mac_addr[MAC_LEN];	/*subtype:4*/
	unsigned char			chassis_id_net_addr[IPV4_LEN]; /*subtype:5*/
	
	unsigned char			port_id_subtype;
	unsigned char			port_if_name[INTERFACE_NAMSIZ];/*subtype:5*/
	unsigned char			network_addr[IPV4_LEN];	/*subtype:4*/
	
	unsigned char			pt_descr[IFM_ALIAS_STRING_LEN+1];
	unsigned char			sys_name[MAX_SIZE];
	unsigned char			sys_descr[SYSTEM_DESCR_MAX_SIZE];
};

/*lldp debug define */

#define LLDP_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_LLDP, fmt, ##__VA_ARGS__ ); \
    } while(0)

#endif

