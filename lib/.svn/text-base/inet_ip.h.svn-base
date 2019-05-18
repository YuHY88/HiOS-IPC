/*
*		ip address common define and function
*/

#ifndef HIOS_INET_H
#define HIOS_INET_H


#include <lib/types.h>



#define IP_ADDR_LEN              4          /* IPV4 地址字节长度 */
#define IPV6_ADDR_LEN            16         /* IPV6 地址字节长度 */
#define IP_MASK_LEN              32         /* IPV4 地址掩码长度 */
#define IPV6_MASK_LEN            128        /* IPV6 地址掩码长度 */
#define IP_ADDR_STRLEN           16         /* ipv4 地址字符串长度 */
#define IPV6_ADDR_STRLEN         51         /* ipv6 地址字符串长度 */


/* Defines for Multicast INADDR */
#define IPV4_UNSPEC_GROUP   	0xe0000000	/* 224.0.0.0   */
#define IPV4_ALLHOSTS_GROUP 	0xe0000001	/* 224.0.0.1   */
#define IPV4_ALLRTRS_GROUP      0xe0000002	/* 224.0.0.2 */
#define IPV4_MAX_LOCAL_GROUP    0xe00000ff	/* 224.0.0.255 */


#define	IPV4_LOOPBACK		    0x7f000001	/* loopback 地址 127.0.0.1   */
#define	IPV4_BROADCAST	        0xffffffff  /* 广播地址 */
#define	IPV4_ANYCAST		    0x00000000  /* Address to accept any incoming messages. */
#define	IPV4_LOOPBACKNET		127         /* Network number for local host loopback. */


/*
 * Definitions of the bits in an Internet address integer.
 * On subnets, host and network parts are found according
 * to the subnet mask, not these masks.
 */
#ifdef IN_CLASSA
#undef IN_CLASSA
#define	IN_CLASSA(a)		((((long int) (a)) & 0x80000000) == 0)
#endif
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_BROADCAST	0x00ffffff
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		(0xffffffff & ~IN_CLASSA_NET)
#define	IN_CLASSA_MAX		128

#ifdef IN_CLASSB
#undef IN_CLASSB
#define	IN_CLASSB(a)		((((long int) (a)) & 0xc0000000) == 0x80000000)
#endif
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_BROADCAST	0x0000ffff
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		(0xffffffff & ~IN_CLASSB_NET)
#define	IN_CLASSB_MAX		65536

#ifdef IN_CLASSC
#undef IN_CLASSC
#define	IN_CLASSC(a)		((((long int) (a)) & 0xe0000000) == 0xc0000000)
#endif
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_BROADCAST	0x000000ff
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		(0xffffffff & ~IN_CLASSC_NET)

#ifdef IN_CLASSD
#undef IN_CLASSD
#define	IN_CLASSD(a)		((((long int) (a)) & 0xf0000000) == 0xe0000000)
#endif
#define	IN_MULTICAST(a)		IN_CLASSD(a)
#define IN_MULTICAST_NET	0xF0000000

#ifdef IN_EXPERIMENTAL
#undef IN_EXPERIMENTAL
#define	IN_EXPERIMENTAL(a)	((((long int) (a)) & 0xf0000000) == 0xf0000000)
#endif
#ifdef IN_BADCLASS
#undef IN_BADCLASS
#define	IN_BADCLASS(a)		IN_EXPERIMENTAL((a))
#endif
#define	IN_LOOPBACK(a)		((((long int) (a)) & 0xff000000) == 0x7f000000)


/* ipv6 地址比较 */
#define IPV6_ADDR_SAME(D,S)  (memcmp ((D), (S), IPV6_ADDR_LEN) == 0)  /* 地址相同 */
#define IPV6_ADDR_COPY(D,S)   memcpy ((D), (S), IPV6_ADDR_LEN)         /* 地址拷贝 */
#define IPV6_ADDR_CMP(D,S)    memcmp ((D), (S), IPV6_ADDR_LEN)         /* 地址比较 */


#define IPV6_ADDR_ANYCASET   	0x0000
#define IPV6_ADDR_UNICAST      	0x0001
#define IPV6_ADDR_MULTICAST    	0x0002
#define IPV6_ADDR_ANYCAST		0x0004
#define IPV6_ADDR_LOOPBACK		0x0010
#define IPV6_ADDR_LINKLOCAL		0xfe80        /* 链路本地地址的前 10bit */
#define IPV6_ADDR_SITELOCAL		0x0040
#define IPV6_ADDR_COMPATv4		0x0080
#define IPV6_ADDR_SCOPE_MASK	0x00f0
#define IPV6_ADDR_MAPPED		0x1000
#define IPV6_ADDR_RESERVED		0x2000       /* reserved address space */


#ifndef IN6ADDRSZ
#define IN6ADDRSZ IPV6_ADDR_LEN
#endif

#ifdef SPRINTF_CHAR
# define SPRINTF(x) strlen(sprintf/**/x)
#else
# define SPRINTF(x) ((size_t)sprintf x)
#endif


/* 地址族类型 */
enum INET_FAMILIY
{
  INET_FAMILY_INVALID = 0,
  INET_FAMILY_IPV4,
  INET_FAMILY_IPV6,
};


/* ipv6 地址*/
struct ipv6_addr
{
  uint8_t ipv6[16];
};


/* IPV6 & IPV4 地址*/
struct inet_addr
{
	enum INET_FAMILIY type;
	union {
	  uint8_t  ipv6[16]; /* ipv6 地址分 8 段显示 */
	  uint32_t ipv4;
	}addr;
};


/* IPV6 & IPV4 地址和掩码长度 */
struct inet_prefix
{
  enum INET_FAMILIY type;
  int   prefixlen;
  union {
    uint8_t  ipv6[16];   /* ipv6 地址分 8 段显示 */
    uint32_t ipv4;
  }addr;
};


/* IPv4 地址和掩码长度 */
struct inet_prefixv4
{
  int      prefixlen;
  uint32_t addr;
};


/* IPv6 地址和掩码长度 */
struct inet_prefixv6
{
  int      prefixlen;
  uint8_t  addr[16];
};


/* 网络序 ipv4 地址与字符串相互转换 */
char *inet_ntop(int af, const void *src, char *dst, int size);
int inet_pton(int af, const char *src, void *dst);
//int inet_aton(const char *cp, struct in_addr *addr);
//char *inet_ntoa(struct in_addr in);


/* 主机序ipv4地址与字符串相互转换 */
char *inet_ipv4tostr(u_int32 addr, char *s);
u_int32 inet_strtoipv4(char *s);
char *inet_masktostr(u_int32 addr, u_int32 mask);

char *inet_ipv6tostr(struct ipv6_addr *addr, char *s, int size);
int inet_strtoipv6(char *s, struct ipv6_addr *dest);
char *inet_prefixtostr(struct inet_prefix *prefix, char *dest, int size);
char *inet_addrtostr(struct inet_addr *addr, char *dest, int size);

/* 检查 IPV4 地址的合法性 */
int inet_valid_host(u_int32 addr);/* 检查主机序 IPV4 地址的合法性*/
int inet_valid_mask(u_int32 mask); /* 检查主机序 IPV4 掩码的合法性*/
int inet_valid_ipv4(uint32_t ip);  /* 检查主机序 IPV4 主机地址的合法性*/
int inet_valid_network(uint32_t ip); /* 检查主机序 IPV4 网段地址的合法性*/


/* 计算校验和 */
int inet_checksum(u_int16 *addr, u_int len);


/* 检查主机序 IPV4 地址类型 */
uint32_t ipv4_is_loopback(uint32_t addr);
uint32_t ipv4_is_multicast(uint32_t addr);
uint32_t ipv4_is_local_multicast(uint32_t addr);
uint32_t ipv4_is_broadcast(uint32_t addr);
uint32_t ipv4_is_broadcast1(uint32_t addr,uint8_t masklen);
uint32_t ipv4_is_valid(uint32_t addr);

uint32_t ipv4_is_lbcast(uint32_t addr);
uint32_t ipv4_is_zeronet(uint32_t addr);
uint32_t ipv4_is_private_10(uint32_t addr);
uint32_t ipv4_is_private_172(uint32_t addr);
uint32_t ipv4_is_private_192(uint32_t addr);
uint32_t ipv4_is_linklocal_169(uint32_t addr);
uint32_t ipv4_is_anycast_6to4(uint32_t addr);
uint32_t ipv4_is_test_192(uint32_t addr);
uint32_t ipv4_is_test_198(uint32_t addr);

uint32_t ipv4_get_loopback();


int ipv6_is_multicast(struct ipv6_addr *paddr);
int ipv6_is_zeronet(struct ipv6_addr *paddr);
int ipv6_is_multicast(struct ipv6_addr *paddr);
int ipv6_is_zero(struct ipv6_addr *paddr);
char *inet_ntop4(const u_char *src, char *dst, unsigned int size);
char *inet_ntop6(const u_char *src, char *dst, unsigned int size);
int inet_pton4(const char *src, u_char *dst);
int inet_pton6(const char *src, u_char *dst);
uint32_t ipv4_get_loopback( void );

/* 检查两个 IPV4 地址是否属于同一网段 */
int ipv4_is_same_subnet(uint32_t addr1, uint32_t addr2, uint8_t masklen);
int ipv6_is_same_subnet(struct ipv6_addr *paddr1, struct ipv6_addr *paddr2, uint8_t masklen);
int inet_valid_subnet(u_int32 subnet, u_int32 mask);
int ipv6_is_valid(struct ipv6_addr *paddr);
int ipv6_is_loopback(struct ipv6_addr *paddr);
int ipv6_is_linklocal(struct ipv6_addr *paddr);
int ipv6_is_sitelocal(struct ipv6_addr *paddr);
int ipv6_is_ipv4map(struct ipv6_addr *paddr);
//int ip_is_valid(struct inet_addr *paddr);
uint32_t ipv6_is_global_unicast(struct ipv6_addr *paddr);

#endif


