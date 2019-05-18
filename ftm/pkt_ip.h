/*
*    define of ip packet
*/


#ifndef HIOS_PKT_IP_H
#define HIOS_PKT_IP_H

#include <lib/pkt_buffer.h>
#include <lib/types.h>
#include <lib/thread.h>
#include "lib/msg_ipc.h"


#define IP_HEADER_SIZE              20    /* IPv4 头长度 */
#define IP_PSEUDO_HDR_SIZE          12    /* IPv4 伪头长度，用于计算 udp/tcp checksum */

#define IP_TTL_MAX                  255   /* TTL 最大值 */
#define IP_TTL_DEFAULT              64    /* ttl 默认值 */
#define IP_TOS_DEFAULT              3     /* tos 默认值 */


/* ip 头中的 frag_off 字段定义 */
#define IP_FLAG_CE             0x8000   /* Flag 中的 bit 15 表示拥塞: "Congestion" */
#define IP_FLAG_DF             0x4000   /* Flag 中的 bit 14 表示不分片: "Don't Fragment" */
#define IP_FLAG_MF             0x2000   /* Flag 中的 bit 13 表示分片报文的第 1 片: "More Fragments" */
#define IP_FLAG_OFFSET         0x1FFF   /* flag 中的低 12bit,表示片偏移 "Fragment Offset" */
	

/*
 *	 ip 头中的 protocol 字段定义 
 */ 
#define IP_P_HOPOPT	     0		 /* IPv6 逐跳选项   */
#define IP_P_ICMP	     1       /* ICMP   */
#define IP_P_IGMP	     2       /* IGMP   */
#define IP_P_IPIP	     4       /* IPIP   */
#define IP_P_TCP	     6       /* TCP   */
#define IP_P_UDP	     17      /* UDP   */
#define IP_P_IPV6	     41      /* IPV6   */
#define IP_P_IPV6_ROUTE	 43      /* IPv6-Route   */
#define IP_P_IPV6_FRAG	 44      /* IPv6-Frag   */
#define IP_P_IDRP	     45      /* IDRP   */
#define IP_P_RSVP	     46      /* RSVP   */
#define IP_P_GRE	     47      /* GRE   */
#define IP_P_IPV6_ESP	 50      /* IPV6 ESP */
#define IP_P_IPV6_AH	 51      /* IPV6 AH */
#define IP_P_IPV6_ICMP	 58      /* IPV6 ICMP */
#define IP_P_IPV6_NONEXT 59      /* IPV6 NONEXT */
#define IP_P_IPV6_OPTS	 60      /* IPV6 OPTS */
#define IP_P_EIGRP	     88      /* EIGRP */
#define IP_P_OSPF	     89      /* OSPF */
#define IP_P_PIM	     103     /* PIM */
#define IP_P_VRRP	     112     /* VRRP */
#define IP_P_L2TP	     115     /* L2TP */
#define IP_P_PTP	     123     /* PTP */
//#define IP_P_ISIS_IPV4	 124     /* ISIS over IPv4  */
#define IP_P_RESERV	     255     /* RESERV */

#if 0
/* ipv4 头数据结构，20 字节 */
struct iphdr
{
#if BIG_ENDIAN_ON          /* CPU 大端序 */
	uint8_t	version:4,
  		    ihl:4;         /* ip 头长度，包括 ip 基本头和 option, 20 - 60 字节，以 4 字节为单位 */
#else                      /* CPU 小端序 */
    uint8_t ihl:4,         /* ip 头长度，包括 ip 基本头和 option, 20 - 60 字节，以 4 字节为单位 */
		    version:4;
#endif
	uint8_t	    tos;
	uint16_t	tot_len;   /* 报文的总长度，包括 payload 和 header */        
	uint16_t	id;        /* 报文的 ID */     
	uint16_t	frag_off;  /* 高 3bit: flag，低 13bit: 报文的分片偏移地址 */
	uint8_t	    ttl;
	uint8_t	    protocol;  /* ip 层协议号 */
	uint16_t	check;     /* 包头校验和 */
	uint32_t    saddr;     /* 源 ip */
	uint32_t    daddr;     /* 目的 ip */
	
	/*The options start here. */
};
#endif


/* ip 头选项数据结构，可变长 */
struct ip_options 
{
  uint32_t faddr;				    /* Saved first hop address */
  uint8_t  optlen;
  uint8_t  srr;
  uint8_t  rr;
  uint8_t  ts;
  uint8_t  is_setbyuser:1,			/* Set by setsockopt?			*/
                is_data:1,			/* Options in __data, rather than skb	*/
                is_strictroute:1,	/* Strict source route			*/
                srr_is_hit:1,		/* Packet destination addr was our one	*/
                is_changed:1,		/* IP checksum more not valid		*/	
                rr_needaddr:1,		/* Need to record addr of outgoing dev	*/
                ts_needtime:1,		/* Need to record timestamp		*/
                ts_needaddr:1;		/* Need to record addr of outgoing dev  */
  uint8_t  router_alert;
  uint8_t  pad1;
  uint8_t  pad2;
  uint8_t  data[0];
};


#define optlength(opt) (sizeof(struct ip_options) + opt->optlen)


/* tcp, udp 计算校验和使用的伪首部结构，12 字节 */
struct pseudo_hdr
 {
	 uint32_t   saddr;       /* 源 ip */
	 uint32_t   daddr;       /* 目的 ip */
	 uint8_t    placeholder; /* 占位符 */
	 uint8_t    protocol;    /* ip 协议 udp/tcp */
	 uint16_t   length;      /* udp/tcp 报文长度 */
 };


/* GRE 报文头格式， 8 字节 */
struct gre_hdr
{
	uint16_t    cbit:1,      /* 1: 表示 checksum 字段和 reserv1 字段有效 */
		        reserv0:12,  /* 固定为 0 */
		        version:3;   /* 固定为 0 */
	uint16_t    proto_type;  /* 乘客协议的类型, 使用 ethtype */
	uint16_t    checksum;    /* 包含 GRE 头和 payload 的校验和 */
	uint16_t    reserv1;     /* 固定为 0 */
};


#define PKT_RETRY_NUM          3        /* 报文重传次数 */
#define PKT_RETRY_LIST_LEN     100      /* 报文重传链表长度 */

#define IP_FRAG_NUM            100      /* 支持的分片报文数量 */
#define IP_FRAG_LIST_MAX 	   100		/* 分片报文重组链表个数 */


#define IP_PROTO_IS_IPV4(ipcb)  (ipcb->pkt_type == PKT_TYPE_IP || ipcb->pkt_type == PKT_TYPE_IPBC||ipcb->pkt_type == PKT_TYPE_IPMC)
#define IP_PROTO_IS_IPV6(ipcb)  (ipcb->pkt_type == PKT_TYPE_IPV6 || ipcb->pkt_type == PKT_TYPE_IPV6MC)

#define IPV6_GET_IFLINKLOCAL(pifm,if_linklocal)\
    if (pifm && (pifm->pl3if)) \
    if_linklocal = (struct ipv6_addr *)(pifm->pl3if->ipv6_link_local.addr);
    

void ip_init(void);
int  ip_rcv(struct pkt_buffer *pkt);
int  ip_rcv_local(struct pkt_buffer *pkt); /* 本地接收 */
int  ip_forward(struct pkt_buffer *pkt);    /* 查路由转发 */
int  ip_output(struct pkt_buffer *pkt);     /* 出接口分片 */
int  ip_xmit(struct pkt_buffer *pkt);  
int  ip_encap(struct pkt_buffer *pkt);  /* 封装 ip 头 */
int  ip_decap(struct pkt_buffer *pkt);  /* 解封装 ip 头 */
int  gre_decap(struct pkt_buffer *pkt); /* 解封装 gre 头 */
int  gre_encap(struct pkt_buffer *pkt); /* 封装 gre 头 */
int gre_rcv(struct pkt_buffer *pkt);
int ip_xmit_retry(struct thread *thread);


#endif

