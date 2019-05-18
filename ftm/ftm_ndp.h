/*
*       ftm ndp table manage
*/
#ifndef FTM_NDP_H
#define FTM_NDP_H


#include <route/ndp.h>



#define NDP_INDEX_BASE  20000 /* NDP 索引从 20000 开始 */

/*reference frome RFC4861*/
#define NDP_STALE_TIMER      30000    /*neighbor reachable time*/
#define NDP_RETRY_TIMER      1000     /*ns pkt retry timeval*/
#define NDP_FIRST_PROBE_TIME 5000000  /*waiting time before send first probe pkt*/

#define NDP_IPV6_ADDR_MASTER    0
#define NDP_IPV6_ADDR_SLAVE     1
#define NDP_IPV6_ADDR_LINKLOCAL 2



#define FTM_NDP_ZLOG_DEBUG(format,...)\
    zlog_debug(FTM_DBG_NDP, "%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

#define FTM_NDP_ZLOG_ERR(format,...)\
    zlog_err("%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

#define GSTATIC_NDP_LT_GLIMIT (gndp.num_static < ND_STATIC_NUM)
#define GSTATIC_NDP_EQ_GLIMIT (gndp.num_static == ND_STATIC_NUM)

#define GNDP_NUM_LT_GLIMIT (gndp.count + gndp.num_static < ND_TOTAL_NUM)
#define GNDP_NUM_EQ_GLIMIT (gndp.count + gndp.num_static == ND_TOTAL_NUM)

#define GDYNAMIC_NDP_LT_GLIMIT (gndp.count < gndp.num_limit)
#define GDYNAMIC_NDP_EQ_GLIMT (gndp.count == gndp.num_limit)

#define PIF_DYNAMIC_NDP_LT_PLIMIT(pifm) (pifm->parp->neighbor_count < pifm->parp->neighbor_limit)
#define PIF_DYNAMIC_NDP_EQ_PLIMIT(pifm) (pifm->parp->neighbor_count == pifm->parp->neighbor_limit)

#define PIF_DYNAMIC_NDP_LT_GLIMIT(pifm) (pifm->parp->neighbor_count < garp.num_limit)
#define PIF_DYNAMIC_NDP_EQ_GLIMIT(pifm) (pifm->parp->neighbor_count == garp.num_limit)

#define FTM_NDPIF_CTRL_STRUCT_INIT(pifm)\
    pifm->parp->neighbor_limit = ND_TOTAL_NUM;\
    pifm->parp->ifindex = pifm->ifm.ifindex;\
    pifm->parp->neighbor_count = 0;\
    pifm->parp->ndp_disable = 1;\
    pifm->parp->ndp_proxy = 0;\


#define FTM_NDPIF_CTRL_STRUCT_CLR(pifm)\
    pifm->parp->neighbor_limit = 0;\
    pifm->parp->neighbor_count = 0;\
    pifm->parp->ndp_disable = 0;\
    pifm->parp->ndp_proxy = 0;\


#define FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp)\
	ftm_msg_send_to_hal(pndp, sizeof(struct ndp_neighbor), 1, IPC_TYPE_NDP, 0, IPC_OPCODE_UPDATE, 0);

#define NDP_ENTRY_TIME_DECREASE(time)\
    if (time > NDP_TIMER)\
        time -= NDP_TIMER;\
    else                         \
        time = 0;

extern struct hash_table ndp_table;
extern struct ndp_global gndp;


//NDP模块初始化
int ndp_init();

//ndp表初始化
void ndp_table_init(uint32_t size);
struct ndp_neighbor *ndp_lookup_active(struct ipv6_addr *paddr, uint16_t vpn, uint32_t ifindex);
//添加nd表项
int ndp_add(struct ndp_neighbor *pnd_entry);

//更新nd表项
int ndp_update(struct ndp_neighbor *nd_old, struct ndp_neighbor *nd_new);

//删除nd表项
int ndp_delete(struct ipv6_addr *paddr, uint16_t vpn, uint32_t ifindex);

//查找nd 表项
struct ndp_neighbor *ndp_lookup(uint8_t paddr[16], uint16_t vpn, uint32_t ifindex);

//老化指定接口的nd 表项
int ndp_delete_interface(uint32_t ifindex);

//老化指定vpn 的nd 表项
int ndp_delete_vpn(uint16_t vpnid);

//老化所有nd 表项
int ndp_delete_all(void);

//接口down 时删除本接口下动态nd表项
void ndp_delete_if_linkdown(uint32_t ifindex);


//接收ndp控制层消息
void ftm_ndp_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);

//添加ndp全局配置信息
int ftm_ndp_global_add(struct ndp_global *pdata, uint8_t subtype);

//增加接口控制信息
int ftm_ndpif_add(struct ftm_ifm *pifm);
int ftm_ndpif_delete(struct ftm_ifm *pifm);

//更新接口控制信息
int ftm_ndpif_update(void *pdata, uint32_t ifindex, uint8_t subtype);
void ftm_ndpif_addr_delete(struct ftm_ifm *pifm, uint8_t ip_flag);
int ftm_ndpif_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t index);
void ftm_ndp_init(void);
int ndp_status_timer();
void ndp_probe_fast(struct ndp_neighbor *ndp);
int ndp_fake_entry_age();

#endif

