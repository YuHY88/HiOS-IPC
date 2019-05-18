/**
 * @file      : route_register.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 16:49:25
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_L3IF_REGISTER_H
#define HIOS_L3IF_REGISTER_H

#include <lib/ifm_common.h>
#include "route.h"

struct l3if_register
{
    int module_id;					/*the module id of register*/
    u_int32 ifindex ;		        /*the ifindexs specified by the module*/
};

extern int l3if_register_msg(int module_id, uint8_t subtype);
extern int l3if_register_msg_new(int module_id, uint8_t subtype ,u_int32 ifindex);
extern int l3if_unregister_msg(int module_id, uint8_t subtype);

extern void l3if_event_list_init(void);
extern int l3if_event_notify(enum IFNET_EVENT type, struct ifm_event *pevent);

extern int route_register_msg(int module_id, uint8_t subtype);
extern int route_unregister_msg(int module_id, uint8_t subtype);
extern void route_event_list_init(void);
extern int route_event_notify(enum ROUTE_PROTO type, struct route_entry *proute,
                                enum IPC_OPCODE opcode);
extern int route_event_frr_notify(enum ROUTE_PROTO type, struct route_entry *proute,
                                    enum IPC_OPCODE opcode);
extern int route_register_notify(int module_id, uint8_t subtype);

extern int routev6_register_msg(int module_id, uint8_t subtype);
extern int routev6_unregister_msg(int module_id, uint8_t subtype);
extern void routev6_event_list_init(void);
extern int routev6_event_notify(enum ROUTE_PROTO type, struct route_entry *proute,
                                enum IPC_OPCODE opcode);
extern int routev6_register_notify(int module_id, uint8_t subtype);

extern int route_event_register_notify(int module_id, enum ROUTE_PROTO type,
                                        struct route_entry *proute, int data_num);


#endif


