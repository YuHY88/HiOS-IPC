
#ifndef HIOS_LDP_ADDR_H
#define HIOS_LDP_ADDR_H
#include <lib/ifm_common.h>

int ldp_send_addresses_maping(struct ldp_sess *psess);
int ldp_respond_ifaddr_event(uint32_t ifindex, struct ifm_event *pevent, uint32_t flag);
int ldp_send_address_maping(struct ldp_adjance *padjance, uint32_t ipaddr);
int ldp_send_address_withdraw(struct ldp_adjance *padjance, uint32_t ipaddr);
int ldp_peer_ifaddr_add(struct ldp_sess *psess, struct inet_addr *paddr);
int ldp_peer_ifaddr_del(struct ldp_sess *psess, struct inet_addr *paddr);
int ldp_addr_add_nhp(struct ldp_sess *psess, struct inet_addr *paddr);
int ldp_addr_del_nhp(struct ldp_sess *psess, struct inet_addr *paddr);
int ldp_recv_address_maping(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
int ldp_recv_address_withdraw(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
struct inet_addr *ldp_peer_ifaddr_lookup(struct ldp_sess *psess, struct inet_addr *paddr);
int ldp_peer_ifaddrs_clear(struct ldp_sess *psess);


#endif
