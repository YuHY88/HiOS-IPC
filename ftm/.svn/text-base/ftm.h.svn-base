/*
*    FTM MSG process 
*/



#ifndef HIOS_FTM_H
#define HIOS_FTM_H

#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/thread.h>
#include <pthread.h>

/*add for new ipc by renxy*/
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/hptimer.h>
//#include "sigevent.h"

#define FTM_LOG_DEBUG(format, ...)\
    zlog_debug(FTM_DBG_ALL,"%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define FTM_LOG_ERROR(format, ...)\
    zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);


struct ftm_devm_status
{
	uint8_t devm_type;
	uint8_t ms_status;
	uint8_t slave_in;
	uint8_t pad;
};


/* Master of threads. */
struct thread_master *ftm_master;
extern pthread_mutex_t ftm_msg_lock;
extern struct ftm_devm_status ftm_devm;

/*ftm debug info*/
#define FTM_DBG_PKT			0x00000001
#define FTM_DBG_IP			0x00000002
#define FTM_DBG_TCP			0x00000004
#define FTM_DBG_UDP			0x00000008
#define FTM_DBG_L3			0x00000010
#define FTM_DBG_ARP			0x00000020
#define FTM_DBG_NDP			0x00000040
#define FTM_DBG_ALL      	0xFFFFFFFF

int ftm_msg_rcv_control(void);
int ftm_msg_rcv(struct ipc_mesg_n *pmesg);
int ftm_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen);
void ftm_msg_ipc_close();
void ftm_msg_ipc_init();
int ftm_msg_send_to_hal(void *pdata, int data_len, int data_num, 
						  enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


extern int ftm_route_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);
extern int ftm_lsp_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);
extern int ftm_arp_msg(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode, sint32 sender_id);
extern int ftm_pw_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);
extern int ftm_proto_msg(void *pdata, int module_id, uint8_t subtype, enum IPC_OPCODE opcode);
extern int ftm_ifm_msg ( void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t ifindex );
extern int ftm_vlan_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

extern void ftm_config_finish_func(void);
extern int ftm_tpoam_msg(struct ipc_msghdr_n *phdr,void *pdata, int data_len, uint8_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

#endif
















