#ifndef VTYSH_SYNC_H
#define VTYSH_SYNC_H

#include "vtysh.h"
#include <lib/devm_com.h>
#include <lib/msg_ipc.h>


//#define VTYSH_SYNC_DEBUG 0

extern volatile vector vtyvec_sync;
extern struct vtysh_sync syncvty;

/* VTY slot type*/
enum VTY_SLOT_TYPE
{
	VTY_UNKNOW_SLOT = 0, 
	VTY_MAIN_SLOT, 
	VTY_SLAVE_SLOT,
	VTY_SLOT_ERR
};
#if 0
struct vty_master
{
	enum vty_sync_status status;
	enum 
	
};


struct vty_slave
{
	int self_status;
	int peer_status;
	
};
#endif
enum vty_sync_msg_type
{
	VTY_SYNC_CREAT,
	VTY_SYNC_CMD,
	VTY_SYNC_EXIT,
	VTY_SYNC_CATCH_CMD,
	VTY_SYNC_CLOSE_ALL,
	VTY_SYNC_REBOOT,
	VTY_SYNC_REBOOT_ACK,
	VTY_SYNC_REQ
};

enum vty_sync_status
{
	/*slave un-started*/
	VTYSH_UNREADY,
	/*daemon start*/
	VTYSH_START,
	/*begin to config from file*/
	VTYSH_BATCH_CONFIG_START,	
	/*finish of config from file*/
	VTYSH_BATCH_CONFIG_FINISH,
	/*master has sent batch file to slave*/
	VTYSH_BATCH_CONFIG_SENT,
	/*slave has recv batch file from slave*/
	VTYSH_BATCH_CONFIG_RECV,
	/*syncing catch cmd after slave vtysh ready*/
	VTYSH_SYNC_CATCH_CMD,
	/*real-time config is enabled*/
	VTYSH_REALTIME_SYNC_ENABLE,
};

struct vty_sync_msg
{
	enum vty_sync_msg_type msg_type;
	
	/*vty type of main vtysh*/
	int vty_type;

	char ttyname[SU_ADDRSTRLEN];

	/*data for cmd*/
	char cmd[VTY_BUFSIZ];
};

struct vtysh_sync
{
	/*on master or slave slot?*/
	enum VTY_SLOT_TYPE vty_slot;

	/*status of self*/
	enum vty_sync_status self_status;

	/*status of peer*/
	enum vty_sync_status peer_status;

	/*record slef and peer slot number*/
	int self_slot;
	int peer_slot;

	/*record master and slave slot number*/
	int main_slot_num;
	int slave_slot_num;

	int all_cards_ready_flag;
	int vtysh_start_record;

	/*主中，生成批量同步文件到备批量同步完成的时间段，
	 *该flag值为1，此时主卡需要缓存所有用户命令，且不清空*/
	int slave_batch_busy_flag;
	int ms_x_flag;   /* 主备倒换过标识*/

	/*can record 65535 cmds*/
	uint32_t cmd_main_send_cnt;
	uint32_t cmd_slave_recv_cnt;
	uint32_t cmd_slave_exec_cnt;
	uint32_t cmd_slave_success_cnt;
	uint32_t cmd_slave_failed_cnt;
};

enum slot_msg_type
{
	VTY_SLOT_MSG_TYPE_BOOT = 1,
	VTY_SLOT_MSG_TYPE_EXCHANGE,
	VTY_SLOT_MSG_TYPE_ERR
};

#if 0
struct vtysh_main_slot
{
	int 
};

struct vtysh_slave_slot
{

};
#endif
extern void vtysh_init_slave(void);
extern void vtysh_cmd_sync_slot(void *arg);
extern void vtysh_send_start_to_ha(void);
extern void vtysh_send_dataok_to_ha(void);
extern void *vtysh_master_handle_peer_start (void *arg);
extern void *vtysh_slave_sync_exec_cmd(void *arg);
extern struct login_session *vtysh_slave_sync_vty_thread_creat(struct vty_sync_msg *);
extern void *vtysh_slave_realtime_sync(void *arg);
extern void vtysh_master_slave_exchange(struct devm_com *devm_com);
extern void *vtysh_master_config_recover_thread(void *arg);
extern void vtysh_master_config_recover(void);
extern void vtysh_master_slave_exchange_stom(struct devm_com *devm_com);
extern void vtysh_devm_msg_delete_slot(struct ipc_mesg_n *mesg);
extern void vtysh_devm_msg_recv(struct ipc_mesg_n *mesg);
extern void vtysh_handle_bootstart(struct devm_com *devm_com);
extern void vtysh_handle_bootok(struct devm_com *devm_com);
extern int vtysh_check_slot_change(struct devm_com *devm_com);
extern int vtysh_get_slot_msg_type(struct devm_com *devm_com);
extern int vtysh_thread_creat(void *(*func)(void *));
extern void vtysh_master_slave_exchange_mtos(struct devm_com *devm_com);
extern void vtysh_check_sync_vty_close(void);
extern void vtysh_clear_sync_vty(void);
extern int vtysh_slave_batch_config(void);
extern void vtysh_slave_sync_start(void);
extern int vtysh_slave_recv_realtime_cmd(struct vty_sync_msg *sync_msg);
extern int vtysh_slave_sync_exit(struct vty_sync_msg *sync_msg);
extern void vtysh_slave_recv_msg(struct ipc_mesg_n *mesg);
extern struct login_session *vty_sync_session_get(struct vty_sync_msg *sync_msg);
extern void vtysh_master_handle_slave_dataok(void);
extern void vtysh_handle_bootend(struct devm_com *devm_com);
extern int vtysh_master_sync_cmd(struct vty *vty, enum vty_sync_msg_type type, const char *cmd);
extern int vtysh_mster_catch_realtime_cmd(const char *line, struct vty *vty);
extern int vtysh_master_recv_slave_start(void);
extern int vtysh_cdp_send(struct vty *vty,enum vty_sync_msg_type type, char *cmd);
extern int vtysh_ha_send(struct vty *vty,enum vty_sync_msg_type type, const char *cmd);
extern int vtysh_master_sync_exit(struct vty *vty, enum vty_sync_msg_type type, char *cmd);
extern void vtysh_init_sync(void);
extern int vtysh_get_slot_type(struct devm_com *devm_com);
extern void vtysh_send_start_to_ha(void);
extern int vtysh_master_sync_vty(struct vty *vty, enum vty_sync_msg_type type, char *cmd);
extern void vty_batch_delete(struct vty *vty);
extern struct vty *vty_batch_create (void);
extern int vtysh_creat_batch_config_file(void);
extern void vtysh_master_handle_batch_sync(void);
extern void vtysh_batch_sync_start(void);
extern void vtysh_send_batchfile_to_filem(void);
extern void vtysh_get_slot(void);

int vtysh_sync_compelete(void);

#endif


