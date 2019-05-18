#ifndef HIOS_IFM_H
#define HIOS_IFM_H


#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <qos/qos_mapping.h>
#include "ifm_errno.h"

/*按位定义debug信息的类型*/
#define IFM_DBG_COMMON     (1 << 0 )
#define IFM_DBG_ALL     IFM_DBG_COMMON

/* combo 接口的光电模式 */
enum COMBO_MODE
{
    COMBO_MODE_FIBER = 0,  /* 光口 */
    COMBO_MODE_COPER       /* 电口 */
};


enum IFM_OPCODE
{
    IFM_OPCODE_INVALID = 0,
    IFM_OPCODE_ADD,      /* add entry */
    IFM_OPCODE_DELETE,   /* delete entry */
    IFM_OPCODE_UPDATE,   /* update entry */
    IFM_OPCODE_MAX = 255
};


/*保存接口全部信息的结构体*/
struct ifm_entry
{
    char            alias[IFM_ALIAS_STRING_LEN];/*别名*/
    struct list     *sonlist; /*子接口链表 */
    struct ifm_info ifm;      /* 接口公共信息 */
    struct ifm_port port_info;/* 物理端口的信息 */
    struct ifm_counter *pcounter;/*接口统计信息*/
    uint8_t  alarm;/*接口告警标志位*/
    uint8_t  devm_flag;/*devm 设置接口hide 或者删除标志*/

};


#define IFM_ENTRTY_ALL_LOOP(node, N)\
    for (N = 0; N  < HASHTAB_SIZE; N++)\
        for((node) = ifm_table.buckets[N]; node; (node) = (node)->next)\

#define IFM_ENTRTY_LOOP(node, N)\
            for (N = 0; N  < HASHTAB_SIZE; N++)\
                for((node) = ifm_table.buckets[N]; node; (node) = (node)->next)\
                    if(!(((struct ifm_entry *)(node->data))->port_info.hide))\

struct hash_table ifm_table;
extern struct thread_master *ifm_master;

int is_sla_enable;
uint8_t dcn_flag;


void ifm_table_init ( int size );
int ifm_create_interface ( uint32_t ifindex );
int ifm_create_ethernet ( struct ifm_common *pifcom );
int ifm_create_sub_ethernet ( uint32_t ifindex );
int ifm_create_physical ( struct ifm_common *pifcom );
int ifm_create_sub_physical ( uint32_t ifindex );
int ifm_create_vlanif ( uint32_t ifindex );
int ifm_create_tunnel ( uint32_t ifindex );
int ifm_create_trunk ( uint32_t ifindex );
int ifm_create_sub_trunk ( uint32_t ifindex );
int ifm_create_loopback ( uint32_t ifindex );
int ifm_create_tdm ( struct ifm_common *pifcom );
int ifm_create_sub_tdm ( uint32_t ifindex );
int ifm_create_stm ( struct ifm_common *pifcom );
int ifm_create_sub_stm ( uint32_t ifindex );
int ifm_create_clock ( struct ifm_common *pifcom  );
int ifm_delete ( uint32_t ifindex );
int ifm_get_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type ); //根据索引号获取信息
int ifm_set_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type, enum IFM_OPCODE opcode ); //根据索引号修改信息
struct ifm_entry *ifm_create ( uint32_t ifindex );
struct ifm_entry *ifm_lookup ( uint32_t ifindex ); /*在hash中搜寻指定ifindex的节点内容*/
struct ifm_entry *ifm_lookup2 ( uint32_t ifindex );
int ifm_add ( struct ifm_entry *pifm );
int ifm_delete_if ( uint32_t ifindex );
int ifm_checkout_mac ( uchar *mac );
int ifm_checkout_encap ( struct ifm_entry *parent_pifm, struct ifm_entry *pifm, struct ifm_encap *encap );
int ifm_checkout_trunk ( struct ifm_entry *pifm, enum IFNET_INFO type );
int ifm_checkout_outband ( struct ifm_entry *pifm, enum IFNET_INFO type );
int ifm_init_info ( uint32_t ifindex, struct ifm_info *pinfo );
int ifm_set_son ( int ifindex, struct ifm_entry *psubifm, enum IFM_OPCODE opcode );
int ifm_set_shutdown ( struct ifm_entry *pifm, uint8_t shutdown );
int ifm_set_status ( struct ifm_entry *pifm, uint8_t status );
int ifm_set_mode ( struct ifm_entry *pifm, uint8_t mode );
int ifm_set_encap ( struct ifm_entry *pifm, struct ifm_encap *encap );
int ifm_set_flap ( struct ifm_entry *pifm,  uint8_t  flap_period );
int ifm_set_down_flap ( struct ifm_entry *pifm,  uint8_t  flap_period );
int ifm_set_speed ( struct ifm_entry *pifm,  struct ifm_port *speed );
int ifm_set_statistics ( struct ifm_entry *pifm,  uint8_t  statistics );
int ifm_set_tpid ( struct ifm_entry *pifm,  uint16_t  tpid );
int ifm_set_mtu ( struct ifm_entry *pifm,  uint16_t  mtu );
int ifm_set_jumbo ( struct ifm_entry *pifm,  uint16_t  jumbo );
int ifm_set_loopback ( struct ifm_entry *pifm,  struct ifm_reflector reflector );
int ifm_set_mac ( struct ifm_entry *pifm,  uchar *mac );
int ifm_set_alias ( struct ifm_entry *pifm,  char *alias );
int ifm_set_subtype ( struct ifm_entry *pifm,  int subtype  );
int ifm_set_sfp_off ( struct ifm_entry *pifm ,  uint32_t sfp_off );
int ifm_set_sfp_on ( struct ifm_entry *pifm, uint32_t  sfp_on );
int ifm_set_sfp_als ( struct ifm_entry *pifm, uint8_t sfp_als );
int ifm_set_sfp_tx ( struct ifm_entry *pifm, uint8_t sfp_tx );
int ifm_get_counter ( uint32_t ifindex, struct ifm_counter *pCounter);
int ifm_get_speed ( uint32_t ifindex, struct ifm_port *port);
int ifm_set_trunk ( uint32_t ifindex, uint16_t trunk_id,  int opcode );
int ifm_set_dcn (u_char enable );
int ifm_delete_sons ( struct ifm_entry *pifm );
int ifm_delete_son_all ( int ifindex );
int ifm_delete_son_if ( uint32_t ifindex );
int ifm_add_wait_ack ( struct ifm_entry *pifm );
int ifm_set_sons_sflag_init ( struct list *sonlist );
int ifm_set_sons_status_thread ( struct thread *thread );
int ifm_set_sons_status ( struct ifm_entry *pifm, uint8_t status );
int ifm_link_alarm_report ( struct ifm_entry *pifm, int opcode );
int ifm_intf_cmp_func ( struct ifm_entry *ifp1 , struct ifm_entry *ifp2 );
int ifm_set_loopback_info( struct ifm_entry*pifm, struct ifm_loopback * loop_info );
int ifm_set_combo( struct ifm_entry*pifm, uint8_t fiber_sub );
int ifm_checkout_dcn(struct ifm_entry * pifm);
int ifm_set_flow_control(struct ifm_entry *pifm, struct ifm_flow_control *flow_ctrl);
int ifm_get_flow_control(uint32_t ifindex, struct ifm_flow_control *flow);
int ifm_checkout_mode ( struct ifm_entry *pifm, enum IFNET_INFO type );
int ifm_set_sample_interval( struct ifm_entry *pifm, uint8_t interval );
int ifm_get_phymaster(uint32_t ifindex, uint32_t *ms);

#endif




