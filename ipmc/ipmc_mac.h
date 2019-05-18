/* 
*  multicast mac management 
*/
#ifndef IPMC_MAC_H
#define IPMC_MAC_H

#include <lib/types.h>
#include <lib/hash1.h>
#include "ipmc.h"


/* Hash of multicast mac table */
extern struct hash_table mc_mac_table;


/* multicast mac ��Ĳ������� */
void mc_mac_table_init(unsigned int size); /* �鲥 mac hash ���� */
int mc_mac_add(struct mcmac_t *pmac);         /* ���һ���鲥 mac */
int mc_mac_delete(struct mcmac_t *pmac);     /* ɾ��һ���鲥 mac */
struct mcmac_t *mc_mac_lookup(struct mcmac_t *pmac); /* ����һ���鲥 mac ��*/


#endif
