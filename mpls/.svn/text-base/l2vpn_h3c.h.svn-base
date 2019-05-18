/**
 * @file      : l2vpn_h3c.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年4月27日 9:49:35
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_L2VPN_H3C_H
#define HIOS_L2VPN_H3C_H


#include <lib/hash1.h>
#include <lib/types.h>
#include <lib/mpls_common.h>

extern struct hash_table pw_class_table;
extern struct hash_table xc_group_table;
extern struct hash_table service_instance_table;





extern void pw_class_table_init(uint32_t size);
extern void pw_class_free(struct pw_class *pclass);
extern int pw_class_add(struct pw_class *pclass);
extern int pw_class_delete(uchar *pname);
extern struct pw_class *pw_class_create(void);
extern struct pw_class *pw_class_lookup(uchar *pname);

extern void connection_free(struct cont_entry *pcont);
extern int connection_add(struct list *plist, struct cont_entry *pcont);
extern int connection_delete(struct list *plist, struct cont_entry *pcont);
extern int connection_delete_uni(struct cont_entry *pcont);
extern int connection_delete_nni_master(struct cont_entry *pcont);
extern int connection_delete_nni_backup(struct cont_entry *pcont);
extern struct cont_entry *connection_create(void);
extern struct cont_entry *connection_lookup(struct list *plist, uchar *pname);

extern void xc_group_table_init(uint32_t size);
extern void xc_group_free(struct xc_group *pgroup);
extern int xc_group_add(struct xc_group *pgroup);
extern int xc_group_delete(uchar *pname);
extern int xc_group_delete_connection(struct list *plist);
extern struct xc_group *xc_group_create(void);
extern struct xc_group *xc_group_lookup(uchar *pname);


extern void service_instance_table_init(uint32_t size);
extern void service_instance_free(struct serv_ins *psi);
extern int pwid_check(uint32_t pwid);
extern int service_instance_add(struct serv_ins *psi);
extern int service_instance_delete(uint32_t ifindex);
extern struct serv_ins *service_instance_create(void);
extern struct serv_ins *service_instance_lookup(uint32_t ifindex);

extern uint32_t l2vpn_get_service_instance_ifindex(uint32_t ifindex, uint16_t instance);
extern int l2vpn_check_service_instance_encap(uint32_t ifindex, enum encap_type type, uint16_t vid);

extern int l2vpn_add_master_pw(struct cont_entry *pcont);
extern int l2vpn_delete_master_pw(struct cont_entry *pcont);
extern int l2vpn_add_backup_pw(struct cont_entry *pcont);
extern int l2vpn_delete_backup_pw(struct cont_entry *pcont);


#endif


