/*
*       manage the fibv4 and fibv6 table
*
*/

#ifndef HIOS_FTM_FIB_H
#define HIOS_FTM_FIB_H

#include <lib/route_com.h>


#define FIB_SIZE    ROUTE_NUM_MAX
#define FIBV6_SIZE  ROUTEV6_NUM_MAX


/* fib 全局数据结构 */
struct fib_global
{
	uint32_t router_id;          /* ipv4 router_id */
	struct ipv6_addr routerv6_id;/* ipv6 router_id */
	uint32_t fib_limit;          /* 芯片注册的最大 ipv4 路由数量 */	
	uint32_t fibv6_limit;        /* 芯片注册的最大 ipv6 路由数量 */
	uint32_t fib_num;            /* ipv4 路由计数 */	
	uint32_t fibv6_num;          /* ipv6 路由计数 */
};


/* fib 数据结构 */
struct fib_entry
{
	uint32_t        ipaddr;      /* ipv4 地址 */
	uchar           masklen;     /* ipv4 掩码长度 0 - 32 */
	uint8_t         pad;
	uint16_t        vpn;
	struct nhp_info nhp;
};


/* fibv6 数据结构 */
struct fibv6_entry
{
	uint8_t         ipaddr[16];  /* ipv6 地址 */
	uchar           masklen;     /* ipv6 掩码长度 0 - 128 */
	uint8_t         pad;
	uint16_t        vpn;
	struct nhp_info nhp;
};


struct ptree *fib_tree[L3VPN_SIZE+1];   /* ipv4 fib 表 */
struct ptree *fibv6_tree[L3VPN_SIZE+1]; /* ipv6 fib 表 */

extern uint32_t fib_num;
extern uint32_t fibv6_num;

extern struct fib_global g_fib;  /* fib 的全局数据结构 */
void ftm_fib_init(void); /* fib 妯″潡鍒濆鍖?*/

/* Prototypes. */
int ftm_fib_tree_init(void);
void ftm_fib_tree_delete(uint16_t vrf);
void ftm_fib_tree_clear(void);

int ftm_fib_add(struct fib_entry *pfib);
int ftm_fib_update(struct fib_entry *pfib_old, struct fib_entry *pfib_new);
int ftm_fib_delete(struct fib_entry *pfib);
struct fib_entry *ftm_fib_lookup(uint32_t ipaddr, uchar masklen, uint16_t vrf);/* exactly match */
struct fib_entry *ftm_fib_match(uint32_t ipaddr, uchar masklen, uint16_t vrf); /* mask longest match */

int ftm_fibv6_tree_init(void);
void ftm_fibv6_tree_delete(uint16_t vrf);
void ftm_fibv6_tree_clear(void);

int ftm_fibv6_add(struct fibv6_entry *pfib);
int ftm_fibv6_update(struct fibv6_entry *pfib_old, struct fibv6_entry *pfib_new);
int ftm_fibv6_delete(struct fibv6_entry *pfib);
struct fibv6_entry *ftm_fibv6_lookup(struct ipv6_addr *paddr, uchar masklen, uint16_t vrf);
struct fibv6_entry *ftm_fibv6_match(struct ipv6_addr *paddr, uchar masklen, uint16_t vrf);

int ftm_fib_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);

#endif

