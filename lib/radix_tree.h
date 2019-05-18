/*
 * Routing Table
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */

#ifndef _ZEBRA_RADIX_H
#define _ZEBRA_RADIX_H

#include <netinet/in.h> 
#include <stdint.h>
#include "zebra.h"
#include "types.h"
#include "prefix.h"



/* radix tree structure. */
struct radix_tree
{
  struct route_node *top;
  u_int32_t num;
};



/* Each routing entry. */
struct route_node
{  
  struct prefix p; /* Actual prefix of this radix. */

  /* Tree link. */
  struct route_node *parent;
  struct route_node *link[2];
#define l_left   link[0]
#define l_right  link[1]
  
  unsigned int lock; /* Lock of this node */  
  void *info;        /* node data */
};




/* Prototypes of tree */
struct route_node *route_tree_top (struct radix_tree *tree);
void route_tree_free (struct radix_tree *tree);
void route_tree_dump (struct radix_tree *tree);

/* Prototypes of node */
void route_node_free (struct route_node *node);
struct route_node *route_node_set (struct prefix *prefix);
struct route_node * route_lock_node_rt (struct route_node *node);
void route_unlock_node_rt (struct radix_tree *tree, struct route_node *node);
struct route_node *route_node_match_rt (struct radix_tree *tree, struct prefix *p);
struct route_node *route_node_match_ipv4_rt (struct radix_tree *tree, struct in_addr *addr);
#ifdef HAVE_IPV6
struct route_node *route_node_match_ipv6_rt (struct radix_tree *tree, struct in6_addr *addr);
#endif /* HAVE_IPV6 */

struct route_node *route_node_lookup_rt (struct radix_tree *tree, struct prefix *p);
struct route_node *route_node_add (struct radix_tree *tree, struct prefix *p);
void route_node_delete (struct radix_tree *tree, struct route_node *node);
struct route_node *route_next_rt (struct radix_tree *tree, struct route_node *node);
struct route_node *route_next_until_rt (struct radix_tree *tree, struct route_node *node, struct route_node *limit);









#endif 
