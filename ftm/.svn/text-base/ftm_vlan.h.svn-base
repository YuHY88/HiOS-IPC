#ifndef HIOS_FTM_VLAN_H
#define HIOS_FTM_VLAN_H

#include <lib/types.h>
#include <l2/vlan.h>
#include <lib/linklist.h>


/* 全局 vlan 的数据结构 */
struct ftm_vlan
{
	uint32_t ifindex;      /* vlanif 接口的 ifindex */
    struct vlan_info vlan;
	struct list portlist;  /* vlan 的成员端口 */
};

extern struct ftm_vlan ftm_vlan_table[VLAN_MAX];


/* 全局 vlan 的操作函数 */
void ftm_vlan_table_init(void); /* 初始化 vlan 数组 */
struct ftm_vlan *ftm_vlan_lookup(uint16_t vlan_id);
int ftm_vlan_enable(struct vlan_range *pvlan);
int ftm_vlan_disable(struct vlan_range *pvlan);

int ftm_vlan_add_port(struct vlan_range *pvlan, uint32_t ifindex);    /* vlan 添加端口 */
int ftm_vlan_delete_port(struct vlan_range *pvlan, uint32_t ifindex);/* vlan 删除端口 */

uint32_t ftm_vlan_get_vlanif(uint16_t vlan_id);


/* 处理 vlan 的 IPC 消息 */
int ftm_vlan_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

#endif

