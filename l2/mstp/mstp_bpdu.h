/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_bpdu.h
*
*  date: 2016.12.28
*
*  modify:
*
*/

#ifndef _MSTP_BPDU_H_
#define _MSTP_BPDU_H_

#include "mstp.h"

#define	MSTP_DEST_MAC_0				0x01
#define MSTP_DEST_MAC_1				0x80
#define	MSTP_DEST_MAC_2				0xC2
#define	MSTP_DEST_MAC_3				0x00
#define	MSTP_DEST_MAC_4				0x00
#define	MSTP_DEST_MAC_5				0x00


#define TCN_BPDU_LEN				4
#define MIN_BPDU_LEN				7
#define BPDU_LLC_SAP				0x42
#define BPDU_LLC_CONTRL				0x03

/*bpdu size*/
#define MSTP_MAX_FRAME_SIZE			1500
#define MSTP_BPDU_LEN				102		//mstp bpdu len
#define MSTP_CONFIG_LEN				35
#define MSTP_RST_BPDU_LEN			36
#define MSTP_EXT_BPDU_BASE_LEN		64
#define MSTP_BASE_MSTI_LENTH		16

/*bpdu type*/
#define    MSTP_BPDUTYPE_CONFIG		0x01
#define    MSTP_BPDUTYPE_TCN		0x02
#define    MSTP_BPDUTYPE_RSTP		0x03
#define    MSTP_BPDUTYPE_MSTP		0x04


/*recvd msg port role*/
#define		BPDU_ROLE_MASTER		0x00
#define		BPDU_ROLE_ALTBCK		0x04
#define		BPDU_ROLE_ROOT			0x08
#define		BPDU_ROLE_DESIGNATED	0x0C

/*bpdu msg flag*/
#define		BPDU_FLAG_TC			0x01
#define		BPDU_FLAG_PROPOSAL		0x02
#define		BPDU_FLAG_LEARNING		0x10
#define		BPDU_FLAG_FORWARDING	0x20
#define		BPDU_FLAG_AGREEMENT		0x40
#define		BPDU_FLAG_MASTER		0x80
#define		BPDU_FLAG_TCACK			0x80


typedef enum
{
	BPDUDOWN_CLR,		/* clear BPDU protect down port info*/
	BPDUDOWN_SET,		/* because bpdu protect the port link down*/
}bpdudown_set_e;


/*bpdu type value*/
/*when encode and decode msg use*/
enum valid_bpdu_type
{
	BPDU_TYPE_STP			= 0x00,
	BPDU_TYPE_RSTP			= 0x02,			
	BPDU_TYPE_MSTP			= 0x02,
	BPDU_TYPE_TCN			= 0x80
};

/*****************************************************/
/*bpdu format*/
/*stp use bit1 and bit8; rstp & mstp use bits[7-1],bit8 unused*/
#if 0
struct bpdu_flags
{
    unsigned char			tc			:1;
    unsigned char    		proposal	:1;
	unsigned char    		port_role	:2;
	unsigned char    		learning	:1;
	unsigned char    		forwarding	:1;
	unsigned char    		agreement	:1;
	unsigned char    		tca			:1;
};
#else
union bpdu_flags
{
    unsigned char			tc			:1;/*bit1:Topology Change flag*/
    unsigned char    		proposal	:1;/*It is unused in STP Configuration BPDUs and shall be transmitted as 0 and ignored on receipt.*/
	unsigned char    		port_role	:2;/*Port Role in RST and MST BPDUs*//*It is unused in STP Configuration BPDUs and shall be transmitted as 0 and ignored on receipt.*/ 
	unsigned char    		learning	:1; /*in RST and MST BPDUs*/	/*It is unused in STP Configuration BPDUs and shall be transmitted as 0 and ignored on receipt.*/
	unsigned char    		forwarding	:1;/*It is unused in STP Configuration BPDUs and shall be transmitted as 0 and ignored on receipt*/
	unsigned char    		agreement	:1;/*It is unused in STP Configuration BPDUs and shall be transmitted as 0 and ignored on receipt.*/

	/*Topology Change Acknowledge Flag in STP Configuration BPDUs.
	It is unused in RST and MST BPDUs and shall be transmitted as 0 and ignored on receipt.*/
	unsigned char    		tca			:1;

	unsigned char			flag;
};
#endif

union msti_flag
{
	/*Bits 1, 2, 3 and 4, 5, 6, 7, and 8, respectively, of Octet 1 convey
	the Topology Change flag, Proposal flag, Port Role, Learning flag,
	Forwarding flag, Agreement flag, and Master flag for this MSTI.*/
	unsigned char			tc			:1; 
	unsigned char			Proposal	:1; 
	unsigned char			pt_role 	:2; 
	unsigned char			learning	:1;
	unsigned char			fowarding	:1;
	unsigned char			agreement	:1;
	unsigned char			master		:1; 	//FIXME

	unsigned char			flag;
};

struct bridge_id
{
	unsigned char			pri[2];
	unsigned char			mac_addr[MAC_LEN];
};


/*every mstp port have one this struct to store port recv msg*/
/*只有MST Configuration Identifier中的四个字段完全相同的，
并且互联的交换设备，才属于同一个域

只要两台交换设备的以下配置相同，这两台交换设备就属于同一个MST域。
MST域的域名。
多生成树实例和VLAN的映射关系。
MST域的修订级别*/
struct mst_config_id
{
	/*value always 0x00*/
    unsigned char			cfg_format_id_selector;			/*39*/

	/*The (MST) Configuration Name:
	**default:Configuration Name is bridgeAddress*/
	unsigned char			cfg_name[NAME_LEN];				/*40-71*/

	/*The Revision Level is encoded as a number
	**default: value is 0x0000*/
    unsigned short			revison_level;					/*72-73*/

	/*The Configuration Digest*/
	unsigned char			cfg_digest[DIGEST_LEN];			/*74-89*/
}__attribute__((packed));

/*every msti lenth is 16 bytes*/
struct msti_config_msg
{
	union msti_flags
	{
		/*Bits 1, 2, 3 and 4, 5, 6, 7, and 8, respectively, of Octet 1 convey
		the Topology Change flag, Proposal flag, Port Role, Learning flag,
		Forwarding flag, Agreement flag, and Master flag for this MSTI.*/
		unsigned char		tc			:1;	
		unsigned char		Proposal	:1; 
		unsigned char		pt_role		:2;	
		unsigned char		learning	:1;
		unsigned char		fowarding	:1;
		unsigned char		agreement	:1;
		unsigned char		master		:1;		//FIXME


		unsigned char		flags;
	}flag;

	/*Octets 2 through 9 convey the Regional Root Identifier.
	msti_id:bits 4 through 1 of Octet 1, and bits 8 through 1 of Octet 2
	The 4 most significant bits of each MSTI`s Regional Root Identifier is priority.

	it means:(pri[0])[bit7-4]is bridge_priority, (pri[0])[3-0]and (pri[1])[bit7-0] is MSTID
	*/
	struct bridge_id		rg_root_id;					/*2-9*/

	/*Internal Root Path Cost*/
	unsigned int			msti_in_rpc; 				/*10-13*/

	/* Bits [5~8] convey the value of the Bridge Identifier Priority for this MSTI
	Bits [1~4] shall be transmitted as 0, and ignored on receipt*/
	unsigned char			msti_br_pri;				/*14*/

	/* Bits [5~8] convey the value of the Port Identifier Priority for this MSTI
	Bits [1~4] shall be transmitted as 0, and ignored on receipt*/
	unsigned char			msti_pt_pri;				/*15*/

	unsigned char			msti_remaining_hops;		/*16*/
}__attribute__((packed));;


/*stp & rstp & mstp bpdu encode consists of the following structure*/
struct mac_header
{
	unsigned char			dmac[MAC_LEN];
	unsigned char			smac[MAC_LEN];
};

struct eth_header
{
	unsigned char			pkt_len[2];
	unsigned char			llc_dsap;
	unsigned char			llc_ssap;
	unsigned char			llc_control;	
};

struct stp_bpdu_header		/*used for config msg & TCN msg*/
{
	unsigned short			protocol_id;				/*Octets 1 and 2 */

	/**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	unsigned char			protocol_version_id;		/*Octets 3*/

	/*recv:compare with valid_bpdu_type*/
	/*send:fill up with valid_bpdu_type*/
	unsigned char			bpdu_type;					/*Octets 4*/
};


struct mstp_bpdu
{
	/*bpdu start*/
//    struct bpdu_flags		flags;						/*5*/
	union bpdu_flags		flags;

	struct bridge_id		root_id;					/*6-13*/
	unsigned int			root_path_cost;				/*14-17*/

	/*send:
	**rstp&mstp:cist regional root identifier
	**stp:CIST Bridge Identifier of the transmitting Bridge
	**recv:
	**stp&rstp:Regional Root Identifier and the CIST Designated Bridge Identifier
	**mstp:CIST Regional Root Identifier*/
	struct bridge_id		tx_bridge_id;				/*18-25*/

	unsigned short			port_id;					/*26-27*/
	unsigned short			msg_age;					/*28-29*/
	unsigned short			max_age;					/*30-31*/
	unsigned short			hello_time;					/*32-33*/
	unsigned short			forward_delay;				/*34-35*/
	/*stp bpdu end*/

	/*This shall be transmitted as 0*/
	unsigned char			version_1_lenth;			/*36*/
	/*rstp bpdu end*/

	/*Its value is the number of octets taken by the parameters that follow in the BPDU*/
	unsigned short			version_3_lenth;			/*37-38*/

	/*Octets 39 through 89 convey the elements of 
	**the MST(Multiple Spanning Tree Region) Configuration Identifier
	**同一个MST域的设备具有下列特点:
	**1.都启动了MSTP
	**2.具有相同的域名
	**3.具有相同的VLAN到生成树实例映射配置
	**4.具有相同的MSTP修订级别配置
	*/	
	struct mst_config_id	mst_cfg_id;					/*39-89*/

	unsigned int			cist_in_root_path_cost;		/*90-93*/

	/*CIST Bridge Identifier of the transmitting Bridge.
	**bits[12-1]: system id of the CIST Bridge Identifier shall be transmitted as 0.
	**The behavior on receipt is unspecified if it is non-zero.
	**bits[16-13]:constitute the manageable priority*/
	struct bridge_id		cist_bid;					/*94-101*/

	unsigned char			cist_remain_hops;			/*102*/

	/*A sequence of zero or more, up to a maximum of 64,
	**every msti_config_msg lenth is 16 bytes*/
	struct msti_config_msg	msti_cfg_msg;				/*103-xxx*/

}__attribute__((packed));

#endif


