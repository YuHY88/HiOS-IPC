/*
*      igmp packet structure and function
*/

#ifndef IGMP_PKT_H
#define IGMP_PKT_H

#include <lib/types.h>
#include <lib/pkt_buffer.h>

extern struct igmp_pkt_debug igmp_pkt_dbg;

#define IGMP_DBG_RCV   	 	0x01
#define IGMP_DBG_SND		0x02
#define IGMP_DBG_OTHER		0x04
#define IGMP_DBG_ALL		(IGMP_DBG_RCV| IGMP_DBG_SND| IGMP_DBG_OTHER)


/*IGMP version*/
#define IGMP_VERSION1 	0x1
#define IGMP_VERSION2 	0x2
#define IGMP_VERSION3 	0x3


/*IGMP message ipaddress*/
#define IGMP_GENERAL_QUERY_IPADDRESS	0xe0000001      /* 224.0.0.1 */
#define IGMP_V2_LEAVE_IPADDRESS			0xe0000002      /* 224.0.0.2 */
#define IGMP_V3_REPORT_IPADDRESS		0xe0000016      /* 224.0.0.22 */

/*IGMP message length*/
#define IGMP_HLEN_MIN		    20       /* 最小包头长度 */
#define IGMP_HLEN_MAX		    60       /* 最大包头长度 */
#define IGMP_PLEN_MIN			8        /* 最小报文长度 */

/* IGMP packet types define */
#define IGMP_QUERY_MEMBER		0x11     /* 成员查询报文 */
#define IGMP_REPORT_MEMBER_V1	0x12     /* 成员报告报文 */
#define IGMP_REPORT_MEMBER_V2	0x16
#define IGMP_REPORT_MEMBER_V3	0x22
#define IGMP_REPORT_LEAVE_V2	0x17     /* 离开组报告报文 */

/* IGMP packet code */
enum IGMP_CODE
{
	IGMP_CODE_INVALID = 0,
	IGMP_CODE_GENERAL_QUERY,
	IGMP_CODE_GROUP_QUERY,
	IGMP_CODE_GROUP_SOURCE_QUERY,
	IGMP_CODE_GENERAL_REPORT,
	IGMP_CODE_GROUP_REPORT,
	IGMP_CODE_GROUP_SOURCE_REPORT,
	IGMP_CODE_LEAVE_REPORT,
	IGMP_CODE_MAX = 255
};


/*IGMP qurey packet*/
struct igmpv3_query
{
	uint8_t			type;		/* type */
	uint8_t			code;		/* max resp code */
	uint16_t		chsum;		/* checksum */
	uint32_t		group;  	/* group address */
	
#if BIG_ENDIAN_ON
	uint8_t 		qrv:3,		/* Querier's Robustness Variable */
					sflag:1,	/* S flag, suppress Router-side processing */
					resv:4;		/* reserved */
#else
	uint8_t			resv:4,
					sflag:1,
					qrv:3;
#endif
	uint8_t			qqi_code;	/* Querier's Query Interval Code */
	uint16_t		src_num;	/* Number of Sources */
	uint32_t		sip[1];	    /* Source Address */
};


/* IGMP v1/v2 report packet */
struct igmpv12_report
{
	uint8_t 	    type; 		/* type */
	uint8_t		    code;		/* max resp code */
	uint16_t	    chsum;		/* checksum */
	uint32_t	    group;  	/* group address */
};


/*IGMPv3 Group Record in Report*/
struct igmppv3_grec
{
	uint8_t			type;		/*record type*/
	uint8_t			aux_data;	/*aux data len*/
	uint16_t		src_num;	/*number of sources*/
	uint32_t 		group;		/*group address*/
	uint32_t 		sip[1];	    /*source addresses*/
};


/*IGMPv3 Report Packet*/
struct igmpv3_report
{
	uint8_t				type;		/* report type */
	uint8_t 			resv1;		/* reserved1 */
	uint16_t			chsum;		/* checksum */
	uint16_t			resv2;		/* reserved2 */
	uint16_t			grec_num;	/* number of group records */
	struct igmppv3_grec grec[1];	/* group record */
};


void igmp_pkt_register(void);
void igmp_pkt_unregister(void);

int igmp_ipmc_pkt_send(uint32_t src_ip, uint32_t dst_ip, uint32_t ifindex, void *data, int data_len);

int igmp_pkt_recv(struct pkt_buffer *ppkt);

void igmp_packet_dump(void *data, uint8_t msg_type, uint8_t sub_type);

#endif

