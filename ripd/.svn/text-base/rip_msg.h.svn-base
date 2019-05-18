#ifndef _ZEBRA_RIP_MSG_H
#define _ZEBRA_RIP_MSG_H

#include "lib/types.h"

/*snmp get information*/
#define RIP_SNMP_INSTANCE_GET  			0
#define RIP_SNMP_INTERFACE_GET  		1
#define RIP_SNMP_REDIS_ROUTE_GET  		2
#define RIP_SNMP_ROUTE_SUMMARY_GET  	3
#define RIP_SNMP_IF_NBR_GET  			4


struct rip_brief
{
    uint32_t		id;                /* instance id */
    uint32_t		version;           /* instance version */
    uint32_t 		distance;          /*instance preference */
    uint32_t		update_time;       /* instance update time */
    uint32_t		timeout_time;      /* instance time out time */
    uint32_t		garbage_time;      /* instance grabage collect time */
	u_char			auto_summary;
};

struct ripinter_brief
{
    uint32_t        ifindex;              		/* interface ifindex */
	
    char            name[NAME_STRING_LEN];		/* interface name */
    uint32_t        id; 						/*rip instance id which this interface belongs to */
	char 			auth_password[17];
	uint32_t		auth_md5_id;
	u_char			split_horizon_status;
	u_char			poison_reverse_status;
	u_char			passive_status;
	uint32_t		metric;
	u_char			rip_version;
	u_char			pkt_send_type;
};

struct rip_redistribute_route_key
{
	u_int32_t rip_id_key;						/* rip instance id <1-255> */
	u_char redis_route_type_key;
	u_int32_t redis_target_instance_id_key;
};

struct rip_redistribute_route_info
{
	u_int32_t rip_id;							/* rip instance id <1-255> */
	u_char redis_route_type;
	u_int32_t redis_target_instance_id;

	u_int32_t redis_route_metric;
};

struct rip_route_summary_key
{
	uint32_t        ifindex_key;              	/* interface ifindex */
	struct in_addr sum_net_ip_key;
	struct in_addr sum_net_mask_key;
};

struct rip_route_summary_info
{
	uint32_t        ifindex;              		/* interface ifindex */
	struct in_addr 	sum_net_ip;
	struct in_addr 	sum_net_mask;
	char 			sum_if_desc[32];			//描述接口名称
};

struct rip_if_nbr_key
{
	uint32_t        ifindex_key;              	/* interface ifindex */
	struct in_addr 	remote_nbr_ip_key;
};

struct rip_if_nbr_info
{
	uint32_t        ifindex;              		/* interface ifindex */
	struct in_addr 	remote_nbe_ip;
	char 			nbr_if_desc[32];			//描述接口名称
};


extern void ripc_init(void);

int rip_interface_get_bulk(uint32_t ifindex, int data_len, struct ripinter_brief ripinter_buff[]);
int rip_instance_get_bulk(uint32_t pid, int data_len, struct rip_brief ripins_buff[]);
//void rip_ifm_manage(struct ipc_mesg *mesg);
void rip_ifm_manage_new(struct ipc_mesg_n *mesg);

//void rip_route_manage(struct ipc_mesg *mesg);
void rip_route_manage_new(struct ipc_mesg_n *mesg);

//int add_rip_fifo_ifm(struct ipc_mesg *mesg);
//void read_mesg_form_ifmfifo(void);
//int rip_ifm_msg_rcv(struct thread *thread);
//int rip_route_msg_rcv(struct thread *thread);
int rip_pkt_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);


#endif

