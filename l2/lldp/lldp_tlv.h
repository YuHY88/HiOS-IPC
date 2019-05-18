/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_tlv.h		(lldp tlv)
*
*  date: 2016.7.29
*
*  modify: 2016.9.23
*
*/

#ifndef _LLDP_TLV_H_
#define _LLDP_TLV_H_


#define LLDP_DECODE_TYPE_LENGTH(T,L)              					\
        do {                                      					\
             T = (*buf++);                    						\
             L = ((((unsigned short)T) & 0x01) << 8) + (*buf++);	\
             T = ((T) >> 1);                      					\
           }while (0)
           
#define LLDP_TLV_UINT16_DECODE(T,V)                           		\
		do {														\
			(V) = ((unsigned short)((T)[0]) << 8) 					\
				+ ((unsigned short)((T)[1]));						\
			(T) += 2; 												\
		} while (0)
		
#define LLDP_TLV_UINT32_DECODE(T,V)									\
		do {													    \
			(V) = ((unsigned int)((T)[0]) << 24)				    \
				+ ((unsigned int)((T)[1]) << 16)				    \
				+ ((unsigned int)((T)[2]) << 8)				    \
				+ ((unsigned int)((T)[3])); 					    \
				(T) += 4; 											\
		} while (0)

#define LLDP_ENCODE_TYPE_LENGTH(T,L)                                \
		do {																			\
			tlv_header = htons(((unsigned short)(T) << 9) + ((unsigned short)(L)));	\
			memcpy(buf, &tlv_header, 2);										  		\
			buf += 2;																	\
		} while (0)



/* TLV Types */
#define END_OF_LLDPDU_TLV				0
#define CHASSIS_ID_TLV					1
#define PORT_ID_TLV						2
#define TIME_TO_LIVE_TLV				3
#define PORT_DESCRIPTION_TLV			4
#define SYSTEM_NAME_TLV					5
#define SYSTEM_DESCRIPTION_TLV			6
#define SYSTEM_CAPABILITIES_TLV			7
#define MANAGEMENT_ADDRESS_TLV			8
/* 9 - 126 are reserved */
#define ORG_SPECIFIC_TLV				127


/* Chassis ID TLV Subtypes */

#define CHASSIS_ID_TLV_MIN_SIZE         2
#define CHASSIS_ID_TLV_MAX_SIZE         256


/* 0 is reserved */
#define CHASSIS_ID_CHASSIS_COMPONENT	1
#define CHASSIS_ID_INTERFACE_ALIAS		2
#define CHASSIS_ID_PORT_COMPONENT		3
#define CHASSIS_ID_MAC_ADDRESS			4
#define CHASSIS_ID_NETWORK_ADDRESS		5
#define CHASSIS_ID_INTERFACE_NAME		6
#define CHASSIS_ID_LOCALLY_ASSIGNED		7 
/* 8-255 are reserved */


/* Port ID TLV Subtypes */
/* 0 is reserved */
#define PORT_ID_INTERFACE_ALIAS			1
#define PORT_ID_PORT_COMPONENT			2
#define PORT_ID_MAC_ADDRESS				3
#define PORT_ID_NETWORK_ADDRESS			4
#define PORT_ID_INTERFACE_NAME			5
#define PORT_ID_AGENT_CIRCUIT_ID		6
#define PORT_ID_LOCALLY_ASSIGNED		7
/* 8-255 are reserved */ 

/*TTL*/
#define TTL_LEN							2

/*Management Address TLV format*/


/* Management Address Subtype */
#define MGMT_ADDR_SUB_IPV4              1
#define MGMT_ADDR_SUB_IPV6              2
#define MGMT_ADDR_SUB_ALL802            6

#define IF_NUMBERING_IFINDEX       		2



#define MGMT_ADDR_STR_LEN				1	/*management address string length (1 octet)*/ 
#define MGMT_ADDR_SUBTYPE				1	/* management address subtype (1 octet)*/
#define MAC_LEN							6	/*management address (6 bytes for MAC)*/ 
#define IPV4_LEN						4	/*management address (4 bytes for IPv4)*/ 
#define IF_NUM_SUBTYPE					1	/*interface numbering subtype (1 octet)*/ 
#define IF_NUM							4	/*interface number (4 bytes)*/ 
#define OID								1	/*OID string length (1 byte)*/ 
#define OBJ_IDENTIFIER					0	/*object identifier (0 to 128 octets)*/ 

#define MGMT_ADDR_MAC_TLV_LENGTH		MGMT_ADDR_STR_LEN + MGMT_ADDR_SUBTYPE \
										+ MAC_LEN + IF_NUM_SUBTYPE \
										+ IF_NUM + OID \
										+ OBJ_IDENTIFIER

#define MGMT_ADDR_IP_TLV_LENGTH			MGMT_ADDR_STR_LEN + MGMT_ADDR_SUBTYPE \
										+ IPV4_LEN + IF_NUM_SUBTYPE \
										+ IF_NUM + OID \
										+ OBJ_IDENTIFIER

/**********************************************************************/


/* IEEE 802.1 OUI */
#define IEEE_8021_OUI_1								0x00
#define IEEE_8021_OUI_2								0x80
#define IEEE_8021_OUI_3								0xC2

/* IEEE 802.1 Organizationally Specific TLVs Subtypes */
#define PORT_VLAN_ID_TLV_SUBTYPE					1
#define PORT_AND_PROTOCOL_VLAN_ID_TLV_SUBTYPE		2
#define VLAN_NAME_TLV_SUBTYPE                       3
#define PROTOCOL_IDENTITY_TLV_SUBTYPE               4
#define VID_USAGE_DIGEST_TLV_SUBTYPE                5
#define MANAGEMENT_VID_TLV_SUBTYPE					6	
#define LINK_AGGREGATION_TLV_SUBTYPE_8021			7

/********************************************************************/

/* IEEE 802.3 OUI */
#define IEEE_8023_OUI_1             				0x00
#define IEEE_8023_OUI_2             				0x12
#define IEEE_8023_OUI_3             				0x0F

/* IEEE 802.3 Organizationally Specific TLVs Subtypes */
#define MAC_PHY_CONFIG_STATUS_TLV_SUBTYPE           1
#define POWER_VIA_MDI_TLV_SUBTYPE                   2
#if 1/*deprecated*/
#define LINK_AGGREGATION_TLV_SUBTYPE_8023           3
#endif
#define MAXIMUM_FRAME_SIZE_TLV_SUBTYPE              4

#endif

