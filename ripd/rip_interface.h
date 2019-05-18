/* RIP interface routines
 *
 * This file is part of Quagga
 *
 * Quagga is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * Quagga is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Quagga; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef _QUAGGA_RIP_INTERFACE_H
#define _QUAGGA_RIP_INTERFACE_H


#include "ifm_common.h"

#define INTERFACE_NAMSIZE   32

extern char *inet_ntoa(struct in_addr);
extern int inet_aton(const char *, struct in_addr *);
extern int rs_down_to_null(void *t);
extern void rip_inter_if_set_value(uint32_t , struct interface *, vrf_id_t);
extern void version_change_all_interface(struct rip_instance *, int, int);
extern void rip_enable_apply_all(struct rip_instance *rip);
extern int rip_interface_down(struct ifm_event *pevent, vrf_id_t vrf_id);
extern int rip_interface_up(struct ifm_event *pevent, vrf_id_t vrf_id);
extern int rip_interface_change_mode(struct ifm_event *pevent, vrf_id_t vrf_id);
extern int rip_interface_delete(struct ifm_event *pevent, vrf_id_t vrf_id);
extern int rip_interface_address_add(struct ifm_event *pevent, vrf_id_t vrf_id);
extern int rip_interface_address_delete(struct ifm_event *pevent, vrf_id_t vrf_id);
extern void rip_summary_free(void *val);
extern struct rip_info *rip_add_new(const struct prefix *p, struct rip_instance *rip, int flag);
extern struct listnode *ri_summary_prefix_lookup(struct list *list, struct prefix *p);
extern void rip_summary_subprefix_flagset(struct route_node *rp, int sumtype, int opflag, u_int32_t *ifmt);

struct rip_summary* rip_summary_new(void);
void rip_neighbor_free(void *val);
int ri_summary_prefix_cmp(struct prefix *p1, struct prefix *p2);
int summary_only_route_delete(struct rip_summary *rs);
struct rip_interface *rip_interface_new(void);
void rip_request_interface_send(struct interface *ifp, u_char version);
void rip_request_interface(struct interface *ifp);
void rip_request_neighbor(struct interface *ifp, struct rip_instance *rip);
int rip_version_change_table_check(struct rip_instance *rip, int pver, int nver, uint32_t ifindex);
int rip_if_ipv4_address_check(struct interface *ifp);
void ifm_ipc_get_prefix(struct ifm_event *pevent, struct prefix *p);
void rip_interface_if_set_value(struct ifm_event *pevent, struct interface *ifp);
struct interface *rip_find_interface(struct vty *vty, uint32_t ifindex);
struct connected *rip_ifm_interface_address_read(int type, struct ifm_event *pevent, vrf_id_t vrf_id);
struct interface *rip_ifm_interface_state_read(struct ifm_event *pevent, vrf_id_t vrf_id);
int rip_if_down(struct interface *ifp, int flag);
int rip_route_ifaddress_del(struct connected *ifc, struct rip_instance *rip);
void rip_apply_address_del(struct connected *ifc, struct rip_instance *rip);
void rip_connect_set(struct interface *ifp, int set, struct rip_instance *rip);
int rip_neighbor_add(struct interface *ifp, struct prefix *p);
int rip_neighbor_delete(struct interface *ifp, struct prefix *p);
int rs_up_to_null(void *t);
void rip_authentication_type(struct rip_interface *ri, char *type);
void show_rip_interface_brief(struct vty *vty, struct interface *ifp, struct rip_instance *rip);
void show_ip_rip_interface_sub(struct vty *vty, struct interface *ifp, struct rip_instance *rip);
int rip_interface_config_write(struct vty *vty);
int ripng_if_ipv6_lladdress_check(struct interface *ifp);
int ripng_route_ifaddress_del(struct connected *ifc, struct ripng_instance *ripng);
void ripng_apply_address_del(struct connected *ifc, struct ripng_instance *ripng);
void show_ripng_interface_sub(struct vty *vty, struct interface *ifp);
int rip_interface_new_hook(struct interface *ifp);
int rip_interface_delete_hook(struct interface *ifp);
void rip_l3if_init(void);
void rip_enable_apply(struct interface *ifp, struct rip_instance *rip);



#endif /* _QUAGGA_RIP_INTERFACE_H */
