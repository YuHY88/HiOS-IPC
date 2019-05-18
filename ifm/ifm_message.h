#ifndef HIOS_IFM_MESSAGE_H
#define HIOS_IFM_MESSAGE_H
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>

struct list ifm_rmsg_list;
pthread_mutex_t ifm_rmsg_lock;
struct list ifm_smsg_comm_list;
struct list ifm_smsg_ftm_list;
struct list ifm_smsg_hal_list;
pthread_mutex_t ifm_smsg_comm_lock;
pthread_mutex_t ifm_smsg_ftm_lock;
pthread_mutex_t ifm_smsg_hal_lock;

struct ifm_send_msg
{
    struct ipc_msghdr_n msghdr;
    void *pdata;
};
struct port_info
{
	struct ifm_info ifm;
	struct ifm_port port_info;
	char            alias[IFM_ALIAS_STRING_LEN];/*别名*/
	
};

struct ifm_snmp_sfp
{

	uint32_t      ifindex;
	uint8_t       hide;        /* 1: 表示为隐藏接口，不对用户呈现*/
	uint8_t       fiber;       /* 端口的光电类型   enum IFNET_FIBERTYPE*/
	uint32_t      sfp_off;           /* sfp_off size*/
    uint32_t      sfp_on;			/* sfp_on size*/
    uint8_t       sfp_als;         /* sfp auto flags */
    uint8_t       sfp_tx;            /* sfp manual */
	uint32_t      sfp_off_time;      /* off remain time */ 
    uint32_t      sfp_on_time;       /* on remain time */
    uint8_t       sfp_optical;           /* glowing flags*/
}__attribute__ ( ( packed ) );

struct ifm_snmp_info
{
    struct ifm_info ifm;
    struct ifm_port port_info;
	char            alias[IFM_ALIAS_STRING_LEN];/*别名*/

};
struct ifm_snmp_subencap
{
	struct ifm_info ifm;
};
struct ifm_snmp
{
	struct ifm_info ifm;
	struct ifm_port port_info;
	struct ifm_counter pcounter;
	char            alias[IFM_ALIAS_STRING_LEN];
};

struct list snmp_info_list;
struct list  port_common_list;

struct list  snmp_subencap_list;
struct list  snmp_sfp_list;
struct list  snmp_list;
#if 0
int ifm_msg_rcv_handle ( struct thread *pthread );
void ifm_msg_send_pthread ( void *arg );
void ifm_msg_rcv_pthread ( void *arg );
void ifm_msg_send_com ( struct ipc_msghdr *pmsghdr, void *pdata );
void ifm_msg_send_ftm ( struct ipc_msghdr *pmsghdr, void *pdata );
void ifm_msg_send_hal ( struct ipc_msghdr *pmsghdr, void *pdata );
void ifm_msg_send_list ( int ipc_id, struct list *plist );
int ifm_rcv_getif_msg ( struct ipc_pkt *pmesg );
int ifm_rcv_devm_msg ( struct ipc_pkt *pmesg );
int ifm_register_msg ( int module_id, uint8_t type, uint8_t subtype );
int ifm_send_hal_wait_ack ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
void *ifm_send_hal_wait_reply ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
int ifm_send_ftm ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
int ifm_send_hal ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
int ifm_get_snmp_sort_bulk(uint32_t index, struct ifm_snmp_info *pifm_snmp, int array_c);
int ifm_get_snmp_sfp_bulk( uint32_t ifindex, struct ifm_snmp_sfp *pifm_snmp, int leng );
int ifm_rcv_qosif_msg ( struct ipc_pkt *pmesg );
int ifm_get_port_sort_bulk(uint32_t index, struct port_info * pifm_info, int array_c);
int ifm_get_snmp_subencap_bulk( uint32_t ifindex, struct ifm_snmp_subencap* pifm_snmp, int leng );
#endif

int ifm_msg_rcv_handle ( struct thread *pthread );
void ifm_msg_send_pthread ( void *arg );
void ifm_msg_rcv_pthread ( void *arg );
void ifm_msg_send_com ( struct ipc_msghdr_n *pmsghdr, void *pdata );
void ifm_msg_send_ftm ( struct ipc_msghdr_n *pmsghdr, void *pdata );
void ifm_msg_send_hal ( struct ipc_msghdr_n *pmsghdr, void *pdata );
void ifm_msg_send_list ( int ipc_id, struct list *plist );
int ifm_rcv_getif_msg ( struct ipc_mesg_n *pmesg );
int ifm_rcv_devm_msg ( struct ipc_mesg_n *pmesg );
int ifm_register_msg ( int module_id, uint8_t type, uint8_t subtype );
int ifm_send_ftm ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
int ifm_send_hal ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
int ifm_get_snmp_sort_bulk(uint32_t index, struct ifm_snmp_info *pifm_snmp, int array_c);
int ifm_get_snmp_sfp_bulk( uint32_t ifindex, struct ifm_snmp_sfp *pifm_snmp, int leng );
int ifm_rcv_qosif_msg ( struct ipc_mesg_n *pmesg );
int ifm_msg_send_ack( struct ipc_msghdr_n * rcvhdr );
int ifm_msg_send_noack( uint32_t errcode,struct ipc_msghdr_n * rcvhdr );
int ifm_send_hal_wait_ack ( void *pdata, int data_len,uint16_t data_num,int module_id,int sender_id,
				enum IPC_TYPE msg_type,uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index );
//int ifm_send_hal_wait_reply(void * pdata,uint32_t data_len,uint16_t data_num,int module_id,
//		int sender_id,enum IPC_TYPE msg_type,uint16_t msg_subtype,enum IPC_OPCODE opcode,uint32_t msg_index);

int ifm_msg_rcv_handle_n ( struct ipc_mesg_n *pmesg, int imlen );
int ifm_rcv_ifm_msg( struct ipc_mesg_n * pmesg );
int ifm_get_snmp_subencap_bulk( uint32_t ifindex, struct ifm_snmp_subencap* pifm_snmp, int leng );



#endif
