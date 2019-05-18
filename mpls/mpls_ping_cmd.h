/*
*       manage the static ping table
*
*/

#ifndef MPLS_PING_CMD_H
#define MPLS_PING_CMD_H


uint32_t ping_select_ldp_lsp(struct inet_prefix *destprefix, struct inet_addr *nhpaddr);
void mpls_ping_CmdInit(void);
struct lsp_entry * lookup_lsp_by_tunnel_index(uint32_t lsp_index);
#endif


