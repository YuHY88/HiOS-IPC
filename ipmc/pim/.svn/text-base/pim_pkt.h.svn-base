/*定义pim报文相关的结构体，宏*/
#ifndef PIM_PKT_H
#define PIM_PKT_H

#include <lib/types.h>

#define PIM_VERSION	2
#define PIM_HDR_LEN  4
#define ALLPIM_ROUTER_GROUP (0xE000000D)

/* PIM message types */
#define PIM_HELLO		         0x0	/* PIM-SM and PIM-DM */
#define PIM_REGISTER		     0x1	/* PIM-SM only */
#define PIM_REGISTER_STOP	     0x2	/* PIM-SM only */
#define PIM_JOIN_PRUNE		     0x3	/* PIM-SM and PIM-DM */
#define PIM_BOOTSTRAP		     0x4	/* PIM-SM only */
#define PIM_ASSERT		         0x5	/* PIM-SM and PIM-DM */
#define PIM_GRAFT_MSG		     0x6	/* PIM-DM only */
#define PIM_GRAFT_ACK		     0x7	/* PIM-DM only */
#define PIM_CAND_RP_ADV		     0x8	/* PIM-SM only */
#define PIM_STATE_REFRESH        0x9    /* PIM-DM only */

/*地址格式对应的结构体*/
struct encode_ipv4_ucast
{
	uint8_t addr_family;			/* Addr Family */
	uint8_t encode_type;			/* Encoding Type */
	uint32_t unicast_addr;			/* Unicast Address */
};

#define PIM_ENCODE_IPV4_UCAST_ADDR_LEN 6
struct encode_ipv4_source
{
	uint8_t addr_family;			/* Addr Family */
	uint8_t encode_type;			/* Encoding Type */
	uint8_t flag;					/* Reserved(5) +S bit + WC bit+ RPT bit*/
	uint8_t mask_len;
	uint32_t src_addr;				/* Source Address */
};

#define PIM_ENCODE_IPV4_SRC_ADDR_LEN 8
struct encode_ipv4_group
{
	uint8_t addr_family;			/* Addr Family */
	uint8_t encode_type;			/* Encoding Type */
	uint8_t reserved;				/* Bidirectional + Reserved + admin scope zone*/
	uint8_t mask_len;
	uint32_t grp_addr;				/* Group Address */
};
#define PIM_ENCODE_IPV4_GRP_ADDR_LEN 8

/*PIM报文头部*/
struct pim_hdr 
{
#if BIG_ENDIAN_ON
	uint8_t pim_vers:4,	    	/* PIM Version  */
			pim_types:4;		/* PIM Type */
#else 
	uint8_t pim_types:4,	    /* PIM Type  */
			pim_vers:4;			/* PIM Version */
#endif
	uint8_t pim_reserved;		/* Reserved	*/
	uint16_t pim_cksum;	   		/* checksum */
};

struct pim_join_prune_hdr
{
	struct encode_ipv4_ucast upstream;			/* Upstream Neighbor Address */
	uint8_t reserved;							/* Reserved */
	uint8_t group_num;							/* Num groups */
	uint16_t holdtime;							/* Holdtime */
	//struct pim_join_prune_grp *grp_list;		/* Multicast Group Address 1..m*/
};

/*每个组信息包括：组地址，加枝数目，剪枝数目，加枝源地址列表，剪枝源地址列表。*/
struct pim_join_prune_grp
{
	struct encode_ipv4_group grp;				/* Group Address */
	uint16_t join_num;							/* Number of Joined Sources */
	uint16_t prune_num;							/* Number of Pruned Sources */
	//struct encode_ipv4_source *join_list;		/* Joined Source Address 1..n*/
	//struct encode_ipv4_source *prune_list;		/* Pruned Source Address 1..n*/
};

/* PIM Hello报文*/
/*option不是固定的，没有固定的结构体*/

/*PIM Register报文*/
/*pim_hdr + flag(border bit + null_reg bit) + (ip+udp+data非空注册报文)/(ip+udp空注册报文)*/
struct pim_register_t{
	uint32_t reg_flags;
};

/*PIM Register-stop报文*/
/*pim_hdr + encode_ipv4_group(组地址) + encode_ipv4_unicast(单播地址)*/
typedef struct pim_register_stop{
	struct  encode_ipv4_group encode_grp;
	struct  encode_ipv4_ucast encode_src;
}pim_register_stop_t;

/*PIM join/prune报文*/

/*接收PIM协议报文*/
sint32 pim_pkt_recv(struct pkt_buffer *ppkt);

/*设备使能组播功能，才能接收处理组播报文*/
/*处理PIM报文，处理IGMP报文*/
void pim_pkt_register(void);
void pim_pkt_unregister(void);

#endif /* PIM_PKT_H */
