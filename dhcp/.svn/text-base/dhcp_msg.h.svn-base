#ifndef _ZEBRA_DHCP_MSG_H
#define _ZEBRA_DHCP_MSG_H
#include <lib/types.h>

/*snmp get information*/
#define DHCP_SNMP_IPPOOL_GET			0
#define DHCP_SNMP_IPBIND_GET			1
#define DHCP_SNMP_IFCONFIG_GET			2
#define DHCP_SNMP_DYNAMIC_IPBIND_GET	3

enum DHCP_EVENT
{
	DHCP_EVENT_INVALID = 0,
	DHCP_EVENT_FROM_CMD,		
	DHCP_EVENT_FROM_PKT,
	DHCP_EVENT_FROM_IPC,	
	DHCP_EVENT_FROM_TIMER	
};

struct NM_INFO
{
	uint32_t nm_ip;
	uint16_t nm_port;
};

struct TRAP_INFO
{
	uint8_t  series_id;
	uint8_t  model_id;
	uint8_t  mac[MAC_LEN];
	uint8_t  reserved;
	uint32_t ip;
	uint32_t mask;
	uint32_t gateway;
};


struct DHCP_TRAP
{
	struct NM_INFO 		nm_info;
	struct TRAP_INFO   trap_info;
};

void dhcp_event_register(void);
void dhcp_pkt_register(void);
int dhcp_pkt_rcv(struct ipc_mesg_n *pmsg, int imlen);
int dhcp_ipc_rcv(struct thread *argv);
void dhcp_pkt_dump (void *data, int len);
int dhcp_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);
int dhcp_rcv_l3if(struct ipc_mesg_n *pmesg, int imlen);
int dhcp_rcv_ifevent(struct ipc_msghdr_n *pmsghdr, void *pdata);

#endif

