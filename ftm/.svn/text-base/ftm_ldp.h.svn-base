#ifndef HIOS_FTM_LDP_H
#define HIOS_FTM_LDP_H


extern struct hash_table ldp_frr_table;

#define FTM_LDP_FRR_GROUP_MAX               1024




void ftm_ldp_frr_table_init(unsigned int size);
int ftm_ldp_frr_send_hal(struct ldp_frr_t *pldp_frr, int opcode);
int ftm_ldp_frr_add(struct ldp_frr_t *pldp_frr);
int ftm_ldp_frr_del(struct ldp_frr_t *pldp_frr);
struct ldp_frr_t * ftm_ldp_frr_lookup(uint32_t frr_index);
int ftm_ldp_frr_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                        enum IPC_OPCODE opcode);

#endif

