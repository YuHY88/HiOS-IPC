/*
*       manage the static mac table
*
*/

#ifndef HIOS_STATIC_MAC_H
#define HIOS_STATIC_MAC_H


#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/vty.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include "l2_if.h"

#define MAC_STATIC_NUM 1024     /* 静态 MAC 的规格 */
#define MAC_AGE_TIME   300      /* 动态 MAC 的老化时间，默认 5 分钟 */
#define MAC_LEN        6
#define MAC_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_MAC, fmt, ##__VA_ARGS__ ); \
    } while(0)

enum MAC_ERROR_TYPE
{
	MAC_ERROR_NO,  			/* 正确 */
	MAC_ERROR_OVERSIZE,  	/* 超出数量限制 */
	MAC_ERROR_EXISTED,		/* 已经存在 */
	MAC_ERROR_NOT_EXISTED,	/* 不存在 */
	MAC_ERROR_HASH,			/* hash操作失败*/
	MAC_ERROR_MALLOC,		/* 内存分配失败*/
	MAC_ERROR_NULL,			/* 参数为NULL*/
	MAC_ERROR_VSI,			/* vsi没有创建 */
	MAC_ERROR_PW,			/* pw没有创建 */
	MAC_ERROR_BIND,			/* pw没有绑定vsi */
	MAC_ERROR_HAL_FAIL		/* hal返回失败*/	
};

enum PARSE_CLI_RET
{
    PARSE_SUCCEED,
    PARSE_INVALID_CHAR,
    PARSE_IPLEN_SHORT,
    PARSE_IPLEN_LONG,
    PARSE_IPMASK_LONG,
    PARSE_MACLEN_SHORT,
    PARSE_MACLEN_LONG,
    PARSE_MALFORMED,
    PARSE_COMMON_ERROR
};

/* mac 消息类型 */
enum MAC_INFO
{
    MAC_INFO_MAC = 0,                   /* 静态 MAC 消息*/
    MAC_INFO_AGE_TIME_SET,              /* MAC老化时间设置 */
    MAC_INFO_AGE_TIME_GET,              /* MAC老化时间查询 */   
    MAC_INFO_MAC_MOVE,                  /* MAC 漂移设置*/
    MAC_INFO_MAC_LIMIT,                 /* MAC 学习限制*/
    MAC_INFO_MAC_CONFIG,                /* MAC 全局配置信息*/
    MAC_INFO_MAC_CLEAR,               /*mac clear*/
    MAC_INFO_STATIC_DEL_ALL,            /* Delete all static mac*/
}; 
 
/* mac 状态 */
enum MAC_STATUS
{
    MAC_STATUS_STATIC = 0,      /* 静态 MAC */
    MAC_STATUS_DYNAMIC,         /* 动态 MAC */
    MAC_STATUS_BLACKHOLE,				/* 黑洞 MAC */    
    MAC_STATUS_AGED,            /* 处于老化状态的 MAC */
};

/*mac 全局配置*/
struct mac_configuration
{
    uint32_t    age_time;  		/*mac老化时间*/
	uint32_t	limit_num;		/* mac 限制数，1-16384,默认值为0 */
	uint8_t 	limit_action;	/* 超出mac学习限制值时报文处理 ,0: disable--discard, 1: enable--forward */
    uint8_t 	mac_move;		/*mac漂移,0-enable,1-disable*/
	uint8_t		pad[2];
};

/* mac表 key 结构 */   
struct mac_key
{
    uchar            mac[MAC_LEN];
    uint16_t         vlanid;  	
};

/* mac 表结构 */    
struct mac_entry
{
    struct mac_key   key;
    uint32_t         outif;             /* output interface */
    enum MAC_STATUS  status;            /* MAC 状态 */
    uint16_t         time;              /* 剩余老化时间*/
    int              addr_index;        /* added by liubo 2019-1-16 for mib_l2func*/
};

/*added by liubo 2019-1-16 for mib_l2func*/
struct mac_snmp_index
{
    struct mac_key   key;
    int              addr_index;
};

/*added by liubo 2019-1-16 for mib_l2func to get dynamic mac*/
struct mac_snmp_entry
{
    struct mac_key   key;
    uint32_t         outif;             /* output interface */
    int              addr_index;        /* find mac table from this index*/
};

/* 静态 mac 的 hash 表，使用 mac 和 vlanid 作为 key */
struct hash_table mac_static_table;


/*mac 配置信息保存，包括mac老化时间和mac漂移*/
struct mac_configuration mac_config;

/* 对基于接口的静态 mac 表的操作函数 */
void mac_static_table_init(unsigned int size);
int mac_static_add(struct mac_entry *pmac);
int mac_static_local_add(struct mac_entry *pmac);
int mac_static_delete(uchar *pmac, uint16_t vlanid);

int mac_static_print (struct vty *vty,struct mac_entry *pmac_entry);
struct mac_entry *mac_static_lookup(uchar *pmac, uint16_t vlanid);
struct mac_entry * mac_static_lookup_by_ifindex(uint32_t ifindex);
int mac_static_delete_by_ifindex(uint32_t ifindex);
int mac_static_delete_all_by_ifindex(uint32_t ifindex);
struct mac_entry * mac_static_lookup_next_by_mac_entry(struct mac_entry * pmac);
int mac_static_add_by_ifindex(uint32_t ifindex);

/*mac全局配置*/
void mac_config_init(void);
int mac_move_set(struct mac_configuration *config);
int mac_ageing_time_set(struct mac_configuration *config);
int mac_limit_set(struct mac_configuration *config);

/*初始化*/
void mac_static_cmd_init(void);
int mac_static_config_write (struct vty *vty);

/*处理mac地址*/
int cli_parse_mac_addr(uchar *clistr, uchar *deststr);
int cli_mac_form_dotstring(struct vty *vty, uchar *clistr, uchar *deststr);

/*mib查询*/
int mac_static_reply_mac_config(struct ipc_msghdr_n *phdr);
int mac_static_get_bulk( struct mac_key *key_entry,struct mac_entry entry_buff[]);
int mac_static_reply_mac_info_bulk(void *pdata,struct ipc_msghdr_n *phdr);

#endif

