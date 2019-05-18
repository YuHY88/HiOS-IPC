
#ifndef HIOS_LDP_NETFILTER_H
#define HIOS_LDP_NETFILTER_H


#define LDP_ROUTE_EGRESS_NODE_NO   0
#define LDP_ROUTE_EGRESS_NODE_YES  1

#define LDP_PREFIX_FILTER_NUM_MAX   1024

extern struct hash_table ldp_filter_tabel;

struct ldp_filter
{
    uint32_t            key;        /**/
    struct inet_prefix  prefix;     /**/
};

void ldp_filter_table_init(unsigned int size);
struct ldp_filter *ldp_filter_create(struct inet_prefix *prefix);
int ldp_filter_add(struct ldp_filter *pfilter);
int ldp_filter_del(struct ldp_filter *pfilter);
void ldp_filter_clear(void);
struct ldp_filter *ldp_filter_lookup(struct inet_prefix *prefix);

int is_ldp_route_egress_node(struct route_entry *proute);

void ldp_filter_config_write(struct vty *vty);

#endif



