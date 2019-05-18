/*
 * snmp_config_table.h
 */
#ifndef SNMP_CONFIG_TABLE_H
#define SNMP_CONFIG_TABLE_H

#include <lib/types.h>
#include <lib/vty.h>
#include <lib/linklist.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>


//#include <lib/inet_ip.h>

#define DEBUG_SNMP

#if defined(DEBUG_SNMP) && !defined(SNMP_DEBUG)
#define SNMP_LOG(level, str, args...) zlog(zlog_default, level, str, ## args)
#else
#define SNMP_LOG(x)
#endif


/* 按位定义debug信息的类型 */
#define SNMP_DBG_MIB_GET	(1 << 0)		//mib get  debug
#define SNMP_DBG_MIB_SET	(1 << 1)		//mib set debug
#define SNMP_DBG_TRAP		(1 << 2)		//trap debug
#define SNMP_DBG_PACKET		(1 << 3)		//packet  debug
#define SNMP_DBG_CACHE		(1 << 4)		//packet  debug
#define SNMP_DBG_U0			(1 << 5)		//packet  debug
#define SNMP_DBG_ALL		(SNMP_DBG_MIB_GET |\
							 SNMP_DBG_MIB_SET |\
							 SNMP_DBG_TRAP |\
							 SNMP_DBG_CACHE |\
							 SNMP_DBG_PACKET |\
							 SNMP_DBG_U0)	//所有debug




#define SNMP_ERROR           -1
#define SNMP_OK              ERRNO_SUCCESS
#define SNMP_FAIL            ERRNO_FAIL
#define SNMP_NOT_FOUND       ERRNO_NOT_FOUND
#define SNMP_MALLOC_FAIL     ERRNO_MALLOC
#define SNMP_OVERSIZE        ERRNO_OVERSIZE
#define SNMP_IPC_FAIL        ERRNO_IPC
#define SNMP_PARAM_ERR       ERRNO_PARAM_ILLEGAL
#define SNMP_PORT_ALLOC_FAIL ERRNO_PORT_ALLOC
#define SNMP_PKT_SEND_FAIL   ERRNO_PKT_SEND
#define SNMP_PKT_RECV_FAIL   ERRNO_PKT_RECV


#define SNMP_COMMUNITY_ITEM         0
#define SNMP_TRAP_ITEM              1
#define SNMP_INFORM_ITEM            2
#define SNMP_USER_ITEM              3

/* snmp show item */
#define STR_SHOW_COMMON_CONFIG      "------------------common config-------------------"
#define STR_SHOW_COMMUNITY_CONFIG   "-----------------community config-----------------"
#define STR_SHOW_V1_TRAP_CONFIG     "-----------------v1 trap config-------------------"
#define STR_SHOW_V2C_TRAP_CONFIG    "-----------------v2c trap config------------------"
#define STR_SHOW_INFORM_CONFIG      "------------------inform config-------------------"
#define STR_SHOW_V3_TRAP_CONFIG     "-----------------v3 trap config-------------------"
#define STR_SHOW_USER_CONFIG        "-------------------user config--------------------"
#define STR_SHOW_TAIL               "--------------------------------------------------"

#define ITEM_COMMUNITY_MAX_NUM      8
#define ITEM_SERVER_MAX_NUM        16
#define ITEM_USER_MAX_NUM           8

//snmp version
typedef enum
{
    V1 = 0,
    V2C,
    V3,
    ALL
} SNMP_VERSION;


//snmp packet statistics from ftm
typedef struct _packet_statistics_
{
    uint32_t packet_received;
    uint32_t packet_send;
} PACKET_STATISTICS;


//commnuity struct
#define COMMUNITY_NAME_MAX_LEN  31

typedef enum
{
    RO = 0,
    RW
} COMMUNITY_ATTR;

typedef struct _community_item
{
    char                name[COMMUNITY_NAME_MAX_LEN + 1];
    COMMUNITY_ATTR      attribute;  //0:ro 1:rw
    int                 acl_num;
} COMMUNITY_ITEM;


//trap struct
#define TRAP_NAME_MAX_LEN       31

enum ip_type
{
    IPv4 = 0,
    IPv6
};

/* IPv6 & IPv4_ 地址*/
struct _ip_addr
{
    enum ip_type    type;
    uint32_t        addr[4];
    /*union {
      uint32_t ipv6[4];
      uint32_t ipv4;
    }addr;*/
};

typedef struct _trap_item
{
    struct _ip_addr     ip_addr;

    uint32_t            port;
    int                 version;
    char                name[TRAP_NAME_MAX_LEN + 1];
    uint32_t            vpn;
}   TRAP_ITEM;


//inform struct
typedef struct _inform_item
{
    struct _ip_addr     ip_addr;

    uint32_t            port;
    char                name[TRAP_NAME_MAX_LEN + 1];
    uint32_t            vpn;
}   INFORM_ITEM;



//v3 user struct
#define USER_NAME_MAX_LEN       31

#define AUTH_SECRET_MAX_LEN     31
#define AUTH_SECRET_MIN_LEN     8

#define ENCRY_SECRET_MAX_LEN    31
#define ENCRY_SECRET_MIN_LEN    8

typedef enum
{
    GUEST = 0,
    OPERATOR,
    ADMIN
} USER_PERMISSION;

typedef enum
{
    NO_AUTH = 0,
    MD5,
    SHA
} AUTH_METHOD;

typedef enum
{
    NO_ENCRY = 0,
    DES,
    AES
} ENCYR_METHOD;


typedef struct _user_item
{
    char                name[USER_NAME_MAX_LEN + 1];
    USER_PERMISSION     permission;                             //0:guest   1:operator  2:admin
    AUTH_METHOD         auth_method;                            //0:noauth  1:md5       2:sha
    ENCYR_METHOD        encry_method;                           //0:noencry 1:DES       2:AES
    char                auth_secret[AUTH_SECRET_MAX_LEN + 1];   //auth secret
    char                encry_secret[ENCRY_SECRET_MAX_LEN + 1]; //encryption secret

} USER_ITEM;


typedef struct _U0_item
{
	struct in_addr 	ipv4;
	uint32_t 		ifindex;
	
} U0_ITEM;



extern struct list *snmp_community_list;
extern struct list *snmp_trap_list;
extern struct list *snmp_inform_list;
extern struct list *snmp_user_list;

extern SNMP_VERSION snmp_version;
extern u_int16_t    snmp_server_vpn;
extern u_int32_t    snmp_trap_dcn_src_ip;
extern u_int32_t    snmp_trap_if_src;
extern u_int32_t    snmp_trap_if_src_ip;

extern uint32_t 	g_snmp_U0_ipv4;

extern PACKET_STATISTICS snmp_packet_statistics;

int snmp_init_config(void);
int snmp_community_item_config_all(void);
int snmp_community_item_config_new(COMMUNITY_ITEM *item);
int snmp_trap_item_config_all(void);
int snmp_trap_item_config_new(TRAP_ITEM *item);
int snmp_inform_item_config_all(void);
int snmp_inform_item_config_new(INFORM_ITEM *item);
int snmp_user_item_config_all(void);
int snmp_user_item_config_new(USER_ITEM *item);

int   snmp_config_item_add(int item_num, void *item);
int   snmp_config_item_del(int item_num, void *item);
void *snmp_config_item_get(int item_num, void *item);

int snmp_show_init_items(int item_num, struct vty *vty);
int snmp_show_items(int item_num, struct vty *vty);
int snmp_show_version(struct vty *vty);
int snmp_show_all_items(struct vty *vty);

int snmp_write_config_items(struct vty *vty);
int snmp_community_item_cmp(COMMUNITY_ITEM *val1, COMMUNITY_ITEM *val2);
int snmp_trap_item_cmp(TRAP_ITEM *val1, TRAP_ITEM *val2);
int snmp_inform_item_cmp(INFORM_ITEM *val1, INFORM_ITEM *val2);
int snmp_user_item_cmp(USER_ITEM *val1, USER_ITEM *val2);
int snmp_trap_v3_del(TRAP_ITEM *item);


COMMUNITY_ITEM *snmp_community_lookup_snmp(COMMUNITY_ITEM *item , int exact) ;
USER_ITEM *snmp_user_lookup_snmp(USER_ITEM *item , int exact) ;
TRAP_ITEM *snmp_trap_lookup_snmp(TRAP_ITEM *item , int exact) ;

extern void snmp_trap_enable_set(int trap_enable);
extern int snmp_trap_enable_get(void);

extern void snmp_trap_link_enable_set(int trap_enable);
extern int snmp_trap_link_enable_get(void);

void sys_grp_write(struct vty *vty);

int snmp_trap_source_if_cfg(struct vty *vty, uint32_t ifindex);
int snmp_no_trap_source_if_cfg(void);
int snmp_trap_source_ip_cfg(struct vty *vty, uint32_t ip);

int snmp_handle_route_msg(struct ipc_mesg_n *pmsg);
int snmp_handle_ospf_msg(struct ipc_mesg_n *pmsg);

void snmp_U0_hash_init(void);
uint32_t snmp_U0_hash_get_ifindex_by_ip(uint32_t s_addr);

#endif /*SNMP_CONFIG_TABLE_H*/

