#ifndef _ZEBRA_OSPF_BFD_H
#define _ZEBRA_OSPF_BFD_H

#include "mpls/bfd/bfd_session.h"
#include "mpls/bfd/bfd.h"



/* bfd 会话状态 */
enum OSPF_BFD_STATUS
{
	OSPF_BFD_STATUS_ADMINDOWN = 0, /* 管理 down 状态 */
	OSPF_BFD_STATUS_DOWN,          /* down 状态 */
	OSPF_BFD_STATUS_INIT,          /* INIT 状态 */
	OSPF_BFD_STATUS_UP 	           /* UP 状态 */
};

/* bfd 会话从up变成其他状态的原因*/
enum OSPF_BFD_DIAG
{
	OSPF_BFD_DIAG_NO_DIAG = 0, 	  	/* 无诊断*/
	OSPF_BFD_DIAG_DETECT_EXPIRE,  	/* 检测定时器超时*/
	OSPF_BFD_DIAG_EHCO_FAIL,        /* 回声功能失败*/
	OSPF_BFD_DIAG_NEIGHBOR_DOWN,	/* 会话邻居down*/
	OSPF_BFD_DIAG_FORWARD_RESET,	/*转发平面重新设置*/
	OSPF_BFD_DIAG_PATH_DOWN,		/*路径down*/
	OSPF_BFD_DIAG_CON_PATH_DOWN,	/*链接路径down*/
	OSPF_BFD_DIAG_ADMIN_DOWN 		/*管理down*/
};

#define OSPF_SUBTYPE_BIND_BFD 		3
#define OSPF_SUBTYPE_UNBIND_BFD 	4
#define OSPF_SUBTYPE_GET_REPLY 		12

/* bfd session 数据结构 */
struct ospf_bfd_sess_info
{
	enum OSPF_BFD_STATUS  status; 	/* 工作状态 */
	enum OSPF_BFD_DIAG diag;		/*会话最后一个从up变成其他状态的原因*/
	uint16_t session_id;
	uint16_t remote_id; 		  	/* remote discriminator */
	uint16_t local_id;			 	/* local discriminator */	
	uint32_t index; 			  	/* 检测对象的索引，for 接口是 ifindex，for pw 是 pwindex，for lsp 是 lspindex */
	uint32_t dstip; 			  	/* 对端的 IP 地址 */
	uint32_t srcip; 			  	/* 源端的 IP 地址 */
	uint32_t remote_send_interval;
	uint32_t remote_recv_interval;
	uint16_t remote_multiplier;
	uint16_t remote_state;
	uint32_t cc_interval_cfg;	  	/* 配置的本地 cc 间隔 */
	uint32_t cc_interval_send; 	  	/* 协商结果: 报文发送间隔,单位ms */
	uint32_t cc_interval_recv; 	  	/* 协商结果: 报文接收间隔,单位ms */
	uint16_t cc_multiplier_cfg;	  	/* 配置的本地检测次数*/	
	uint16_t cc_multiplier; 	  	/* 协商结果: 检测次数*/
	//uint64_t recv_count;			/*统计会话接收报文数目*/
	//uint64_t send_count;			/*统计会话发送报文数目*/
	//uint64_t up_count;			/*统计会话up次数*/
	//uint64_t down_count;			/*统计会话down次数*/
};
	
extern const char *ospf_bfd_diag_string[8];

extern void ospf_bfd_update_session (struct ospf_neighbor *nbr, int old_state);
extern void ospf_bfd_if_update (struct ospf_interface *oi);
extern int ospf_bfd_add_session_by_interface (struct ospf_interface *oi);
extern int ospf_bfd_modify_session_by_interface (struct ospf_interface *oi);
//extern void ospf_bfd_session_down (struct bfd_sess *session_info);
extern void ospf_bfd_session_down (struct bfd_info *base_sess_info);

extern void ospf_bfd_session_info_load(struct bfd_sess *session_info);

extern void ospf_bfd_session_status_update(struct bfd_info *bfd_sess_info);



#endif

