#ifndef HIOS_DHCP_IF_H
#define HIOS_DHCP_IF_H
#include <netinet/in.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/zassert.h>
#include <lib/inet_ip.h>
#include <lib/ifm_common.h>
#include <lib/msg_ipc.h>
#include <lib/hptimer.h>

enum DHCP_ROLE
{
	E_DHCP_NOTHING = 0,
	E_DHCP_SERVER,	
	E_DHCP_CLIENT,
	E_DHCP_RELAY,
	E_DHCP_ZERO,
};

struct if_info
{
	enum IFNET_MODE mode;/* 接口模式 */
	
	uint32_t ip_addr;
	uint32_t mask;	
	uchar  mac[MAC_LEN];	
	uchar status; /* 接口状态0 :down   1: up */
};

struct xid_item {
	uint32_t timestamp;
	uint32_t xid;
	struct sockaddr_in ip;
	struct xid_item *next;
};

struct relay_ctrl
{
	uint32_t 			nexthop; 	
	struct xid_item	 	*xid_list;
};

struct client_ctrl
{
	int state;/* client状态机 */
	uint32_t ip; 
	uint32_t mask; 		
	uint32_t gate;/* 网关 */
	uint32_t dns;	
	uint32_t server_addr;
	
	struct timeval timeout;
	uint32_t loopcount;/* unuse */
	uint32_t packet_num;/* 发送包个数 */

	uint32_t t1 ;/* 1/2租期 */
	uint32_t t2 ;/* 3/4租期 */
	uint32_t xid ;
	uint32_t start;/* client流程起始时间 */
	uint32_t lease;/* 租期 */

	uint8_t clientid[9];/* option61 */
	uint8_t hostname[7];/* option12 */

	uint32_t trap_ip; 
	uint16_t trap_port; 
	uint8_t  online;
	uint16_t vlanid;
	uint8_t ifvalid;

	struct thread *timer_thread;
	TIMERID timer;
};/* end struct client_ctrl*/

struct if_dhcp
{
	uint32_t 			ifindex;	
	enum IFNET_MODE 	mode;/* 接口模式 */	
	
	uint32_t 			ip_addr;
	uint32_t 			ip_addr_slave;	/* 接口从 IP */
	uint32_t			mask;	
	uchar    			mac[MAC_LEN];	
	uchar   			status; /* 接口状态0 :down   1: up */
	uint32_t 			nexthop; 	
	uint32_t            zero;
	pthread_t 			pthread_zero; /* 零配置线程 */
	uchar   			save; /* 固化状态0 :非固化   1: 固化 */
	enum   DHCP_ROLE    dhcp_role; 
	//struct ip_pool     *ip_pool;
	struct client_ctrl *c_ctrl;	
	struct relay_ctrl  *r_ctrl;	
};

extern struct list dhcp_if_list;
extern u_char ip_masklen (struct in_addr);
void dhcp_if_cmd_init();
int  dhcp_if_add(struct if_dhcp *pif);
int  dhcp_if_delete(uint32_t ifindex);
struct if_dhcp *dhcp_if_lookup(uint32_t ifindex);
struct if_dhcp *dhcp_if_lookup_by_ip(uint32_t ip , uint8_t relay_flag);
int  dhcp_if_save_add(struct if_dhcp *pif);
int  dhcp_if_save_delete(uint32_t ifindex);
struct if_dhcp *dhcp_if_save_lookup(uint32_t ifindex);

int dhcp_if_down(uint32_t ifindex);
int dhcp_if_up(uint32_t ifindex);
int dhcp_if_ip_add(uint32_t ifindex,struct inet_prefix ipaddr,uint8_t slave);
int dhcp_if_ip_del(uint32_t ifindex,uint8_t slave);
int dhcp_if_mode_change(uint32_t ifindex,enum IFNET_MODE  mode);
int dhcp_if_config_write (struct vty *vty);
int dhcp_if_factory_write (struct vty *vty);
int dhcp_client_enable(uint32_t ifindex);
int dhcp_client_disable(uint32_t ifindex);
int dhcp_client_save_enable(uint32_t ifindex);
int dhcp_client_save_disable(uint32_t ifindex);
int dhcp_ifconfig_get_bulk(void *pdata, int data_len, struct if_dhcp pif[]);
int dhcp_com_ifconfig_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr);
int show_dhcp_if (struct vty *vty,enum DHCP_ROLE role);
int show_dhcp_if_relay(struct vty *vty,struct if_dhcp *pif);
int show_dhcp_if_client(struct vty *vty,struct if_dhcp *pif);
int show_dhcp_if_server(struct vty *vty,struct if_dhcp *pif);
void dhcp_ip_address_save_init(void);
void dhcp_if_cmd_init(void);
int debug_dhcp_ifconfig_get_bulk(struct if_dhcp *pif);
struct listnode *dhcp_ifconfig_get_list_nextnode(uint32_t ifindex);
int dhcp_mac_is_empty(uchar *mac);
#endif

