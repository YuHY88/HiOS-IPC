/*
 * IS-IS Rout(e)ing protocol                  - isis_spf.c
 *                                              The SPT algorithm
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

#include <zebra.h>

#include "thread.h"
#include "linklist.h"
#include "vty.h"
#include "log.h"
#include "command.h"
#include "memory.h"
#include "prefix.h"
#include "hash.h"
#include "if.h"
#include "table.h"
#include "timer.h"

#include "isis_constants.h"
#include "isis_common.h"
#include "isis_flags.h"
#include "dict.h"
#include "isisd.h"
#include "isis_misc.h"
#include "isis_adjacency.h"
#include "isis_circuit.h"
#include "isis_tlv.h"
#include "isis_pdu.h"
#include "isis_lsp.h"
#include "isis_dynhn.h"
#include "isis_spf.h"
#include "isis_route.h"
#include "isis_csm.h"
#include "isis_route.h"
int isis_run_spf_l1(void *argv);
int isis_run_spf_l2(void *argv);
extern char *inet_ntoa(struct in_addr in);
extern int lsp_att_check(struct isis_lsp *lsp, struct isis_area *area);
extern int lsp_att_check(struct isis_lsp *lsp, struct isis_area *area);
extern struct isis_statistics isis_sta;

/* 7.2.7 */
static void
remove_excess_adjs(struct list *adjs)
{
  struct listnode *node, *excess = NULL;
  struct isis_adjacency *adj, *candidate = NULL;
  int comp;

  for (ALL_LIST_ELEMENTS_RO(adjs, node, adj))
  {
    if (excess == NULL)
      excess = node;
    candidate = listgetdata(excess);

    if (candidate->sys_type < adj->sys_type)
    {
      excess = node;
      candidate = adj;
      continue;
    }
    if (candidate->sys_type > adj->sys_type)
      continue;

    comp = memcmp(candidate->sysid, adj->sysid, ISIS_SYS_ID_LEN);
    if (comp > 0)
    {
      excess = node;
      candidate = adj;
      continue;
    }
    if (comp < 0)
      continue;

    if (candidate->circuit->circuit_id > adj->circuit->circuit_id)
    {
      excess = node;
      candidate = adj;
      continue;
    }

    if (candidate->circuit->circuit_id < adj->circuit->circuit_id)
      continue;

    comp = memcmp(candidate->snpa, adj->snpa, ETH_ALEN);
    if (comp > 0)
    {
      excess = node;
      candidate = adj;
      continue;
    }
  }

  list_delete_node(adjs, excess);

  return;
}

static const char *
vtype2string(enum vertextype vtype)
{
  switch (vtype)
  {
  case VTYPE_PSEUDO_IS:
    return "pseudo_IS";
    break;
  case VTYPE_PSEUDO_TE_IS:
    return "pseudo_TE-IS";
    break;
  case VTYPE_NONPSEUDO_IS:
    return "IS";
    break;
  case VTYPE_NONPSEUDO_TE_IS:
    return "TE-IS";
    break;
  case VTYPE_ES:
    return "ES";
    break;
  case VTYPE_IPREACH_INTERNAL:
    return "IP internal";
    break;
  case VTYPE_IPREACH_EXTERNAL:
    return "IP external";
    break;
  case VTYPE_IPREACH_TE:
    return "IP TE";
    break;
#if 1
  case VTYPE_IP6REACH_INTERNAL:
    return "IP6 internal";
    break;
  case VTYPE_IP6REACH_EXTERNAL:
    return "IP6 external";
    break;
#endif /* 0 */
  default:
    return "UNKNOWN";
  }
  return NULL; /* Not reached */
}

static const char *
vid2string(struct isis_vertex *vertex, u_char *buff)
{
  switch (vertex->type)
  {
  case VTYPE_PSEUDO_IS:
  case VTYPE_PSEUDO_TE_IS:
    return sysid_print(vertex->N.id);
    //return vertex->N.id;
    break;
  case VTYPE_NONPSEUDO_IS:
  case VTYPE_NONPSEUDO_TE_IS:
  case VTYPE_ES:
    return sysid_print(vertex->N.id);
    //return vertex->N.id;
    break;

  case VTYPE_IPREACH_INTERNAL:
  case VTYPE_IPREACH_EXTERNAL:
  case VTYPE_IPREACH_TE:
#if 1
  case VTYPE_IP6REACH_INTERNAL:
  case VTYPE_IP6REACH_EXTERNAL:
#endif /* 0 */
    prefix2str((struct prefix *)&vertex->N.prefix, (char *)buff, BUFSIZ);
    break;

  default:
    return (char *)"UNKNOWN";
  }

  return (char *)buff;
}

static struct isis_vertex *
isis_vertex_new(void *id, enum vertextype vtype)
{
  struct isis_vertex *vertex;

  vertex = XCALLOC(MTYPE_ISIS_VERTEX, sizeof(struct isis_vertex));

  vertex->type = vtype;
  switch (vtype)
  {
  case VTYPE_ES:
  case VTYPE_NONPSEUDO_IS:
  case VTYPE_NONPSEUDO_TE_IS:
    memcpy(vertex->N.id, (u_char *)id, ISIS_SYS_ID_LEN);
    break;
  case VTYPE_PSEUDO_IS:
  case VTYPE_PSEUDO_TE_IS:
    memcpy(vertex->N.id, (u_char *)id, ISIS_SYS_ID_LEN + 1);
    break;
  case VTYPE_IPREACH_INTERNAL:
  case VTYPE_IPREACH_EXTERNAL:
  case VTYPE_IPREACH_TE:
#if 1
  case VTYPE_IP6REACH_INTERNAL:
  case VTYPE_IP6REACH_EXTERNAL:
#endif /* 0 */
    memcpy(&vertex->N.prefix, (struct prefix *)id,
           sizeof(struct prefix));
    break;
  default:
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "WTF!");
  }

  vertex->Adj_N = list_new();
  vertex->parents = list_new();
  vertex->children = list_new();

  return vertex;
}

static void
isis_vertex_del(struct isis_vertex *vertex)
{
  list_delete(vertex->Adj_N);
  vertex->Adj_N = NULL;
  list_delete(vertex->parents);
  vertex->parents = NULL;
  list_delete(vertex->children);
  vertex->children = NULL;

  memset(vertex, 0, sizeof(struct isis_vertex));
  XFREE(MTYPE_ISIS_VERTEX, vertex);

  return;
}

static void
isis_vertex_adj_del(struct isis_vertex *vertex, struct isis_adjacency *adj)
{
  struct listnode *node, *nextnode;
  if (!vertex)
    return;
  for (node = listhead(vertex->Adj_N); node; node = nextnode)
  {
    nextnode = listnextnode(node);
    if (listgetdata(node) == adj)
      list_delete_node(vertex->Adj_N, node);
  }
  return;
}

struct isis_spftree *
isis_spftree_new(struct isis_area *area)
{
  struct isis_spftree *tree;

  tree = XCALLOC(MTYPE_ISIS_SPFTREE, sizeof(struct isis_spftree));
  if (tree == NULL)
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: isis_spftree_new Out of memory!");
    return NULL;
  }

  tree->tents = list_new();
  tree->paths = list_new();
  tree->area = area;
  tree->last_run_timestamp = 0;
  tree->last_run_duration = 0;
  tree->runcount = 0;
  tree->pending = 0;
  return tree;
}

void isis_spftree_del(struct isis_spftree *spftree)
{
  THREAD_TIMER_OFF_NEW(spftree->t_spf);
  spftree->t_spf = 0;

  spftree->tents->del = (void (*)(void *))isis_vertex_del;
  list_delete(spftree->tents);
  spftree->tents = NULL;

  spftree->paths->del = (void (*)(void *))isis_vertex_del;
  list_delete(spftree->paths);
  spftree->paths = NULL;

  XFREE(MTYPE_ISIS_SPFTREE, spftree);

  return;
}

void isis_spftree_adj_del(struct isis_spftree *spftree, struct isis_adjacency *adj)
{
  struct listnode *node;
  if (!adj)
    return;
  for (node = listhead(spftree->tents); node; node = listnextnode(node))
    isis_vertex_adj_del(listgetdata(node), adj);
  for (node = listhead(spftree->paths); node; node = listnextnode(node))
    isis_vertex_adj_del(listgetdata(node), adj);
  return;
}

void spftree_area_init(struct isis_area *area)
{
  if (area->is_type & IS_LEVEL_1)
  {
    if (area->spftree[0] == NULL)
      area->spftree[0] = isis_spftree_new(area);
#if 1
    if (area->spftree6[0] == NULL)
      area->spftree6[0] = isis_spftree_new(area);
#endif
  }

  if (area->is_type & IS_LEVEL_2)
  {
    if (area->spftree[1] == NULL)
      area->spftree[1] = isis_spftree_new(area);
#if 1
    if (area->spftree6[1] == NULL)
      area->spftree6[1] = isis_spftree_new(area);
#endif
  }

  return;
}

void spftree_area_del(struct isis_area *area)
{
  if (area->is_type & IS_LEVEL_1)
  {
    if (area->spftree[0] != NULL)
    {
      isis_spftree_del(area->spftree[0]);
      area->spftree[0] = NULL;
    }
#if 1
    if (area->spftree6[0])
    {
      isis_spftree_del(area->spftree6[0]);
      area->spftree6[0] = NULL;
    }
#endif
  }

  if (area->is_type & IS_LEVEL_2)
  {
    if (area->spftree[1] != NULL)
    {
      isis_spftree_del(area->spftree[1]);
      area->spftree[1] = NULL;
    }
#if 1
    if (area->spftree6[1] != NULL)
    {
      isis_spftree_del(area->spftree6[1]);
      area->spftree6[1] = NULL;
    }
#endif
  }

  return;
}

void spftree_area_adj_del(struct isis_area *area, struct isis_adjacency *adj)
{
  if (area->is_type & IS_LEVEL_1)
  {
    if (area->spftree[0] != NULL)
      isis_spftree_adj_del(area->spftree[0], adj);
#if 1
    if (area->spftree6[0] != NULL)
      isis_spftree_adj_del(area->spftree6[0], adj);
#endif
  }

  if (area->is_type & IS_LEVEL_2)
  {
    if (area->spftree[1] != NULL)
      isis_spftree_adj_del(area->spftree[1], adj);
#if 1
    if (area->spftree6[1] != NULL)
      isis_spftree_adj_del(area->spftree6[1], adj);
#endif
  }

  return;
}

/* 
 * Find the system LSP: returns the LSP in our LSP database 
 * associated with the given system ID.
 */
static struct isis_lsp *
isis_root_system_lsp(struct isis_area *area, int level, u_char *sysid)
{
  struct isis_lsp *lsp;
  u_char lspid[ISIS_SYS_ID_LEN + 2];

  memcpy(lspid, sysid, ISIS_SYS_ID_LEN);
  LSP_PSEUDO_ID(lspid) = 0;
  LSP_FRAGMENT(lspid) = 0;
  lsp = lsp_search(lspid, area->lspdb[level - 1]);
  if (lsp && lsp->lsp_header->rem_lifetime != 0)
    return lsp;
  return NULL;
}

/*
 * Add this IS to the root of SPT
 */
static struct isis_vertex *
isis_spf_add_root(struct isis_spftree *spftree, int level, u_char *sysid)
{
  struct isis_vertex *vertex;
  struct isis_lsp *lsp;
#ifdef EXTREME_DEBUG
  u_char buff[BUFSIZ];
#endif /* EXTREME_DEBUG */

  lsp = isis_root_system_lsp(spftree->area, level, sysid);
  if (lsp == NULL)
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: could not find own l%d LSP!", level);

  if (!spftree->area->oldmetric)
    vertex = isis_vertex_new(sysid, VTYPE_NONPSEUDO_TE_IS);
  else
    vertex = isis_vertex_new(sysid, VTYPE_NONPSEUDO_IS);

  listnode_add(spftree->paths, vertex);

#ifdef EXTREME_DEBUG
  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: added this IS  %s %s depth %d dist %d to PATHS",
             vtype2string(vertex->type), vid2string(vertex, buff),
             vertex->depth, vertex->d_N);
#endif /* EXTREME_DEBUG */

  return vertex;
}

static struct isis_vertex *
isis_find_vertex_tent(struct list *list, void *id, enum vertextype vtype)
{
  struct listnode *node;
  struct isis_vertex *vertex;
  struct prefix *p1, *p2;

  for (ALL_LIST_ELEMENTS_RO(list, node, vertex))
  {
    if (vertex->type != vtype)
      continue;
    switch (vtype)
    {
    case VTYPE_IPREACH_INTERNAL:
    case VTYPE_IPREACH_EXTERNAL:
    case VTYPE_IPREACH_TE:
#if 1
    case VTYPE_IP6REACH_INTERNAL:
    case VTYPE_IP6REACH_EXTERNAL:
#endif /* 0 */
      p1 = (struct prefix *)id;
      p2 = (struct prefix *)&vertex->N.id;
      if (p1->family == p2->family && p1->prefixlen == p2->prefixlen &&
          memcmp(&p1->u.prefix, &p2->u.prefix, PSIZE(p1->prefixlen)) == 0)
        return vertex;
    default:
      break;
    }
  }

  return NULL;
}

static struct isis_vertex *
isis_find_vertex(struct list *list, void *id, enum vertextype vtype)
{
  struct listnode *node;
  struct isis_vertex *vertex;
  struct prefix *p1, *p2;

  for (ALL_LIST_ELEMENTS_RO(list, node, vertex))
  {
    if (vertex->type != vtype)
      continue;
    switch (vtype)
    {
    case VTYPE_ES:
    case VTYPE_NONPSEUDO_IS:
    case VTYPE_NONPSEUDO_TE_IS:
      if (memcmp((u_char *)id, vertex->N.id, ISIS_SYS_ID_LEN) == 0)
        return vertex;
      break;
    case VTYPE_PSEUDO_IS:
    case VTYPE_PSEUDO_TE_IS:
      if (memcmp((u_char *)id, vertex->N.id, ISIS_SYS_ID_LEN + 1) == 0)
        return vertex;
      break;
    case VTYPE_IPREACH_INTERNAL:
    case VTYPE_IPREACH_EXTERNAL:
    case VTYPE_IPREACH_TE:
#if 1
    case VTYPE_IP6REACH_INTERNAL:
    case VTYPE_IP6REACH_EXTERNAL:
#endif /* 0 */
      p1 = (struct prefix *)id;
      p2 = (struct prefix *)&vertex->N.id;
      if (p1->family == p2->family && p1->prefixlen == p2->prefixlen &&
          memcmp(&p1->u.prefix, &p2->u.prefix,
                 PSIZE(p1->prefixlen)) == 0)
        return vertex;
      break;
    }
  }

  return NULL;
}

/*
 * Add a vertex to TENT sorted by cost and by vertextype on tie break situation
 */
static struct isis_vertex *
isis_spf_add2tent(struct isis_spftree *spftree, enum vertextype vtype,
                  void *id, uint32_t cost, int depth, int family,
                  struct isis_adjacency *adj, struct isis_vertex *parent)
{
  struct isis_vertex *vertex, *v;
  struct listnode *node;
  struct isis_adjacency *parent_adj;
  u_char buff[BUFSIZ];

  //assert (isis_find_vertex (spftree->paths, id, vtype) == NULL);
  //assert (isis_find_vertex (spftree->tents, id, vtype) == NULL);
  if (isis_find_vertex(spftree->paths, id, vtype) != NULL || isis_find_vertex(spftree->tents, id, vtype) != NULL)
    return NULL;

  vertex = isis_vertex_new(id, vtype);
  vertex->d_N = cost;
  vertex->depth = depth;

  if (parent)
  {
    listnode_add(vertex->parents, parent);
    if (listnode_lookup(parent->children, vertex) == NULL)
      listnode_add(parent->children, vertex);
  }

  if (parent && parent->Adj_N && listcount(parent->Adj_N) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(parent->Adj_N, node, parent_adj))
      listnode_add(vertex->Adj_N, parent_adj);
  }
  else if (adj)
  {
    listnode_add(vertex->Adj_N, adj);
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: add to TENT %s %s %s depth %d dist %d adjcount %d",
             print_sys_hostname(vertex->N.id),
             vtype2string(vertex->type), vid2string(vertex, buff),
             vertex->depth, vertex->d_N, listcount(vertex->Adj_N));

  if (list_isempty(spftree->tents))
  {
    listnode_add(spftree->tents, vertex);
    return vertex;
  }

  /* XXX: This cant use the standard ALL_LIST_ELEMENTS macro */
  for (node = listhead(spftree->tents); node; node = listnextnode(node))
  {
    v = listgetdata(node);
    if (v->d_N > vertex->d_N)
    {
      listnode_add_prev(spftree->tents, node, vertex);
      break;
    }
    else if (v->d_N == vertex->d_N && v->type > vertex->type)
    {
      /*  Tie break, add according to type */
      listnode_add_prev(spftree->tents, node, vertex);
      break;
    }
  }

  if (node == NULL)
    listnode_add(spftree->tents, vertex);

  return vertex;
}

static void
isis_spf_add_local(struct isis_spftree *spftree, enum vertextype vtype,
                   void *id, struct isis_adjacency *adj, uint32_t cost,
                   int family, struct isis_vertex *parent)
{
  struct isis_vertex *vertex;

  vertex = isis_find_vertex(spftree->tents, id, vtype);

  if (vertex)
  {
    /* C.2.5   c) */
    if (vertex->d_N == cost)
    {
      if (adj)
        listnode_add(vertex->Adj_N, adj);
      /*       d) */
      if (listcount(vertex->Adj_N) > ISIS_MAX_PATH_SPLITS)
        remove_excess_adjs(vertex->Adj_N);
      if (parent && (listnode_lookup(vertex->parents, parent) == NULL))
        listnode_add(vertex->parents, parent);
      if (parent && (listnode_lookup(parent->children, vertex) == NULL))
        listnode_add(parent->children, vertex);
      return;
    }
    else if (vertex->d_N < cost)
    {
      /*       e) do nothing */
      return;
    }
    else
    { /* vertex->d_N > cost */
      /*         f) */
      struct listnode *pnode, *pnextnode;
      struct isis_vertex *pvertex;
      listnode_delete(spftree->tents, vertex);
      //assert (listcount (vertex->children) == 0);
      if (listcount(vertex->children) != 0)
      {
        zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
        return;
      }

      for (ALL_LIST_ELEMENTS(vertex->parents, pnode, pnextnode, pvertex))
        listnode_delete(pvertex->children, vertex);
      isis_vertex_del(vertex);
    }
  }

  isis_spf_add2tent(spftree, vtype, id, cost, 1, family, adj, parent);
  return;
}


/* the last param will be set when L2 import L1 */
static void
process_ip(struct isis_spftree *spftree, struct prefix *prefix, uint32_t dist,
           uint16_t depth, int family, struct isis_vertex * vertex, int level, int distribute, uint8_t up_down_bit)
{
  if (listcount(vertex->Adj_N) > 0)
  { 
  	if(prefix->family == AF_INET)
    	isis_route_create(prefix, dist, depth, vertex->Adj_N, spftree->area, level, distribute, up_down_bit);
	if(prefix->family == AF_INET6)
		isis_ipv6_route_create(prefix, dist, depth, vertex->Adj_N, spftree->area, level, distribute, up_down_bit);
  }
}

static void
process_N(struct isis_spftree *spftree, enum vertextype vtype, void *id,
          uint32_t dist, uint16_t depth, int family,
          struct isis_vertex *parent)
{
  struct isis_vertex *vertex;
  u_char buff[255];

  //assert (spftree && parent);
  if (!(spftree && parent))
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  /* RFC3787 section 5.1 */
  if (spftree->area->newmetric == 1)
  {
    if (dist > MAX_WIDE_PATH_METRIC)
      return;
  }
  /* C.2.6 b)    */
  else if (spftree->area->oldmetric == 1)
  {
    if (dist > MAX_NARROW_PATH_METRIC)
      return;
  }

  /*       c)    */
  if (vtype <= VTYPE_ES)
  {
    vertex = isis_find_vertex(spftree->paths, id, vtype);
    if (vertex)
    {
      zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: process_N %s %s %s dist %d already found from PATH",
                 print_sys_hostname(vertex->N.id),
                 vtype2string(vtype), vid2string(vertex, buff), dist);

      //assert (dist >= vertex->d_N);
      if (!(dist >= vertex->d_N))
      {
        zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
        return;
      }

      return;
    }
  }
#if 1
  vertex = isis_find_vertex_tent(spftree->tents, id, vtype);
  /*       d)    */
  if (vertex)
  {
    /*        1) */
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: process_N %s %s %s dist %d parent %s adjcount %d",
               print_sys_hostname(vertex->N.id),
               vtype2string(vtype), vid2string(vertex, buff), dist,
               (parent ? print_sys_hostname(parent->N.id) : "null"),
               (parent ? listcount(parent->Adj_N) : 0));

    if (vertex->d_N == dist)
    {
      struct listnode *node;
      struct isis_adjacency *parent_adj;
      for (ALL_LIST_ELEMENTS_RO(parent->Adj_N, node, parent_adj))
        if (listnode_lookup(vertex->Adj_N, parent_adj) == NULL)
          listnode_add(vertex->Adj_N, parent_adj);
      /*      2) */
      if (listcount(vertex->Adj_N) > ISIS_MAX_PATH_SPLITS)
        remove_excess_adjs(vertex->Adj_N);
      if (listnode_lookup(vertex->parents, parent) == NULL)
        listnode_add(vertex->parents, parent);
      if (listnode_lookup(parent->children, vertex) == NULL)
        listnode_add(parent->children, vertex);
      /*      3) */
      return;
    }
    else if (vertex->d_N < dist)
    {
      return;
      /*      4) */
    }
    else
    {
      struct listnode *pnode, *pnextnode;
      struct isis_vertex *pvertex;
      listnode_delete(spftree->tents, vertex);
      //assert (listcount (vertex->children) == 0);
      if (listcount(vertex->children) != 0)
      {
        zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
        return;
      }

      for (ALL_LIST_ELEMENTS(vertex->parents, pnode, pnextnode, pvertex))
        listnode_delete(pvertex->children, vertex);
      isis_vertex_del(vertex);
    }
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: process_N add2tent %s %s dist %d parent %s",
             print_sys_hostname(id), vtype2string(vtype), dist,
             (parent ? print_sys_hostname(parent->N.id) : "null"));

  isis_spf_add2tent(spftree, vtype, id, dist, depth, family, NULL, parent);
  return;
#endif
}

int lsp_type_check(struct isis_spftree *spftree, struct isis_lsp *lsp, int family)
{
  struct isis_lsp *frag;
  struct listnode *node;

  if ((spftree->area->oldmetric == 1) && (spftree->area->newmetric == 1))
    return 1;

  if (family == AF_INET)
  {
    if ((spftree->area->oldmetric == 1) && (spftree->area->newmetric == 0))
    {
      if (lsp->tlv_data.ipv4_int_reachs != NULL)
        return 1;

      for (ALL_LIST_ELEMENTS_RO(lsp->lspu.frags, node, frag))
      {
        if (frag->tlv_data.ipv4_int_reachs != NULL)
          return 1;
      }

      if (spftree->area->compatible == 1)
      {
        if (lsp->tlv_data.te_ipv4_reachs != NULL || lsp->tlv_data.te_circuit_ipv4_reachs != NULL)
          return 1;

        for (ALL_LIST_ELEMENTS_RO(lsp->lspu.frags, node, frag))
        {
          if (frag->tlv_data.te_ipv4_reachs != NULL || frag->tlv_data.te_circuit_ipv4_reachs != NULL)
            return 1;
        }
      }
    }

    if ((spftree->area->oldmetric == 0) && (spftree->area->newmetric == 1))
    {
      if (lsp->tlv_data.te_ipv4_reachs != NULL || lsp->tlv_data.te_circuit_ipv4_reachs != NULL)
        return 1;

      for (ALL_LIST_ELEMENTS_RO(lsp->lspu.frags, node, frag))
      {
        if (frag->tlv_data.te_ipv4_reachs != NULL || frag->tlv_data.te_circuit_ipv4_reachs != NULL)
          return 1;
      }

      if (spftree->area->compatible == 1)
      {
        if (lsp->tlv_data.ipv4_int_reachs != NULL)
          return 1;

        for (ALL_LIST_ELEMENTS_RO(lsp->lspu.frags, node, frag))
        {
          if (frag->tlv_data.ipv4_int_reachs != NULL)
            return 1;
        }
      }
    }
  }

  return 0;
}

/*
 * C.2.6 Step 1
 */
static int
isis_spf_process_lsp_ip_reach(struct isis_spftree *spftree, struct isis_lsp *lsp,
                              uint32_t cost, uint16_t depth, int family,
                              u_char *root_sysid, struct isis_vertex *parent,
                              struct list *att, struct isis_area *area, int *att_check, int level)
{
  struct listnode *node, *fragnode = NULL;
  uint32_t dist;
  struct ipv4_reachability *ipreach;
  struct te_ipv4_reachability *te_ipv4_reach;
  struct te_is_neigh *te_is_neigh;
  static const u_char null_sysid[ISIS_SYS_ID_LEN];
  struct prefix prefix;
  struct ipv6_reachability *ip6reach;
  
  u_char distribute;
  uint8_t up_down_bit;

  void *data;

  if (!speaks(lsp->tlv_data.nlpids, family))
    return ISIS_OK;

  if (family == AF_INET)
  {
    if (!lsp_type_check(spftree, lsp, family))
      return ISIS_OK;
  }

  if ((memcmp(lsp->lsp_header->lsp_id, root_sysid, ISIS_SYS_ID_LEN) == 0))
    return ISIS_OK;

  if (cost > 16777215)
    return ISIS_OK;

  if (area->is_type == IS_LEVEL_1_AND_2 && lsp->level == IS_LEVEL_2)
  {
    *att_check += lsp_att_check(lsp, area);
  }

lspfragloop:
  if (lsp->lsp_header->seq_num == 0)
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "isis_spf_process_lsp(): lsp with 0 seq_num - ignore");
    return ISIS_WARNING;
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: process_lsp %s", print_sys_hostname(lsp->lsp_header->lsp_id));

  if (lsp->tlv_data.te_is_neighs)
  {
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.te_is_neighs, node, te_is_neigh))
    {
      if (!memcmp(te_is_neigh->neigh_id, null_sysid, ISIS_SYS_ID_LEN))
        continue;
      /*get the metric , the cost is configed on the out interface , 
	  the route shouldn't be calculated in spf when the cost >= 16777215 */
      dist = GET_TE_METRIC(te_is_neigh);
      if (dist >= 16777215 && memcmp(lsp, lsp->lspu.zero_lsp, sizeof(struct isis_lsp)))
        continue;
    }
  }

  /* dispose the tlv when ipv4 and oldmetric is enable , otherwise ignore it */
  if (family == AF_INET && lsp->tlv_data.ipv4_int_reachs && spftree->area->oldmetric == 1)
  {
    prefix.family = AF_INET;
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.ipv4_int_reachs, node, ipreach))
    {
      /* ignore the 8 bit whenever , RFC1195 */
      dist = cost + (ipreach->metrics.metric_default & 0x7f);
      distribute = ipreach->metrics.metric_default & CTRL_INFO_DISTRIBUTION;
	  up_down_bit = 0;
	  
      if (dist > MAX_NARROW_PATH_METRIC)
      {
      	  continue;
      }

      prefix.u.prefix4 = ipreach->prefix;
      prefix.prefixlen = ip_masklen(ipreach->mask);
      apply_mask(&prefix);

	/* I test on huawei's device , if the internal one existed , ignore the external one */
      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL1 - 1] && lsp->level == ISIS_LEVEL2)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL1 - 1], &prefix))
          continue;
      }

      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL2 - 1] && lsp->level == ISIS_LEVEL1)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL2 - 1], &prefix))
          continue;
      }
	
      process_ip(spftree, &prefix, dist, depth + 1, family, parent, level, distribute , up_down_bit);

      if ( att != NULL &&
	  	   (ISIS_MASK_LSP_ATT_BITS(lsp->lsp_header->lsp_bits) == 0x08) &&
           !ISIS_MASK_LSP_OL_BIT(lsp->lsp_header->lsp_bits))
      {
        if (listnode_lookup(att, &prefix) == NULL)
        {
          data = XMALLOC(MTYPE_PREFIX, sizeof(struct prefix));
          memcpy(data, &prefix, sizeof(struct prefix));
          listnode_add(att, data);
        }
      }
    }
  }

  /* dispose the tlv when ipv4 and oldmetric is enable , otherwise ignore it */
  if (family == AF_INET && lsp->tlv_data.ipv4_ext_reachs && spftree->area->oldmetric == 1)
  {
    prefix.family = AF_INET;
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.ipv4_ext_reachs, node, ipreach))
    {
      dist = cost + (ipreach->metrics.metric_default & 0x7f);
      distribute = ipreach->metrics.metric_default & CTRL_INFO_DISTRIBUTION;
	  up_down_bit = 0;
	  
      if (dist > MAX_NARROW_PATH_METRIC)
      {
          continue;
      }

      prefix.u.prefix4 = ipreach->prefix;
      prefix.prefixlen = ip_masklen(ipreach->mask);
      apply_mask(&prefix);


      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL1 - 1] && lsp->level == ISIS_LEVEL2)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL1 - 1], &prefix))
        {
          continue;
        }
      }

      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL2 - 1] && lsp->level == ISIS_LEVEL1)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL2 - 1], &prefix))
        {
          continue;
        }
      }


      process_ip(spftree, &prefix, dist, depth + 1, family, parent, level, distribute ,up_down_bit);

      if ((ISIS_MASK_LSP_ATT_BITS(lsp->lsp_header->lsp_bits) == 0x08) &&
          att != NULL && !ISIS_MASK_LSP_OL_BIT(lsp->lsp_header->lsp_bits))
      {
        if (listnode_lookup(att, &prefix) == NULL)
        {
          data = XMALLOC(MTYPE_PREFIX, sizeof(struct prefix));
          memcpy(data, &prefix, sizeof(struct prefix));
          listnode_add(att, data);
        }
      }
    }
  }

  if (family == AF_INET && lsp->tlv_data.te_circuit_ipv4_reachs && spftree->area->newmetric == 1)
  {
    prefix.family = AF_INET;
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.te_circuit_ipv4_reachs, node, te_ipv4_reach))
    {
      if ((te_ipv4_reach->control & 0x3F) > IPV4_MAX_BITLEN)
      {
        continue;
      }

	    distribute = 0;
      up_down_bit = te_ipv4_reach->control & CTRL_INFO_DIRECTION;
	  
      dist = cost + ntohl(te_ipv4_reach->te_metric);

      if (dist > MAX_WIDE_PATH_METRIC)
      	dist = MAX_WIDE_PATH_METRIC;

      prefix.u.prefix4 = newprefix2inaddr(&te_ipv4_reach->prefix_start, te_ipv4_reach->control);
      prefix.prefixlen = (te_ipv4_reach->control & 0x3F);
      apply_mask(&prefix);
	  

      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL1 - 1] && lsp->level == ISIS_LEVEL2)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL1 - 1], &prefix))
          continue;
      }

      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL2 - 1] && lsp->level == ISIS_LEVEL1)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL2 - 1], &prefix))
          continue;
      }


      process_ip(spftree, &prefix, dist, depth + 1, family, parent, level, distribute, up_down_bit );
      if ((ISIS_MASK_LSP_ATT_BITS(lsp->lsp_header->lsp_bits) == 0x08) &&
          att != NULL && !ISIS_MASK_LSP_OL_BIT(lsp->lsp_header->lsp_bits))
      {
        if (listnode_lookup(att, &prefix) == NULL)
        {
          data = XMALLOC(MTYPE_PREFIX, sizeof(struct prefix));
          memcpy(data, (void *)(&prefix), sizeof(struct prefix));
          listnode_add(att, data);
        }
      }
    }
  }

  if (family == AF_INET && lsp->tlv_data.te_ipv4_reachs && spftree->area->newmetric == 1)
  {
    prefix.family = AF_INET;
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.te_ipv4_reachs, node, te_ipv4_reach))
    {
      if ((te_ipv4_reach->control & 0x3F) > IPV4_MAX_BITLEN)
      {
        continue;
      }

      dist = cost + ntohl(te_ipv4_reach->te_metric);
      up_down_bit = te_ipv4_reach->control & CTRL_INFO_DIRECTION;
	    distribute = te_ipv4_reach->control & CTRL_INFO_DISTRIBUTION;
	  
      if (dist > MAX_WIDE_PATH_METRIC)
      	dist = MAX_WIDE_PATH_METRIC;

      prefix.u.prefix4 = newprefix2inaddr(&te_ipv4_reach->prefix_start, te_ipv4_reach->control);
      prefix.prefixlen = (te_ipv4_reach->control & 0x3F);
      apply_mask(&prefix);


      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL1 - 1] && lsp->level == ISIS_LEVEL2)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL1 - 1], &prefix))
          continue;
      }

      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[0][ISIS_LEVEL2 - 1] && lsp->level == ISIS_LEVEL1)
      {
        if (route_node_lookup(area->ext_reach[0][ISIS_LEVEL2 - 1], &prefix))
          continue;
      }


      process_ip(spftree, &prefix, dist, depth + 1, family, parent, level, distribute, up_down_bit );
      if ((ISIS_MASK_LSP_ATT_BITS(lsp->lsp_header->lsp_bits) == 0x08) &&
          att != NULL && !ISIS_MASK_LSP_OL_BIT(lsp->lsp_header->lsp_bits))
      {
        if (listnode_lookup(att, &prefix) == NULL)
        {
          data = XMALLOC(MTYPE_PREFIX, sizeof(struct prefix));
          memcpy(data, (void *)(&prefix), sizeof(struct prefix));
          listnode_add(att, data);
        }
      }
    }
  }
  
  if (family == AF_INET6 && lsp->tlv_data.ipv6_reachs)
  {
    prefix.family = AF_INET6;
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.ipv6_reachs, node, ip6reach))
    {
      if (ip6reach->prefix_len > IPV6_MAX_BITLEN)
          continue;

	  /*
			RFC5308 , if a prefix is advertised with a metric larger
			than MAX_V6_PATH_METRIC (0xFE000000), this prefix MUST not be
			considered during the normal Shortest Path First (SPF) computation.
	  */
	  if(ntohl(ip6reach->metric) >  MAX_WIDE_PATH_METRIC)
	  	  continue;

      dist = cost + ntohl(ip6reach->metric);
	  /*
			RFC5308 , If during the SPF, a path metric would exceed
			MAX_V6_PATH_METRIC, it SHALL be considered to be MAX_V6_PATH_METRIC.
	  */
	  if(dist > MAX_WIDE_PATH_METRIC)
	  		dist = MAX_WIDE_PATH_METRIC;
	  
      distribute = ip6reach->control_info & CTRL_INFO_DISTRIBUTION;
	  up_down_bit = ip6reach->control_info & CTRL_INFO_DIRECTION;
	  
      prefix.prefixlen = ip6reach->prefix_len;
      memcpy(&prefix.u.prefix6.s6_addr, ip6reach->prefix, PSIZE(ip6reach->prefix_len));
      apply_mask(&prefix);
	  
      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[1][ISIS_LEVEL1 - 1] && lsp->level == ISIS_LEVEL2)
      {
        if (route_node_lookup(area->ext_reach[1][ISIS_LEVEL1 - 1], &prefix))
          continue;
      }

      if (spftree->area->is_type == IS_LEVEL_1_AND_2 && area->ext_reach[1][ISIS_LEVEL2 - 1] && lsp->level == ISIS_LEVEL1)
      {
        if (route_node_lookup(area->ext_reach[1][ISIS_LEVEL2 - 1], &prefix))
          continue;
      }

      process_ip(spftree, &prefix, dist, depth + 1, family, parent, level, distribute, up_down_bit);

      if ((ISIS_MASK_LSP_ATT_BITS(lsp->lsp_header->lsp_bits) == 0x08) &&
          att != NULL && !ISIS_MASK_LSP_OL_BIT(lsp->lsp_header->lsp_bits))
      {
        if (listnode_lookup(att, &prefix) == NULL)
        {
          data = XMALLOC(MTYPE_PREFIX, sizeof(struct prefix));
          memcpy(data, (void *)(&prefix), sizeof(struct prefix));
          listnode_add(att, data);
        }
      }
    }
  }


  if (fragnode == NULL)
  {
    fragnode = listhead(lsp->lspu.frags);
  }
  else
  {
    fragnode = listnextnode(fragnode);
  }

  //if (fragnode)
  //  {
  //    lsp = listgetdata (fragnode);
  //    goto lspfragloop;
  //  }

  /*
  	������һ����ƬLSP��
  	����÷�ƬLSP��ʣ������ʱ��Ϊ0���򲻲���·�ɼ���
  	*/
  if (fragnode)
  {
    lsp = listgetdata(fragnode);

    while (lsp == NULL || lsp->lsp_header == NULL || lsp->lsp_header->rem_lifetime == 0)
    {
      if (fragnode)
      {
        fragnode = listnextnode(fragnode);
        if (fragnode)
        {
          lsp = listgetdata(fragnode);
        }
        else
        {
          return ISIS_OK;
        }
      }
      else
      {
        return ISIS_OK;
      }
    }

    goto lspfragloop;
  }

  return ISIS_OK;
}

/*
 * C.2.6 Step 1
 */
static int
isis_spf_process_lsp(struct isis_spftree *spftree, struct isis_lsp *lsp,
                     uint32_t cost, uint16_t depth, int family,
                     u_char *root_sysid, struct isis_vertex *parent,
                     struct list *att, struct isis_area *area, int *att_check)
{
  struct listnode *node, *fragnode = NULL;
  uint32_t dist;
  struct is_neigh *is_neigh;
  struct te_is_neigh *te_is_neigh;
  enum vertextype vtype;
  static const u_char null_sysid[ISIS_SYS_ID_LEN];

  if (!speaks(lsp->tlv_data.nlpids, family))
  {
    return ISIS_OK;
  }

  if (family == AF_INET)
  {
    if (!lsp_type_check(spftree, lsp, AF_INET))
      return ISIS_OK;
  }

  if (area->is_type == IS_LEVEL_1_AND_2 && lsp->level == IS_LEVEL_2)
    *att_check += lsp_att_check(lsp, area);

lspfragloop:
  if (lsp->lsp_header->seq_num == 0)
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "isis_spf_process_lsp(): lsp with 0 seq_num - ignore");
    return ISIS_WARNING;
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: process_lsp %s", print_sys_hostname(lsp->lsp_header->lsp_id));

  if ((!ISIS_MASK_LSP_OL_BIT(lsp->lsp_header->lsp_bits)))
  {
    if (lsp->tlv_data.is_neighs)
    {
      for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.is_neighs, node, is_neigh))
      {
        /* C.2.6 a) */
        /* Two way connectivity */
        if (!memcmp(is_neigh->neigh_id, root_sysid, ISIS_SYS_ID_LEN))
        {
          continue;
        }
        if (!memcmp(is_neigh->neigh_id, null_sysid, ISIS_SYS_ID_LEN))
          continue;
        dist = cost + is_neigh->metrics.metric_default;
        vtype = LSP_PSEUDO_ID(is_neigh->neigh_id) ? VTYPE_PSEUDO_IS
                                                  : VTYPE_NONPSEUDO_IS;
        process_N(spftree, vtype, (void *)is_neigh->neigh_id, dist,
                  depth + 1, family, parent);
      }
    }
    if (lsp->tlv_data.te_is_neighs)
    {
      for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.te_is_neighs, node,
                                te_is_neigh))
      {
        if (!memcmp(te_is_neigh->neigh_id, root_sysid, ISIS_SYS_ID_LEN))
        {
          continue;
        }
        if (!memcmp(te_is_neigh->neigh_id, null_sysid, ISIS_SYS_ID_LEN))
          continue;

        if (GET_TE_METRIC(te_is_neigh) >= 16777215)
          continue;

        dist = cost + GET_TE_METRIC(te_is_neigh);
        vtype = LSP_PSEUDO_ID(te_is_neigh->neigh_id) ? VTYPE_PSEUDO_TE_IS
                                                     : VTYPE_NONPSEUDO_TE_IS;
        process_N(spftree, vtype, (void *)te_is_neigh->neigh_id, dist,
                  depth + 1, family, parent);

        //if(LSP_FRAGMENT(lsp->lsp_header->lsp_id) == 0)
        //*att_check += lsp_att_check(lsp,area);
      }
    }
  }

  if (fragnode == NULL)
  {
    fragnode = listhead(lsp->lspu.frags);
  }
  else
  {
    fragnode = listnextnode(fragnode);
  }

  /*
  ������һ����ƬLSP��
  ����÷�ƬLSP��ʣ������ʱ��Ϊ0���򲻲���·�ɼ���
  */
  if (fragnode)
  {
    lsp = listgetdata(fragnode);

    while (lsp == NULL || lsp->lsp_header == NULL || lsp->lsp_header->rem_lifetime == 0)
    {
      if (fragnode)
      {
        fragnode = listnextnode(fragnode);
        if (fragnode)
        {
          lsp = listgetdata(fragnode);
        }
        else
        {
          return ISIS_OK;
        }
      }
      else
      {
        return ISIS_OK;
      }
    }

    goto lspfragloop;
  }

  return ISIS_OK;
}

static int
isis_spf_process_pseudo_lsp(struct isis_spftree *spftree,
                            struct isis_lsp *lsp0, uint32_t cost,
                            uint16_t depth, int family,
                            u_char *root_sysid,
                            struct isis_vertex *parent)
{
  struct listnode *node, *fragnode = NULL;
  struct is_neigh *is_neigh;
  struct te_is_neigh *te_is_neigh;
  enum vertextype vtype;
  uint32_t dist;
  struct isis_lsp *lsp = lsp0;
  if (cost >= 16777215)
    return ISIS_OK;

pseudofragloop:

  if (lsp->lsp_header->seq_num == 0)
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "isis_spf_process_pseudo_lsp(): lsp with 0 seq_num"
                                    " - do not process");
    return ISIS_WARNING;
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: process_pseudo_lsp %s",
             print_sys_hostname(lsp->lsp_header->lsp_id));

  /* RFC3787 section 4 SHOULD ignore overload bit in pseudo LSPs */

  if (lsp->tlv_data.is_neighs)
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.is_neighs, node, is_neigh))
    {
      /* Two way connectivity */
      if (!memcmp(is_neigh->neigh_id, root_sysid, ISIS_SYS_ID_LEN))
        continue;
      dist = cost + is_neigh->metrics.metric_default;
      vtype = LSP_PSEUDO_ID(is_neigh->neigh_id) ? VTYPE_PSEUDO_IS
                                                : VTYPE_NONPSEUDO_IS;
      process_N(spftree, vtype, (void *)is_neigh->neigh_id, dist,
                depth + 1, family, parent);
    }
  if (lsp->tlv_data.te_is_neighs)
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.te_is_neighs, node, te_is_neigh))
    {
      /* Two way connectivity */
      if (!memcmp(te_is_neigh->neigh_id, root_sysid, ISIS_SYS_ID_LEN))
        continue;
      if (GET_TE_METRIC(te_is_neigh) >= 16777215)
        continue;
      dist = cost + GET_TE_METRIC(te_is_neigh);
      vtype = LSP_PSEUDO_ID(te_is_neigh->neigh_id) ? VTYPE_PSEUDO_TE_IS
                                                   : VTYPE_NONPSEUDO_TE_IS;
      process_N(spftree, vtype, (void *)te_is_neigh->neigh_id, dist,
                depth + 1, family, parent);
    }

  if (fragnode == NULL)
    fragnode = listhead(lsp->lspu.frags);
  else
    fragnode = listnextnode(fragnode);

  if (fragnode)
  {
    lsp = listgetdata(fragnode);
    goto pseudofragloop;
  }

  return ISIS_OK;
}

static int
isis_spf_preload_tent(struct isis_spftree *spftree, int level,
                      int family, u_char *root_sysid,
                      struct isis_vertex *parent)
{
  struct isis_circuit *circuit;
  struct listnode *cnode, *anode, *ipnode, *te_node;
  struct isis_adjacency *adj;
  struct isis_lsp *lsp;
  struct list *adj_list;
  struct list *adjdb;
  struct prefix_ipv4 *ipv4;
  struct prefix_ipv6 *ipv6;
  struct prefix prefix;
  struct te_is_neigh *te_is_neigh;
  int retval = ISIS_OK;
  u_char lsp_id[ISIS_SYS_ID_LEN + 2];
  static u_char null_lsp_id[ISIS_SYS_ID_LEN + 2];
  uint32_t dist;

  /*the flag will be set 1 when metric greater than of equal to 16777215*/
  u_char flag = 0;

  for (ALL_LIST_ELEMENTS_RO(spftree->area->circuit_list, cnode, circuit))
  {
    if (circuit->state != C_STATE_UP)
      continue;
    if (!(circuit->is_type & level))
      continue;
    if (circuit->te_metric[level - 1] >= 16777215 && family == AF_INET)
      continue;
    if (circuit->te_metric_v6[level - 1] >= 16777215 && family == AF_INET6)
      continue;
    if (family == AF_INET && !circuit->ip_router)
      continue;
#if 1
    if (family == AF_INET6 && !circuit->ipv6_router)
      continue;
#endif /* 0 */
    /* 
       * Add IP(v6) addresses of this circuit
       */
    if (family == AF_INET)
    {
      prefix.family = AF_INET;
      for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, ipnode, ipv4))
      {
        //prefix.u.prefix4 = ipv4->prefix;
        prefix.u.prefix4.s_addr = htonl(ipv4->prefix.s_addr);
        prefix.prefixlen = ipv4->prefixlen;
        apply_mask(&prefix);
        if (spftree->area->oldmetric)
        {
          isis_spf_add_local(spftree, VTYPE_IPREACH_INTERNAL, &prefix,
                             NULL, 0, family, parent);
        }
        else
        {
          isis_spf_add_local(spftree, VTYPE_IPREACH_TE, &prefix,
                             NULL, 0, family, parent);
        }
      }
    }

#if 1
    if (family == AF_INET6)
    {
      prefix.family = AF_INET6;
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_non_link, ipnode, ipv6))
      {
        prefix.prefixlen = ipv6->prefixlen;
        prefix.u.prefix6 = ipv6->prefix;
        apply_mask(&prefix);
        isis_spf_add_local(spftree, VTYPE_IP6REACH_INTERNAL,
                           &prefix, NULL, 0, family, parent);
      }
    }
#endif /* 0 */
    if (circuit->circ_type == CIRCUIT_T_BROADCAST)
    {
      /*
	   * Add the adjacencies
	   */
      adj_list = list_new();
      adjdb = circuit->u.bc.adjdb[level - 1];
      lsp = lsp_search(lsp_id, spftree->area->lspdb[level - 1]);

      isis_adj_build_up_list(adjdb, adj_list);
      if (listcount(adj_list) == 0)
      {
        list_delete(adj_list);
        adj_list = NULL;
        //if (isis->debugs & DEBUG_SPF_EVENTS)
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: no L%d adjacencies on circuit %s",
                   level, circuit->interface->name);
        continue;
      }
      for (ALL_LIST_ELEMENTS_RO(adj_list, anode, adj))
      {
        if (!speaks(&adj->nlpids, family))
          continue;
        /*  resolve the problem that when oppsite set the metric 16777215 
		  												   by zdw 2018.4.24    */
        memset(lsp_id, 0, ISIS_SYS_ID_LEN + 2);
        memcpy(lsp_id, adj->sysid, ISIS_SYS_ID_LEN);
        lsp = lsp_search(lsp_id, spftree->area->lspdb[level - 1]);

        if (lsp != NULL)
        {
          if (lsp->tlv_data.te_is_neighs)
          {
            for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.te_is_neighs, te_node, te_is_neigh))
            {
              if (!memcmp(te_is_neigh->neigh_id, null_lsp_id, ISIS_SYS_ID_LEN))
                continue;

              if (!memcmp(te_is_neigh->neigh_id, circuit->u.bc.l1_desig_is, ISIS_SYS_ID_LEN + 1))
              {
                dist = GET_TE_METRIC(te_is_neigh);
                if (dist >= 16777215)
                {
                  adj->cost_out_of_range = 1;
                  flag = 1;
                }
                else
                {
                  adj->cost_out_of_range = 0;
                  flag = 0;
                }
              }
            }
          }
        }

        if (flag == 1)
        {
          flag = 0;
          continue;
        }

        switch (adj->sys_type)
        {
        case ISIS_SYSTYPE_ES:
          if(family == AF_INET)
            isis_spf_add_local(spftree, VTYPE_ES, adj->sysid, adj,
                              circuit->te_metric[level - 1],
                              family, parent);
          if (family == AF_INET6)
            isis_spf_add_local(spftree, VTYPE_ES, adj->sysid, adj,
                              circuit->te_metric_v6[level - 1],
                              family, parent);  
          break;
        case ISIS_SYSTYPE_IS:
        case ISIS_SYSTYPE_L1_IS:
        case ISIS_SYSTYPE_L2_IS:
          if (!spftree->area->oldmetric)
          {
            if(family == AF_INET)
              isis_spf_add_local(spftree,
                                spftree->area->oldmetric ? VTYPE_NONPSEUDO_IS : VTYPE_NONPSEUDO_TE_IS,
                                adj->sysid, adj,
                                circuit->te_metric[level - 1],
                                family, parent);
            if (family == AF_INET6)
              isis_spf_add_local(spftree,
                                spftree->area->oldmetric ? VTYPE_NONPSEUDO_IS : VTYPE_NONPSEUDO_TE_IS,
                                adj->sysid, adj,
                                circuit->te_metric_v6[level - 1],
                                family, parent);
          }  
          if (spftree->area->oldmetric)
          {
            if(family == AF_INET)
              isis_spf_add_local(spftree,
                                spftree->area->oldmetric ? VTYPE_NONPSEUDO_IS : VTYPE_NONPSEUDO_TE_IS,
                                adj->sysid, adj,
                                circuit->metrics[level - 1].metric_default,
                                family, parent);
            if (family == AF_INET6)
              isis_spf_add_local(spftree,
                                spftree->area->oldmetric ? VTYPE_NONPSEUDO_IS : VTYPE_NONPSEUDO_TE_IS,
                                adj->sysid, adj,
                                circuit->metrics_v6[level - 1].metric_default,
                                family, parent);
          }
          memcpy(lsp_id, adj->sysid, ISIS_SYS_ID_LEN);
          LSP_PSEUDO_ID(lsp_id) = 0;
          LSP_FRAGMENT(lsp_id) = 0;
          lsp = lsp_search(lsp_id, spftree->area->lspdb[level - 1]);
          if (lsp == NULL || lsp->lsp_header->rem_lifetime == 0)
            zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: No LSP %s found for IS adjacency "
                                            "L%d on %s (ID %u)",
                       rawlspid_print(lsp_id), level,
                       circuit->interface->name, circuit->circuit_id);
          break;
        case ISIS_SYSTYPE_UNKNOWN:
        default:
          zlog_debug(ISIS_DEBUG_TYPE_SPF, "isis_spf_preload_tent unknow adj type");
        }
      }
      list_delete(adj_list);
      adj_list = NULL;
      /*
	   * Add the pseudonode 
	   */
      if (level == 1)
        memcpy(lsp_id, circuit->u.bc.l1_desig_is, ISIS_SYS_ID_LEN + 1);
      else
        memcpy(lsp_id, circuit->u.bc.l2_desig_is, ISIS_SYS_ID_LEN + 1);
      /* can happen during DR reboot */
      if (memcmp(lsp_id, null_lsp_id, ISIS_SYS_ID_LEN + 1) == 0)
      {
        // if (isis->debugs & DEBUG_SPF_EVENTS)
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: No L%d DR on %s (ID %d)",
                   level, circuit->interface->name, circuit->circuit_id);
        continue;
      }
      adj = isis_adj_lookup(lsp_id, adjdb);
      /* if no adj, we are the dis or error */
      if (!adj && !circuit->u.bc.is_dr[level - 1])
      {
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: No adjacency found from root "
                                        "to L%d DR %s on %s (ID %d)",
                   level, rawlspid_print(lsp_id),
                   circuit->interface->name, circuit->circuit_id);
        continue;
      }
      lsp = lsp_search(lsp_id, spftree->area->lspdb[level - 1]);
      if (lsp == NULL || lsp->lsp_header->rem_lifetime == 0)
      {
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: No lsp (%p) found from root "
                                        "to L%d DR %s on %s (ID %d)",
                   (void *)lsp, level, rawlspid_print(lsp_id),
                   circuit->interface->name, circuit->circuit_id);
        continue;
      }
      if(family == AF_INET)
          isis_spf_process_pseudo_lsp(spftree, lsp,
                                      circuit->te_metric[level - 1], 0,
                                      family, root_sysid, parent);
      if(family == AF_INET6)
          isis_spf_process_pseudo_lsp(spftree, lsp,
                                      circuit->te_metric_v6[level - 1], 0,
                                      family, root_sysid, parent);
    }
    else if (circuit->circ_type == CIRCUIT_T_P2P)
    {
      adj = circuit->u.p2p.neighbor;
      if (!adj)
        continue;
      switch (adj->sys_type)
      {
      case ISIS_SYSTYPE_ES:
      if(family == AF_INET)
        isis_spf_add_local(spftree, VTYPE_ES, adj->sysid, adj,
                           circuit->te_metric[level - 1], family,
                           parent);
      if(family == AF_INET6)
        isis_spf_add_local(spftree, VTYPE_ES, adj->sysid, adj,
                           circuit->te_metric_v6[level - 1], family,
                           parent);
        break;
      case ISIS_SYSTYPE_IS:
      case ISIS_SYSTYPE_L1_IS:
      case ISIS_SYSTYPE_L2_IS:
        if (speaks(&adj->nlpids, family))
        {
          if(family == AF_INET)
            isis_spf_add_local(spftree,
                             spftree->area->oldmetric ? VTYPE_NONPSEUDO_IS : VTYPE_NONPSEUDO_TE_IS,
                             adj->sysid,
                             adj, circuit->te_metric[level - 1],
                             family, parent);

          if(family == AF_INET6)
            isis_spf_add_local(spftree,
                             spftree->area->oldmetric ? VTYPE_NONPSEUDO_IS : VTYPE_NONPSEUDO_TE_IS,
                             adj->sysid,
                             adj, circuit->te_metric_v6[level - 1],
                             family, parent);
        }
        break;
      case ISIS_SYSTYPE_UNKNOWN:
      default:
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "isis_spf_preload_tent unknown adj type");
        break;
      }
    }
    else if (circuit->circ_type == CIRCUIT_T_LOOPBACK)
    {
      continue;
    }
    else
    {
      zlog_debug(ISIS_DEBUG_TYPE_SPF, "isis_spf_preload_tent unsupported media");
      retval = ISIS_WARNING;
    }
  }

  return retval;
}

int lsp_area_addr_lookup(struct area_addr *addr, struct isis_area *area)
{
  struct listnode *anode;
  struct area_addr *addrl;

  for (ALL_LIST_ELEMENTS_RO(area->area_addrs, anode, addrl))
  {
    if (addr->addr_len == addrl->addr_len && memcmp(addr->area_addr, addrl->area_addr, addr->addr_len) == 0)
    {
      return 1;
    }
  }
  return 0;
}
int lsp_att_check(struct isis_lsp *lsp, struct isis_area *area)
{
  struct listnode *lnode;
  struct area_addr *addrl;
  u_char sysid[ISIS_SYS_ID_LEN];

  memcpy(sysid, lsp->lsp_header->lsp_id, ISIS_SYS_ID_LEN);

  if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN) == 0)
  {
    return 0;
  }

  if (lsp->tlv_data.area_addrs && listcount(lsp->tlv_data.area_addrs))
  {
    for (ALL_LIST_ELEMENTS_RO(lsp->tlv_data.area_addrs, lnode, addrl))
    {
      if (!lsp_area_addr_lookup(addrl, area))
        return 1;
    }
  }
  return 0;
}

/*
 * The parent(s) for vertex is set when added to TENT list
 * now we just put the child pointer(s) in place
 */
static void
add_to_paths(struct isis_spftree *spftree, struct isis_vertex *vertex,
             int level)
{
  //u_char buff[BUFSIZ];

  if (isis_find_vertex(spftree->paths, vertex->N.id, vertex->type))
    return;
  listnode_add(spftree->paths, vertex);

  //#ifdef EXTREME_DEBUG
  //zlog_debug ("ISIS-Spf: added %s %s %s depth %d dist %d to PATHS",
  //print_sys_hostname (vertex->N.id),
  //vtype2string (vertex->type), vid2string (vertex, buff),
  //vertex->depth, vertex->d_N);
  //#endif /* EXTREME_DEBUG */
  /*
if (vertex->type > VTYPE_ES)
{
	if (listcount (vertex->Adj_N) > 0)
	{
		isis_route_create ((struct prefix *) &vertex->N.prefix, vertex->d_N,
			 vertex->depth, vertex->Adj_N, spftree->area, level);
	}
	else if (isis->debugs & DEBUG_SPF_EVENTS)
      zlog_debug ("ISIS-Spf: no adjacencies do not install route for "
				  "%s depth %d dist %d", vid2string (vertex, buff),
				  vertex->depth, vertex->d_N);
}
*/
  return;
}

static void
init_spt(struct isis_spftree *spftree)
{
  spftree->tents->del = spftree->paths->del = (void (*)(void *))isis_vertex_del;
  list_delete_all_node(spftree->tents);
  list_delete_all_node(spftree->paths);
  spftree->tents->del = spftree->paths->del = NULL;
  return;
}

int isis_spf_check_adj(u_char *sys_id, struct isis_spftree *spftree, int level)
{
  struct isis_circuit *circuit;
  struct listnode *cnode;

  int i = 0;
  for (ALL_LIST_ELEMENTS_RO(spftree->area->circuit_list, cnode, circuit))
  {
    if (isis_adj_lookup(sys_id, circuit->u.bc.adjdb[level - 1]) != NULL)
    {
      i++;
      break;
    }
  }
  return i;
}

static int
isis_run_spf(struct isis_area *area, int level, int family, u_char *sysid)
{
  int retval = ISIS_OK;
  struct listnode *node;
  struct isis_vertex *vertex;
  struct isis_vertex *root_vertex;
  struct isis_spftree *spftree = NULL;
  u_char lsp_id[ISIS_SYS_ID_LEN + 2];
  struct isis_lsp *lsp;
  struct route_table *table = NULL;
  struct timeval time_now;
  unsigned long long start_time, end_time;
  struct list *att = NULL;
  int att_check = 0;
  uchar l1_refreshd_flag = 0;

  if (level == 1)
    att = list_new();
  /* Get time that can't roll backwards. */
  time_get_time(TIME_CLK_MONOTONIC, &time_now);
  start_time = time_now.tv_sec;
  start_time = (start_time * 1000000) + time_now.tv_usec;

  if (family == AF_INET)
    spftree = area->spftree[level - 1];
#if 1
  else if (family == AF_INET6)
    spftree = area->spftree6[level - 1];
#endif

  if (!spftree)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  if (!sysid)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  /* Make all routes in current route table inactive. */
  if (family == AF_INET)
    table = area->route_table[level - 1];
#if 1
  else if (family == AF_INET6)
    table = area->route_table6[level - 1];
#endif
  isis_route_invalidate_table(area, table);

  //isis_direct_route_add(area, table, level, family);

  /*
   * C.2.5 Step 0
   */
  init_spt(spftree);
  /*              a) */
  root_vertex = isis_spf_add_root(spftree, level, sysid);
  /*              b) */
  retval = isis_spf_preload_tent(spftree, level, family, sysid, root_vertex);

  isis_direct_route_add(area, table, level, family);

  if (retval != ISIS_OK)
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: failed to load TENT SPF-root:%s", print_sys_hostname(sysid));
    goto out;
  }

  /*
     * C.2.7 Step 2
     */
  if (listcount(spftree->tents) == 0)
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: TENT is empty SPF-root:%s", print_sys_hostname(sysid));
    goto out;
  }

  while (listcount(spftree->tents) > 0)
  {
    node = listhead(spftree->tents);
    vertex = listgetdata(node);

    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: get TENT node %s %s depth %d dist %d to PATHS",
               print_sys_hostname(vertex->N.id),
               vtype2string(vertex->type), vertex->depth, vertex->d_N);

    /* Remove from tent list and add to paths list */
    list_delete_node(spftree->tents, node);
    add_to_paths(spftree, vertex, level);

    switch (vertex->type)
    {
    case VTYPE_PSEUDO_IS:
    case VTYPE_NONPSEUDO_IS:
    case VTYPE_PSEUDO_TE_IS:
    case VTYPE_NONPSEUDO_TE_IS:
      memcpy(lsp_id, vertex->N.id, ISIS_SYS_ID_LEN + 1);
      LSP_FRAGMENT(lsp_id) = 0;
      lsp = lsp_search(lsp_id, area->lspdb[level - 1]);
      if (lsp && lsp->lsp_header->rem_lifetime != 0)
      {
        if (LSP_PSEUDO_ID(lsp_id))
        {
          isis_spf_process_pseudo_lsp(spftree, lsp, vertex->d_N,
                                      vertex->depth, family, sysid, vertex);
        }
        else
        {
          isis_spf_process_lsp(spftree, lsp, vertex->d_N, vertex->depth,
                               family, sysid, vertex, att, area, &att_check);
        }
      }
      else
      {
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: No LSP found for %s", rawlspid_print(lsp_id));
      }
      break;
    default:
      break;
    }
  }

  for (ALL_LIST_ELEMENTS_RO(spftree->paths, node, vertex))
  {
    switch (vertex->type)
    {
    case VTYPE_PSEUDO_IS:
    case VTYPE_NONPSEUDO_IS:
    case VTYPE_PSEUDO_TE_IS:
    case VTYPE_NONPSEUDO_TE_IS:
      memcpy(lsp_id, vertex->N.id, ISIS_SYS_ID_LEN + 1);
      LSP_FRAGMENT(lsp_id) = 0;
      if (LSP_PSEUDO_ID(lsp_id) != 0)
      {
        LSP_PSEUDO_ID(lsp_id) = 0;
      }
      lsp = lsp_search(lsp_id, area->lspdb[level - 1]);
      if (lsp && lsp->lsp_header->rem_lifetime != 0)
      {
        isis_spf_process_lsp_ip_reach(spftree, lsp, vertex->d_N, vertex->depth,
                                      family, sysid, vertex, att, area, &att_check, level);
      }
      else
      {
        zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf: No LSP found for %s", rawlspid_print(lsp_id));
      }
      break;
    default:
      break;
    }
  }

  if (area->is_type == IS_LEVEL_1_AND_2 && level == IS_LEVEL_2)
  {
    if (att_check > 0)
    {
      if (area->attached_bit == 0)
      {
        area->attached_bit = LSPBIT_ATT;
        lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
        l1_refreshd_flag = 1;
      }
    }
    else
    {
      if (area->attached_bit != 0)
      {
        area->attached_bit = 0;
        lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
        l1_refreshd_flag = 1;
      }
    }

    if (l1_refreshd_flag != 1 && (area->area_import_to_l1 == 1 || area->area_import_to_l1_v6))
    {
      lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    }
  }

out:
  if (level == 1)
  {
    isis_route_create_default(area, att, family);

    struct route_node *rnode;
    struct isis_route_info *rinfo;

    if (family == AF_INET)
    {
      for (rnode = route_top(area->route_table[0]); rnode; rnode = route_next(rnode))
      {
        if (rnode->info == NULL)
          continue;

        rinfo = rnode->info;
        if (!CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
        {
          area->force_l2_lsp_refresh = 1;
          break;
        }
      }
    }

    if (family == AF_INET6)
    {
      for (rnode = route_top(area->route_table6[0]); rnode; rnode = route_next(rnode))
      {
        if (rnode->info == NULL)
          continue;

        rinfo = rnode->info;
        if (!CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
        {
          area->force_l2_lsp_refresh = 1;
          break;
        }
      }
    }
  }
  isis_route_validate_after_spf(area, family);

  spftree->pending = 0;
  spftree->runcount++;
  spftree->last_run_timestamp = time(NULL);
  time_get_time(TIME_CLK_MONOTONIC, &time_now);
  end_time = time_now.tv_sec;
  end_time = (end_time * 1000000) + time_now.tv_usec;
  spftree->last_run_duration = end_time - start_time;
  return retval;
}

int isis_run_spf_l1(void *argv)
{
  struct isis_area *area;
  struct timeval time_now;
  unsigned long long start_time, end_time, run_duration;
  int retval = ISIS_OK;

  area = (struct isis_area *)argv;
  
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  area->spftree[0]->t_spf = 0;
  area->spftree[0]->pending = 0;
  area->force_l2_lsp_refresh = 0;

  if (!(area->is_type & IS_LEVEL_1))
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-SPF (%s) area does not share level", area->area_tag);
    return ISIS_WARNING;
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf (%s) L1 SPF needed, periodic SPF", area->area_tag);

  if (area->ip_circuits || area->force_spf_l1)
  {
    //retval = isis_run_spf (area, 1, AF_INET, isis->sysid);
    time_get_time(TIME_CLK_MONOTONIC, &time_now);
    start_time = time_now.tv_sec;
    start_time = (start_time * 1000000) + time_now.tv_usec;
    retval = isis_run_spf(area, 1, AF_INET, area->sysid);
    time_get_time(TIME_CLK_MONOTONIC, &time_now);
    end_time = time_now.tv_sec;
    end_time = (end_time * 1000000) + time_now.tv_usec;
    run_duration = end_time - start_time;
    if (run_duration > isis_sta.duration_time)
      isis_sta.duration_time = run_duration;
  }
  if (area->ip_circuits == 0)
    area->force_spf_l1 = 0;

  if (area->is_type == IS_LEVEL_1_AND_2 && area->force_l2_lsp_refresh == 1)
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);

  return retval;
}

int isis_run_spf_l2(void *argv)
{
  struct isis_area *area;
  int retval = ISIS_OK;

  area = (struct isis_area *)argv;
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  area->spftree[1]->t_spf = 0;
  area->spftree[1]->pending = 0;

  if (!(area->is_type & IS_LEVEL_2))
  {
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-SPF (%s) area does not share level", area->area_tag);
    return ISIS_WARNING;
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf (%s) L2 SPF needed, periodic SPF", area->area_tag);

  if (area->ip_circuits || area->force_spf_l2)
  {
    retval = isis_run_spf(area, 2, AF_INET, area->sysid);
  }

  if (area->ip_circuits == 0)
    area->force_spf_l2 = 0;
  return retval;
}

int isis_spf_schedule(struct isis_area *area, int level)
{
  struct isis_spftree *spftree = area->spftree[level - 1];
  time_t now = time(NULL);
  int diff = now - spftree->last_run_timestamp;

  //assert (diff >= 0);
  if (!(diff >= 0))
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  //assert (area->is_type & level);
  if (!(area->is_type & level))
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  //  if (isis->debugs & DEBUG_SPF_EVENTS)
  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf (%s) L%d SPF schedule called, lastrun %d sec ago",
             area->area_tag, level, diff);

  if (spftree->pending)
    return ISIS_OK;

  THREAD_TIMER_OFF_NEW(spftree->t_spf);
  spftree->t_spf = 0;

  /* wait configured min_spf_interval before doing the SPF */
  /*
	�����з�Ƭʱ�㷨�ļ������
	*/
  if (diff >= area->min_spf_interval[level - 1])
  {
    //return isis_run_spf (area, level, AF_INET, isis->sysid);
    if (level == 1)
    {
      THREAD_TIMER_MSEC_ON_NEW((char *)"ISISRunL1Spf", spftree->t_spf, isis_run_spf_l1, area, 100);
    }
    else
    {
      THREAD_TIMER_MSEC_ON_NEW((char *)"ISISRunL2Spf", spftree->t_spf, isis_run_spf_l2, area, 100);
    }

    spftree->pending = 1;
    return ISIS_OK;
  }

  if (level == ISIS_LEVEL1)
  {
    THREAD_TIMER_MSEC_ON_NEW((char *)"ISISRunL1Spf", spftree->t_spf, isis_run_spf_l1, area,
                             area->min_spf_interval[0] * 1000 - diff);
  }
  else
  {
    THREAD_TIMER_MSEC_ON_NEW((char *)"ISISRunL2Spf", spftree->t_spf, isis_run_spf_l2, area,
                             area->min_spf_interval[1] * 1000 - diff);
  }

  // if (isis->debugs & DEBUG_SPF_EVENTS)
  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISIS-Spf (%s) L%d SPF scheduled %d sec from now",
             area->area_tag, level, area->min_spf_interval[level - 1] - diff);

  spftree->pending = 1;

  return ISIS_OK;
}

#if 1
int isis_run_spf6_l1(void *argv)
{
  struct isis_area *area;
  struct timeval time_now;
  unsigned long long start_time, end_time, run_duration;
  int retval = ISIS_OK;
  area = (struct isis_area *)argv;
  //area = THREAD_ARG (thread);
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  area->spftree6[0]->t_spf = 0;
  area->spftree6[0]->pending = 0;
  area->force_l2_lsp_refresh = 0;

  if (!(area->is_type & IS_LEVEL_1))
  {
    //if (isis->debugs & DEBUG_SPF_EVENTS)
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISISv6-SPF (%s) area does not share level",
               area->area_tag);
    return ISIS_WARNING;
  }

  // if (isis->debugs & DEBUG_SPF_EVENTS)
  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISISv6-Spf (%s) L1 SPF needed, periodic SPF", area->area_tag);

  if (area->ipv6_circuits || area->force_spf_v6_l1)
  {
    //retval = isis_run_spf (area, 1, AF_INET, isis->sysid);
    time_get_time(TIME_CLK_MONOTONIC, &time_now);
    start_time = time_now.tv_sec;
    start_time = (start_time * 1000000) + time_now.tv_usec;
    retval = isis_run_spf(area, 1, AF_INET6, area->sysid);
    time_get_time(TIME_CLK_MONOTONIC, &time_now);
    end_time = time_now.tv_sec;
    end_time = (end_time * 1000000) + time_now.tv_usec;
    run_duration = end_time - start_time;
    if (run_duration > isis_sta.duration_time)
      isis_sta.duration_time = run_duration;
  }

  if (area->ipv6_circuits == 0)
    area->force_spf_v6_l1 = 0;

  if (area->is_type == IS_LEVEL_1_AND_2 && area->force_l2_lsp_refresh == 1)
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);

  return retval;
}

int isis_run_spf6_l2(void *argv)
{
  struct isis_area *area;
  int retval = ISIS_OK;
  area = (struct isis_area *)argv;
  //area = THREAD_ARG (thread);
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  area->spftree6[1]->t_spf = 0;
  area->spftree6[1]->pending = 0;

  if (!(area->is_type & IS_LEVEL_2))
  {
    // if (isis->debugs & DEBUG_SPF_EVENTS)
    zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISISv6-SPF (%s) area does not share level", area->area_tag);
    return ISIS_WARNING;
  }

  // if (isis->debugs & DEBUG_SPF_EVENTS)
  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISISv6-Spf (%s) L2 SPF needed, periodic SPF", area->area_tag);

  if (area->ipv6_circuits || area->force_spf_v6_l2)
  {
    retval = isis_run_spf(area, 2, AF_INET6, area->sysid);
  }

  if (area->ipv6_circuits == 0)
    area->force_spf_v6_l2 = 0;
  return retval;
}

int isis_spf_schedule6(struct isis_area *area, int level)
{
  int retval = ISIS_OK;
  struct isis_spftree *spftree = area->spftree6[level - 1];
  time_t now = time(NULL);
  time_t diff = now - spftree->last_run_timestamp;

  if (!(diff >= 0))
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  if (!(area->is_type & level))
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return ISIS_ERROR;
  }

  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISISv6-Spf (%s) L%d SPF schedule called, lastrun %lld sec ago",
             area->area_tag, level, (long long)diff);

  if (spftree->pending)
    return ISIS_OK;

  THREAD_TIMER_OFF_NEW(spftree->t_spf);
  spftree->t_spf = 0;

  /* wait configured min_spf_interval before doing the SPF */
  if (diff >= area->min_spf_interval[level - 1])
  {
    if (level == 1)
    {
      THREAD_TIMER_MSEC_ON_NEW((char *)"ISISV6RunL1Spf", spftree->t_spf, isis_run_spf6_l1, area, 100);
    }
    else
    {
      THREAD_TIMER_MSEC_ON_NEW((char *)"ISISV6RunL2Spf", spftree->t_spf, isis_run_spf6_l2, area, 100);
    }

    spftree->pending = 1;
    return ISIS_OK;
  }

  if (level == 1)
  {
    THREAD_TIMER_MSEC_ON_NEW((char *)"ISISV6RunL1Spf", spftree->t_spf, isis_run_spf6_l1, area,
                             area->min_spf_interval[0] * 1000 - diff);
  }
  else
  {
    THREAD_TIMER_MSEC_ON_NEW((char *)"ISISV6RunL2Spf", spftree->t_spf, isis_run_spf6_l2, area,
                             area->min_spf_interval[1] * 1000 - diff);
  }

  //  if (isis->debugs & DEBUG_SPF_EVENTS)
  zlog_debug(ISIS_DEBUG_TYPE_SPF, "ISISv6-Spf (%s) L%d SPF scheduled %lld sec from now",
             area->area_tag, level,
             (long long)(area->min_spf_interval[level - 1] - diff));

  spftree->pending = 1;

  return retval;
}
#endif

static void
isis_print_paths(struct vty *vty, struct list *paths, u_char *root_sysid)
{
  struct listnode *node;
  struct listnode *anode;
  struct isis_vertex *vertex;
  struct isis_adjacency *adj;
  u_char buff[BUFSIZ];

  vty_out(vty, "Vertex               Type         Metric "
               "  Next-Hop       Interface             Parent%s",
          VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(paths, node, vertex))
  {
    if (memcmp(vertex->N.id, root_sysid, ISIS_SYS_ID_LEN) == 0)
    {
      vty_out(vty, "%-20s %-12s %-8s", sysid_print(root_sysid), "", "");
      vty_out(vty, "%-34s", "");
    }
    else
    {
      int rows = 0;
      if (vertex->type > 5)
        continue;
      vty_out(vty, "%-20s %-12s %-8u ", vid2string(vertex, buff),
              vtype2string(vertex->type), vertex->d_N);
      for (ALL_LIST_ELEMENTS_RO(vertex->Adj_N, anode, adj))
      {
        if (adj)
        {
          if (rows)
          {
            vty_out(vty, "%s", VTY_NEWLINE);
            vty_out(vty, "%-20s %-12s %-8s ", "", "", "");
          }
          vty_out(vty, "%-12s %-17s ", sysid_print(adj->sysid), adj->circuit->interface->name);
          ++rows;
        }
      }
      if (rows == 0)
        vty_out(vty, "%-36s ", "");
    }

    /* Print list of parents for the ECMP DAG */
    if (listcount(vertex->parents) > 0)
    {
      struct listnode *pnode;
      struct isis_vertex *pvertex;
      int rows = 0;
      for (ALL_LIST_ELEMENTS_RO(vertex->parents, pnode, pvertex))
      {
        if (rows)
        {
          vty_out(vty, "%s", VTY_NEWLINE);
          vty_out(vty, "%-72s", "");
        }
        vty_out(vty, "%s",
                vid2string(pvertex, buff));
        ++rows;
      }
    }
    else
    {
      vty_out(vty, "    NULL ");
    }

#if 0
      if (listcount (vertex->children) > 0) {
	  struct listnode *cnode;
	  struct isis_vertex *cvertex;
	  for (ALL_LIST_ELEMENTS_RO (vertex->children, cnode, cvertex)) {
	      vty_out (vty, "%s", VTY_NEWLINE);
	      vty_out (vty, "%-72s", "");
	      vty_out (vty, "%s(%d) ", 
	               vid2string (cvertex, buff), cvertex->type);
	    }
	}
#endif
    vty_out(vty, "%s", VTY_NEWLINE);
  }
}

DEFUN(show_isis_topology,
      show_isis_topology_cmd,
      "show isis topology",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS paths to Intermediate Systems\n")
{
  struct listnode *node;
  struct isis_area *area;
  int level;
  int flag = 0;

  if (!isis->area_list || isis->area_list->count == 0)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);
    for (level = 0; level < ISIS_LEVELS; level++)
    {
      if (area->ip_circuits > 0 && area->spftree[level] && area->spftree[level]->paths->count > 0)
      {
        flag++;
        vty_out(vty, "IS-IS paths to level-%d routers that speak IP%s",
                level + 1, VTY_NEWLINE);
        //isis_print_paths (vty, area->spftree[level]->paths, isis->sysid);
        isis_print_paths(vty, area->spftree[level]->paths, area->sysid);
        vty_out(vty, "%s", VTY_NEWLINE);
      }
#if 0
	  if (area->ipv6_circuits > 0 && area->spftree6[level]
	      && area->spftree6[level]->paths->count > 0)
	    {
	      vty_out (vty,
		       "IS-IS paths to level-%d routers that speak IPv6%s",
		       level + 1, VTY_NEWLINE);
	      //isis_print_paths (vty, area->spftree6[level]->paths, isis->sysid);
	      isis_print_paths (vty, area->spftree6[level]->paths, area->sysid);
	      vty_out (vty, "%s", VTY_NEWLINE);
	    }
#endif /* 0 */
    }

    vty_out(vty, "%s", VTY_NEWLINE);
  }

  if (flag == 0)
    vty_out(vty, "No topology in this process%s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

DEFUN(show_isis_topology_l1,
      show_isis_topology_l1_cmd,
      "show isis topology level-1",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS paths to Intermediate Systems\n"
      "Paths to all level-1 routers in the area\n")
{
  struct listnode *node;
  struct isis_area *area;

  if (!isis->area_list || isis->area_list->count == 0)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);

    if (area->ip_circuits > 0 && area->spftree[0] && area->spftree[0]->paths->count > 0)
    {
      vty_out(vty, "IS-IS paths to level-1 routers that speak IP%s",
              VTY_NEWLINE);
      //isis_print_paths (vty, area->spftree[0]->paths, isis->sysid);
      isis_print_paths(vty, area->spftree[0]->paths, area->sysid);
      vty_out(vty, "%s", VTY_NEWLINE);
    }
#if 0
      if (area->ipv6_circuits > 0 && area->spftree6[0]
	  && area->spftree6[0]->paths->count > 0)
	{
	  vty_out (vty, "IS-IS paths to level-1 routers that speak IPv6%s",
		   VTY_NEWLINE);
	  //isis_print_paths (vty, area->spftree6[0]->paths, isis->sysid);
	  isis_print_paths (vty, area->spftree6[0]->paths, area->sysid);
	  vty_out (vty, "%s", VTY_NEWLINE);
	}
#endif /* 0 */
    vty_out(vty, "%s", VTY_NEWLINE);
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_topology_l2,
      show_isis_topology_l2_cmd,
      "show isis topology level-2",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS paths to Intermediate Systems\n"
      "Paths to all level-2 routers in the domain\n")
{
  struct listnode *node;
  struct isis_area *area;

  if (!isis->area_list || isis->area_list->count == 0)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);

    if (area->ip_circuits > 0 && area->spftree[1] && area->spftree[1]->paths->count > 0)
    {
      vty_out(vty, "IS-IS paths to level-2 routers that speak IP%s",
              VTY_NEWLINE);
      //isis_print_paths (vty, area->spftree[1]->paths, isis->sysid);
      isis_print_paths(vty, area->spftree[1]->paths, area->sysid);
      vty_out(vty, "%s", VTY_NEWLINE);
    }
#if 0
      if (area->ipv6_circuits > 0 && area->spftree6[1]
	  && area->spftree6[1]->paths->count > 0)
	{
	  vty_out (vty, "IS-IS paths to level-2 routers that speak IPv6%s",
		   VTY_NEWLINE);
	  //isis_print_paths (vty, area->spftree6[1]->paths, isis->sysid);
	  isis_print_paths (vty, area->spftree6[1]->paths, area->sysid);
	  vty_out (vty, "%s", VTY_NEWLINE);
	}
#endif /* 0 */
    vty_out(vty, "%s", VTY_NEWLINE);
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_topology_h3c,
      show_isis_topology_h3c_cmd,
      "display isis topology",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "IS-IS paths to Intermediate Systems\n")
{
  struct listnode *node;
  struct isis_area *area;
  int level;
  int flag = 0;

  if (!isis->area_list || isis->area_list->count == 0)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);
    for (level = 0; level < ISIS_LEVELS; level++)
    {
      if (area->ip_circuits > 0 && area->spftree[level] && area->spftree[level]->paths->count > 0)
      {
        flag++;
        vty_out(vty, "IS-IS paths to level-%d routers that speak IP%s",
                level + 1, VTY_NEWLINE);
        //isis_print_paths (vty, area->spftree[level]->paths, isis->sysid);
        isis_print_paths(vty, area->spftree[level]->paths, area->sysid);
        vty_out(vty, "%s", VTY_NEWLINE);
      }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
  }

  if (flag == 0)
    vty_out(vty, "No topology in this process%s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

DEFUN(show_isis_topology_l1_h3c,
      show_isis_topology_l1_h3c_cmd,
      "display isis topology level-1",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "IS-IS paths to Intermediate Systems\n"
      "Paths to all level-1 routers in the area\n")
{
  struct listnode *node;
  struct isis_area *area;

  if (!isis->area_list || isis->area_list->count == 0)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);

    if (area->ip_circuits > 0 && area->spftree[0] && area->spftree[0]->paths->count > 0)
    {
      vty_out(vty, "IS-IS paths to level-1 routers that speak IP%s",
              VTY_NEWLINE);
      //isis_print_paths (vty, area->spftree[0]->paths, isis->sysid);
      isis_print_paths(vty, area->spftree[0]->paths, area->sysid);
      vty_out(vty, "%s", VTY_NEWLINE);
    }
    vty_out(vty, "%s", VTY_NEWLINE);
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_topology_l2_h3c,
      show_isis_topology_l2_h3c_cmd,
      "display isis topology level-2",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "IS-IS paths to Intermediate Systems\n"
      "Paths to all level-2 routers in the domain\n")
{
  struct listnode *node;
  struct isis_area *area;

  if (!isis->area_list || isis->area_list->count == 0)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);

    if (area->ip_circuits > 0 && area->spftree[1] && area->spftree[1]->paths->count > 0)
    {
      vty_out(vty, "IS-IS paths to level-2 routers that speak IP%s",
              VTY_NEWLINE);
      isis_print_paths(vty, area->spftree[1]->paths, area->sysid);
      vty_out(vty, "%s", VTY_NEWLINE);
    }
    vty_out(vty, "%s", VTY_NEWLINE);
  }
  return CMD_SUCCESS;
}

void isis_spf_cmds_init()
{
  install_element(CONFIG_NODE, &show_isis_topology_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_topology_l1_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_topology_l2_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_topology_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_topology_l1_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_topology_l2_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_topology_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_topology_l1_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_topology_l2_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_topology_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_topology_l1_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_topology_l2_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_topology_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_topology_l1_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_topology_l2_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_topology_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_topology_l1_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_topology_l2_h3c_cmd, CMD_LOCAL);
}
