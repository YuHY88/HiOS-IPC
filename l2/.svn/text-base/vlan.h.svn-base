
#ifndef HIOS_VLAN_H
#define HIOS_VLAN_H

#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/vty.h>
#include "l2_if.h"

#define VLAN_MAX         4095     /* vlan 的最大值 */
#define VLAN_MAC_LIMIT   16*1024  /* vlan 的 mac 限制数 */
#define DEFAULT_VLAN_ID  4096
#define VLAN_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_VLAN, fmt, ##__VA_ARGS__ ); \
    } while(0)


/* vlan 信息类型*/
enum VLAN_INFO
{
     VLAN_INFO_INVALID = 0,        
     VLAN_INFO_ENABLE,          /* vlan 使能 */
     VLAN_INFO_MAC_LEARN,       /* mac 学习 */
     VLAN_INFO_MAC_LIMIT,       /* mac 限制 */     
     VLAN_INFO_STORM_SUPRESS,   /* 风暴抑制 */     
     VLAN_INFO_PORT,            /* vlan 端口成员 */
     VLAN_INFO_NEXT_IFINDEX,
     VLAN_INFO_MAX = 8
};

/*tagged vlan or untagged vlan*//*edit by jhz,20180313*/
enum VLAN_TAG_INFO
{
	TAGGED=0,
	UNTAGGED=1,
	DOT1Q_TUNNEL
};

/* 全局 vlan 的配置信息 */
struct vlan_info
{
	uint16_t vlanid;	
	uint8_t  enable;        /* 0: disable, 1: enable，默认 disable */
	uint8_t  mac_learn;     /* 0: 不学习，1: 学习，默认学习 */
	uint8_t  storm_unicast; 	/* 未知单播风暴抑制 ,0: disable, 1: enable*/
	uint8_t  storm_broadcast;	 /* 广播风暴抑制 ,0: disable, 1: enable*/
	uint8_t  storm_multicast; 	/* 未知组播风暴抑制 ,0: disable, 1: enable*/
	uint8_t  limit_action;	/* 超出mac学习限制值时报文处理 ,0: disable--discard, 1: enable--forward */
	uint32_t limit_num;     /* mac 限制数，1-16384,默认值为0 */ 
};

/*vlan下的端口出报文是否剥标签*//*edit by jhz,20180313*/
struct port_tagged
{
	uint32_t ifindex;
	uint8_t tag;
	uint8_t pad[3];
};

/* 全局 vlan 的数据结构 */
struct vlan_entry
{
    uchar name[NAME_STRING_LEN];
    struct vlan_info vlan;
	struct list portlist;  /* vlan 的成员端口 */
	struct list port_tag_list;  /* vlan 的成员端口,携带tag与否标签 *//*edit by jhz,20180313*/
};

extern struct vlan_entry *vlan_table[VLAN_MAX];


/* 全局 vlan 的操作函数 */
int vlan_table_init(void); /* 初始化 vlan_table */
int vlan_create(uint16_t v_start,uint16_t v_end);
int vlan_delete(uint16_t v_start,uint16_t v_end);
struct vlan_entry *vlan_lookup(uint16_t vlan_id);
int vlan_clear(void);
int vlan_set_info(uint16_t v_start,uint16_t v_end, void *pinfo1, void *pinfo2,void *pinfo3,enum VLAN_INFO type);
int vlan_add_port(struct vlan_entry *pvlan, uint32_t ifindex);    /* vlan 添加端口 */
int vlan_delete_port(struct vlan_entry *pvlan, uint32_t ifindex);/* vlan 删除端口 */
void vlan_add_interface(uint16_t v_start,uint16_t v_end,uint32_t ifindex,uint8_t tag);/*接口添加vlan时，vlan结构体也增加对应信息*/
void vlan_delete_interface(uint16_t v_start,uint16_t v_end,uint32_t ifindex);/*接口删除switch vlan时，vlan结构体也删除保存的接口信息*/
int l2if_get_vlan_info_bulk(uint32_t v_start,struct vlan_info entry_buff[]);
int vlan_reply_vlan_info_bulk(struct ipc_msghdr_n *phdr, uint16_t vlanid);
int portlist_sort(void* newif, void* oldif);
int tagged_portlist_sort(void* newif, void* oldif);

/*vlan 命令行函数*/
void vlan_cli_init (void);  /* 注册全局 VLAN 命令行*/
void vlan_single_config_write(uint16_t vlanid,struct vty *vty);
int  vlan_config_write (struct vty *vty);/* 全局 vlan 的配置保存 */


#endif
