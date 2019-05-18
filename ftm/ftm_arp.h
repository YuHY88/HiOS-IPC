/* 动态 arp 的定义 */
#ifndef HIOS_FTM_ARP_H
#define HIOS_FTM_ARP_H


#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/msg_ipc_n.h>

#include <route/arp.h>

#include "ftm_ifm.h"
#include "pkt_arp.h"


#define ARP_TO_STATIC_DIS 0
#define ARP_TO_STATIC_PART 1
#define ARP_TO_STATIC_ALL 2

#define ARP_MASTER        0
#define ARP_SLAVE         1


#define DEV_BOARD_INFO_GET_FAIL 0
#define DEV_BOARD_IS_MASTER     1
#define DEV_BOARD_IS_SLAVE      2

#define DEV_BOARD_IN_PLACE      1
#define DEV_BOARD_OUT_PLACE     2

#define ARP_INDEX_BASE  10000 /* ARP 索引从 10000 开始 */

#define FTM_ARP_ZLOG_DEBUG(format,...)\
    zlog_debug(FTM_DBG_ARP, "%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

#define FTM_ARP_ZLOG_ERR(format,...)\
    zlog_err("%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);


#define GSTATIC_ARP_LT_GLIMIT (garp.num_static < ARP_STATIC_NUM)
#define GSTATIC_ARP_EQ_GLIMIT (garp.num_static == ARP_STATIC_NUM)

#define GARP_NUM_LT_GLIMIT (garp.count + garp.num_static < ARP_TOTAL_NUM)
#define GARP_NUM_EQ_GLIMIT (garp.count + garp.num_static == ARP_TOTAL_NUM)

#define GDYNAMIC_ARP_LT_GLIMIT (garp.count < garp.num_limit)
#define GDYNAMIC_ARP_EQ_GLIMT (garp.count == garp.num_limit)

#define PIF_DYNAMIC_ARP_LT_PLIMIT(pifm) (pifm->parp->arp_num < pifm->parp->arp_num_max)
#define PIF_DYNAMIC_ARP_EQ_PLIMIT(pifm) (pifm->parp->arp_num == pifm->parp->arp_num_max)

#define PIF_DYNAMIC_ARP_LT_GLIMIT(pifm) (pifm->parp->arp_num < garp.num_limit)
#define PIF_DYNAMIC_ARP_EQ_GLIMIT(pifm) (pifm->parp->arp_num == garp.num_limit)

#define SLOT_DYNAMIC_ARP_LT_SLIMIT(ifindex) (garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][1] < garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][0])

#define SLOT_DYNAMIC_ARP_EQ_SLIMIT(ifindex) (garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][1] == garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][0])

#define FTM_ARP_DYNAMIC_BOARD_SYNC(parp, opcode)\
		if (DEV_TYPE_SHELF == ftm_devm.devm_type)\
		{\
	        if (DEV_BOARD_IS_MASTER == ftm_devm.ms_status)\
	        {\
	        	if (DEV_BOARD_IN_PLACE == ftm_devm.slave_in)\
	        	{\
		            FTM_ARP_ZLOG_DEBUG();\
			        ret = ipc_send_msg_ha_n2(parp, sizeof(struct arp_entry), 1, MODULE_ID_FTM, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, opcode, 0, 0);\
		            if (ret < 0)\
		            {\
		                FTM_ARP_ZLOG_ERR("ipc_send ha arp entry error!opcode:%d ip:0x%x vpn:%d\n", opcode, parp->key.ipaddr, parp->key.vpnid);\
		            }\
	        	}\
	        }\
		}
#define SLOT_DYNAMIC_ARP_CNT_INC(ifindex)\
    if(IFM_SLOT_ID_GET(ifindex) < SLOT_MAX + 1)\
    {\
        garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][1] ++;\
    }

#define SLOT_DYNAMIC_ARP_CNT_DEC(ifindex)\
    if((IFM_SLOT_ID_GET(ifindex) < SLOT_MAX + 1) && (garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][1] > 0))\
    {\
        garp.slot_num_limit[IFM_SLOT_ID_GET(ifindex)][1] --;\
    }

#define FTM_ARPIF_CTRL_STRUCT_INIT(pifm)\
	pifm->parp->arp_num_max = ARP_TOTAL_NUM;\
    pifm->parp->ifindex = pifm->ifm.ifindex;\
    pifm->parp->arp_disable = 1;\
    pifm->parp->arp_proxy = 0;\
    pifm->parp->arp_num = 0;

#define FTM_ARPIF_CTRL_STRUCT_CLR(pifm)\
	pifm->parp->arp_num_max = 0;\
	pifm->parp->arp_disable = 0;\
	pifm->parp->arp_proxy = 0;\
    pifm->parp->arp_num = 0;
    



extern struct hash_table arp_table;
extern struct arp_global garp;
extern struct arp_to_static garp_to_static_list;

struct arp_entry * arp_lookup(uint32_t uiIpaddr, uint16_t usVpn);
struct arp_entry * arp_lookup_active(uint32_t ipaddr, uint16_t vpn);
void arp_add_entry(struct arp_entry *pstArp, int ms_flag);
int  arp_add(struct arp_entry *pstArp, int ms_flag);
int  arp_update_info(struct arp_entry *parp_new, struct arp_entry *parp_old);
int  arp_update(struct arp_entry *parp_new, struct arp_entry *parp_old);
int  arp_delete_entry(struct arp_entry *pstArp,struct hash_bucket *pstItem, int ms_flag);
int  arp_delete(uint32_t uiIpaddr, uint16_t usVpn, int ms_flag);
int  arp_delete_one(void);
int  arp_delete_all(void);
int  arp_delete_interface(uint32_t ifindex);
int  arp_delete_ip(uint32_t ipaddr);
int  arp_delete_vpn(uint16_t vpnid);
int  arp_delete_if_linkdown(uint32_t ifindex);
void arp_delete_if_proxy_disable(struct ftm_ifm *pifm);
void arp_table_init(unsigned int uiSize);
void arp_fake_entry_time_flush(uint8_t time);


void ftm_arp_init(void);
int ftm_arp_msg(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode, sint32 sender_id);
//int ftm_arp_msg_mib_num(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode, uint16_t sender_id);
int ftm_arp_devm_msg(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode);
int ftm_arp_global_add(void *pdata, uint8_t subtype);
int ftm_arpif_add(struct ftm_ifm *pifm);
int ftm_arpif_delete(struct ftm_ifm *pifm);
int ftm_arpif_update(void *pdata, uint32_t ifindex, uint8_t subtype);
int ftm_arpif_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t index);
int ftm_arpif_addr_delete(struct ftm_ifm *pifm,uint8_t ip_flag);
void arp_delete_vlanif_port_down(uint32_t port);
void ftm_arp_reset_slot(uint8_t slot_num);
void ftm_arp_slot_numlimit(uint32_t data);

int ftm_arp_rcv_ha(struct ipc_mesg_n *pmesg);
int ftm_arp_dynamic_board_sync(void);
void ftm_arp_dynamic_sync_restore(struct ipc_mesg_n *pmesg);
int8_t ftm_arp_cur_board_is_backup(void);
int ftm_arp_get_statis(struct ftm_arp_count *arp_statis);

#endif
