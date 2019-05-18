/*
*    switch mode interface
*/

#ifndef HIOS_L2_IF_H
#define HIOS_L2_IF_H

#include <lib/ifm_common.h>
#include "lldp/lldp.h"
#include <lib/msg_ipc.h>
#include <lib/vty.h>
#include <lib/types.h>
#include <lib/devm_com.h>
#include "trunk.h"

/*for 15x trunk l3 function*/
#define DEF_RESV_VLAN9  4087
#define DEF_RESV_VLAN8  4086
#define DEF_RESV_VLAN7  4085 

/*for 15x port l3 function*/
#define DEF_RESV_VLAN6  4093
#define DEF_RESV_VLAN5  4092
#define DEF_RESV_VLAN4  4091
#define DEF_RESV_VLAN3  4090
#define DEF_RESV_VLAN2  4089
#define DEF_RESV_VLAN1  4088

extern unsigned int devtype;
extern int reserve_vlan_table[6];

/* switch 接口的模式，默认是 hybrid 模式 */
enum SWITCH_MODE
{
	SWITCH_MODE_HYBRID = 0,  /* hybrid 模式，支持 trunk vlan 和 access vlan */
	SWITCH_MODE_TRUNK,       /* trunk 模式，只支持 trunk vlan */
	SWITCH_MODE_ACCESS,      /* access 模式，支持 access vlan */
	SWITCH_MODE_MAX = 8
};


/* L2IF 下的配置信息类型 */
enum L2IF_INFO
{
	L2IF_INFO_INVALID = 0,	
	L2IF_INFO_MODE,         /* switch mode */
	L2IF_INFO_MAC_LEARN,    /* mac 学习 */
	L2IF_INFO_ISOLATE,      /* 端口隔离 */
	L2IF_INFO_DOT1Q_TUNNEL,
	L2IF_INFO_ACCESS_VLAN,	
	L2IF_INFO_SWITCH_VLAN,
	L2IF_INFO_SWITCH_UNTAG_VLAN,  /* untag vlan*//*add by jhz,20180313*/
	L2IF_INFO_QINQ,
	L2IF_INFO_QINQ_RANGE,
	L2IF_INFO_MAC_LIMIT,
	L2IF_INFO_STORM_SUPRESS,
	L2IF_INFO_RESERVE_VLAN,           /* 15X reserve vlan*/
	L2IF_INFO_RESERVE_VLAN_UPDATE,    /* 15X reserve vlan modify*/
	L2IF_INFO_VLAN_RLEG,             /* 15X rleg*/
    L2IF_INFO_SWITCH_VLAN_TAG,
	L2IF_INFO_MAX = 16
};


/* 端口转发状态 */
enum PORT_STATUS
{
	PORT_STATUS_FORWARD = 0,  /* 正常转发状态 */
	PORT_STATUS_BLOCK,        /* block 状态 */
	PORT_STATUS_SHUTDOWN      /* shutdown 状态 */
};


/* 端口动作 */
enum PORT_ACTION
{
	PORT_ACTION_INVALID = 0,   /* 什么也不做 */
	PORT_ACTION_BLOCK,         /* block 端口 */
	PORT_ACTION_SHUTDOWN,      /* shutdown 端口 */
};

/*端口下配置的vlan为tagged、untagged*//*add by jhz,20180313*/
struct vlan_tagged
{
	uint16_t vlanid;
	uint8_t tag;
	uint8_t pad;
};

/*风暴抑制信息*/
struct  ifm_storm_control
{
	uint8_t  storm_unicast; 	 /* 未知单播风暴抑制 ,0: disable, 1: enable*/
	uint8_t  storm_broadcast;	 /* 广播风暴抑制 ,0: disable, 1: enable*/
	uint8_t  storm_multicast;    /* 组播风暴抑制 ,0: disable, 1: enable*/
	uint8_t  pad;
	uint32_t  unicast_rate; 	 /* 未知单播风暴抑制速率*/
	uint32_t  broadcast_rate;	 /* 广播风暴抑制速率*/
	uint32_t  multicast_rate;     /* 组播风暴抑制速率*/
};

/* switch mode 的接口信息 */
struct ifm_switch
{
    enum SWITCH_MODE mode;
	uint8_t          mac_learn;  /* 端口的 MAC 学习*/
	uint8_t          isolate;    /* 端口隔离 */
	uint8_t          dot1q_cos;  /* dot1q tunnel 的 cos */
	uint8_t          limit_action;/* 超出mac学习限制值时报文处理 ,0: disable--discard, 1: enable--forward */
	uint32_t         limit_num;   /* mac学习限制数，1-16384,默认值为0 */ 
	uint16_t         access_vlan; /* 端口的 access vlan */
	uint16_t         dot1q_tunnel;/* 端口的 dot1q-tunnel vlan */
	struct  ifm_storm_control storm_control;/*端口的风暴抑制控制*/
    struct list      *vlan_list;  /* 端口的 trunk vlan list，存储 vlanid      */
    struct list      *vlan_tag_list;  /* 端口的 trunk vlan tag list，存储 vlan_tagged 结构 *//*add by jhz,20180313*/
    struct list      *qinq_list;  /* vlan mapping list, 存储 vlan_mapping 结构 */
};


/* l2 模块的公共接口数据结构 */
struct l2if
{
     uint32_t              ifindex;
	 uchar                 mac[MAC_LEN];
	 uint16_t              trunkid;        /* 接口所属的 trunk */
     enum IFNET_LINKSTAT   down_flag;      /* 0: linkup, 1: linkdown */         
     enum IFNET_MODE       mode;           /* 接口 l2/l3/switch 模式 */
     struct ifm_switch     switch_info;    /* switch mode 的接口信息 */
	 struct lldp_port      *lldp_port_info;/* lldp 的接口数据*/
	 struct efm_if         *pefm;          /* efm 的接口数据 */
	 struct loop_detect    *ploopdetect;   /* loopdetect 的接口数据 */
	 struct synce_if   		*synce_info;     /*syncE的接口数据*/	 
	 struct mstp_port		*mstp_port_info;
     struct l2cp_process    *l2cp_master;    /*add by yangzailin for l2cp*/
	 char alias[IFM_ALIAS_STRING_LEN+1];
	 struct list    		*pislt;          /*port isolate information*/
};

/* l2 各模块debug信息 */

#define L2_DBG_ELPS			0x20
#define L2_DBG_ERPS		    0x40
#define L2_DBG_LACP		    0x80
#define L2_DBG_LLDP		    0x100
#define L2_DBG_MSTP	        0x200
#define L2_DBG_TRUNK		0x400
#define L2_DBG_MAC		    0x800
#define L2_DBG_QINQ		    0x1000
#define L2_DBG_VLAN		    0x2000
#define L2_DBG_EFM			0x4000
#define L2_DBG_COMMON       0x8000
#define L2_DBG_ALL          0x1FFE0

#define L2_COMMON_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_COMMON, fmt, ##__VA_ARGS__ ); \
    } while(0)


/* 以太物理接口的 hash 表 */
extern struct hash_table l2if_table;


/* l2if table 操作函数 */
void l2if_table_init ( int size );
int vlanlist_sort(void* nvlan, void* ovlan);
int vlantag_list_sort(void* nvlan, void* ovlan);
int l2if_add(struct l2if *pif);     /* 添加到 hash 表 */
int l2if_delete(uint32_t ifindex); /* 从 hash 表删除 */
int vlanlist_sort(void* nvlan, void* ovlan);/*vlanlist sort func*/
struct l2if *l2if_lookup(uint32_t ifindex);  /* 查找一个接口 */
struct l2if * l2if_create(uint32_t ifindex); /* 新建一个接口 */
struct l2if * l2if_get(uint32_t ifindex);     /* 查找一个接口，如果不存在则创建 */
void l2if_down(uint32_t ifindex); /* 接口 down 事件处理 */
void l2if_up(uint32_t ifindex);    /* 接口 up 事件处理 */
void l2if_mode_change(uint32_t ifindex, uint32_t mode); /* 接口 mode 改变事件处理 */

void l2if_delete_switch(struct l2if *pif);
int l2if_init_switch(struct ifm_switch  *pswitch,uint32_t ifindex);
int l2if_get_switch_info_bulk(uint32_t ifindex,struct l2if entry_buff[]);
int l2if_reply_switch_info_bulk(struct ipc_msghdr_n *phdr, uint32_t ifindex);
int l2if_reply_switch_vlan_bulk(struct ipc_msghdr_n *phdr, uint32_t ifindex);
int l2if_trunk_delete_vlan (uint32_t ifindex);
int l2if_trunk_delete_port (uint16_t trunkid,uint32_t ifindex);
int l2if_trunk_add_port (uint16_t trunkid,uint32_t ifindex);
uint32_t* l2if_get_ifindex_list (uint16_t *total);
int l2if_delete_range_switch_vlan (uint32_t ifindex,uint16_t v_start,uint16_t v_end);
int l2if_delete_dot1q_tunnel_by_vlan(uint32_t ifindex,uint16_t vlanid);
int l2if_clear_mac_limit(struct l2if *pif);

/*l2if 命令行初始化函数*/
void l2if_cli_init (void);
void trunk_vlan_cli_init (void);
void l2if_vlan_cli_init (void);

/*保存配置函数*/
int l2if_config_write (struct vty *vty);
void l2if_vlan_config_write(struct vty *vty, struct list *list);
void l2if_vlan_tag_config_write(struct vty *vty, struct list *list);
void l2if_qinq_config_write(struct vty *vty, struct list *list);
void l2if_lldp_config_write(struct vty *vty, struct lldp_port *port);
void l2if_mstp_config_write(struct vty *vty, struct mstp_port	*port);
int l2if_set_shutdown ( uint32_t ifindex, uint8_t enable );
void lldp_subif_port_event(struct l2if *pif, uint32_t ifindex, uint8_t flag);
void lldp_if_port_event(struct l2if *pif, uint8_t flag);

void l2if_speed_change(uint32_t ifindex);
int l2if_reply_switch_tag_vlan_bulk(struct ipc_msghdr_n *phdr, uint32_t ifindex);
int l2if_clear_mac_by_port_vlan(char * pdata,uint32_t data_num,uint32_t ifindex);

int l2if_storm_suppress_get_bulk( struct mib_l2if_storm_control_info *storm_control_buf, uint32_t ifindex );
int l2if_rcv_storm_suppress_get_bulk( struct ipc_msghdr_n  *pmsghdr );
int l2if_dot1q_tunnel_get_bulk( struct mib_l2if_dot1q_tunnel_info *dot1q_tunnel_buf, uint32_t ifindex );
int l2if_rcv_dot1q_tunnel_get_bulk( struct ipc_msghdr_n  *pmsghdr );

#endif
