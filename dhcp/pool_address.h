#ifndef _DHCP_ADDRESS_H
#define _DHCP_ADDRESS_H
#include "pool.h"
/* pool_address 表结构 */
enum ADDR_TYPE
{
	e_type_dynamic = 0,
	e_type_static,	
};

enum ADDR_STATE
{
	e_state_nouse = 0,
	e_state_send_offer,	
	e_state_send_ack, 
	e_state_released,	
	e_state_declined,	
	e_state_expired,		
};
struct dhcpOfferedAddr {
	uint8_t 			chaddr[16];
	uint32_t 		 	yiaddr;	/* network order */
    enum ADDR_TYPE  	 type;	
	uint32_t 			expires;	/* host order */
	uint32_t 			now;	/* current time */
    enum ADDR_STATE   	state;
	uint32_t 			ifindex;	
	uint8_t 			pool_index;	
};

extern uint8_t blank_chaddr[];


extern struct hash_table dhcp_addr_table;
extern uint32_t dhcp_find_address(int check_expired,int check_if);
struct dhcpOfferedAddr *  dhcp_addr_add_by_node(struct dhcpOfferedAddr * addrnode);
struct dhcpOfferedAddr *  dhcp_addr_lookup(uint32_t ip);
struct dhcpOfferedAddr *  dhcp_addr_add(uint32_t ip, uint8_t *mac);
void dhcp_addr_del_by_ifindex(uint32_t ifindex);
void dhcp_addr_del(uint32_t ip,struct ip_pool *pool_node);
void dhcp_addr_del_by_pool(struct ip_pool *pool_node);
struct dhcpOfferedAddr *  dhcp_addr_lookup_by_mac(uint8_t *mac,struct ip_pool *pool_node);
int lease_expired(struct dhcpOfferedAddr *lease);
int dhcp_dynamic_ipbind_get_bulk(void *pdata, int data_len, struct dhcpOfferedAddr lease[]);
extern int dhcp_com_dynamic_ipbind_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr);
int debug_dhcp_dynamic_ipbind_get_bluk(struct dhcpOfferedAddr *lease);
struct ifm_l3 *dhcp_route_get_interface_ip_bulk(uint32_t ifindex,int module_id, int *pdata_num);
int check_interface_ip_address(uint32_t start, uint32_t end, uint32_t gateway,uint32_t ip);

void dhcp_addr_table_init(unsigned int uiSize);
#endif
