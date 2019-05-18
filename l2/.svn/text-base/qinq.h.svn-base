
#ifndef HIOS_QINQ_H
#define HIOS_QINQ_H

#include <lib/types.h>
#include "l2_if.h"

#define QINQ_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_QINQ, fmt, ##__VA_ARGS__ ); \
    } while(0)

/* Qinq 转换的数据结构 */
struct vlan_mapping
{
	struct vlan_range cvlan;     /* 转换前的 cvlan */
	struct vlan_range svlan;     /* 转换前的 svlan */
	struct vlan_range cvlan_new; /* 转换后的 cvlan */
	struct vlan_range svlan_new; /* 转换后的 svlan */
	uchar cvlan_cos;             /* 转换后的 cvlan cos : 0 -7 有效， 8 表示无效 */
	uchar svlan_cos;             /* 转换后的 svlan cos : 0 -7 有效， 8 表示无效 */
	uchar range_qinq;            /*	L2IF_INFO_QINQ or L2IF_INFO_QINQ_RANGE*/
	enum VLAN_ACTION cvlan_act;  /* cvlan 的转换动作*/
	enum VLAN_ACTION svlan_act;  /* svlan 的转换动作*/
};

/*用于网管批量查询*/
struct l2if_vlan_mapping
{
	uint32_t ifindex;
	struct vlan_mapping t_vlan_mapping;
};


/*set qinq*/
int qinq_add(struct l2if *p_l2if, struct vlan_mapping *pinfo,uint8_t send_range,uint8_t qinqtype);
int qinq_delete(struct l2if *p_l2if, struct vlan_mapping *pinfo,uint8_t send_range,uint8_t qinqtype);
int qinq_range_add(struct l2if *p_l2if, struct vlan_mapping *pinfo);
int qinq_range_delete(struct l2if *p_l2if, struct vlan_mapping *pinfo);
int l2if_clear_qinq(struct l2if *p_l2if);

/*set dot1q tunnel*/
int dot1q_tunnel_add(struct l2if *p_l2if, uint16_t vlan, uint8_t cos);
int dot1q_tunnel_delete(struct l2if *p_l2if);


/*检查转换前的cvlan ，svlan是否已使用*/
int qinq_check_vlan_range(struct vlan_range *pvlan_old, struct vlan_range *pvlan_new);

/*检查转换动作是否相同*/
int qinq_check_action_same(struct vlan_mapping *p_vlan_mapping1,struct vlan_mapping *p_vlan_mapping2);
/*检查vlan_range完全相同*/
int qinq_check_vlan_range_same(struct vlan_range *pvlan_old, struct vlan_range *pvlan_new);


/*检查vlanid是否已添加*/
int check_input_in_vlanlist(struct l2if *p_l2if,uint16_t vlanid);
void qinq_device_type_init(void);

/* 接口上的配置函数 */
void l2if_qinq_cli_init (void);

/*switch 模式切换函数*/
int l2if_set_switch_mode(struct l2if *pif, enum SWITCH_MODE mode);

/*配置 access vlan 函数*/
int l2if_delete_access_vlan(struct l2if *p_l2if);
int l2if_add_access_vlan(struct l2if *p_l2if, uint16_t vlanid);

/*配置switch->vlan_list 的函数 */
int l2if_delete_vlan(struct l2if *pif, struct vlan_range *switch_vlan,uint8_t flag );  /* 从链表删除 */ 
int l2if_add_vlan(struct l2if *pif, struct vlan_range *switch_vlan,uint8_t tag); /* 添加到链表 */ 
int l2if_clear_switch_vlan(struct l2if *p_l2if);
int l2if_vlan_send_hal(uint32_t switch_vlan_count[][2],uint16_t line_num,uint32_t ifindex,uint8_t tag, enum SWITCH_MODE mode);
int l2if_vlan_send_hal_up_tagged(uint32_t switch_vlan_count[][2],uint16_t line_num,struct l2if *p_l2if,uint8_t tag);

/*Response  query*/
int l2if_get_vlan_mapping(struct l2if_vlan_mapping *key_entry,struct l2if_vlan_mapping entry_buff[],int data_num);
int l2if_get_vlan_mapping_bulk( struct l2if_vlan_mapping *key_entry,struct l2if_vlan_mapping entry_buff[]);
int l2if_reply_vlan_mapping_bulk(void *pdata,struct ipc_msghdr_n *phdr);

int l2if_clear_storm_suppress(struct l2if *pif);
#endif
