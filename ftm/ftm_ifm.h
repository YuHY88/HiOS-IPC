#ifndef FTM_IFM_H
#define FTM_IFM_H

#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/msg_ipc_n.h>
#include <lib/ifm_common.h>
#include <ifm/ifm.h>
#include <ifm/ifm_errno.h>
#include <route/route_if.h>
#include <mpls/tunnel.h>
#include "ftm_pw.h"


/* 获取接口 mac 地址 */
#define FTM_IFM_GET_MAC(pif, smac) \
if(pif->pparent)\
    memcpy(smac, pif->pparent->ifm.mac, MAC_LEN);  \
else                                               \
    memcpy(smac, pif->ifm.mac, MAC_LEN);


/* 获取接口 tpid */
#define FTM_IFM_GET_TPID(pif) \
if(pif->pparent)\
    return pif->pparent->ifm.tpid;  \
else                                \
    return pif->ifm.tpid;


/* ftm 接口数据结构 */
struct ftm_ifm
{
    struct ifm_info    ifm;
    struct ftm_ifm     *pparent; /* 父接口的指针 */
    struct ifm_l3      *pl3if;   /* l3 mode 的接口配置 */
    struct ifm_arp     *parp;    /* arp 的接口信息 */
    struct ftm_ifm_l2  *pl2if;   /* l2 mode 的接口配置 */
    struct tunnel_t    *ptunnel; /* tunnel 接口 */
    struct ifm_counter *pcounter;
};


extern struct hash_table ftm_ifm_table;



/* Prototypes of ifm hash */
void ftm_ifm_table_init ( unsigned int size );
int ftm_ifm_add ( struct ifm_info *pifm_info );
int ftm_ifm_delete ( uint32_t ifindex );
struct ftm_ifm *ftm_ifm_lookup ( uint32_t ifindex );
int ftm_ifm_get_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type ); //根据索引号获取信息
int ftm_ifm_set_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type ); //根据索引号修改信息

int ftm_l3if_add ( void *pdata, uint32_t ifindex, uint8_t subtype );
int ftm_l3if_delete ( void *pdata, uint32_t ifindex, uint8_t subtype );


int ftm_ifm_msg ( void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t ifindex );
int ftm_l3if_msg ( void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t index );

struct ftm_ifm_l2 *ftm_ifm_get_l2if ( uint32_t ifindex );
struct ifm_l3 *ftm_ifm_get_l3if ( uint32_t ifindex );
struct ftm_ifm_l2 *ftm_ifm_create_l2if ( struct ftm_ifm *pifm );
struct ifm_l3 *ftm_ifm_create_l3if ( struct ftm_ifm *pifm );
void ftm_ifm_free_l2if ( struct ftm_ifm *pifm );
void ftm_ifm_free_l3if ( struct ftm_ifm *pifm );
uint32_t ftm_ifm_get_l3if_ipv4(struct ifm_l3 *pl3if, uint32_t dip);
struct ipv6_addr *ftm_ifm_get_l3if_ipv6(struct ifm_l3 *pl3if, struct ipv6_addr *pdip);

int ftm_vlanif_create ( struct ftm_ifm *pvlanif );
void ftm_vlanif_delete ( uint32_t ifindex );
/* 接口 up 事件的处理 */
void ftm_ifm_up ( struct ftm_ifm *pifm );
/* 接口 down 事件的处理 */
void ftm_ifm_down ( struct ftm_ifm *pifm );
/* 接口 模式改变 事件的处理 */
void ftm_ifm_mode_change ( struct ftm_ifm *pifm );

#endif




