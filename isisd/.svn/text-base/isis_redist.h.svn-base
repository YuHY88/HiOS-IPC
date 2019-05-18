/*
 * IS-IS Rout(e)ing protocol - isis_redist.h
 *
 * Copyright (C) 2013-2015 Christian Franke <chris@opensourcerouting.org>
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
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

#ifndef ISIS_REDIST_H
#define ISIS_REDIST_H
#include "route_com.h"

#define REDIST_PROTOCOL_COUNT 2

#define DEFAULT_ROUTE 14
#define DEFAULT_ORIGINATE 1
#define DEFAULT_ORIGINATE_ALWAYS 2


/* default value is ext */
#define REDIST_EXT 0
#define REDIST_INT 1

struct isis_ext_info
{
  uint32_t metric;
  u_char internal_external_flag;
  struct list *rds_nhp;
};

struct isis_redist_nh
{
	uint8_t origin;
	uint32_t metric;
	u_char distance;
	uint8_t instance;
	struct in_addr nh;
	struct in6_addr nh6;
};

struct isis_redist
{
  int redist;
  uint32_t metric;
  enum ROUTE_PROTO protocol;
  uint8_t instance;
  int level;
  uchar internal_external_flag;
  char *map_name;
  struct route_map *map;
};

struct isis_area;
struct prefix;

struct route_table *get_ext_reach(struct isis_area *area,
                                  int family, int level);
void
isis_redist_add(int type, struct prefix *p, u_char distance, uint32_t metric, 
						u_int8_t instance, uint16_t vpn, void * data);

void
isis_redist_delete(int type, struct prefix *p, u_char distance, uint32_t metric, 
				uint8_t instance, uint16_t vpn, void * data);

int isis_redist_config_write(struct vty *vty, struct isis_area *area,int family);
void isis_redist_init(void);
void isis_redist_area_finish(struct isis_area *area);
int redist_type_check(int type, int level, int family);
int proto_redistnum_isis(int afi, const char *s);
int isis_write_to_config(struct vty *vty, struct list *redist, int family,struct isis_area * area);
struct isis_redist_nh * redist_info_create(int type, u_char distance, uint32_t metric, 
		u_int8_t instance, void* nh, struct isis_redist *redist, int family);
int rds_lookup(struct list * rds_list, struct isis_redist_nh *nh_info, int family);
int rds_delete(struct list * rds_list, struct isis_redist_nh *nh_info, int family);
void isis_redist_info_add_to_table(int type, struct prefix *p, u_char distance, uint32_t metric, 
		u_int8_t instance, void* nh, struct isis_redist *redist, struct isis_area *area, int level);
void isis_redist_info_del_from_table(int type, struct prefix *p, u_char distance, uint32_t metric, 
		u_int8_t instance, void * nh, struct isis_area *area, int level);
void isis_redist_metric_update(struct isis_area *area, int level,
                  int family, struct isis_redist *redist);
void route_redist_delete(struct route_table *table);
void isis_redist_set(struct isis_area *area, int level,int family, int type, uint32_t metric,uint8_t  instance,int originate_type,uchar internal_external_flag);
void isis_redist_unset(struct isis_area *area, int level,int family, int type,uint8_t instance,struct vty *vty);


#endif
