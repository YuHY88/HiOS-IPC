
#ifndef HIOS_LDP_FEC_H
#define HIOS_LDP_FEC_H


#define LDP_SESS_FEC_ACTIVE     1
#define LDP_SESS_FEC_INACTIVE   0


struct ldp_fec_t
{
    struct inet_prefix  prefix;            /* IP prefix of the fec */
    uint16_t            nhp_cnt;           /* Nhp counter of the fec learing from the session for ecmp and frr in route */
    uint32_t            outlabel;          /* Outlabel learing from the session ,used for the nhp if route becomes active */
};

int ldp_respond_route_event(struct route_entry *proute, uint32_t opcode);
struct ldp_fec_t *ldp_fec_create(uint32_t iptype, uint32_t ipaddr, uint32_t prefixlen);
struct ldp_fec_t *ldp_fec_add(struct ldp_sess *psess, struct inet_prefix  *prefix, uint32_t outlabel);
int ldp_fec_del(struct ldp_sess *psess, struct ldp_fec_t *pldp_fec);
struct ldp_fec_t *ldp_fec_lookup(struct ldp_sess *psess, struct inet_prefix *prefix);
int ldp_fec_clear(struct ldp_sess *psess);
int ldp_fec_outlabel_update(struct ldp_sess *psess, struct ldp_fec_t * pldp_fec, uint32_t outlabel);
int ldp_fec_flag_update(struct ldp_sess *psess, struct ldp_fec_t * pldp_fec, uint16_t flag);


#endif
