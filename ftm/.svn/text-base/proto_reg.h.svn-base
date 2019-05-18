
#ifndef HIOS_PROTO_H
#define HIOS_PROTO_H


#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>



#define proto_hash_size 1024


/* Hash of Ð­Òé×¢²á±í */
extern struct hash_table ip_proto_hash;   /* ip and udp protocol hash table */
extern struct hash_table eth_proto_hash;  /* eth protocol hash table */
extern struct hash_table mpls_proto_hash; /* mpls protocol hash table */


int ip_proto_delete(struct ip_proto *proto);
int eth_proto_delete(struct eth_proto *proto);
int mpls_proto_delete(struct mpls_proto *proto);

int ip_proto_lookup(struct ip_proto *proto);

int eth_proto_lookup(struct eth_proto *proto);

int mpls_proto_lookup(struct mpls_proto *proto);

void proto_hash_init(void);

extern int ftm_proto_msg(void *pdata, int module_id, uint8_t subtype, enum IPC_OPCODE opcode);

#endif
















