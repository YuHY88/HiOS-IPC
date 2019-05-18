
#ifndef HIOS_POOL_H
#define HIOS_POOL_H

#include <lib/types.h>
#include "dhcp_server.h"
struct pool
{
	//uint32_t network;
	uint8_t  mask_len;
	uint32_t mask;
	
	uint32_t dns_server;
	uint32_t snmp_server;

	uint32_t gateway;		/* gateway IP, in network order */
	uint32_t start;			/* Start address of leases, network order */
	uint32_t end;			/* End   address of leases, network order */
	uint32_t total;			/* count of total leases */
	uint32_t used;			/* count of used leases */
	uint32_t expired;      /* count of expired leases */
	uint32_t static_num;      /* count of static leases */

	uint32_t leasetime;
	
	uint32_t start_from_gate;		/* Start address of gate, network order */
	uint32_t end_from_gate;		/* End   address of gate, network order */

	
	uint8_t *indexd; //dynamic index
	uint8_t *indexs; //static index
	
	//struct dhcpOfferedAddr *leases;
	//struct static_lease *static_leases; /* List of ip/mac pairs to assign static leases */
};


struct ip_pool
{
	uchar 			pool_index;
    struct pool 	dhcp_pool;
};


#define POOL_TAB_MSIZE 1024
#define POOL_NAME_LEN  33

/* Prototypes. */

void 			  dhcp_pool_table_init(unsigned int size);
struct ip_pool * dhcp_pool_add(uchar pool_index);
int 			  dhcp_pool_delete(uchar pool_index);
struct ip_pool*  dhcp_pool_lookup(uchar pool_index);
extern char *inet_ntoa(struct in_addr in);

//unsigned int compute(void *hash_key);
//int 		 compare(void *item, void *hash_key);

int dhcp_ippool_get_bulk(void *pdata, int data_len, struct ip_pool ip_pool[]);
int dhcp_com_ippool_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr);
int debug_dhcp_ippool_get_bulk(struct ip_pool *pool_node);
#endif

