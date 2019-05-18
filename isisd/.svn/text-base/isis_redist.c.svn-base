/*
 * IS-IS Rout(e)ing protocol - isis_redist.c
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

#include <zebra.h>

#include "command.h"
#include "if.h"
#include "linklist.h"
#include "memory.h"
#include "memtypes.h"
#include "prefix.h"
#include "routemap.h"
#include "stream.h"
#include "table.h"
#include "vty.h"
#include "module_id.h"
#include "log.h"
#include "isisd/dict.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_misc.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_tlv.h"
#include "isisd/isisd.h"
#include "isisd/isis_lsp.h"
#include "isisd/isis_route.h"
#include "isisd/isis_zebra.h"

#define ISIS_REDIST_STR_ISISD \
  "(static|connect|ibgp|ebgp)"

#define ISISV6_REDIST_STR_ISISD \
  "(static|connect|ibgp6|ebgp6)"
  

#define ISIS_REDIST_HELP_STR_ISISD \
  "static routes \n" \
  "connected routes (directly attached subnet or host)\n" \
  "ibgp route\n" \
  "ebgp route\n" 

#define ISIS_REDIST_STR_ISISD_WITH_INSTANCE \
  "(ospf|isis|rip)"

#define ISISV6_REDIST_STR_ISISD_WITH_INSTANCE \
  "(ospfv3|isisv6|ripng)"

#define ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE \
  "ospf route\n" \
  "isis route\n" \
  "rip route\n" \

static int
redist_protocol(int family)
{
  if (family == AF_INET)
    return 0;
  else
    return 1;
}

static int
is_default(struct prefix *p)
{
  if (p->family == AF_INET)
    if (p->u.prefix4.s_addr == 0 && p->prefixlen == 0)
      return 1;

  if (p->family == AF_INET6)
    if (IN6_IS_ADDR_UNSPECIFIED(&p->u.prefix6) && p->prefixlen == 0)
      return 1;

  return 0;
}

#if 0
static struct route_table*
get_ext_info(struct isis *i, int family)
{
  int protocol = redist_protocol(family);

  return i->ext_info[protocol];
}

static struct isis_redist*
get_redist_settings(struct isis_area *area, int family, int type, int level)
{
  int protocol = redist_protocol(family);

  //return &area->redist_settings[protocol][type][level-1];
  return &area->redist_settings[protocol][level-1];
}
#endif

static struct isis_redist*
get_redist_settings(struct isis_area *area, int type,  u_int8_t instance, int family)
{
	struct listnode *node;
	struct isis_redist *redist_set;

    if(family == AF_INET)
    {
	  if(listcount(area->redist_settings) == 0)
		return NULL;
		
	  for (ALL_LIST_ELEMENTS_RO(area->redist_settings, node, redist_set))
	  {	
	    if((int)redist_set->protocol == type && redist_set->instance == instance)
	    {
	      return redist_set; 
	    }
	  }
	  return NULL;
	}

	if(family == AF_INET6)
    {
	  if(listcount(area->redist_settings_v6) == 0)
		return NULL;
		
	  for (ALL_LIST_ELEMENTS_RO(area->redist_settings_v6, node, redist_set))
	  {	
	    if((int)redist_set->protocol == type && redist_set->instance == instance)
	    {
	      return redist_set; 
	    }
	  }
	  return NULL;
	}
	return NULL;
}

struct route_table*
get_ext_reach(struct isis_area *area, int family, int level)
{
  int protocol = redist_protocol(family);

  return area->ext_reach[protocol][level-1];
}

static struct route_node *
isis_redist_route_node_create(route_table_delegate_t *delegate,
                              struct route_table *table)
{
  struct route_node *node;
  node = XCALLOC(MTYPE_ROUTE_NODE, sizeof(*node));
  return node;
}

static void
isis_redist_route_node_destroy(route_table_delegate_t *delegate,
                               struct route_table *table,
                               struct route_node *node)
{
  if (node->info)
  {
    //info = node->info;
    //list_delete(info->rds_nhp);
    XFREE(MTYPE_ISIS, node->info);
  }
  XFREE (MTYPE_ROUTE_NODE, node);
}

static route_table_delegate_t isis_redist_rt_delegate = {
  .create_node = isis_redist_route_node_create,
  .destroy_node = isis_redist_route_node_destroy
};

struct isis_redist_nh * redist_info_create(int type, u_char distance, uint32_t metric, 
		u_int8_t instance, void* nh, struct isis_redist *redist, int family)
{
	struct isis_redist_nh * r_nh;

	r_nh = XMALLOC(MTYPE_ISIS, sizeof(*r_nh));
	r_nh->origin = type;
	r_nh->distance = distance;
	r_nh->instance = instance;
	r_nh->metric = metric;
	if(family == AF_INET)
	  r_nh->nh = *((struct in_addr *) nh);
	if(family == AF_INET6)
	  r_nh->nh6 = *((struct in6_addr *) nh);

	return r_nh;
}

int rds_lookup(struct list * rds_list, struct isis_redist_nh *nh_info, int family)
{
	struct listnode *node;
	struct isis_redist_nh *rnh;
	for(ALL_LIST_ELEMENTS_RO(rds_list, node, rnh))
	{
		if(family == AF_INET)
		{
		  if(memcmp(&rnh->nh, &nh_info->nh, sizeof(struct in_addr)) == 0)
			return 1;
		}

		if(family == AF_INET6)
		{
		  if(memcmp(&rnh->nh6, &nh_info->nh6, sizeof(struct in6_addr)) == 0)
			return 1;	
		}
	}
	return 0;
}

int rds_delete(struct list * rds_list, struct isis_redist_nh *nh_info, int family)
{
	struct listnode *node, *nnode;
	struct isis_redist_nh *rnh;

	for(ALL_LIST_ELEMENTS(rds_list, node, nnode, rnh))
	{
        #if 0
		if(nh->distance == nh_info->distance
		    &&nh->instance == nh_info->instance
		    &&nh->metric == nh_info->metric
		    &&nh->origin == nh_info->origin
		    &&memcmp(&nh->nh, &nh_info->nh, sizeof(struct in_addr)) == 0)
		#endif
		if(family == AF_INET)
		{
		  if(memcmp(&rnh->nh, &nh_info->nh, sizeof(struct in_addr)) == 0)
		  {
			XFREE(MTYPE_ISIS,rnh);
			list_delete_node(rds_list,node);
			return 1;
		  }
		}

		if(family == AF_INET6)
		{
		  if(memcmp(&rnh->nh6, &nh_info->nh6, sizeof(struct in6_addr)) == 0)
		  {
			XFREE(MTYPE_ISIS,rnh);
			list_delete_node(rds_list,node);
			return 1;
		  }
		}
	}
	return 0;
}

void 
isis_redist_info_add_to_table(int type, struct prefix *p, u_char distance, uint32_t metric, 
		u_int8_t instance,void* nh, struct isis_redist *redist, struct isis_area *area, int level)
{
	int family;
	struct route_table *er_table;
	struct route_node *er_node;
	struct isis_ext_info *new_info, *old_info;
	struct isis_redist_nh *r_nh;
	struct isis_redist_nh nh_info;
	
    int flag = 0;

    family = p->family;
	er_table = get_ext_reach(area, family, level);
    
	er_node = route_node_get(er_table, p);

	if(er_node->info)
    {
       route_unlock_node(er_node);

       old_info = er_node->info;

       nh_info.distance = distance;
       nh_info.instance = instance;
       nh_info.metric = metric;

       if(family == AF_INET)
         nh_info.nh = *((struct in_addr *)nh);

       if(family == AF_INET6)
         nh_info.nh6 = *((struct in6_addr *)nh);
       nh_info.origin = type;

	   if(old_info->internal_external_flag != redist->internal_external_flag)
	   {
	   		old_info->internal_external_flag = redist->internal_external_flag;
			flag = 1;
	   }
	   
       if(rds_lookup(old_info->rds_nhp, &nh_info ,family) == 1)
       {
       	   if(flag != 1)
           	return;  
       }
       else
       { 
       		r_nh = redist_info_create(type, distance, metric, instance, nh, redist, family);
       		listnode_add(old_info->rds_nhp, r_nh);
       		if ( redist->metric != 0xffffffff && redist->metric < old_info->metric )
       		{
				old_info->metric = redist->metric;
       			flag = 1;
       		}
       }
    }
    else
    {
    	new_info = XMALLOC(MTYPE_ISIS, sizeof(*new_info));
    	new_info->rds_nhp = list_new();
    	r_nh = redist_info_create(type, distance, metric, instance, nh, redist, family);    	
    	listnode_add(new_info->rds_nhp, r_nh);
    	if (redist->metric != 0xffffffff)
       	{
			new_info->metric = redist->metric;
       	}
       	er_node->info = new_info;
		new_info->internal_external_flag = redist->internal_external_flag;
    	flag = 1;
    }

    if(flag == 1)
    {
    	lsp_regenerate_schedule(area, level, 0);
    }
}

void 
isis_redist_info_del_from_table(int type, struct prefix *p, u_char distance, uint32_t metric, 
		u_int8_t instance, void * nh, struct isis_area *area, int level)
{
	int family;
	struct route_table *er_table;
	struct route_node *er_node;
	struct isis_ext_info  *old_info;
	struct isis_redist_nh *r_nh;
	struct isis_redist_nh nh_info;
	struct listnode *node;
    int flag = 0;
    struct isis_redist *redist;

	family = p->family;
	er_table = get_ext_reach(area, family, level);
	
	er_node = route_node_lookup(er_table, p);

	if(!er_node)
		return;

	route_unlock_node(er_node);
	
	if(!er_node->info)
	    return;

    old_info = er_node->info;

	nh_info.distance = distance;
	nh_info.instance = instance;
	nh_info.metric = metric;
	if(family == AF_INET)
	  nh_info.nh = *((struct in_addr *)nh);
	if(family == AF_INET6)
	  nh_info.nh6 = *((struct in6_addr *)nh);
	nh_info.origin = type;

	if(rds_delete(old_info->rds_nhp, &nh_info, family) == 0)
	{
		return;
	}

    if(listcount(old_info->rds_nhp) == 0)
    {
        list_delete(old_info->rds_nhp);
		old_info->rds_nhp =NULL;
        XFREE(MTYPE_ISIS,er_node->info);
        er_node->info = NULL;      
    	route_unlock_node(er_node);
    	flag = 1;
    }
    else
    {
    	for(ALL_LIST_ELEMENTS_RO(old_info->rds_nhp, node, r_nh))
    	{
    		redist = get_redist_settings(area, r_nh->origin, r_nh->instance, family);
			
    		if(redist->metric != old_info->metric)
    		{
    			old_info->metric = redist->metric;
    			flag = 1;
    		}
    	}
    }
    

    if(flag == 1)
    {
    	lsp_regenerate_schedule(area, level, 0);
    }

}

void
isis_redist_add(int type, struct prefix *p, u_char distance, uint32_t metric, 
						u_int8_t instance, uint16_t vpn, void * nh)
{
	struct listnode *node;
	struct isis_area *area;
	int level;
	struct isis_redist *redist;

    if(p->family == AF_INET)
    {
	  if(!p->u.prefix4.s_addr)
	    return;
	}

	for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
	{ 
		redist = get_redist_settings(area, type, instance, p->family);
		if(redist == NULL)
			continue;

		if(area->vpn != vpn)
			continue;

		for (level = 1; level <= ISIS_LEVELS; level++)
		{
		    if(redist->level & level)
		    {
		    	isis_redist_info_add_to_table(type, p, distance, metric, instance, 
					nh, redist,area, level);
		    }
		} 
	}
}

void
isis_redist_delete(int type, struct prefix *p, u_char distance, uint32_t metric, 
				uint8_t instance, uint16_t vpn, void * nh)
{
  struct isis_area *area;
  struct listnode * node;
  struct isis_redist *redist;
  int level;
  
  if(p->family == AF_INET)
  {
    if(!p->u.prefix4.s_addr)
  	  return;
  }
  	
  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {   
      redist = get_redist_settings(area, type, instance, p->family);

      if(!redist)
          continue;

	  if(area->vpn != vpn)
	  	  continue;
	  
	  for (level = 1; level < 3; level++)
	  {
          if(redist->level & level)
              isis_redist_info_del_from_table(type, p, distance, metric, instance, 
                         nh, area, level);
      }   
  }
}

static int
isis_redist_check_and_register(int type ,int family)
{
  struct isis_area *area;
  struct listnode *anode, *node;
  struct isis_redist *redist_set;
  struct list *rlist = NULL;
  
  for (ALL_LIST_ELEMENTS_RO(isis->area_list, anode, area))
  {
      if(family == AF_INET)
      	rlist = area->redist_settings;
  	  if(family == AF_INET6)
      	rlist = area->redist_settings_v6;
	  
  	  if(rlist)
  	  {
          for (ALL_LIST_ELEMENTS_RO(rlist, node, redist_set))
          {
  	          if((int)redist_set->protocol == type)
  	          {
                  return 1;   
  		      }
          }
      }
  }
  
  return 0;
}

int 
redist_type_check(int type, int level, int family)
{
    struct isis_area *area;
    struct listnode *node, *anode;
    struct isis_redist *redist_set;
	
    for (ALL_LIST_ELEMENTS_RO(isis->area_list, anode, area))
    {		
      if(family == AF_INET)
      {
        for (ALL_LIST_ELEMENTS_RO(area->redist_settings, node, redist_set))
        {       
  	       if(redist_set->protocol == (u_int32)type)		
  		       return 1;	
        }  	
      }
      if(family == AF_INET6)
      {
        for (ALL_LIST_ELEMENTS_RO(area->redist_settings_v6, node, redist_set))
        {       
  	       if(redist_set->protocol == (u_int32)type)		
  		       return 1;	
        } 
      }

    }
    return 0;	
}

void isis_redist_metric_update(struct isis_area *area, int level,
                  int family, struct isis_redist *redist)
{
	struct route_table *er_table;
	struct route_node *rn;
	struct isis_ext_info *info;	
	struct listnode *node, *nnode;
    struct isis_redist_nh *r_nh;
    struct isis_redist *r_set;
    int lv = 1;
    uint32_t min_metric = 0xFFFFFFFF;
    int flag = 0;
	
	for (lv = 1; lv <= ISIS_LEVELS; lv++)
	{
		if((level & lv) == 0)
			continue;

	    er_table = get_ext_reach(area, family, lv);

	    if (!er_table)
			continue;
		for (rn = route_top(er_table); rn; rn = route_next(rn))
		{
	  		if (!rn->info)
				continue;
	  		info = rn->info;
			
			min_metric = 0xFFFFFFFF;
			flag = 0;
			
	  		if (redist->protocol == ROUTE_PROTO_MAX)
	  		{
				if (!is_default(&rn->p))
					continue;
	  		}
	  		else
	  		{
			    for(ALL_LIST_ELEMENTS(info->rds_nhp, node, nnode, r_nh))
                {                
          	        if(r_nh->origin != redist->protocol || (r_nh->instance != redist->instance))
          	        {
          	            r_set = get_redist_settings(area, r_nh->origin, r_nh->instance, family);
						if(r_set == NULL)
							continue;
          	            if(r_set->metric < min_metric)
          	                min_metric = r_set->metric;
          	        }
          	        else
          	        {
          	        	if(redist->metric < min_metric)
          	        		min_metric = redist->metric;
          	        	flag = 1;
          	        }
                }
	  		}
	  		if(!flag)
	  		    continue;
	  		else
	  		{ 
	  			info->metric = min_metric;
	  		}	  		
		}	
	}
}

static void isis_redist_table_delete(int del_level, struct isis_redist *redist, 
								struct isis_area *area, int family)
{
  struct route_table *er_table = get_ext_reach(area, family, del_level);
  struct route_node *rn;
  struct isis_ext_info *info;
  struct listnode *node, *nnode;
  struct isis_redist_nh *r_nh;
  struct isis_redist *r_set;

  if(!redist)
  	return;

  redist->redist = 0;

  if (!er_table)
  {
      return;
  }

  for (rn = route_top(er_table); rn; rn = route_next(rn))
  {
      if (!rn->info)
        continue;
      info = rn->info;

      if (redist->protocol == ROUTE_PROTO_MAX)
      {
          if (!is_default(&rn->p))
              continue;
      }
      else
      {
          for(ALL_LIST_ELEMENTS(info->rds_nhp, node, nnode, r_nh))
          {
          	  if(r_nh->origin != redist->protocol)
          	  {
          	      r_set = get_redist_settings(area,r_nh->origin,r_nh->instance, family);
          	      info->metric = r_set->metric;
          	      continue;
          	  }

			  XFREE(MTYPE_ISIS, r_nh);
          	  list_delete_node(info->rds_nhp, node);
          }

          if(listcount(info->rds_nhp) == 0)
          {
          	  list_delete(info->rds_nhp);
			  info->rds_nhp = NULL;
          	  XFREE(MTYPE_ISIS,rn->info);
          	  rn->info = NULL;
          	  route_unlock_node(rn);
          }
      }
  }
}

#if 0
static void isis_redist_table_add(int from, int to, struct isis_redist *redist,
								struct isis_area *area)
{
	struct route_table *er_table_from = get_ext_reach(area, AF_INET, from);
	struct route_table *er_table_to = get_ext_reach(area, AF_INET, to);
    struct route_node *rn, *rn_to;
	struct isis_ext_info *info, *info_to;
	
    if (!er_table_from || !er_table_to)
    {
        return;
    }

    for (rn = route_top(er_table_from); rn; rn = route_next(rn))
    {
        if (!rn->info)
            continue;
        info = rn->info;

        if (redist->protocol == ROUTE_PROTO_MAX)
        {
            if (!is_default(&rn->p))
                continue;
        }
        else
        {
            if (info->origin != redist->protocol)
                continue;

            if(info->instance != redist->instance)
          	    continue;

          	rn_to = route_node_get(er_table_to, &rn->p);

          	if(!rn_to->info)
          	{
          		info_to = XMALLOC(MTYPE_ISIS, sizeof(*info));
          		info_to->rds_nhp = list_new();
          		
          	}
          	else
          	{
          		memcpy(rn_to->info, info, sizeof(*info));
          	}
        }
    }
}
#endif

void
isis_redist_set(struct isis_area *area, int level,
                int family, int type, uint32_t metric,uint8_t  instance,int originate_type,
                uchar internal_external_flag)
{
  int protocol = redist_protocol(family);
  struct isis_redist *redist = get_redist_settings(area, type, instance, family);
  int i;
  int change_level;
  int flag = 0;

  if(level & IS_LEVEL_1)
  {
      if (!area->ext_reach[protocol][IS_LEVEL_1 - 1])
      {
          area->ext_reach[protocol][IS_LEVEL_1 - 1] =
             route_table_init_with_delegate(&isis_redist_rt_delegate);
      }
  }

  if(level & IS_LEVEL_2)
  {
  	  if (!area->ext_reach[protocol][IS_LEVEL_2 - 1])
      {
          area->ext_reach[protocol][IS_LEVEL_2 - 1] =
             route_table_init_with_delegate(&isis_redist_rt_delegate);
      }
  }

  for (i = 0; i < REDIST_PROTOCOL_COUNT; i++)
  {
    if (!area->isis->ext_info[i])
    {
        area->isis->ext_info[i] =
            route_table_init_with_delegate(&isis_redist_rt_delegate);
    }
  }

  if(!redist)
  {
  	  redist = XMALLOC(MTYPE_ISIS,sizeof(struct isis_redist));
	  if(!redist)
	  	return;

	  memset(redist , 0 , sizeof(struct isis_redist));
  	  redist->redist = (type == ROUTE_PROTO_MAX) ? originate_type : 1;
  	  redist->protocol = type;
  	  redist->instance = instance;
  	  redist->metric = metric;
  	  redist->level = level;
	  redist->internal_external_flag = internal_external_flag;

	  
	  if(family == AF_INET)
	  {
        listnode_add(area->redist_settings, redist);
		route_event_unregister(type,MODULE_ID_ISIS);
        route_event_register(type,MODULE_ID_ISIS);
      }

      if(family == AF_INET6)
	  {
	    routev6_event_unregister(type,MODULE_ID_ISIS);
	    routev6_event_register(type, MODULE_ID_ISIS);
        listnode_add(area->redist_settings_v6, redist);
      }
  }
  else
  {	
  	  if(level != redist->level)
      {
		redist->internal_external_flag = internal_external_flag;
		
		if(level == IS_LEVEL_1_AND_2)
		{
		    int old_level;
		    old_level = redist->level;
			redist->level = level;
			if(redist->metric != metric)
			{
			    redist->metric = metric;
				
				isis_redist_metric_update(area, old_level ,family, redist);
				lsp_regenerate_schedule(area, old_level, 0);
			}
			if(family == AF_INET)
			{
			  route_event_unregister(type,MODULE_ID_ISIS);
			  route_event_register(type,MODULE_ID_ISIS);
			}
			if(family == AF_INET6)
			{
			  routev6_event_unregister(type,MODULE_ID_ISIS);
			  routev6_event_register(type,MODULE_ID_ISIS);
			}
		}
		else
		{
		    if(redist->level == IS_LEVEL_1_AND_2)
			{
				change_level = redist->level - level;

				isis_redist_table_delete(change_level, redist, area, family);

				if(redist->metric != metric)
				{
					redist->metric = metric;
					
					isis_redist_metric_update(area, level ,family, redist);
					flag = 1;					    					    
				}

				redist->level = level;

				lsp_regenerate_schedule(area, change_level, 0);

				if(flag)
				{ 
				   lsp_regenerate_schedule(area, redist->level, 0);
                }                    
			}
		    else
			{
				isis_redist_table_delete(redist->level, redist, area, family);
				lsp_regenerate_schedule(area, redist->level, 0);

				redist->level = level;					
				redist->metric = metric;
				if(family == AF_INET)
				{
				  route_event_unregister(type,MODULE_ID_ISIS);
				  route_event_register(type,MODULE_ID_ISIS);
				}
				if(family == AF_INET6)
				{
				  routev6_event_unregister(type,MODULE_ID_ISIS);
			      routev6_event_register(type,MODULE_ID_ISIS);
				}
			}
		  }
		}
		else
		{
			if(area->oldmetric == 1)
			{
				if(redist->metric != metric || redist->internal_external_flag !=  internal_external_flag)
				{
				    redist->metric = metric;
					redist->internal_external_flag = internal_external_flag;

					isis_redist_metric_update(area, level ,family, redist);	

					/*internal and external should update the route_info*/
					if(family == AF_INET)
					{
					  route_event_unregister(type,MODULE_ID_ISIS);
					  route_event_register(type,MODULE_ID_ISIS);
					}
					if(family == AF_INET6)
					{
					  routev6_event_unregister(type,MODULE_ID_ISIS);
					  routev6_event_register(type,MODULE_ID_ISIS);
					}

					lsp_regenerate_schedule(area, level, 0);
				}
			}
			
			if(area->newmetric == 1)
			{
				if(redist->internal_external_flag != internal_external_flag)
				{
					redist->internal_external_flag = internal_external_flag;
				}

				if(redist->metric != metric)
				{
					redist->metric = metric;
					isis_redist_metric_update(area, level ,family, redist);	
					lsp_regenerate_schedule(area, level, 0);
				}
			}
		}	
  }
}

void
isis_redist_unset(struct isis_area *area, int level,
                  int family, int type,uint8_t instance,struct vty *vty)
{
  struct isis_redist *redist = get_redist_settings(area, type, instance, family);
  struct route_table *er_table = NULL;
  struct route_node *rn;
  struct isis_ext_info *info;
  struct listnode *node, *nnode;
  struct isis_redist_nh *r_nh;
  struct isis_redist *r_set;
  int lv = 1;
  int flag = 0;

  if(!redist)
  {
  	return;
  }

    if(redist->level != level)
    {
        zlog_debug(ISIS_DEBUG_TYPE_ROUTE,"%s Level dismatch!", __func__);
        return;
    }

  redist->redist = 0;
  
  for(lv = 1; lv < 3; lv++)
  {
  	  if((redist->level & lv) == 0)
  	       continue;

      er_table = get_ext_reach(area, family, lv);

        if (!er_table)
        {
            zlog_debug(ISIS_DEBUG_TYPE_ROUTE,"%s: External reachability table uninitialized.", __func__);
            continue;
        }

      for (rn = route_top(er_table); rn; rn = route_next(rn))
      {
          if (!rn->info)
            continue;

          info = rn->info;

          if (type == ROUTE_PROTO_MAX)
          {
              if (!is_default(&rn->p))
                  continue;
          }
          else
          {
              if(info->rds_nhp && listcount(info->rds_nhp) > 0)
              {
                for(ALL_LIST_ELEMENTS(info->rds_nhp, node, nnode, r_nh))
                {
          	      if(r_nh->origin != redist->protocol)
          	      {
          	          r_set = get_redist_settings(area,r_nh->origin,r_nh->instance, family);
          	          if(r_set == NULL)
          	          {
                                zlog_debug(ISIS_DEBUG_TYPE_ROUTE,"Redist table nexthop error!");
                                continue;
          	          }
          	          info->metric = r_set->metric;
          	          flag = 1;
          	          continue;
          	      }
          	      flag = 1;
                  XFREE(MTYPE_ISIS, r_nh);
          	      list_delete_node(info->rds_nhp, node);
                }

                if(listcount(info->rds_nhp) == 0)
                {
                  flag = 1;
          	      list_delete(info->rds_nhp);
				  info->rds_nhp = NULL;
          	      XFREE(MTYPE_ISIS,rn->info);
          	      rn->info = NULL;
          	      route_unlock_node(rn);
                }
              }
          }
      }
  }

  if(family == AF_INET)
    listnode_delete(area->redist_settings, redist);

  if(family == AF_INET6)
  	listnode_delete(area->redist_settings_v6, redist);

  if(flag)
      lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  if(!redist_type_check(type, level, family))
  {
    if(family == AF_INET)
  	  route_event_unregister(type,MODULE_ID_ISIS);
  	if(family == AF_INET6)
  	{
  	  routev6_event_unregister(type,MODULE_ID_ISIS);
  		/*IPV6 unregister*/
  	}
  }
}

void route_redist_delete(struct route_table *table)
{
	struct route_node *rn;
	struct isis_ext_info *info;
	struct listnode *node, *nnode;
	struct isis_redist_nh *r_nh;
	
	for (rn = route_top(table); rn; rn = route_next(rn))
	{
		if (!rn->info)
		  continue;
	
		info = rn->info;
		if(info->rds_nhp && listcount(info->rds_nhp) > 0)
		{
			for (ALL_LIST_ELEMENTS(info->rds_nhp, node, nnode, r_nh))
			{
				if (r_nh)
					XFREE(MTYPE_ISIS,r_nh);
			}
		}
		if (info->rds_nhp)
		{
			list_free(info->rds_nhp);
			info->rds_nhp = NULL;
		}		
	}
}

#if 1
void
isis_redist_area_finish(struct isis_area *area)
{
    int protocol[REDIST_PROTOCOL_COUNT][ROUTE_PROTO_NUM];
    struct listnode *node, *nnode;
    struct isis_redist *redist_set;
    int i = 0;
    
	if(listcount(area->redist_settings) > 0)
	{
	    for (ALL_LIST_ELEMENTS(area->redist_settings, node, nnode, redist_set))
	    {
	    	if(redist_set != NULL)
		    	protocol[0][redist_set->protocol] = 1;
		    if (redist_set)
			    XFREE(MTYPE_ISIS,redist_set);
	    }
	}

	if(listcount(area->redist_settings_v6) > 0)
	{
	    for (ALL_LIST_ELEMENTS(area->redist_settings_v6, node, nnode, redist_set))
	    {
	    	if(redist_set != NULL)
		   		protocol[1][redist_set->protocol] = 1;
		    if (redist_set)
			    XFREE(MTYPE_ISIS,redist_set);
	    }
	}

	if(area->ext_reach[0][IS_LEVEL_1 - 1])
	{
		route_redist_delete(area->ext_reach[0][IS_LEVEL_1 - 1]);
		route_table_finish(area->ext_reach[0][IS_LEVEL_1 - 1]);
		area->ext_reach[0][IS_LEVEL_1 - 1] = NULL;
	}
	if(area->ext_reach[0][IS_LEVEL_2 - 1])
	{
	    route_redist_delete(area->ext_reach[0][IS_LEVEL_2 - 1]);
		route_table_finish(area->ext_reach[0][IS_LEVEL_2 - 1]);
		area->ext_reach[0][IS_LEVEL_2 - 1] = NULL;
	}

	if(area->ext_reach[1][IS_LEVEL_1 - 1])
	{
		route_redist_delete(area->ext_reach[1][IS_LEVEL_1 - 1]);
		route_table_finish(area->ext_reach[1][IS_LEVEL_1 - 1]);
		area->ext_reach[1][IS_LEVEL_1 - 1] = NULL;
	}
	if(area->ext_reach[1][IS_LEVEL_2 - 1])
	{
	    route_redist_delete(area->ext_reach[1][IS_LEVEL_2 - 1]);
		route_table_finish(area->ext_reach[1][IS_LEVEL_2 - 1]);
		area->ext_reach[1][IS_LEVEL_2 - 1] = NULL;
	}

	if (area->redist_settings)
	{
    	list_free(area->redist_settings);
    	area->redist_settings = NULL;
	}
	
	if (area->redist_settings_v6)
	{
    	list_free(area->redist_settings_v6);
    	area->redist_settings_v6 = NULL;
	}

	for(i = 0; i < ROUTE_PROTO_NUM; i++)
	{
		if(protocol[0][i])
		{
			if(!isis_redist_check_and_register(i, AF_INET))
				route_event_unregister(i,MODULE_ID_ISIS);
		}
	}

	for(i = 0; i < ROUTE_PROTO_NUM; i++)
	{
		if(protocol[1][i])
		{
			if(!isis_redist_check_and_register(i, AF_INET6))
			{
			    routev6_event_unregister(i,MODULE_ID_ISIS);
			    /*ipv6 unregister*/
			}
		}
	}
}
#endif

DEFUN(isis_redistribute,
      isis_redistribute_cmd,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {metric <0-4261412864>}",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;

  if (argc < 3)
    return CMD_WARNING;

  family = str2family(argv[0]);

  if(family == AF_INET)
  {
    afi = AFI_IP;
  }
  else
  {
    afi = AFI_IP6;
  }

  type = proto_redistnum(afi, argv[1]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if (argv[3])
  {
      char *endp;
      metric = strtoul(argv[3], &endp, 10);
      if (argv[3][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }
  
  isis_redist_set(area, level, family, type, metric, 0, 0, REDIST_EXT);
  return 0;
}

DEFUN(isis_redistribute_h3c,
      isis_redistribute_h3c_cmd,
      "import-route" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {cost <0-4261412864>}",
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;

  family = str2family("ipv4");
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[0]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[1]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[1]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[1]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if (argv[2])
  {
      char *endp;
      metric = strtoul(argv[2], &endp, 10);
      if (argv[2][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }
  
  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_EXT);
  return 0;
}

DEFUN(isis_ipv6_redistribute_h3c,
      isis_ipv6_redistribute_h3c_cmd,
      "import-route" ISISV6_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {cost <0-4261412864>}",
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;

  family = str2family("ipv6");
  afi = AFI_IP6;

  type = proto_redistnum(afi, argv[0]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[1]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[1]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[1]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if (argv[2])
  {
      char *endp;
      metric = strtoul(argv[2], &endp, 10);
      if (argv[2][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }
  
  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_EXT);
  return 0;
}



DEFUN(no_isis_redistribute,
      no_isis_redistribute_cmd,
      "no route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2)",
      NO_STR
      "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n")
{
  struct isis_area *area = vty->index;
  int type;
  int level;
  int family;
  int afi;

  family = str2family(argv[0]);

  if(family == AF_INET)
  {
    afi = AFI_IP;
  }
  else
  {
    afi = AFI_IP6;
  }

  type = proto_redistnum(afi, argv[1]);
  
  if (type < 0)
    return CMD_WARNING;

  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if (!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;
  
  isis_redist_unset(area, level, family, type, 0,vty);
  return 0;
}

DEFUN(no_isis_redistribute_h3c_without_level,
      no_isis_redistribute_h3c_without_level_cmd,
      "undo import-route" ISIS_REDIST_STR_ISISD,
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD)
{
  struct isis_area *area = vty->index;
  int type;
  int level;
  int family;
  int afi;

  if (argc < 1)
    return CMD_WARNING;

  family = str2family("ipv4");

  afi = AFI_IP;
  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  for(level = 1; level < 4;level ++)
  {
  	isis_redist_unset(area, level, family, type, 0,vty);
  }
  
  return 0;
}


DEFUN(no_isis_ipv6_redistribute_h3c_without_level,
      no_isis_ipv6_redistribute_h3c_without_level_cmd,
      "undo import-route" ISISV6_REDIST_STR_ISISD,
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD)
{
  struct isis_area *area = vty->index;
  int type;
  int level;
  int family;
  int afi;

  family = str2family("ipv6");
  afi = AFI_IP6;
  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  for(level = 1; level < 4;level ++)
  {
  	isis_redist_unset(area, level, family, type, 0,vty);
  }
  
  return 0;
}

DEFUN(isis_redistribute_instance,
      isis_redistribute_instance_cmd,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {metric <0-4261412864>}",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;
  
  if (argc < 4)
    return CMD_WARNING;

  family = str2family(argv[0]);

  if(family == AF_INET)
  {
    afi = AFI_IP;
  }
  else
  {
    afi = AFI_IP6;
  }

  type = proto_redistnum(afi, argv[1]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[2]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[3]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[3]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[3]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if (argv[4])
  {
      char *endp;
      metric = strtoul(argv[4], &endp, 10);
      if (argv[4][0] == '\0' || *endp != '\0')
        return CMD_WARNING;

      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"narrow metric max value is 63!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }

  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  return 0;
}

DEFUN(isis_redistribute_instance_h3c,
      isis_redistribute_instance_h3c_cmd,
      "import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {cost <0-4261412864>}",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;
  
  if (argc < 3)
    return CMD_WARNING;

  family = str2family("ipv4");
  afi = AFI_IP;
  
  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[1]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if (argv[3])
  {
      char *endp;
      metric = strtoul(argv[3], &endp, 10);
      if (argv[3][0] == '\0' || *endp != '\0')
        return CMD_WARNING;

      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"narrow metric max value is 63!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }

  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  return 0;
}


DEFUN(isis_ipv6_redistribute_instance_h3c,
      isis_ipv6_redistribute_instance_h3c_cmd,
      "import-route" ISISV6_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {cost <0-4261412864>}",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;

  family = str2family("ipv6");
  afi = AFI_IP6;
  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[1]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if (argv[3])
  {
      char *endp;
      metric = strtoul(argv[3], &endp, 10);
      if (argv[3][0] == '\0' || *endp != '\0')
        return CMD_WARNING;

      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"narrow metric max value is 63!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }

  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  return 0;
}

DEFUN(no_isis_redistribute_instance,
      no_isis_redistribute_instance_cmd,
      "no route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      NO_STR
      "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n")
{
  struct isis_area *area = vty->index;
  int type;
  int level;
  int family;
  int afi;
  int instance;
  
  if (argc < 4)
    return CMD_WARNING;

  family = str2family(argv[0]);

  if(family == AF_INET)
  {
    afi = AFI_IP;
  }
  else
  {
    afi = AFI_IP6;
  }

  
  type = proto_redistnum(afi, argv[1]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi (argv[2]);  

  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }

  if (!strcmp("level-1", argv[3]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[3]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[3]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;
    
  isis_redist_unset(area, level, family, type, instance,vty);
  return 0;
}

DEFUN(no_isis_redistribute_instance_h3c,
      no_isis_redistribute_instance_h3c_cmd,
      "undo import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n")
{
  struct isis_area *area = vty->index;
  int type;
  int level;
  int family;
  int afi;
  int instance;
  
  if (argc < 3)
    return CMD_WARNING;

  family = str2family("ipv4");
  afi = AFI_IP;
  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi (argv[1]);  

  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }

  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;
    
  isis_redist_unset(area, level, family, type, instance,vty);
  return 0;
}


DEFUN(no_isis_ipv6_redistribute_instance_h3c,
      no_isis_ipv6_redistribute_instance_h3c_cmd,
      "undo import-route" ISISV6_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n")
{
  struct isis_area *area = vty->index;
  int type;
  int level;
  int family;
  int afi;
  int instance;

  family = str2family("ipv6");
  afi = AFI_IP6;
  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi (argv[1]);  

  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }

  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;
    
  isis_redist_unset(area, level, family, type, instance,vty);
  return 0;
}
	  

DEFUN(isis_redistribute_arg,
      isis_redistribute_arg_cmd,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external)",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  uchar flag = 0;

  if (argc < 4)
    return CMD_WARNING;

  family = str2family(argv[0]);
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[1]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }
  
  if(strcasecmp(argv[3],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }

  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_EXT);
  return 0;
}

DEFUN(isis_redistribute_arg_h3c,
      isis_redistribute_arg_h3c_cmd,
      "import-route" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external)",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  uchar flag = 0;
  

  if (argc < 3)
    return CMD_WARNING;

  family = str2family("ipv4");
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[0]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[1]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[1]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[1]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }
  
  if(strcasecmp(argv[2],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }
  
  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_EXT);
  
  return 0;
}

DEFUN(isis_redistribute_two_arg,
      isis_redistribute_two_arg_cmd,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external) metric <0-4261412864>",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  uchar flag = 0;

  if (argc < 5)
    return CMD_WARNING;

  family = str2family(argv[0]);
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[1]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if(strcasecmp(argv[3],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }

  if (argv[4])
  {
      char *endp;
      metric = strtoul(argv[4], &endp, 10);
      if (argv[4][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
	  
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }
  
  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_EXT);
  
  return 0;
}

DEFUN(isis_redistribute_two_arg_h3c,
      isis_redistribute_two_arg_h3c_cmd,
      "import-route" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external) cost <0-4261412864>",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  uchar flag = 0;

  if (argc < 4)
    return CMD_WARNING;

  family = str2family("ipv4");
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[0]);
  if (type < 0)
    return CMD_WARNING;
    
  if (!strcmp("level-1", argv[1]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[1]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[1]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if(strcasecmp(argv[2],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }


  if (argv[3])
  {
      char *endp;
      metric = strtoul(argv[3], &endp, 10);
      if (argv[3][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
	  
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }

  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, 0, 0,REDIST_EXT);
  
  return 0;
}


DEFUN(isis_redistribute_instance_arg,
      isis_redistribute_instance_arg_cmd,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external)",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;
  uchar flag = 0;
  
  if (argc < 5)
    return CMD_WARNING;

  family = str2family(argv[0]);
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[1]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[2]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[3]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[3]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[3]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }
  
  if(strcasecmp(argv[4],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }

  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  
  return 0;
}

DEFUN(isis_redistribute_instance_arg_h3c,
      isis_redistribute_instance_arg_h3c_cmd,
      "import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external)",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;
  uchar flag = 0;
  
  if (argc < 4)
    return CMD_WARNING;

  family = str2family("ipv4");
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[1]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if(strcasecmp(argv[3],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }

  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  
  return 0;
}

DEFUN(isis_redistribute_instance_two_arg,
      isis_redistribute_instance_two_arg_cmd,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external) metric <0-4261412864>",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;
  uchar flag = 0;
  
  if (argc < 6)
    return CMD_WARNING;

  family = str2family(argv[0]);
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[1]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[2]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[3]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[3]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[3]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if(strcasecmp(argv[4],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }

  if (argv[5])
  {
      char *endp;
      metric = strtoul(argv[5], &endp, 10);
      if (argv[5][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
	  
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }

  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_INT);
  else
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  
  return 0;
}

DEFUN(isis_redistribute_instance_two_arg_h3c,
      isis_redistribute_instance_two_arg_h3c_cmd,
      "import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external) cost <0-4261412864>",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")
{
  struct isis_area *area = vty->index;
  int family;
  int afi;
  int type;
  int level;
  unsigned long metric;
  int instance;
  uchar flag = 0;
  
  if (argc < 5)
    return CMD_WARNING;

  family = str2family("ipv4");
  afi = AFI_IP;

  type = proto_redistnum(afi, argv[0]);
  
  if (type < 0)
    return CMD_WARNING;

  instance = atoi(argv[1]);
  
  /*compare protocol and instance number , if isis with same number , warning */
  if((type ==  ROUTE_PROTO_ISIS) && (instance == atoi(area->area_tag)))
  {
    vty_warning_out(vty,"The routes from the same process cannot be redistributed%s", VTY_NEWLINE);
  	return CMD_WARNING;
  }
  
  if (!strcmp("level-1", argv[2]))
    level = IS_LEVEL_1;
  else if (!strcmp("level-2", argv[2]))
    level = IS_LEVEL_2;
  else if(!strcmp("level-1-2", argv[2]))
    level = IS_LEVEL_1_AND_2;
  else
    return CMD_WARNING;

  if ((area->is_type & level) == 0)
  {
      vty_warning_out(vty, "Node is not a level-%d IS%s", level, VTY_NEWLINE);
      return CMD_WARNING;
  }

  if(strcasecmp(argv[3],"internal") == 0)
  {
		metric = 0;
		flag = 1;
  }
  else
  {
		metric = 0;
		flag = 0;
  }

  if (argv[4])
  {
      char *endp;
      metric = strtoul(argv[4], &endp, 10);
      if (argv[4][0] == '\0' || *endp != '\0')
        return CMD_WARNING;
	  
      if((area->oldmetric == 1 && area->newmetric == 0) || 
	  	 (area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1) )
      {
      	if(metric >= 64)
      	{
      		metric = 0xffffffff;
      		vty_warning_out(vty,"metric value err!%s", VTY_NEWLINE);
      		return CMD_SUCCESS;
      	}
      }
  }
  else
  {
      metric = 0;
  }

  if(flag == 1)
  	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_INT);
  else
	  isis_redist_set(area, level, family, type, metric, instance, 0,REDIST_EXT);
  
  return 0;
}
	  
int isis_write_to_config(struct vty *vty, struct list *redist, int family,struct isis_area * area)
{
	struct isis_redist *redist_set;
	struct listnode *node;
	int write = 0;

	if(!listcount(redist))
	  return 0;

	for (ALL_LIST_ELEMENTS_RO(redist, node, redist_set))
	{
	  switch(redist_set->protocol)
	  {
		  case(ROUTE_PROTO_STATIC):
		      if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
		      	if(redist_set->internal_external_flag == REDIST_INT)
		        {
			        if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 static level-1-2 cost-style internal");
		      	}
				else
				{
			        if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 static level-1-2");
	                if(family == AF_INET6)
	                  vty_out(vty, " route redistribute ipv6 static level-1-2");
				}
              }
			  else
			  {
			  	if(redist_set->internal_external_flag == REDIST_INT)
				{
				    if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 static level-%d cost-style internal", redist_set->level);
			  	}
				else
				{
				    if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 static level-%d", redist_set->level);
				    if(family == AF_INET6)
				      vty_out(vty, " route redistribute ipv6 static level-%d", redist_set->level);
				}
			  }			    
			  break;
		  case(ROUTE_PROTO_CONNECT):
		      if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
				  if(redist_set->internal_external_flag == REDIST_INT)
				  {
					  if(family == AF_INET)
						vty_out(vty, " route redistribute ipv4 connect level-1-2 cost-style internal");
				  }
				  else
				  {
					  if(family == AF_INET)
						vty_out(vty, " route redistribute ipv4 connect level-1-2");
					  if(family == AF_INET6)
						vty_out(vty, " route redistribute ipv6 connect level-1-2");
				  }
              }
			  else
			  {	
			  	if(redist_set->internal_external_flag == REDIST_INT)
				{
				    if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 connect level-%d cost-style internal", redist_set->level);
			  	}
				else
				{
				    if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 connect level-%d", redist_set->level);
				    if(family == AF_INET6)
				      vty_out(vty, " route redistribute ipv6 connect level-%d", redist_set->level);
				}		      
			  }	
			  break;
		  case(ROUTE_PROTO_ARP):
		  	  if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
				  if(redist_set->internal_external_flag == REDIST_INT)
				  {
					  if(family == AF_INET)
						vty_out(vty, " route redistribute ipv4 arp level-1-2 cost-style internal");
				  }
				  else
				  {
					  if(family == AF_INET)
						vty_out(vty, " route redistribute ipv4 arp level-1-2");
					  if(family == AF_INET6)
						vty_out(vty, " route redistribute ipv6 arp level-1-2");
				  }

              }
			  else
			  {	
			  	if(redist_set->internal_external_flag == REDIST_INT)
				{
				    if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 arp level-%d cost-style internal", redist_set->level);
			  	}
				else
				{
				    if(family == AF_INET)
				      vty_out(vty, " route redistribute ipv4 arp level-%d", redist_set->level);
				    if(family == AF_INET6)
				      vty_out(vty, " route redistribute ipv6 arp level-%d", redist_set->level);
				}		      
			  }	
			  break;
			  
		  case(ROUTE_PROTO_OSPF):
		  	  if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
				if(redist_set->internal_external_flag == REDIST_INT)
				{
				      vty_out(vty, " route redistribute ipv4 ospf %d level-1-2 cost-style internal", redist_set->instance);
				}
				else
				{
				      vty_out(vty, " route redistribute ipv4 ospf %d level-1-2", redist_set->instance);
				}
              }
			  else
			  {
			  	if(redist_set->internal_external_flag == REDIST_INT)
			    {
				      vty_out(vty, " route redistribute ipv4 ospf %d level-%d cost-style internal", redist_set->instance, redist_set->level);
			  	}
				else
				{
				      vty_out(vty, " route redistribute ipv4 ospf %d level-%d", redist_set->instance, redist_set->level);
				}
			  }	
			  break;
			  
		  case(ROUTE_PROTO_OSPF6):
			  if(redist_set->level == IS_LEVEL_1_AND_2)
			  {
				  vty_out(vty, " route redistribute ipv6 ospf %d level-1-2", redist_set->instance);
			  }
			  else
			  {
				  vty_out(vty, " route redistribute ipv6 ospf %d level-%d", redist_set->instance, redist_set->level);
			  } 
			  break;

		  case(ROUTE_PROTO_ISIS):
		  	  if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
		      	if(redist_set->internal_external_flag == REDIST_INT)
		        {
				      vty_out(vty, " route redistribute ipv4 isis %d level-1-2 cost-style internal", redist_set->instance);
		      	}
				else
				{
				      vty_out(vty, " route redistribute ipv4 isis %d level-1-2", redist_set->instance);
				}
              }
			  else
			  {	
				  if(redist_set->internal_external_flag == REDIST_INT)
				  {
				      vty_out(vty, " route redistribute ipv4 isis %d level-%d cost-style internal", redist_set->instance, redist_set->level);
				  }
				  else
				  {
					  vty_out(vty, " route redistribute ipv4 isis %d level-%d", redist_set->instance, redist_set->level);
				  }
			  }	
			  break;

		  case(ROUTE_PROTO_ISIS6):
			  if(redist_set->level == IS_LEVEL_1_AND_2)
			  {
				  vty_out(vty, " route redistribute ipv6 isis %d level-1-2", redist_set->instance);
			  }
			  else
			  { 
				  vty_out(vty, " route redistribute ipv6 isis %d level-%d", redist_set->instance, redist_set->level);
			  } 
			  break;

		  case(ROUTE_PROTO_RIP):
		  	  if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
		      	if(redist_set->internal_external_flag == REDIST_INT)
		        {
				      vty_out(vty, " route redistribute ipv4 rip %d level-1-2 cost-style internal", redist_set->instance);
		      	}
				else
				{
				      vty_out(vty, " route redistribute ipv4 rip %d level-1-2", redist_set->instance);
				}
				
              }
			  else
			  {
				  if(redist_set->internal_external_flag == REDIST_INT)
				  {
				      vty_out(vty, " route redistribute ipv4 rip %d level-%d cost-style internal", redist_set->instance, redist_set->level);
				  }
				  else
				  {
				      vty_out(vty, " route redistribute ipv4 rip %d level-%d", redist_set->instance, redist_set->level);
				  }
			  }	
			  break;

		  case(ROUTE_PROTO_RIPNG):
				if(redist_set->level == IS_LEVEL_1_AND_2)
				{
					  vty_out(vty, " route redistribute ipv6 rip %d level-1-2", redist_set->instance);
				}
				else
				{
					  vty_out(vty, " route redistribute ipv6 rip %d level-%d", redist_set->instance, redist_set->level);
				} 
				break;

		  case(ROUTE_PROTO_IBGP):
		  	  if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
		      	if(redist_set->internal_external_flag == REDIST_INT)
		      	{
				      vty_out(vty, " route redistribute ipv4 ibgp level-1-2 cost-style internal");
		      	}
				else
				{
				      vty_out(vty, " route redistribute ipv4 ibgp level-1-2");
		      	}
              }
			  else
			  {
			  	if(redist_set->internal_external_flag == REDIST_INT)
			    {
				      vty_out(vty, " route redistribute ipv4 ibgp level-%d cost-style internal", redist_set->level);
			  	}
				else
				{
				      vty_out(vty, " route redistribute ipv4 ibgp level-%d", redist_set->level);
			  	}
				
			  }	
			  break;
			  
		  case(ROUTE_PROTO_IBGP6):
			  if(redist_set->level == IS_LEVEL_1_AND_2)
			  {
				  vty_out(vty, " route redistribute ipv6 ibgp level-1-2");
			  }
			  else
			  {
				  vty_out(vty, " route redistribute ipv6 ibgp level-%d", redist_set->level);
			  } 
			  break;

		  case(ROUTE_PROTO_EBGP):
		  	  if(redist_set->level == IS_LEVEL_1_AND_2)
		      {
			      if(redist_set->internal_external_flag == REDIST_INT)
			      {
				      vty_out(vty, " route redistribute ipv4 ebgp level-1-2 cost-style internal");
			      }
				  else
				  {
				      vty_out(vty, " route redistribute ipv4 ebgp level-1-2");
			      }
              }
			  else
			  {
				  if(redist_set->internal_external_flag == REDIST_INT)
				  {
					      vty_out(vty, " route redistribute ipv4 ebgp level-%d cost-style internal", redist_set->level);
				  }
				  else
				  {
					      vty_out(vty, " route redistribute ipv4 ebgp level-%d", redist_set->level);
				  }
			  }	
			  break;

		  case(ROUTE_PROTO_EBGP6):
			  if(redist_set->level == IS_LEVEL_1_AND_2)
			  {
				  vty_out(vty, " route redistribute ipv6 ebgp level-1-2");
			  }
			  else
			  {
				  vty_out(vty, " route redistribute ipv6 ebgp level-%d", redist_set->level);
			  } 
			  break;

		  default:
		  	  break;
	  }
	  
	  if(redist_set->metric != 0)
	  	  vty_out(vty, " metric %u", redist_set->metric);
	  
	  vty_out(vty, "%s", VTY_NEWLINE);
	  write++;
	}
		  	
	return write;
}

int
isis_redist_config_write(struct vty *vty, struct isis_area *area,
                         int family)
{
  int write = 0;
  
  write = write + isis_write_to_config(vty, area->redist_settings, AF_INET,area);
  write = write + isis_write_to_config(vty, area->redist_settings_v6, AF_INET6,area);
  
  return write;
}

void
isis_redist_init(void)
{
  install_element(ISIS_NODE, &isis_redistribute_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_isis_redistribute_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &isis_redistribute_instance_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_isis_redistribute_instance_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &isis_redistribute_arg_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &isis_redistribute_two_arg_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &isis_redistribute_instance_arg_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &isis_redistribute_instance_two_arg_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V4_NODE, &isis_redistribute_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &no_isis_redistribute_h3c_without_level_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &isis_redistribute_instance_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &no_isis_redistribute_instance_h3c_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V4_NODE, &isis_redistribute_arg_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &isis_redistribute_two_arg_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &isis_redistribute_instance_arg_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &isis_redistribute_instance_two_arg_h3c_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V6_NODE, &isis_ipv6_redistribute_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_isis_ipv6_redistribute_h3c_without_level_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &isis_ipv6_redistribute_instance_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_isis_ipv6_redistribute_instance_h3c_cmd, CMD_SYNC);
}
