/*
*    ip4 mc main process
*/

#ifndef IPMC_MAIN_H
#define IPMC_MAIN_H

#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/thread.h>
#include <lib/vty.h>
#include <lib/log.h>

#define IPMC_SEND_BUF_SIZE (4*1024) /*4KB*/
#define PIM_MRT_WC_MAX 1024
#define PIM_MRT_SG_MAX 1024

	/*pim debug define */
#define PIM_DBG_FLAG     0x01
#define PIM_DBG_ALL     0x1F
		
#define PIM_LOG_DBG(fmt, ...) \
		do { \
			zlog_debug ( PIM_DBG_FLAG, fmt, ##__VA_ARGS__ ); \
		} while(0)

#define IPMC_LOG_DEBUG(format, ...)\
    zlog_debug(PIM_DBG_FLAG,"%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define IPMC_LOG_NOTICE(format, ...)\
    zlog_notice("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define IPMC_LOG_ERROR(format, ...)\
    zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);


/* Master of threads. */
extern struct thread_master *ipmc_master;
extern int ipmc_errno;
extern char *ipmc_send_buf;


void ipmc_init(void); /* init all of ip4_mc */
void ipmc_die(void);  /* free all of ip4_mc */
//int ipmc_msg_rcv(struct thread *pthread);
int ipmc_msg_rcv(struct ipc_mesg_n *pmesg, int imlen);
struct ifm_l3 *pim_getbulk_if_ip(int module_id, int *pdata_num);
int ipmc_msg_send_hal_wait_reply(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
int ipmc_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

int ipmc_msg_send_to_hal(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


//struct ifm_l3 *pim_get_if_ip(int module_id, uint32_t ifindex);

#endif

