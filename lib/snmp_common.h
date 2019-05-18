/******************************************************************************
 * Filename: snmp_common.h
 *	Copyright (c) 2018-2018 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: snmp common functions for other programs.
 *
 * History:
 * 2018.6.6  lipf created
 *
******************************************************************************/

#ifndef __SNMP_COMMON_H_
#define __SNMP_COMMON_H_

#include "libnet/libnet-asn1.h"

#include "msg_ipc.h"
#include "msg_ipc_n.h"


#define SNMP_TYPE_BOOLEAN 			ASN_BOOLEAN
#define SNMP_TYPE_INTEGER 			ASN_INTEGER			//32位整型
#define SNMP_TYPE_BIT_STR 			ASN_BIT_STR
#define SNMP_TYPE_OCTET_STR			ASN_OCTET_STR		//字符串
#define SNMP_TYPE_NULL				ASN_NULL
#define SNMP_TYPE_OBJECT_ID			ASN_OBJECT_ID		//OID
#define SNMP_TYPE_SEQUENCE			ASN_SEQUENCE
#define SNMP_TYPE_SET				ASN_SET
#define SNMP_TYPE_UNIVERSAL			ASN_UNIVERSAL
#define SNMP_TYPE_APPLICATION		ASN_APPLICATION
#define SNMP_TYPE_CONTEXT			ASN_CONTEXT
#define SNMP_TYPE_PRIVATE			ASN_PRIVATE

#define SNMP_TYPE_IPADDRESS 		(ASN_APPLICATION|0)		//IPv4类型
#define SNMP_TYPE_COUNTER	 		(ASN_APPLICATION|1)		//32位计数值类型
#define SNMP_TYPE_GAUGE		 		(ASN_APPLICATION|2)
#define SNMP_TYPE_UNSIGNED	 		(ASN_APPLICATION|2)		//无符号32位整型
#define SNMP_TYPE_TIMETICKS	 		(ASN_APPLICATION|3)		//时间类型，单位10ms
#define SNMP_TYPE_OPAQUE	 		(ASN_APPLICATION|4)
#define SNMP_TYPE_COUNTER64	 		(ASN_APPLICATION|6)		//64位计数值类型



struct snmp_trap_hdr
{	
	size_t		len_msg;
	uint32_t 	cnt_data;
	
	size_t		len_trap_oid;
	oid*		trap_oid;	
}
__attribute__ ((packed));


struct snmp_trap_data
{
	uchar  		type;
	size_t 		len_data_oid;	
	size_t		len_data;
	oid*		data_oid;
	uchar*		data;
}
__attribute__ ((packed));


#define SNMP_TRAP_HDR_LEN		sizeof(struct snmp_trap_hdr)
#define SNMP_TRAP_DATA_LEN		sizeof(struct snmp_trap_data)



struct trap_info
{
	oid		iOid;
	uchar 	type;
	uchar	*buf;
	size_t	len;		
};


int snmp_add_trap_msg_hdr(uchar *buf, struct snmp_trap_hdr *phdr);
int snmp_add_trap_msg_data(uchar *buf, struct snmp_trap_data *pdata);
int snmp_send_trap_msg(uchar *buf, int sender_id, uint8_t subtype, enum IPC_OPCODE opcode);

void ipc_ack_to_snmp(struct ipc_msghdr_n *phdr_rcv, void *pdata, int data_len, int data_num);
void ipc_noack_to_snmp(struct ipc_msghdr_n *phdr_rcv);


#endif /* __SNMP_COMMON_H_ */


