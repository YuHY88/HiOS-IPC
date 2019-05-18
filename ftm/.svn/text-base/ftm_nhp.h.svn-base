/*
*       manage the nhp table
*
*/


#ifndef HIOS_FTM_NHP_H
#define HIOS_FTM_NHP_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/route_com.h> 
#include "route/ndp.h"
#include "ftm_arp.h"
#include "ftm_ndp.h"

/* traverse nhp list time 3s */
#define TRAVERSE_NHP_LIST_TIME  3

/* Hash of nhp table */
struct hash_table nhp_table;
struct hash_table ecmp_table;
struct hash_table nexthop_table;


/* Prototypes. */
void ftm_nhp_table_init(unsigned int size);
int ftm_nhp_add(struct nhp_entry *nhp);
int ftm_nhp_update(struct nhp_entry *nhp);
int ftm_nhp_delete(uint32_t nhp_index);
struct nhp_entry *ftm_nhp_lookup(uint32_t nhp_index);
int ftm_ecmp_update_nhp(struct nhp_entry *pnhp);
int ftm_nhp_update_arp(struct arp_entry *parp, int opcode);
int ftm_nhp_update_ndp(struct ndp_neighbor *pndp, int opcode);

void ftm_ecmp_table_init(unsigned int size);
int ftm_ecmp_add(struct ecmp_group *ecmp);
int ftm_ecmp_delete(uint32_t ecmp_index);
struct ecmp_group *ftm_ecmp_lookup(uint32_t ecmp_index);

void *nhp_lookup(uint32_t nhp_index, uint8_t nhp_type);
int ftm_nhp_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);
int ftm_ecmp_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode);

//int ftm_traverse_nhp_list(struct thread *thread);
int ftm_traverse_nhp_list(void *para);
void ftm_nhp_list_add_node(struct nhp_entry *pnhp);
void ftm_nhp_list_delete_node(struct nhp_entry *pnhp);

#endif
















