/* 
*  multicast common define 
*/

#ifndef IPMC_H
#define IPMC_H

#include <lib/linklist.h>
#include <lib/types.h>
#include <lib/hash1.h>


#define IPMC_MFIB_NUM       1024       /* 组播 IP 规格 */
#define IPMC_MMAC_NUM       1024       /* 组播 MAC 规格 */
#define IPMC_MGROUP_NUM     1024       /* 组播组规格 */

/*使用位图来存储出接口*/
#define IPMC_IF_MAX 16
#define IFBIT_MASK (sizeof(uint8_t)*8)

#define IFBIT_SIZE ((IPMC_IF_MAX) / (IFBIT_MASK))
struct if_set
{
	uint8_t if_bits[IFBIT_SIZE];
};

#define IF_ISSET(X, Y) (((Y)->if_bits[(X)/(IFBIT_MASK)]) & (1 << (((IFBIT_MASK) -1)-((X)%(IFBIT_MASK)))))
#define IF_SET(X, Y) (((Y)->if_bits[(X)/(IFBIT_MASK)]) |= (1 << (((IFBIT_MASK) -1)-((X)%(IFBIT_MASK)))))
#define IF_CLR(X, Y) (((Y)->if_bits[(X)/(IFBIT_MASK)]) &= ~(1 << (((IFBIT_MASK) -1)-((X)%(IFBIT_MASK)))))
#define IF_SETZERO(p)  (memset((p),(0), sizeof(*(p))))
#define IF_COPY(d, s, n)  (memcpy((d),(s), (n)))
#define IF_PLUS(d, s, i) ((d)->if_bits[(i)] |= (s)->if_bits[(i)])
#define IF_MINUS(d, s, i) ((d)->if_bits[(i)] &= ~((s)->if_bits[(i)]))
static inline uint32_t IF_ISZERO(const struct if_set *oifset)
{
	const struct if_set zero = {};

	return !memcmp(&zero, oifset, sizeof(struct if_set));
}


/* 组播 IPC 的 subtype */
enum IPMC_SUBTYPE
{
	IPMC_SUBTYPE_INVALID = 0,
	IPMC_SUBTYPE_GLOBAL,		/*全局组播功能*/
	IPMC_SUBTYPE_IF,			/* 组播IF */
	IPMC_SUBTYPE_MFIB,          /* 组播 fib */
	IPMC_SUBTYPE_MGROUP,        /* 组播 group */
	IPMC_SUBTYPE_MMAC,          /* 组播 mac */
	IPMC_SUBTYPE_MGROUP_IF,    /*multicast group add/delete interface*/
};

struct mcif_t
{
	uint32_t ifindex;
	uint32_t index;
};

struct mc_cache_t
{
	uint32_t  dip;					/* group ip address */
	uint32_t  sip;					/* source ip address */
	uint16_t  iif;					/* iif index*/
	uint16_t  vpnid;				/* multcast vpn id */	
	struct if_set oif;				/* oif index list*/
};

/* 组播组数据结构 */
struct mcgroup_t
{
    uint16_t  group_id;        /* multicast group id */
	struct list iflist;        /* 组播组的出接口链表，保存 ifindex */
};


/* 组播 ip 数据结构 */
struct mcip_t
{
	uint32_t  dip;					/* group ip address */
	uint32_t  sip;					/* source ip address */
	uint32_t  source_ifindex;		/* source interface index */
	uint32_t  source_port_nocheck;	/* source port no check flag */
	uint16_t  vpnid;				/* multcast vpn id */	
    uint16_t  group_id;				/* multicast group id */
};


/* 组播 mac 数据结构 */
struct mcmac_t
{
	uint8_t  dmac[MAC_LEN];   /* group mac */
	uint8_t  smac[MAC_LEN];   /* source mac */
    uint16_t group_id;        /* multicast group id */
};


#endif

