/*
 * IS-IS Rout(e)ing protocol - isis_zebra.h   
 *
 * Copyright (C) 2001,2002   Sampo Saaristo
 *                           Tampere University of Technology      
 *                           Institute of Communications Engineering
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public Licenseas published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
 * more details.

 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef _ZEBRA_ISIS_ZEBRA_H
#define _ZEBRA_ISIS_ZEBRA_H

#include "ifm_common.h"
//extern struct zclient *zclient;
extern struct isis_statistics isis_sta;

void isis_zebra_init (struct thread_master *);
void isis_zebra_route_update (struct prefix *prefix,struct isis_area *area,
			      struct isis_route_info *route_info);
int isis_distribute_list_update (int routetype);
void isis_zebra_redistribute_set(int type);
void isis_zebra_redistribute_unset(int type);
void isis_interface_if_set_value (struct ifm_event *pevent, struct interface *ifp, char updflag);
struct interface * ifm_interface_state_read (struct ifm_event *pevent, vrf_id_t vrf_id,u_int8_t stat_flag);
struct connected * ifm_interface_address_read (int type, struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_zebra_if_address_add (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_zebra_if_address_del (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_interface_change_mode (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_interface_mtu_change (struct ifm_event *pevent, vrf_id_t vrf_id);
void isis_zebra_route_del_ipv4 (struct prefix *prefix, struct isis_area *area,struct isis_route_info *route_info);
void isis_route_info_delete_old (struct isis_route_info *route_info);
int isis_zebra_route_del_ipv4_merge (struct prefix *prefix,struct isis_area *area,struct isis_route_info *route_info);
int isis_zebra_route_del_ipv6_merge (struct prefix *prefix,struct isis_area *area,struct isis_route_info *route_info);
void isis_zebra_route_del_ipv6 (struct prefix *prefix,struct isis_area *area,struct isis_route_info *route_info);
void isis_route_info_delete_ipv6_old (struct isis_route_info *route_info);

#endif /* _ZEBRA_ISIS_ZEBRA_H */
