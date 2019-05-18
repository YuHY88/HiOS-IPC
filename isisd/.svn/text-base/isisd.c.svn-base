/*
 * IS-IS Rout(e)ing protocol - isisd.c
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
#include "vty.h"
#include "command.h"
#include "log.h"
#include "memory.h"
#include "time.h"
#include "linklist.h"
#include "if.h"
#include "hash.h"
#include "stream.h"
#include "prefix.h"
#include "table.h"
#include "ifm_common.h"

#include "isisd/dict.h"
#include "isisd/include-netbsd/iso.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_csm.h"
#include "isisd/isisd.h"
#include "isisd/isis_dynhn.h"
#include "isisd/isis_adjacency.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_misc.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_tlv.h"
#include "isisd/isis_lsp.h"
#include "isisd/isis_spf.h"
#include "isisd/isis_route.h"
#include "isisd/isis_zebra.h"
#include "isisd/isis_events.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_ipc.h"

#ifdef TOPOLOGY_GENERATE
#include "spgrid.h"
u_char DEFAULT_TOPOLOGY_BASEIS[6] = {0xFE, 0xED, 0xFE, 0xED, 0x00, 0x00};
#endif /* TOPOLOGY_GENERATE */

extern int inet_aton(const char *cp, struct in_addr *addr);

struct isis_master isis_m;

struct isis *isis = NULL;
extern struct isis_statistics isis_sta;
//extern uint32_t  ifm_get_ifindex_by_name ( const char *type, char *ifname );
//extern int ifm_get_name_by_ifindex ( uint32_t ifindex, char *name );
extern char *inet_ntoa(struct in_addr in);
/*
 * Prototypes.
 */
int isis_area_get(struct vty *, const char *, u_int16_t vpn);
int isis_area_destroy(struct vty *, const char *);
int area_net_title(struct vty *, const char *);
int area_clear_net_title(struct vty *, const char *);
int show_isis_interface_common(struct vty *, const char *ifname, char);
int show_isis_neighbor_common(struct vty *, const char *id, char);
int clear_isis_neighbor_common(struct vty *, const char *id);
int isis_config_write(struct vty *);

void isis_new(unsigned long process_id)
{
  isis = XCALLOC(MTYPE_ISIS, sizeof(struct isis));
  /*
   * Default values
   */
  isis->max_area_addrs = 3;
  isis->process_id = process_id;
  isis->router_id = 0;
  isis->area_list = list_new();
  isis->init_circ_list = list_new();
  isis->uptime = time(NULL);
  isis->nexthops = list_new();
#if 1
  isis->nexthops6 = list_new();
#endif /* 0 */
  dyn_cache_init();
  /*
   * uncomment the next line for full debugs
   */
  /* isis->debugs = 0xFFFF; */
}

struct isis_area *
isis_area_create(const char *area_tag, u_int16_t vpn)
{
  struct isis_area *area;

  area = XCALLOC(MTYPE_ISIS_AREA, sizeof(struct isis_area));

  /*
   * The first instance is level-1-2 rest are level-1, unless otherwise
   * configured
   */

  area->is_type = IS_LEVEL_1_AND_2;
  area->force_spf_l1 = 0;
  area->force_spf_l2 = 0;
  area->force_spf_v6_l1 = 0;
  area->force_spf_v6_l2 = 0;
  area->vpn = vpn;
  /*
   * intialize the databases
   */
  if (area->is_type & IS_LEVEL_1)
  {
    area->lspdb[0] = lsp_db_init();
    area->route_table[0] = route_table_init();
#if 1
    area->route_table6[0] = route_table_init();
#endif /* 0 */
  }
  if (area->is_type & IS_LEVEL_2)
  {
    area->lspdb[1] = lsp_db_init();
    area->route_table[1] = route_table_init();
#if 1
    area->route_table6[1] = route_table_init();
#endif /* 0 */
  }

  spftree_area_init(area);

  area->circuit_list = list_new();
  area->area_addrs = list_new();
  THREAD_TIMER_ON_NEW((char *)"ISISLspTick", area->t_tick, lsp_tick, area, 1);
  flags_initialize(&area->flags);

  /*
   * Default values
   */
  area->max_lsp_lifetime[0] = DEFAULT_LSP_LIFETIME;    /* 1200 */
  area->max_lsp_lifetime[1] = DEFAULT_LSP_LIFETIME;    /* 1200 */
  area->lsp_refresh[0] = DEFAULT_MAX_LSP_GEN_INTERVAL; /* 900 */
  area->lsp_refresh[1] = DEFAULT_MAX_LSP_GEN_INTERVAL; /* 900 */
  area->lsp_gen_interval[0] = DEFAULT_MIN_LSP_GEN_INTERVAL;
  area->lsp_gen_interval[1] = DEFAULT_MIN_LSP_GEN_INTERVAL;
  area->min_spf_interval[0] = MINIMUM_SPF_INTERVAL;
  area->min_spf_interval[1] = MINIMUM_SPF_INTERVAL;
  area->dynhostname = 1;
  area->oldmetric = 0;
  area->newmetric = 1;
  area->lsp_frag_threshold = 90;
  area->lsp_mtu = DEFAULT_LSP_MTU;
  area->distance4 = DEFAULT_DISTANCE;
  area->distance6 = DEFAULT_DISTANCE;

  area->area_import_to_l1 = 0;
  area->area_import_to_l2 = 1;

  area->area_import_to_l1_v6 = 0;
  area->area_import_to_l2_v6 = 1;
  //area->attached_bit = LSPBIT_ATT;
  area->attached_bit = 0;

#ifdef TOPOLOGY_GENERATE
  memcpy(area->topology_baseis, DEFAULT_TOPOLOGY_BASEIS, ISIS_SYS_ID_LEN);
#endif /* TOPOLOGY_GENERATE */

  area->area_tag = strdup(area_tag);
  listnode_add(isis->area_list, area);
  area->isis = isis;

  area->summary = list_new();
  area->summary_l1 = list_new();
  area->summary_l2 = list_new();

  area->summary_ipv6 = list_new();
  area->summary_ipv6_l1 = list_new();
  area->summary_ipv6_l2 = list_new();

  area->redist_settings = list_new();
  area->redist_settings_v6 = list_new();
  return area;
}

struct isis_area *
isis_area_lookup(const char *area_tag)
{
  struct isis_area *area;
  struct listnode *node;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
    if ((area->area_tag == NULL && area_tag == NULL) ||
        (area->area_tag && area_tag && strcmp(area->area_tag, area_tag) == 0))

      return area;

  return NULL;
}

int isis_area_get(struct vty *vty, const char *area_tag, u_int16_t vpn)
{
  struct isis_area *area;

  area = isis_area_lookup(area_tag);

  if (area)
  {
    if (vpn == area->vpn)
    {
      vty->node = ISIS_NODE;
      vty->index = area;
      return CMD_SUCCESS;
    }

    if (area->vpn != vpn)
    {
      vty_warning_out(vty, "Warning: You can't change the vpn instance number unless you delete the isis instance%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  }

  area = isis_area_create(area_tag, vpn);

  //if (isis->debugs & DEBUG_EVENTS || isis->debugs & ISIS_DEBUG_TYPE_ALL)
  zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "New IS-IS area instance %s", area->area_tag);

  vty->node = ISIS_NODE;
  vty->index = area;

  return CMD_SUCCESS;
}

int isis_area_destroy(struct vty *vty, const char *area_tag)
{
  struct isis_area *area;
  struct listnode *node, *nnode;
  struct isis_circuit *circuit;
  struct area_addr *addr;

  area = isis_area_lookup(area_tag);
  if (area == NULL)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->circuit_list)
  {
    for (ALL_LIST_ELEMENTS(area->circuit_list, node, nnode, circuit))
    {
      if (circuit == NULL)
      {
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "%s():%d circuit pointer is NULL!\n", __FUNCTION__, __LINE__);
        continue;
      }

      circuit->ip_router = 0;
#if 1
      circuit->ipv6_router = 0;
#endif
      //isis_csm_state_change (ISIS_DISABLE, circuit, area);
      if (isis_csm_state_change(ISIS_DISABLE, circuit, area))
        isis_circuit_conf_default(circuit);
    }

    list_delete(area->circuit_list);
    area->circuit_list = NULL;
  }

  if (area->lspdb[0] != NULL)
  {
    lsp_db_destroy(area->lspdb[0]);
    area->lspdb[0] = NULL;
  }

  if (area->lspdb[1] != NULL)
  {
    lsp_db_destroy(area->lspdb[1]);
    area->lspdb[1] = NULL;
  }

  spftree_area_del(area);
  isis_route_invalidate(area);
  isis_route_validate(area);

  if (area->route_table[0])
  {
    route_table_finish(area->route_table[0]);
    area->route_table[0] = NULL;
  }

  if (area->route_table[1])
  {
    route_table_finish(area->route_table[1]);
    area->route_table[1] = NULL;
  }
#if 1
  if (area->route_table6[0])
  {
    route_table_finish(area->route_table6[0]);
    area->route_table6[0] = NULL;
  }

  if (area->route_table6[1])
  {
    route_table_finish(area->route_table6[1]);
    area->route_table6[1] = NULL;
  }
#endif /* 0 */

  isis_redist_area_finish(area);

  if (area->area_addrs)
  {
    for (ALL_LIST_ELEMENTS(area->area_addrs, node, nnode, addr))
    {
      list_delete_node(area->area_addrs, node);
      XFREE(MTYPE_ISIS_AREA_ADDR, addr);
    }
    list_delete(area->area_addrs);
    area->area_addrs = NULL;
  }

  THREAD_TIMER_OFF_NEW(area->t_tick);
  area->t_tick = 0;
  THREAD_TIMER_OFF_NEW(area->t_lsp_refresh[0]);
  area->t_lsp_refresh[0] = 0;
  THREAD_TIMER_OFF_NEW(area->t_lsp_refresh[1]);
  area->t_lsp_refresh[1] = 0;

  thread_cancel_event(master, area);

  listnode_delete(isis->area_list, area);

  list_delete(area->summary);
  area->summary = NULL;
  list_delete(area->summary_l1);
  area->summary_l1 = NULL;
  list_delete(area->summary_l2);
  area->summary_l2 = NULL;

  list_delete(area->summary_ipv6);
  area->summary_ipv6 = NULL;
  list_delete(area->summary_ipv6_l1);
  area->summary_ipv6_l1 = NULL;
  list_delete(area->summary_ipv6_l2);
  area->summary_ipv6_l2 = NULL;

  free(area->area_tag);

  XFREE(MTYPE_ISIS_AREA, area);

  return CMD_SUCCESS;
}

int isis_area_destroy_without_vty(const char *area_tag)
{
  struct isis_area *area;
  struct listnode *node, *nnode;
  struct isis_circuit *circuit;
  struct area_addr *addr;

  area = isis_area_lookup(area_tag);
  if (area == NULL)
    return CMD_SUCCESS;

  if (area->circuit_list)
  {
    for (ALL_LIST_ELEMENTS(area->circuit_list, node, nnode, circuit))
    {
      if (circuit == NULL)
      {
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "%s():%d circuit pointer is NULL!\n", __FUNCTION__, __LINE__);
        continue;
      }

      circuit->ip_router = 0;
#if 1
      circuit->ipv6_router = 0;
#endif
      //isis_csm_state_change (ISIS_DISABLE, circuit, area);
      if (isis_csm_state_change(ISIS_DISABLE, circuit, area))
        isis_circuit_conf_default(circuit);
    }

    list_delete(area->circuit_list);
    area->circuit_list = NULL;
  }

  if (area->lspdb[0] != NULL)
  {
    lsp_db_destroy(area->lspdb[0]);
    area->lspdb[0] = NULL;
  }

  if (area->lspdb[1] != NULL)
  {
    lsp_db_destroy(area->lspdb[1]);
    area->lspdb[1] = NULL;
  }

  spftree_area_del(area);
  isis_route_invalidate(area);
  isis_route_validate(area);

  if (area->route_table[0])
  {
    route_table_finish(area->route_table[0]);
    area->route_table[0] = NULL;
  }

  if (area->route_table[1])
  {
    route_table_finish(area->route_table[1]);
    area->route_table[1] = NULL;
  }
#if 1
  if (area->route_table6[0])
  {
    route_table_finish(area->route_table6[0]);
    area->route_table6[0] = NULL;
  }

  if (area->route_table6[1])
  {
    route_table_finish(area->route_table6[1]);
    area->route_table6[1] = NULL;
  }
#endif /* 0 */

  isis_redist_area_finish(area);

  if (area->area_addrs)
  {
    for (ALL_LIST_ELEMENTS(area->area_addrs, node, nnode, addr))
    {
      list_delete_node(area->area_addrs, node);
      XFREE(MTYPE_ISIS_AREA_ADDR, addr);
    }
    list_delete(area->area_addrs);
    area->area_addrs = NULL;
  }

  THREAD_TIMER_OFF_NEW(area->t_tick);
  area->t_tick = 0;
  THREAD_TIMER_OFF_NEW(area->t_lsp_refresh[0]);
  area->t_lsp_refresh[0] = 0;
  THREAD_TIMER_OFF_NEW(area->t_lsp_refresh[1]);
  area->t_lsp_refresh[1] = 0;

  thread_cancel_event(master, area);

  listnode_delete(isis->area_list, area);

  list_delete(area->summary);
  area->summary = NULL;
  list_delete(area->summary_l1);
  area->summary_l1 = NULL;
  list_delete(area->summary_l2);
  area->summary_l2 = NULL;

  free(area->area_tag);

  XFREE(MTYPE_ISIS_AREA, area);

  return CMD_SUCCESS;
}

int area_net_title(struct vty *vty, const char *net_title)
{
  struct isis_area *area;
  struct area_addr *addr;
  struct area_addr *addrp;
  struct listnode *node;

  u_char buff[255];
  area = vty->index;

  if (!area)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* We check that we are not over the maximal number of addresses */
  if (listcount(area->area_addrs) >= isis->max_area_addrs)
  {
    vty_warning_out(vty, "Maximum of net entity (%d) already reached %s",
                    isis->max_area_addrs, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  addr = XMALLOC(MTYPE_ISIS_AREA_ADDR, sizeof(struct area_addr));
  memset(addr, 0, sizeof(struct area_addr));
  addr->addr_len = dotformat2buff(buff, net_title);
  memcpy(addr->area_addr, buff, addr->addr_len);

  zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "added area address %s for area %s (address length %d)",
             net_title, area->area_tag, addr->addr_len);

  if (addr->addr_len < 8 || addr->addr_len > 20)
  {
    vty_warning_out(vty, "net entity must be at least 8..20 octets long (%d)%s",
                    addr->addr_len, VTY_NEWLINE);
    XFREE(MTYPE_ISIS_AREA_ADDR, addr);
    return CMD_SUCCESS;
  }

  if (addr->area_addr[addr->addr_len - 1] != 0)
  {
    vty_warning_out(vty, "nsel byte (last byte) in net entity must be 0%s",
                    VTY_NEWLINE);
    XFREE(MTYPE_ISIS_AREA_ADDR, addr);
    return CMD_SUCCESS;
  }

  //if (isis->sysid_set == 0)
  if (area->sysid_set == 0)
  {
    /*
         * First area address - get the SystemID for this router
         */
    //memcpy (isis->sysid, GETSYSID (addr), ISIS_SYS_ID_LEN);
    //isis->sysid_set = 1;
    memcpy(area->sysid, GETSYSID(addr), ISIS_SYS_ID_LEN);
    area->sysid_set = 1;
    //if (isis->debugs & DEBUG_EVENTS|| isis->debugs & ISIS_DEBUG_TYPE_ALL)
    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Router has SystemID %s", sysid_print(area->sysid));
  }
  else
  {
    /*
       * Check that the SystemID portions match
       */
    //if (memcmp (isis->sysid, GETSYSID (addr), ISIS_SYS_ID_LEN))
    /*
    		same instance must have the same sysid
      */
    if (memcmp(area->sysid, GETSYSID(addr), ISIS_SYS_ID_LEN))
    {
      vty_warning_out(vty,
                      "System ID must not change when defining additional area"
                      " addresses%s",
                      VTY_NEWLINE);
      XFREE(MTYPE_ISIS_AREA_ADDR, addr);
      return CMD_SUCCESS;
    }

    /* now we see that we don't already have this address */
    for (ALL_LIST_ELEMENTS_RO(area->area_addrs, node, addrp))
    {
      if ((addrp->addr_len + ISIS_SYS_ID_LEN + ISIS_NSEL_LEN) != (addr->addr_len))
        continue;
      if (!memcmp(addrp->area_addr, addr->area_addr, addr->addr_len))
      {
        XFREE(MTYPE_ISIS_AREA_ADDR, addr);
        return CMD_SUCCESS; /* silent fail */
      }
    }
  }

  /*
   * Forget the systemID part of the address
   */
  addr->addr_len -= (ISIS_SYS_ID_LEN + ISIS_NSEL_LEN);
  listnode_add(area->area_addrs, addr);

  /* only now we can safely generate our LSPs for this area */
  if (listcount(area->area_addrs) > 0)
  {
#if 1
    if (area->is_type & IS_LEVEL_1)
    {
      if (!area->lspdb[0])
        area->lspdb[0] = lsp_db_init();
      if (!area->route_table[0])
        area->route_table[0] = route_table_init();
      if (!area->route_table6[0])
        area->route_table6[0] = route_table_init();
    }
    if (area->is_type & IS_LEVEL_2)
    {
      if (!area->lspdb[1])
        area->lspdb[1] = lsp_db_init();
      if (!area->route_table[1])
        area->route_table[1] = route_table_init();
      if (!area->route_table6[1])
        area->route_table6[1] = route_table_init();
    }

    spftree_area_init(area);
    if (area->t_tick == 0)
    {
      THREAD_TIMER_ON_NEW((char *)"ISISLspTick", area->t_tick, lsp_tick, area, 1);
      flags_initialize(&area->flags);
    }
#endif
    if (area->is_type & IS_LEVEL_1)
    {
      lsp_generate(area, IS_LEVEL_1);
    }
    if (area->is_type & IS_LEVEL_2)
    {
      lsp_generate(area, IS_LEVEL_2);
    }
  }

  return CMD_SUCCESS;
}

int area_clear_net_title(struct vty *vty, const char *net_title)
{
  struct isis_area *area;
  struct area_addr addr, *addrp = NULL;
  u_char buff[255];
  struct isis_circuit *circuit;
  struct listnode *node, *nnode, *anode, *annode;
  struct isis_adjacency *adj;
  struct list *adjdb;
  int i = 0;

  area = vty->index;
  if (!area)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  addr.addr_len = dotformat2buff(buff, net_title);
  if (addr.addr_len < 8 || addr.addr_len > 20)
  {
    vty_warning_out(vty, "Unsupported area address length %d, should be 8...20 %s",
                    addr.addr_len, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memcpy(addr.area_addr, buff, (int)addr.addr_len);

  for (ALL_LIST_ELEMENTS_RO(area->area_addrs, node, addrp))
    if ((addrp->addr_len + ISIS_SYS_ID_LEN + 1) == addr.addr_len &&
        !memcmp(addrp->area_addr, addr.area_addr, addr.addr_len))
      break;

  if (!addrp)
  {
    vty_warning_out(vty, "No net entity %s for instance %s %s", net_title,
                    area->area_tag, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  listnode_delete(area->area_addrs, addrp);
  XFREE(MTYPE_ISIS_AREA_ADDR, addrp);
  addrp = NULL;

  /*
   * Last area address - reset the SystemID for this router
   */
  if (listcount(area->area_addrs) == 0)
  {
#if 1
    if (area->circuit_list)
    {
      for (ALL_LIST_ELEMENTS(area->circuit_list, anode, annode, circuit))
      {
//isis_csm_state_change (ISIS_DISABLE, circuit, area);;
#if 1
        if (circuit->state == C_STATE_UP)
        {
          if (circuit->circ_type == CIRCUIT_T_BROADCAST)
          {
            for (i = 0; i < 2; i++)
            {
              adjdb = circuit->u.bc.adjdb[i];
              if (adjdb && adjdb->count)
              {
                for (ALL_LIST_ELEMENTS(adjdb, node, nnode, adj))
                {
                  if (adj)
                  {
                    //THREAD_TIMER_OFF (adj->t_expire);
                    isis_adj_state_change(adj, ISIS_ADJ_DOWN, "clear user request");
                  }
                }
              }
            }
          }
          else if (circuit->circ_type == CIRCUIT_T_P2P &&
                   circuit->u.p2p.neighbor)
          {
            adj = circuit->u.p2p.neighbor;
            if (adj)
            {
              //THREAD_TIMER_OFF (adj->t_expire);
              isis_adj_state_change(adj, ISIS_ADJ_DOWN, "clear user request");
            }
          }
        }
#endif
      }
    }

    if (area->lspdb[0] != NULL)
    {
      lsp_db_destroy(area->lspdb[0]);
      area->lspdb[0] = NULL;
    }
    if (area->lspdb[1] != NULL)
    {
      lsp_db_destroy(area->lspdb[1]);
      area->lspdb[1] = NULL;
    }

    spftree_area_del(area);

    /* invalidate and validate would delete all routes from zebra */
    isis_route_invalidate(area);
    isis_route_validate(area);

    if (area->route_table[0])
    {
      route_table_finish(area->route_table[0]);
      area->route_table[0] = NULL;
    }
    if (area->route_table[1])
    {
      route_table_finish(area->route_table[1]);
      area->route_table[1] = NULL;
    }

#ifdef HAVE_IPV6
    if (area->route_table6[0])
    {
      route_table_finish(area->route_table6[0]);
      area->route_table6[0] = NULL;
    }
    if (area->route_table6[1])
    {
      route_table_finish(area->route_table6[1]);
      area->route_table6[1] = NULL;
    }
#endif /* HAVE_IPV6 */

    THREAD_TIMER_OFF_NEW(area->t_tick);
    area->t_tick = 0;
    THREAD_TIMER_OFF_NEW(area->t_lsp_refresh[0]);
    area->t_lsp_refresh[0] = 0;
    THREAD_TIMER_OFF_NEW(area->t_lsp_refresh[1]);
    area->t_lsp_refresh[1] = 0;
#endif
    memset(area->sysid, 0, ISIS_SYS_ID_LEN);
    area->sysid_set = 0;
    //if (isis->debugs & DEBUG_EVENTS|| isis->debugs & ISIS_DEBUG_TYPE_ALL)
    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Router has no SystemID");
  }

  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);

  return CMD_SUCCESS;
}

/*
 * 'show isis interface' command
 */

int show_isis_interface_common(struct vty *vty, const char *ifname, char detail)
{
  struct listnode *anode, *cnode;
  struct isis_area *area;
  struct isis_circuit *circuit;
  int flag = 0;

  if (!isis)
  {
    vty_error_out(vty, "IS-IS Routing Process not enabled%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, anode, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag, VTY_NEWLINE);

    if (detail == ISIS_UI_LEVEL_BRIEF)
      vty_out(vty, "  Interface   CircId   State   MTU   Type     Level%s",
              VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      flag++;
      if (!ifname)
        isis_circuit_print_vty(circuit, vty, detail);
      else if (strcmp(circuit->interface->name, ifname) == 0)
        isis_circuit_print_vty(circuit, vty, detail);
    }
  }

  if (!flag)
    vty_warning_out(vty, "No circuit is enabled!%s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

int show_isis_instance_interface_common(struct vty *vty, struct isis_area *area, char detail)
{
  struct listnode *cnode;
  struct isis_circuit *circuit;

  vty_out(vty, "Instance %s:%s", area->area_tag, VTY_NEWLINE);

  if (detail == ISIS_UI_LEVEL_BRIEF)
    vty_out(vty, "  Interface   CircId   State   MTU   Type     Level%s",
            VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
  {
    isis_circuit_print_vty(circuit, vty, detail);
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_instance_interface,
      show_isis_instance_interface_cmd,
      "show isis instance <1-255> interface",
      SHOW_STR
      "ISIS network information\n"
      "Instance number\n"
      "Instance number\n"
      "ISIS interface\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_instance_interface_common(vty, area, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_isis_instance_interface_h3c,
      show_isis_instance_interface_h3c_cmd,
      "display isis interface <1-255>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Specify the interface\n"
      "Process ID\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_instance_interface_common(vty, area, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_isis_interface,
      show_isis_interface_cmd,
      "show isis interface",
      SHOW_STR
      "ISIS network information\n"
      "ISIS interface\n")
{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  if (argc == 0)
  {
    return show_isis_interface_common(vty, NULL, ISIS_UI_LEVEL_DETAIL);
  }

  if (argc == 2)
  {
    ifindex = ifm_get_ifindex_by_name((char *)argv[0], (char *)argv[1]);
    if (ifindex == 0)
    {
      vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

    ret = ifm_get_name_by_ifindex(ifindex, if_name);

    if (ret < 0)
    {
      vty_warning_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

    if ((ifp = if_lookup_by_name(if_name)) == NULL)
    {
      vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
    }
    else
    {
      return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
    }
  }

  return CMD_SUCCESS;
}

ALIAS(show_isis_interface,
      show_isis_interface_arg_cmd,
      "show isis interface (ethernet|gigabitethernet|xgigabitethernet) USP",
      SHOW_STR
      "ISIS network information\n"
      "ISIS interface\n"
      "Ethernet interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "GigabitEthernet interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "10GigabitEthernet interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFUN(show_isis_interface_h3c,
      show_isis_interface_h3c_cmd,
      "display isis interface",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Specify the interface\n")
{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  if (argc == 0)
  {
    return show_isis_interface_common(vty, NULL, ISIS_UI_LEVEL_DETAIL);
  }

  if (argc == 2)
  {
    ifindex = ifm_get_ifindex_by_name((char *)argv[0], (char *)argv[1]);
    if (ifindex == 0)
    {
      vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

    ret = ifm_get_name_by_ifindex(ifindex, if_name);

    if (ret < 0)
    {
      vty_warning_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

    if ((ifp = if_lookup_by_name(if_name)) == NULL)
    {
      vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
    }
    else
    {
      return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
    }
  }

  return CMD_SUCCESS;
}

ALIAS(show_isis_interface_h3c,
      show_isis_interface_arg_h3c_cmd,
      "display isis interface (ethernet|gigabitethernet|xgigabitethernet) USP",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Specify the interface\n"
      "Ethernet interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "GigabitEthernet interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "10GigabitEthernet interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFUN(show_isis_loopback_interface,
      show_isis_loopback_interface_cmd,
      "show isis interface loopback <0-128>",
      SHOW_STR
      "ISIS network information\n"
      "ISIS interface\n"
      "LoopBack interface\n"
      "LoopBack interface number\n")
{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  ifindex = ifm_get_ifindex_by_name("loopback", (char *)argv[0]);

  if (ifindex == 0)
  {
    vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  ret = ifm_get_name_by_ifindex(ifindex, if_name);

  if (ret < 0)
  {
    vty_warning_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if ((ifp = if_lookup_by_name(if_name)) == NULL)
  {
    vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
  }
  else
  {
    return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
  }
  return CMD_SUCCESS;
}

DEFUN(show_isis_loopback_interface_h3c,
      show_isis_loopback_interface_h3c_cmd,
      "display isis interface loopback <0-128>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Specify the interface\n"
      "LoopBack interface\n"
      "LoopBack interface number\n")
{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  ifindex = ifm_get_ifindex_by_name("loopback", (char *)argv[0]);

  if (ifindex == 0)
  {
    vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  ret = ifm_get_name_by_ifindex(ifindex, if_name);

  if (ret < 0)
  {
    vty_warning_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if ((ifp = if_lookup_by_name(if_name)) == NULL)
  {
    vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
  }
  else
  {
    return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
  }
  return CMD_SUCCESS;
}

DEFUN(show_isis_trunk_interface,
      show_isis_trunk_interface_cmd,
      "show isis interface trunk TRUNK",
      SHOW_STR
      "ISIS network information\n"
      "ISIS interface\n"
      "Trunk interface\n"
      "The port/subport of trunk, format: <1-128>[.<1-4095>]\n")

{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[0]);

  if (ifindex == 0)
  {
    vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  ret = ifm_get_name_by_ifindex(ifindex, if_name);

  if (ret < 0)
  {
    vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if ((ifp = if_lookup_by_name(if_name)) == NULL)
  {
    vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
  }
  else
  {
    return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
  }
  return CMD_SUCCESS;
}

DEFUN(show_isis_vlanif_interface,
      show_isis_vlanif_interface_cmd,
      "show isis interface vlanif <1-4094>",
      SHOW_STR
      "ISIS network information\n"
      "ISIS interface\n"
      "Vlan interface\n"
      "VLAN interface number\n")

{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[0]);

  if (ifindex == 0)
  {
    vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  ret = ifm_get_name_by_ifindex(ifindex, if_name);

  if (ret < 0)
  {
    vty_warning_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if ((ifp = if_lookup_by_name(if_name)) == NULL)
  {
    vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
  }
  else
  {
    return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
  }
  return CMD_SUCCESS;
}

DEFUN(show_isis_vlanif_interface_h3c,
      show_isis_vlanif_interface_h3c_cmd,
      "display isis interface vlan-interface <1-4094>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Specify the interface \n"
      "Vlan interface\n"
      "Vlan-interface interface number\n")

{
  uint32_t ifindex = 0;
  struct interface *ifp = NULL;
  int ret = -1;
  char if_name[IFNET_NAMESIZE];

  ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[0]);

  if (ifindex == 0)
  {
    vty_warning_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  ret = ifm_get_name_by_ifindex(ifindex, if_name);

  if (ret < 0)
  {
    vty_warning_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if ((ifp = if_lookup_by_name(if_name)) == NULL)
  {
    vty_warning_out(vty, "No such interface name%s", VTY_NEWLINE);
  }
  else
  {
    return show_isis_interface_common(vty, if_name, ISIS_UI_LEVEL_DETAIL);
  }
  return CMD_SUCCESS;
}
/*
 * 'show isis neighbor' command
 */

int show_isis_neighbor_common(struct vty *vty, const char *id, char detail)
{
  struct listnode *anode, *cnode, *node;
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct list *adjdb;
  struct isis_adjacency *adj;
  struct isis_dynhn *dynhn;
  u_char sysid[ISIS_SYS_ID_LEN];
  int i;
  int flag = 0;
  if (!isis)
  {
    vty_error_out(vty, "IS-IS Routing Process not enabled%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(sysid, 0, ISIS_SYS_ID_LEN);
  if (id)
  {
    if (sysid2buff(sysid, id) == 0)
    {
      dynhn = dynhn_find_by_name(id);
      if (dynhn == NULL)
      {
        vty_warning_out(vty, "Invalid system id %s%s", id, VTY_NEWLINE);
        return CMD_SUCCESS;
      }
      memcpy(sysid, dynhn->id, ISIS_SYS_ID_LEN);
    }
  }
  for (ALL_LIST_ELEMENTS_RO(isis->area_list, anode, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag, VTY_NEWLINE);

    if (detail == ISIS_UI_LEVEL_BRIEF)
      vty_out(vty, "  System Id         Interface                L   State"
                   "  Holdtime      SNPA%s",
              VTY_NEWLINE);
    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_BROADCAST)
      {
        for (i = 0; i < 2; i++)
        {
          adjdb = circuit->u.bc.adjdb[i];
          if (adjdb && adjdb->count)
          {
            flag++;
            for (ALL_LIST_ELEMENTS_RO(adjdb, node, adj))
              if (!id || !memcmp(adj->sysid, sysid,
                                 ISIS_SYS_ID_LEN))
                isis_adj_print_vty(adj, vty, detail);
          }
        }
      }
      else if (circuit->circ_type == CIRCUIT_T_P2P &&
               circuit->u.p2p.neighbor)
      {
        flag++;
        adj = circuit->u.p2p.neighbor;
        if (!id || !memcmp(adj->sysid, sysid, ISIS_SYS_ID_LEN))
          isis_adj_print_vty(adj, vty, detail);
      }
    }
  }
  if (flag == 0)
  {
    vty_warning_out(vty, "No neighbor in this process%s", VTY_NEWLINE);
  }
  return CMD_SUCCESS;
}

/*
 * 'clear isis neighbor' command
 */
int clear_isis_neighbor_common(struct vty *vty, const char *id)
{
  struct listnode *anode, *cnode, *cnextnode, *node, *nnode;
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct list *adjdb;
  struct isis_adjacency *adj;
  struct isis_dynhn *dynhn;
  u_char sysid[ISIS_SYS_ID_LEN];
  int i;

  if (!isis)
  {
    vty_error_out(vty, "IS-IS Routing Process not enabled%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(sysid, 0, ISIS_SYS_ID_LEN);
  if (id)
  {
    if (sysid2buff(sysid, id) == 0)
    {
      dynhn = dynhn_find_by_name(id);
      if (dynhn == NULL)
      {
        vty_warning_out(vty, "Invalid system id %s%s", id, VTY_NEWLINE);
        return CMD_SUCCESS;
      }
      memcpy(sysid, dynhn->id, ISIS_SYS_ID_LEN);
    }
  }

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, anode, area))
  {
    for (ALL_LIST_ELEMENTS(area->circuit_list, cnode, cnextnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_BROADCAST)
      {
        for (i = 0; i < 2; i++)
        {
          adjdb = circuit->u.bc.adjdb[i];
          if (adjdb && adjdb->count)
          {
            for (ALL_LIST_ELEMENTS(adjdb, node, nnode, adj))
              if (!id || !memcmp(adj->sysid, sysid, ISIS_SYS_ID_LEN))
                isis_adj_state_change(adj, ISIS_ADJ_DOWN,
                                      "clear user request");
          }
        }
      }
      else if (circuit->circ_type == CIRCUIT_T_P2P &&
               circuit->u.p2p.neighbor)
      {
        adj = circuit->u.p2p.neighbor;
        if (!id || !memcmp(adj->sysid, sysid, ISIS_SYS_ID_LEN))
          isis_adj_state_change(adj, ISIS_ADJ_DOWN,
                                "clear user request");
      }
    }
  }

  return CMD_SUCCESS;
}

/*
 * 'show isis neighbor' command
 */

int show_isis_instance_neighbor_common(struct vty *vty, struct isis_area *area, char detail)
{
  struct listnode *cnode, *node;
  struct isis_circuit *circuit;
  struct list *adjdb;
  struct isis_adjacency *adj;
  int i;

  //for (ALL_LIST_ELEMENTS_RO (isis->area_list, anode, area))
  {
    vty_out(vty, "Instance %s:%s", area->area_tag, VTY_NEWLINE);

    if (detail == ISIS_UI_LEVEL_BRIEF)
      vty_out(vty, "  System Id           Interface        L    State"
                   "  Holdtime     SNPA%s",
              VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_BROADCAST)
      {
        for (i = 0; i < 2; i++)
        {
          adjdb = circuit->u.bc.adjdb[i];
          if (adjdb && adjdb->count)
          {
            for (ALL_LIST_ELEMENTS_RO(adjdb, node, adj))
              isis_adj_print_vty(adj, vty, detail);
          }
        }
      }
      else if (circuit->circ_type == CIRCUIT_T_P2P &&
               circuit->u.p2p.neighbor)
      {
        adj = circuit->u.p2p.neighbor;
        isis_adj_print_vty(adj, vty, detail);
      }
    }
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_instance_neighbor,
      show_isis_instance_neighbor_cmd,
      "show isis instance <1-255> neighbor",
      SHOW_STR
      "ISIS network information\n"
      "ISIS instance information\n"
      "Instance number\n"
      "ISIS neighbor adjacencies\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_instance_neighbor_common(vty, area, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_isis_instance_neighbor_h3c,
      show_isis_instance_neighbor_h3c_cmd,
      "display isis peer <1-255>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "IS-IS neighbor\n"
      "Process ID\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_instance_neighbor_common(vty, area, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_isis_instance_neighbor_detail,
      show_isis_instance_neighbor_detail_cmd,
      "show isis instance <1-255> neighbor detail",
      SHOW_STR
      "ISIS network information\n"
      "ISIS instance information\n"
      "Instance number\n"
      "ISIS neighbor adjacencies\n"
      "show detailed information\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_instance_neighbor_common(vty, area, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_isis_instance_neighbor_detail_h3c,
      show_isis_instance_neighbor_detail_h3c_cmd,
      "display isis peer verbose <1-255>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "IS-IS neighbor\n"
      "Detailed neighbor information\n"
      "Process ID\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_instance_neighbor_common(vty, area, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_isis_neighbor,
      show_isis_neighbor_cmd,
      "show isis neighbor",
      SHOW_STR
      "ISIS network information\n"
      "ISIS neighbor adjacencies\n")
{
  return show_isis_neighbor_common(vty, NULL, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_isis_neighbor_h3c,
      show_isis_neighbor_h3c_cmd,
      "display isis peer",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "IS-IS neighbor\n")
{
  return show_isis_neighbor_common(vty, NULL, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_isis_neighbor_detail,
      show_isis_neighbor_detail_cmd,
      "show isis neighbor detail",
      SHOW_STR
      "ISIS network information\n"
      "ISIS neighbor adjacencies\n"
      "show detailed information\n")
{
  return show_isis_neighbor_common(vty, NULL, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_isis_neighbor_detail_h3c,
      show_isis_neighbor_detail_h3c_cmd,
      "display isis peer verbose",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "IS-IS neighbor\n"
      "Detailed neighbor information\n")
{
  return show_isis_neighbor_common(vty, NULL, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_isis_neighbor_arg,
      show_isis_neighbor_arg_cmd,
      "show isis neighbor WORD",
      SHOW_STR
      "ISIS network information\n"
      "ISIS neighbor adjacencies\n"
      "System id\n")
{
  return show_isis_neighbor_common(vty, argv[0], ISIS_UI_LEVEL_DETAIL);
}

DEFUN(clear_isis_neighbor,
      clear_isis_neighbor_cmd,
      "clear isis neighbor",
      CLEAR_STR
      "Reset ISIS network information\n"
      "Reset ISIS neighbor adjacencies\n")
{
  return clear_isis_neighbor_common(vty, NULL);
}

DEFUN(clear_isis_neighbor_arg,
      clear_isis_neighbor_arg_cmd,
      "clear isis neighbor WORD",
      CLEAR_STR
      "ISIS network information\n"
      "ISIS neighbor adjacencies\n"
      "System id\n")
{
  return clear_isis_neighbor_common(vty, argv[0]);
}

/*
 * 'isis debug', 'show debugging'
 */
void print_debug(struct vty *vty, int flags, int onoff)
{
  char onoffs[4];

  if (onoff)
    strcpy(onoffs, "on");
  else
    strcpy(onoffs, "off");

  if (flags & ISIS_DEBUG_TYPE_ALL)
  {
    vty_out(vty, "IS-IS All debugging is %s%s", onoffs, VTY_NEWLINE);
  }
  else
  {
    if (flags & ISIS_DEBUG_TYPE_HELLO)
      vty_out(vty, "IS-IS HELLO debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_SPF)
      vty_out(vty, "IS-IS SPF debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_LSP)
      vty_out(vty, "IS-IS LSP debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_MSG)
      vty_out(vty, "IS-IS MSG debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_EVENTS)
      vty_out(vty, "IS-IS EVENTS debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_ROUTE)
      vty_out(vty, "IS-IS ROUTE debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_NEIGHBOR)
      vty_out(vty, "IS-IS NEIGHBOR debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_CSNP)
      vty_out(vty, "IS-IS CSNP debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_PSNP)
      vty_out(vty, "IS-IS PSNP debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_SEND)
      vty_out(vty, "IS-IS SEND debugging is %s%s", onoffs, VTY_NEWLINE);
    if (flags & ISIS_DEBUG_TYPE_RECEIVE)
      vty_out(vty, "IS-IS RECEIVE debugging is %s%s", onoffs, VTY_NEWLINE);
  }
}

DEFUN(show_isis_debug,
      show_isis_debug_cmd,
      "show isis debug",
      SHOW_STR
      "ISIS Commands"
      "State of each debugging option\n")
{
  vty_out(vty, "IS-IS debugging status:%s", VTY_NEWLINE);
  print_debug(vty, isis->debugs, 1);
  return CMD_SUCCESS;
}

/*
DEFUN (debug_isis_adj,
       debug_isis_adj_cmd,
       "debug isis adj-packets",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS Adjacency related packets\n")
{
  isis->debugs |= DEBUG_ADJ_PACKETS;
  print_debug (vty, DEBUG_ADJ_PACKETS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_adj,
       no_debug_isis_adj_cmd,
       "no debug isis adj-packets",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS Adjacency related packets\n")
{
  isis->debugs &= ~DEBUG_ADJ_PACKETS;
  print_debug (vty, DEBUG_ADJ_PACKETS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_csum,
       debug_isis_csum_cmd,
       "debug isis checksum-errors",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS LSP checksum errors\n")
{
  isis->debugs |= DEBUG_CHECKSUM_ERRORS;
  print_debug (vty, DEBUG_CHECKSUM_ERRORS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_csum,
       no_debug_isis_csum_cmd,
       "no debug isis checksum-errors",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS LSP checksum errors\n")
{
  isis->debugs &= ~DEBUG_CHECKSUM_ERRORS;
  print_debug (vty, DEBUG_CHECKSUM_ERRORS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_lupd,
       debug_isis_lupd_cmd,
       "debug isis local-updates",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS local update packets\n")
{
  isis->debugs |= DEBUG_LOCAL_UPDATES;
  print_debug (vty, DEBUG_LOCAL_UPDATES, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_lupd,
       no_debug_isis_lupd_cmd,
       "no debug isis local-updates",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS local update packets\n")
{
  isis->debugs &= ~DEBUG_LOCAL_UPDATES;
  print_debug (vty, DEBUG_LOCAL_UPDATES, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_err,
       debug_isis_err_cmd,
       "debug isis protocol-errors",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS LSP protocol errors\n")
{
  isis->debugs |= DEBUG_PROTOCOL_ERRORS;
  print_debug (vty, DEBUG_PROTOCOL_ERRORS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_err,
       no_debug_isis_err_cmd,
       "no debug isis protocol-errors",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS LSP protocol errors\n")
{
  isis->debugs &= ~DEBUG_PROTOCOL_ERRORS;
  print_debug (vty, DEBUG_PROTOCOL_ERRORS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_snp,
       debug_isis_snp_cmd,
       "debug isis snp-packets",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS CSNP/PSNP packets\n")
{
  isis->debugs |= DEBUG_SNP_PACKETS;
  print_debug (vty, DEBUG_SNP_PACKETS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_snp,
       no_debug_isis_snp_cmd,
       "no debug isis snp-packets",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS CSNP/PSNP packets\n")
{
  isis->debugs &= ~DEBUG_SNP_PACKETS;
  print_debug (vty, DEBUG_SNP_PACKETS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_upd,
       debug_isis_upd_cmd,
       "debug isis update-packets",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS Update related packets\n")
{
  isis->debugs |= DEBUG_UPDATE_PACKETS;
  print_debug (vty, DEBUG_UPDATE_PACKETS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_upd,
       no_debug_isis_upd_cmd,
       "no debug isis update-packets",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS Update related packets\n")
{
  isis->debugs &= ~DEBUG_UPDATE_PACKETS;
  print_debug (vty, DEBUG_UPDATE_PACKETS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_spf,
       debug_isis_spf_cmd,
       "debug isis spf",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS Spf debug\n")
{
  isis->debugs |= DEBUG_SPF_EVENTS;
  print_debug (vty, DEBUG_SPF_EVENTS, 1);

  isis->debugs |= DEBUG_SPF_STATS;
  print_debug (vty, DEBUG_SPF_STATS, 1);

  isis->debugs |= DEBUG_SPF_TRIGGERS;
  print_debug (vty, DEBUG_SPF_TRIGGERS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_spf,
       no_debug_isis_spf_cmd,
       "no debug isis spf",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS Spf undebug\n")
{
  isis->debugs &= ~DEBUG_SPF_EVENTS;
  print_debug (vty, DEBUG_SPF_EVENTS, 0);

  isis->debugs &= ~DEBUG_SPF_STATS;
  print_debug (vty, DEBUG_SPF_STATS, 0);

  isis->debugs &= ~DEBUG_SPF_TRIGGERS;
  print_debug (vty, DEBUG_SPF_TRIGGERS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_packet_dump,
       debug_isis_packet_dump_cmd,
       "debug isis packets",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS packet dump\n")
{

  isis->debugs |= DEBUG_SNP_PACKETS;
  print_debug (vty, DEBUG_SNP_PACKETS, 1);

  isis->debugs |= DEBUG_PACKET_DUMP;
  print_debug (vty, DEBUG_PACKET_DUMP, 1);

  isis->debugs |= DEBUG_UPDATE_PACKETS;
  print_debug (vty, DEBUG_UPDATE_PACKETS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_packet_dump,
       no_debug_isis_packet_dump_cmd,
       "no debug isis packets",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS packet dump\n")
{
    isis->debugs &= ~DEBUG_PACKET_DUMP;
    print_debug (vty, DEBUG_PACKET_DUMP, 0);

    isis->debugs &= ~DEBUG_SNP_PACKETS;
    print_debug (vty, DEBUG_SNP_PACKETS, 0);

    isis->debugs &= ~DEBUG_UPDATE_PACKETS;
    print_debug (vty, DEBUG_UPDATE_PACKETS, 0);

    return CMD_SUCCESS;
}

DEFUN (debug_isis_lsp,
       debug_isis_lsp_cmd,
       "debug isis lsp",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS LSPs DEBUG\n")
{
  isis->debugs |= DEBUG_LSP_GEN;
  print_debug (vty, DEBUG_LSP_GEN, 1);

  isis->debugs |= DEBUG_LSP_SCHED;
  print_debug (vty, DEBUG_LSP_SCHED, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_lsp,
       no_debug_isis_lsp_cmd,
       "no debug isis lsp",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS LSPs UNDEBUG\n")
{
  isis->debugs &= ~DEBUG_LSP_GEN;
  print_debug (vty, DEBUG_LSP_GEN, 0);

  isis->debugs &= ~DEBUG_LSP_SCHED;
  print_debug (vty, DEBUG_LSP_SCHED, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_rtevents,
       debug_isis_rtevents_cmd,
       "debug isis route-events",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS Route related events\n")
{
  isis->debugs |= DEBUG_RTE_EVENTS;
  print_debug (vty, DEBUG_RTE_EVENTS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_rtevents,
       no_debug_isis_rtevents_cmd,
       "no debug isis route-events",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS Route related events\n")
{
  isis->debugs &= ~DEBUG_RTE_EVENTS;
  print_debug (vty, DEBUG_RTE_EVENTS, 0);

  return CMD_SUCCESS;
}

DEFUN (debug_isis_events,
       debug_isis_events_cmd,
       "debug isis fsm",
       DEBUG_STR
       "IS-IS information\n"
       "IS-IS Events\n")
{
  isis->debugs |= DEBUG_EVENTS;
  print_debug (vty, DEBUG_EVENTS, 1);

  return CMD_SUCCESS;
}

DEFUN (no_debug_isis_events,
       no_debug_isis_events_cmd,
       "no debug isis fsm",
       UNDEBUG_STR
       "IS-IS information\n"
       "IS-IS Events\n")
{
  isis->debugs &= ~DEBUG_EVENTS;
  print_debug (vty, DEBUG_EVENTS, 0);

  return CMD_SUCCESS;
}
*/

DEFUN(show_hostname,
      show_hostname_cmd,
      "show isis hostname",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS Dynamic hostname mapping\n")
{
  dynhn_print_all(vty);

  return CMD_SUCCESS;
}

static void
vty_out_timestr(struct vty *vty, time_t uptime)
{
  struct tm *tm;
  time_t difftime = time(NULL);
  difftime -= uptime;
  tm = gmtime(&difftime);

#define ONE_DAY_SECOND 60 * 60 * 24
#define ONE_WEEK_SECOND 60 * 60 * 24 * 7
  if (difftime < ONE_DAY_SECOND)
    vty_out(vty, "%02d:%02d:%02d",
            tm->tm_hour, tm->tm_min, tm->tm_sec);
  else if (difftime < ONE_WEEK_SECOND)
    vty_out(vty, "%dd%02dh%02dm",
            tm->tm_yday, tm->tm_hour, tm->tm_min);
  else
    vty_out(vty, "%02dw%dd%02dh",
            tm->tm_yday / 7,
            tm->tm_yday - ((tm->tm_yday / 7) * 7), tm->tm_hour);
  vty_out(vty, " ago");
}

DEFUN(show_isis_summary,
      show_isis_summary_cmd,
      "show isis summary",
      SHOW_STR "IS-IS information\n"
               "IS-IS summary\n")
{
  struct listnode *node, *node2;
  struct isis_area *area;
  struct isis_spftree *spftree;
  int level;

  if (isis == NULL)
  {
    vty_error_out(vty, "ISIS is not running%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  vty_out(vty, "Process Id      : %ld%s", isis->process_id,
          VTY_NEWLINE);
  //if (isis->sysid_set)
  //vty_out (vty, "System Id       : %s%s", sysid_print (isis->sysid),
  //VTY_NEWLINE);

  vty_out(vty, "Up time         : ");
  vty_out_timestr(vty, isis->uptime);
  vty_out(vty, "%s", VTY_NEWLINE);

  if (isis->area_list)
    vty_out(vty, "Number of areas : %d%s", isis->area_list->count,
            VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "Area %s:%s", area->area_tag ? area->area_tag : "null",
            VTY_NEWLINE);

    if (area->sysid_set)
      vty_out(vty, "System Id		 : %s%s", sysid_print(area->sysid),
              VTY_NEWLINE);

    if (listcount(area->area_addrs) > 0)
    {
      struct area_addr *area_addr;
      for (ALL_LIST_ELEMENTS_RO(area->area_addrs, node2, area_addr))
      {
        vty_out(vty, "  Net: %s%s",
                isonet_print(area_addr->area_addr,
                             area_addr->addr_len + ISIS_SYS_ID_LEN +
                                 1),
                VTY_NEWLINE);
      }
    }

    for (level = ISIS_LEVEL1; level <= ISIS_LEVELS; level++)
    {
      if ((area->is_type & level) == 0)
        continue;

      vty_out(vty, "  Level-%d:%s", level, VTY_NEWLINE);
      spftree = area->spftree[level - 1];
      if (spftree->pending)
        vty_out(vty, "    IPv4 SPF: (pending)%s", VTY_NEWLINE);
      else
        vty_out(vty, "    IPv4 SPF:%s", VTY_NEWLINE);

      vty_out(vty, "      minimum interval  : %d%s",
              area->min_spf_interval[level - 1], VTY_NEWLINE);

      vty_out(vty, "      last run elapsed  : ");
      vty_out_timestr(vty, spftree->last_run_timestamp);
      vty_out(vty, "%s", VTY_NEWLINE);

      vty_out(vty, "      last run duration : %u usec%s",
              (u_int32_t)spftree->last_run_duration, VTY_NEWLINE);

      vty_out(vty, "      run count         : %d%s",
              spftree->runcount, VTY_NEWLINE);

#if 0
      spftree = area->spftree6[level - 1];
      if (spftree->pending)
        vty_out (vty, "    IPv6 SPF: (pending)%s", VTY_NEWLINE);
      else
        vty_out (vty, "    IPv6 SPF:%s", VTY_NEWLINE);

      vty_out (vty, "      minimum interval  : %d%s",
          area->min_spf_interval[level - 1], VTY_NEWLINE);

      vty_out (vty, "      last run elapsed  : ");
      vty_out_timestr(vty, spftree->last_run_timestamp);
      vty_out (vty, "%s", VTY_NEWLINE);

      vty_out (vty, "      last run duration : %llu msec%s",
               (unsigned long long)spftree->last_run_duration, VTY_NEWLINE);

      vty_out (vty, "      run count         : %d%s",
          spftree->runcount, VTY_NEWLINE);
#endif
    }
  }
  vty_out(vty, "%s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

/*
 * This function supports following display options:
 * [ show isis database [detail] ]
 * [ show isis database <sysid> [detail] ]
 * [ show isis database <hostname> [detail] ]
 * [ show isis database <sysid>.<pseudo-id> [detail] ]
 * [ show isis database <hostname>.<pseudo-id> [detail] ]
 * [ show isis database <sysid>.<pseudo-id>-<fragment-number> [detail] ]
 * [ show isis database <hostname>.<pseudo-id>-<fragment-number> [detail] ]
 * [ show isis database detail <sysid> ]
 * [ show isis database detail <hostname> ]
 * [ show isis database detail <sysid>.<pseudo-id> ]
 * [ show isis database detail <hostname>.<pseudo-id> ]
 * [ show isis database detail <sysid>.<pseudo-id>-<fragment-number> ]
 * [ show isis database detail <hostname>.<pseudo-id>-<fragment-number> ]
 */
static int
show_isis_database(struct vty *vty, const char *argv, int ui_level)
{
  struct listnode *node = NULL;
  struct isis_area *area = NULL;
  struct isis_lsp *lsp = NULL;
  struct isis_dynhn *dynhn = NULL;
  const char *pos = argv;
  u_char lspid[ISIS_SYS_ID_LEN + 2];
  char sysid[255];
  u_char number[3];
  int level, lsp_count;
  int flag1 = 0;

  if (isis->area_list->count == 0)
  {
    vty_warning_out(vty, "No lsp in this process%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(lspid, 0, ISIS_SYS_ID_LEN + 2);
  memset(sysid, 0, 255);
  memset(number, 0, 3);

  /*
   * extract fragment and pseudo id from the string argv
   * in the forms:
   * (a) <systemid/hostname>.<pseudo-id>-<framenent> or
   * (b) <systemid/hostname>.<pseudo-id> or
   * (c) <systemid/hostname> or
   * Where systemid is in the form:
   * xxxx.xxxx.xxxx
   */
  if (argv)
    strncpy(sysid, argv, 254);

  if (argv && strlen(argv) > 3)
  {
    pos = argv + strlen(argv);
    if ((strlen(argv) < 20) ||
        strncmp(pos - 3, "-", 1) != 0 ||
        strncmp(pos - 6, ".", 1) != 0 ||
        strncmp(pos - 11, ".", 1) != 0 ||
        strncmp(pos - 16, ".", 1) != 0)
    {
      vty_error_out(vty, "Wrong parameter found at '^' position. %s", VTY_NEWLINE);
      return CMD_SUCCESS;
    }

    pos = argv + strlen(argv) - 3;
    if (strncmp(pos, "-", 1) == 0)
    {
      memcpy(number, ++pos, 2);
      lspid[ISIS_SYS_ID_LEN + 1] = (u_char)strtol((char *)number, NULL, 16);
      pos -= 4;
    }
    if (strncmp(pos, ".", 1) == 0)
    {
      memcpy(number, ++pos, 2);
      lspid[ISIS_SYS_ID_LEN] = (u_char)strtol((char *)number, NULL, 16);
      sysid[pos - argv - 1] = '\0';
    }
  }

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    if (area->vpn)
      vty_out(vty, "Instance %s Vpn-instance %d:%s", area->area_tag ? area->area_tag : "null", area->vpn, VTY_NEWLINE);
    else
      vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null", VTY_NEWLINE);

    for (level = 0; level < ISIS_LEVELS; level++)
    {
      if (area->lspdb[level] && dict_count(area->lspdb[level]) > 0)
      {
        lsp = NULL;
        if (argv != NULL)
        {
          /*
                   * Try to find the lsp-id if the argv string is in
                   * the form hostname.<pseudo-id>-<fragment>
                   */
          if (sysid2buff(lspid, sysid))
          {
            lsp = lsp_search(lspid, area->lspdb[level]);
          }
          else if ((dynhn = dynhn_find_by_name(sysid)))
          {
            memcpy(lspid, dynhn->id, ISIS_SYS_ID_LEN);
            lsp = lsp_search(lspid, area->lspdb[level]);
          }
          else if (strncmp(unix_hostname(), sysid, 15) == 0)
          {
            //memcpy (lspid, isis->sysid, ISIS_SYS_ID_LEN);
            memcpy(lspid, area->sysid, ISIS_SYS_ID_LEN);
            lsp = lsp_search(lspid, area->lspdb[level]);
          }
        }

        if (lsp != NULL || argv == NULL)
        {
          flag1++;
          vty_out(vty, "IS-IS Level-%d link-state database:%s",
                  level + 1, VTY_NEWLINE);

          /* print the title in all cases */
          vty_out(vty, "LSP ID                  PduLen  "
                       "SeqNumber   Chksum  Holdtime  ATT/P/OL%s",
                  VTY_NEWLINE);
        }

        if (lsp)
        {
          if (ui_level == ISIS_UI_LEVEL_DETAIL)
            lsp_print_detail(lsp, vty, area->dynhostname);
          else
            lsp_print(lsp, vty, area->dynhostname);
        }
        else if (argv == NULL)
        {
          lsp_count = lsp_print_all(vty, area->lspdb[level],
                                    ui_level,
                                    area->dynhostname);

          vty_out(vty, "    %u LSPs%s%s",
                  lsp_count, VTY_NEWLINE, VTY_NEWLINE);
        }
      }
    }
  }
  if (flag1 == 0)
    vty_warning_out(vty, "LSP not found%s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

static int
show_isis_database_instance(struct vty *vty, struct isis_area *area, int ui_level)
{
  int level, lsp_count;

  if (area->vpn)
    vty_out(vty, "Instance %s Vpn-instance %d:%s", area->area_tag ? area->area_tag : "null", area->vpn, VTY_NEWLINE);
  else
    vty_out(vty, "Instance %s:%s", area->area_tag ? area->area_tag : "null", VTY_NEWLINE);

  for (level = 0; level < ISIS_LEVELS; level++)
  {
    if (area->lspdb[level] && dict_count(area->lspdb[level]) > 0)
    {
      vty_out(vty, "IS-IS Level-%d link-state database:%s", level + 1, VTY_NEWLINE);
      vty_out(vty, "LSP ID                   PduLen SeqNumber   Chksum   Holdtime ATT/P/OL%s", VTY_NEWLINE);

      lsp_count = lsp_print_all(vty, area->lspdb[level],
                                ui_level,
                                area->dynhostname);

      vty_out(vty, "    %u LSPs%s%s",
              lsp_count, VTY_NEWLINE, VTY_NEWLINE);
    }
  }

  return CMD_SUCCESS;
}

DEFUN(show_database_instance_brief,
      show_database_instance_brief_cmd,
      "show isis instance <1-255> database",
      SHOW_STR
      "IS-IS information\n"
      "Istance information\n"
      "Instance number\n"
      "IS-IS link state database\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_database_instance(vty, area, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_database_instance_brief_h3c,
      show_database_instance_brief_h3c_cmd,
      "display isis lsdb <1-255>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Link state database (LSDB)\n"
      "Process ID\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_database_instance(vty, area, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_database_instance_detail,
      show_database_instance_detail_cmd,
      "show isis instance <1-255> database detail",
      SHOW_STR
      "IS-IS information\n"
      "Istance information\n"
      "Instance number\n"
      "IS-IS link state database\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_database_instance(vty, area, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_database_instance_detail_h3c,
      show_database_instance_detail_h3c_cmd,
      "display isis lsdb verbose <1-255>",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Link state database (LSDB)\n"
      "LSDB detailed information\n"
      "Process ID\n")

{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_error_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return show_isis_database_instance(vty, area, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_database_brief,
      show_database_cmd,
      "show isis database",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS link state database\n")
{
  return show_isis_database(vty, NULL, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_database_brief_h3c,
      show_database_h3c_cmd,
      "display isis lsdb",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Link state database (LSDB)\n")
{
  return show_isis_database(vty, NULL, ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_database_lsp_brief,
      show_database_arg_cmd,
      "show isis database WORD",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS link state database\n"
      "LSP ID\n")
{
  return show_isis_database(vty, argv[0], ISIS_UI_LEVEL_BRIEF);
}

DEFUN(show_lsp,
      show_lsp_arg_cmd,
      "show isis lsp WORD",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS link state database\n"
      "LSPID in the form of ####.####.####.##-##\n"
      "Detailed information\n")
{
  return show_isis_database(vty, argv[0], ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_database_detail,
      show_database_detail_cmd,
      "show isis database detail",
      SHOW_STR
      "IS-IS information\n"
      "IS-IS link state database\n"
      "IS-IS information detail\n")
{
  return show_isis_database(vty, NULL, ISIS_UI_LEVEL_DETAIL);
}

DEFUN(show_database_detail_h3c,
      show_database_detail_h3c_cmd,
      "display isis lsdb verbose",
      "Display current system information\n"
      "Intermediate System-to-Intermediate System (IS-IS) module\n"
      "Link state database (LSDB)\n"
      "LSDB detailed information\n")
{
  return show_isis_database(vty, NULL, ISIS_UI_LEVEL_DETAIL);
}

/*
 * 'router isis' command
 */
DEFUN(isis_instance,
      isis_instance_cmd,
      "isis instance <1-255> {vpn-instance <1-127>}",
      ROUTER_STR
      "ISO IS-IS\n"
      "ISO Routing area tag\n"
      "Virtual private network\n"
      "Vpn number\n")
{
  u_int16_t vpn = ISIS_VPN_DEFAULT;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (argv[1])
  {
    if ((atoi(argv[1]) < 1) || (atoi(argv[1]) > 127))
    {
      vty_error_out(vty, "Invalid Vpn %d - should be <1-127>%s", atoi(argv[1]), VTY_NEWLINE);
      return CMD_SUCCESS;
    }
    vpn = atoi(argv[1]);
  }

  return isis_area_get(vty, argv[0], vpn);
}

DEFUN(isis_instance_h3c,
      isis_instance_h3c_cmd,
      "isis <1-255> {vpn-instance <1-127>}",
      "Intermediate System to Intermediate System (ISIS)\n"
      "Process ID\n"
      "VPN Routing/Forwarding instance\n"
      "VPN instance name\n")
{
  u_int16_t vpn = ISIS_VPN_DEFAULT;

  if (atoi(argv[0]) < 1)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_error_out(vty, "Invalid Instance %d - should be <0-127>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (argv[1])
  {
    if ((atoi(argv[1]) < 1) || (atoi(argv[1]) > 127))
    {
      vty_error_out(vty, "Invalid Vpn %d - should be <1-127>%s", atoi(argv[1]), VTY_NEWLINE);
      return CMD_SUCCESS;
    }
    vpn = atoi(argv[1]);
  }

  return isis_area_get(vty, argv[0], vpn);
}

/*
 *'no router isis' command
 */
DEFUN(no_isis_instance,
      no_isis_instance_cmd,
      "no isis instance <1-255>",
      "no\n" ROUTER_STR "ISO IS-IS\n"
      "ISO Routing area tag")
{
  return isis_area_destroy(vty, argv[0]);
}

DEFUN(no_isis_instance_h3c,
      no_isis_instance_h3c_cmd,
      "undo isis <1-255>",
      "Cancel current configuration\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Process ID\n")
{
  return isis_area_destroy(vty, argv[0]);
}

static void
isis_route_update(struct isis_area *area, struct prefix *prefix,
                  struct isis_route_info *route_info, int family)
{
  struct merge_isis_nexthop *nexthop;
  struct merge_isis_nexthop6 *nexthop6;
  struct listnode *node;
  struct route_entry route;
  struct routefifo *newroutefifoentry;

  memset(&route, 0, sizeof(struct route_entry));

  if (family == AF_INET)
  {
    for (ALL_LIST_ELEMENTS_RO(route_info->nexthops, node, nexthop))
    {
      if (nexthop->ip.s_addr == INADDR_ANY)
        continue;
      route.prefix.type = INET_FAMILY_IPV4;
      route.prefix.addr.ipv4 = ntohl(prefix->u.prefix4.s_addr);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = area->vpn;
      route.nhp_num = 1;

      route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
      route.nhp[0].nexthop.addr.ipv4 = ntohl(nexthop->ip.s_addr);
      route.nhp[0].ifindex = nexthop->ifindex;
      route.nhp[0].action = NHP_ACTION_FORWARD;
      route.nhp[0].cost = nexthop->cost;
      route.nhp[0].distance = area->distance4;
      route.nhp[0].vpn = area->vpn;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].active = 0;
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS;

      newroutefifoentry = XCALLOC(MTYPE_ROUTE_FIFO, sizeof(struct routefifo));
      if (newroutefifoentry == NULL)
      {
        zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "%-15s[%d]: memory xcalloc error!", __FUNCTION__, __LINE__);
        return;
      }
      memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
      newroutefifoentry->opcode = IPC_OPCODE_UPDATE;
      FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
    }
  }

  if (family == AF_INET6)
  {
    for (ALL_LIST_ELEMENTS_RO(route_info->nexthops6, node, nexthop6))
    {
      if (!IN6_IS_ADDR_LINKLOCAL(&nexthop6->ip) && !IN6_IS_ADDR_UNSPECIFIED(&nexthop6->ip))
        continue;
      route.prefix.type = INET_FAMILY_IPV6;
      memcpy(&route.prefix.addr.ipv6, &prefix->u.prefix6, 128);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = 0;
      route.nhp_num = 1;

      route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
      memcpy(&route.nhp[0].nexthop.addr.ipv6, &nexthop6->ip, 128);
      route.nhp[0].ifindex = nexthop6->ifindex;
      route.nhp[0].action = NHP_ACTION_FORWARD;
      route.nhp[0].cost = route_info->cost;
      route.nhp[0].distance = area->distance6;
      route.nhp[0].vpn = area->vpn;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].active = 0;
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS6;

      newroutefifoentry = XCALLOC(MTYPE_ROUTE_FIFO, sizeof(struct routefifo));
      if (newroutefifoentry == NULL)
      {
        zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "%-15s[%d]: memory xcalloc error!", __FUNCTION__, __LINE__);
        return;
      }
      memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
      newroutefifoentry->opcode = IPC_OPCODE_UPDATE;
      isis_sta.isis_route_add_merge++;
      FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
    }
  }
}

/* Validating routes in particular table. */
static void
isis_route_distance_update(struct isis_area *area, struct route_table *table)
{
  struct route_node *rnode;
  struct isis_route_info *rinfo;

  if (table == NULL)
    return;

  for (rnode = route_top(table); rnode; rnode = route_next(rnode))
  {
    if (rnode->info == NULL)
      continue;
    rinfo = rnode->info;
    isis_route_update(area, &rnode->p, rinfo, rnode->p.family);
  }
}

#if 1
static void
isis_route_validate_distance_merge(struct isis_area *area, int family)
{
  struct route_table *table = NULL;
  struct route_table *merge;
  struct route_node *rnode, *mrnode;
  struct isis_route_info *rinfo, *minfo;

  merge = route_table_init();

  if (family == AF_INET)
    table = area->route_table[0];
#if 1
  else if (family == AF_INET6)
    table = area->route_table6[0];
#endif
  if (table != NULL)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;

      rinfo = rnode->info;

      mrnode = route_node_get(merge, &rnode->p);
      mrnode->info = rnode->info;
    }
  }

  if (family == AF_INET)
    table = area->route_table[1];
#if 1
  else if (family == AF_INET6)
    table = area->route_table6[1];
#endif
  if (table != NULL)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;

      rinfo = rnode->info;

      mrnode = route_node_get(merge, &rnode->p);
      if (mrnode->info != NULL)
      {
        minfo = mrnode->info;
        if (family == AF_INET)
          isis_route_info_same_merge(mrnode, rinfo, minfo);

        if (family == AF_INET6)
          isis_route_info_ipv6_same_merge(mrnode, rinfo, minfo);
        /*select route to upadte*/
        continue;
      }
      mrnode->info = rnode->info;
    }
  }

  isis_route_distance_update(area, merge);
  route_table_finish(merge);
  merge = NULL;
}
#endif

void isis_route_validate_distance(struct isis_area *area, int family)
{
  if (family == AF_INET)
  {
    if (area->is_type == IS_LEVEL_1)
      isis_route_distance_update(area, area->route_table[0]);
    else if (area->is_type == IS_LEVEL_2)
      isis_route_distance_update(area, area->route_table[1]);
    else
      isis_route_validate_distance_merge(area, AF_INET);
  }

  if (family == AF_INET6)
  {
    if (area->is_type == IS_LEVEL_1)
      isis_route_distance_update(area, area->route_table6[0]);
    else if (area->is_type == IS_LEVEL_2)
      isis_route_distance_update(area, area->route_table6[1]);
    else
      isis_route_validate_distance_merge(area, AF_INET6);
  }

  return;
}

DEFUN(distance,
      distance_cmd,
      "distance <1-255>",
      "distance value"
      "ISO IS-IS\n"
      "ISO Routing area distance")
{
  struct isis_area *area;
  area = vty->index;

  if (!area)
    return CMD_SUCCESS;

  if (area->distance4 != atoi(argv[0]))
  {
    area->distance4 = atoi(argv[0]);
    isis_route_validate_distance(area, AF_INET);
  }

  if (area->distance6 != atoi(argv[0]))
  {
    area->distance6 = atoi(argv[0]);
    isis_route_validate_distance(area, AF_INET6);
  }

  return CMD_SUCCESS;
}

DEFUN(no_distance,
      no_distance_cmd,
      "no distance",
      "distance value default"
      "ISO IS-IS\n"
      "ISO Routing area distance")
{
  struct isis_area *area = NULL;
  area = vty->index;

  if (!area)
    return CMD_SUCCESS;

  if (area->distance4 != DEFAULT_DISTANCE)
  {
    area->distance4 = DEFAULT_DISTANCE;
    isis_route_validate_distance(area, AF_INET);
  }

  if (area->distance6 != DEFAULT_DISTANCE)
  {
    area->distance6 = DEFAULT_DISTANCE;
    isis_route_validate_distance(area, AF_INET6);
  }

  return CMD_SUCCESS;
}

DEFUN(ipv4_distance_h3c,
      ipv4_distance_h3c_cmd,
      "preference <1-255>",
      "Set ISIS route preference\n"
      "Preference value\n")
{
  struct isis_area *area;
  area = vty->index;

  if (!area)
    return CMD_SUCCESS;

  if (area->distance4 == atoi(argv[0]))
    return CMD_SUCCESS;

  area->distance4 = atoi(argv[0]);
  isis_route_validate_distance(area, AF_INET);
  return CMD_SUCCESS;
}

DEFUN(ipv6_distance_h3c,
      ipv6_distance_h3c_cmd,
      "preference <1-255>",
      "Set ISIS route preference\n"
      "Preference value\n")
{
  struct isis_area *area;
  area = vty->index;

  if (!area)
    return CMD_SUCCESS;

  if (area->distance6 == atoi(argv[0]))
    return CMD_SUCCESS;

  area->distance6 = atoi(argv[0]);
  isis_route_validate_distance(area, AF_INET6);

  return CMD_SUCCESS;
}

DEFUN(no_ipv4_distance_h3c,
      no_ipv4_distance_h3c_cmd,
      "undo preference",
      "Cancel current configuration\n"
      "Set ISIS route preference\n")
{
  struct isis_area *area;
  area = vty->index;

  if (!area)
    return CMD_SUCCESS;

  if (area->distance4 != DEFAULT_DISTANCE)
  {
    area->distance4 = DEFAULT_DISTANCE;
    isis_route_validate_distance(area, AF_INET);
  }

  return CMD_SUCCESS;
}

DEFUN(no_ipv6_distance_h3c,
      no_ipv6_distance_h3c_cmd,
      "undo preference",
      "Cancel current configuration\n"
      "Set ISIS route preference\n")
{
  struct isis_area *area;

  area = vty->index;

  if (!area)
    return CMD_SUCCESS;

  if (area->distance6 != DEFAULT_DISTANCE)
  {
    area->distance6 = DEFAULT_DISTANCE;
    isis_route_validate_distance(area, AF_INET6);
  }

  return CMD_SUCCESS;
}

/*
 * 'net' command
 */
DEFUN(net_entity,
      net_entity_cmd,
      "net entity WORD",
      "ISIS Network Entity Title\n"
      "A Network Entity Title for this process (OSI only)\n"
      "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")
{
  return area_net_title(vty, argv[0]);
}

DEFUN(net_entity_h3c,
      net_entity_h3c_cmd,
      "network-entity WORD",
      "Set network entity title (NET) for ISIS\n"
      "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")
{
  return area_net_title(vty, argv[0]);
}

/*
 * 'no net' command
 */
DEFUN(no_net_entity,
      no_net_entity_cmd,
      "no net entity WORD",
      NO_STR
      "ISIS Network Entity Title\n"
      "A Network Entity Title for this process (OSI only)\n"
      "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")
{
  return area_clear_net_title(vty, argv[0]);
}

DEFUN(no_net_entity_h3c,
      no_net_entity_h3c_cmd,
      "undo network-entity WORD",
      "Cancel current configuration\n"
      "Set network entity title (NET) for ISIS\n"
      "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")
{
  return area_clear_net_title(vty, argv[0]);
}

static int area_set_lsp_mtu(struct vty *vty, struct isis_area *area, unsigned int lsp_mtu)
{
  struct isis_circuit *circuit;
  struct listnode *node;
  unsigned int old_lsp_mtu = area->lsp_mtu;

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
  {
    if (lsp_mtu > isis_circuit_pdu_size(circuit))
    {
      vty_warning_out(vty, "Please ensure that the LSP length is smaller than the MTU of the interface enabled with IS-IS. Otherwise, LSP packets cannot be sent.%s", VTY_NEWLINE);
      return CMD_SUCCESS;
    }
  }

  area->lsp_mtu = lsp_mtu;
  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
  {
    if (old_lsp_mtu > isis_circuit_pdu_size(circuit))
    {
      if (circuit->interface != NULL)
      {
        if (if_is_operative(circuit->interface))
        {
          isis_csm_state_change(IF_UP_FROM_Z, circuit, circuit->interface);
        }
      }
      return CMD_SUCCESS;
    }
  }
  lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 1);

  return CMD_SUCCESS;
}

DEFUN(lsp_length,
      lsp_length_cmd,
      "lsp length <128-3000>",
      "IS-IS link state database\n"
      "Configure the maximum size of generated LSPs\n"
      "Maximum size of generated LSPs\n")
{
  struct isis_area *area;
  uint16_t interval;

  interval = atoi(argv[0]);

  if (interval < 128)
  {
    vty_error_out(vty, "Invalid Interval %d - should be <128-3000>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3000)
  {
    vty_error_out(vty, "Invalid Interval %d - should be <128-3000>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = vty->index;
  if (!area)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  unsigned int lsp_mtu;

  VTY_GET_INTEGER_RANGE("lsp-mtu", lsp_mtu, argv[0], 128, 4352);

  return area_set_lsp_mtu(vty, area, lsp_mtu);
}

DEFUN(lsp_length_h3c,
      lsp_length_h3c_cmd,
      "lsp-length <128-3000>",
      "Set maximum originated LSP or received LSP size\n"
      "Max LSP size in bytes\n")
{
  struct isis_area *area;
  uint16_t interval;

  interval = atoi(argv[0]);

  if (interval < 128)
  {
    vty_error_out(vty, "Invalid Interval %d - should be <128-3000>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3000)
  {
    vty_error_out(vty, "Invalid Interval %d - should be <128-3000>%s",
                  atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = vty->index;
  if (!area)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  unsigned int lsp_mtu;

  VTY_GET_INTEGER_RANGE("lsp-mtu", lsp_mtu, argv[0], 128, 4352);

  return area_set_lsp_mtu(vty, area, lsp_mtu);
}

DEFUN(no_lsp_length,
      no_lsp_length_cmd,
      "no lsp length",
      NO_STR
      "IS-IS link state database\n"
      "Configure the maximum size of generated LSPs\n")
{
  struct isis_area *area;

  area = vty->index;
  if (!area)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return area_set_lsp_mtu(vty, area, DEFAULT_LSP_MTU);
}

DEFUN(no_lsp_length_h3c,
      no_lsp_length_h3c_cmd,
      "undo lsp-length",
      "Cancel current configuration\n"
      "Set maximum originated LSP or received LSP size\n")
{
  struct isis_area *area;

  area = vty->index;
  if (!area)
  {
    vty_error_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return area_set_lsp_mtu(vty, area, DEFAULT_LSP_MTU);
}

DEFUN(level,
      level_cmd,
      "level (level-1|level-1-2|level-2)",
      "IS Level for this routing process (OSI only)\n"
      "Act as a station router only\n"
      "Act as both a station router and an area router\n"
      "Act as an area router only\n")
{
  struct isis_area *area;
  int type;

  area = vty->index;

  if (!area)
  {
    vty_error_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->sysid_set == 0)
  {
    vty_error_out(vty, "Can't change level when net entity unset%s ", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  type = string2circuit_t(argv[0]);
  if (!type)
  {
    vty_error_out(vty, "Unknown IS level %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  isis_event_system_type_change(area, type);

  return CMD_SUCCESS;
}

DEFUN(level_h3c,
      level_h3c_cmd,
      "is-level (level-1|level-1-2|level-2)",
      "Set ISIS level for the system\n"
      "Level-1\n"
      "Level-1-2\n"
      "Level-2\n")
{
  struct isis_area *area;
  int type;

  area = vty->index;

  if (!area)
  {
    vty_error_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->sysid_set == 0)
  {
    vty_error_out(vty, "Can't change level when net entity unset%s ", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  type = string2circuit_t(argv[0]);
  if (!type)
  {
    vty_error_out(vty, "Unknown IS level %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  isis_event_system_type_change(area, type);

  return CMD_SUCCESS;
}

DEFUN(no_level,
      no_level_cmd,
      "no level",
      NO_STR
      "IS Level for this routing process (OSI only)\n")
{
  struct isis_area *area;
  int type;

  area = vty->index;
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  /*
   * Put the is-type back to defaults:
   * - level-1-2 on first area
   * - level-1 for the rest
   */
  if (area->is_type == IS_LEVEL_1_AND_2)
    return CMD_SUCCESS;

  else
    type = IS_LEVEL_1_AND_2;

  isis_event_system_type_change(area, type);

  return CMD_SUCCESS;
}

DEFUN(no_level_h3c,
      no_level_h3c_cmd,
      "undo is-level",
      "Cancel current configuration\n"
      "Set ISIS level for the system\n")
{
  struct isis_area *area;
  int type;

  area = vty->index;
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  /*
   * Put the is-type back to defaults:
   * - level-1-2 on first area
   * - level-1 for the rest
   */
  if (area->is_type == IS_LEVEL_1_AND_2)
    return CMD_SUCCESS;

  else
    type = IS_LEVEL_1_AND_2;

  isis_event_system_type_change(area, type);

  return CMD_SUCCESS;
}

static int
set_lsp_gen_interval(struct vty *vty, struct isis_area *area,
                     uint16_t interval, int level)
{
  int lvl;

  for (lvl = IS_LEVEL_1; lvl <= IS_LEVEL_2; ++lvl)
  {
    if (!(lvl & level))
      continue;

    if (interval >= area->lsp_refresh[lvl - 1])
    {
      vty_out(vty, "LSP gen interval %us must be less than "
                   "the LSP refresh interval %us%s",
              interval, area->lsp_refresh[lvl - 1], VTY_NEWLINE);
      return CMD_SUCCESS;
    }
  }

  for (lvl = IS_LEVEL_1; lvl <= IS_LEVEL_2; ++lvl)
  {
    if (!(lvl & level))
      continue;
    area->lsp_gen_interval[lvl - 1] = interval;
  }

  return CMD_SUCCESS;
}

DEFUN(lsp_gen_interval,
      lsp_gen_interval_cmd,
      "lsp gen-interval <1-3600>",
      "IS-IS link state database\n"
      "Minimum interval between regenerating same LSP\n"
      "Minimum interval in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 1)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_gen_interval(vty, area, interval, level);
}

DEFUN(lsp_gen_interval_h3c,
      lsp_gen_interval_h3c_cmd,
      "timer lsp-generation <1-3600>",
      "Set ISIS timer\n"
      "Set the intelligent timer for LSP generation\n"
      "Set the intelligent timer maximum interval (Seconds)\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 1)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_gen_interval(vty, area, interval, level);
}

DEFUN(no_lsp_gen_interval,
      no_lsp_gen_interval_cmd,
      "no lsp gen-interval",
      NO_STR
      "IS-IS link state database\n"
      "Minimum interval between regenerating same LSP\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_MIN_LSP_GEN_INTERVAL;
  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_gen_interval(vty, area, interval, level);
}

DEFUN(no_lsp_gen_interval_h3c,
      no_lsp_gen_interval_h3c_cmd,
      "undo timer lsp-generation",
      "Cancel current configuration\n"
      "Set ISIS timer\n"
      "Set the intelligent timer for LSP generation\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_MIN_LSP_GEN_INTERVAL;
  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_gen_interval(vty, area, interval, level);
}

DEFUN(isis_address_family_ipv4,
      isis_address_family_ipv4_cmd,
      "address-family ipv4",
      "Specify an address family\n"
      "Specify the IPv4 address family\n")
{
  vty->node = ISIS_FAMILY_V4_NODE;
  return CMD_SUCCESS;
}

DEFUN(isis_address_family_ipv6,
      isis_address_family_ipv6_cmd,
      "address-family ipv6",
      "Specify an address family\n"
      "Specify the IPv6 address family\n")
{
  vty->node = ISIS_FAMILY_V6_NODE;
  return CMD_SUCCESS;
}

#if 0
DEFUN (lsp_gen_interval_l1,
       lsp_gen_interval_l1_cmd,
       "lsp-gen-interval level-1 <1-120>",
       "Minimum interval between regenerating same LSP\n"
       "Set interval for level 1 only\n"
       "Minimum interval in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi (argv[0]);
  level = IS_LEVEL_1;
  return set_lsp_gen_interval (vty, area, interval, level);
}

DEFUN (no_lsp_gen_interval_l1,
       no_lsp_gen_interval_l1_cmd,
       "no lsp-gen-interval level-1",
       NO_STR
       "Minimum interval between regenerating same LSP\n"
       "Set interval for level 1 only\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_MIN_LSP_GEN_INTERVAL;
  level = IS_LEVEL_1;
  return set_lsp_gen_interval (vty, area, interval, level);
}

ALIAS (no_lsp_gen_interval_l1,
       no_lsp_gen_interval_l1_arg_cmd,
       "no lsp-gen-interval level-1 <1-120>",
       NO_STR
       "Minimum interval between regenerating same LSP\n"
       "Set interval for level 1 only\n"
       "Minimum interval in seconds\n")

DEFUN (lsp_gen_interval_l2,
       lsp_gen_interval_l2_cmd,
       "lsp-gen-interval level-2 <1-120>",
       "Minimum interval between regenerating same LSP\n"
       "Set interval for level 2 only\n"
       "Minimum interval in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi (argv[0]);
  level = IS_LEVEL_2;
  return set_lsp_gen_interval (vty, area, interval, level);
}

DEFUN (no_lsp_gen_interval_l2,
       no_lsp_gen_interval_l2_cmd,
       "no lsp-gen-interval level-2",
       NO_STR
       "Minimum interval between regenerating same LSP\n"
       "Set interval for level 2 only\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_MIN_LSP_GEN_INTERVAL;
  level = IS_LEVEL_2;
  return set_lsp_gen_interval (vty, area, interval, level);
}

ALIAS (no_lsp_gen_interval_l2,
       no_lsp_gen_interval_l2_arg_cmd,
       "no lsp-gen-interval level-2 <1-120>",
       NO_STR
       "Minimum interval between regenerating same LSP\n"
       "Set interval for level 2 only\n"
       "Minimum interval in seconds\n")
#endif

/*
static int
validate_metric_style_narrow(struct vty *vty, struct isis_area *area)
{
  struct isis_circuit *circuit;
  struct listnode *node;

  if (!vty)
    return CMD_SUCCESS;

  if (!area)
  {
    vty_error_out(vty, "ISIS area is invalid%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
  {
    if ((area->is_type & IS_LEVEL_1) &&
        (circuit->is_type & IS_LEVEL_1) &&
        (circuit->te_metric[0] > MAX_NARROW_LINK_METRIC))
    {
      vty_warning_out(vty, "ISIS circuit %s metric is invalid,change to default%s",
                      circuit->interface->name, VTY_NEWLINE);
      return CMD_SUCCESS;
    }
    if ((area->is_type & IS_LEVEL_2) &&
        (circuit->is_type & IS_LEVEL_2) &&
        (circuit->te_metric[1] > MAX_NARROW_LINK_METRIC))
    {
      vty_warning_out(vty, "ISIS circuit %s metric is invalid,change to default%s",
                      circuit->interface->name, VTY_NEWLINE);
      return CMD_SUCCESS;
    }
  }

  return CMD_SUCCESS;
}

void isis_redist_reconf(struct isis_area *area)
{
  struct route_table *er_table;
  struct isis_ext_info *redist_info;
  struct route_node *rn;
  char level;

  for (level = 1; level < 3; level++)
  {
    if (!(level & area->is_type))
      continue;

    er_table = get_ext_reach(area, str2family("ipv4"), level);

    if (er_table == NULL)
      continue;

    for (rn = route_top(er_table); rn; rn = route_next(rn))
    {
      redist_info = rn->info;
	  
	  if(!redist_info)
	  	continue;
	  
      if (area->oldmetric == 1)
      {
        if (redist_info->metric > 63)
          redist_info->metric = 64;
      }
      else if (area->newmetric == 0 && area->oldmetric == 1)
      {
        if (redist_info != NULL && redist_info->internal_external_flag == ROUTE_IS_EXTERNAL)
          redist_info->metric += 64;
      }
      else
      {
        if (redist_info != NULL && redist_info->internal_external_flag == ROUTE_IS_EXTERNAL && redist_info->metric > 63)
          redist_info->metric -= 64;
      }
    }

    er_table = get_ext_reach(area, str2family("ipv6"), level);

    if (er_table == NULL)
      continue;

    for (rn = route_top(er_table); rn; rn = route_next(rn))
    {
      redist_info = rn->info;
      if (area->newmetric == 1 && area->oldmetric == 0)
      {
        if (redist_info != NULL && redist_info->internal_external_flag == ROUTE_IS_EXTERNAL)
          redist_info->metric -= 64;
      }
      else if (area->newmetric == 0 && area->oldmetric == 1)
      {
        if (redist_info != NULL && redist_info->internal_external_flag == ROUTE_IS_EXTERNAL)
          redist_info->metric += 64;
      }
      else
      {
        if (redist_info != NULL && redist_info->internal_external_flag == ROUTE_IS_EXTERNAL)
          redist_info->metric -= 64;
      }
    }
  }
}

void isis_circuit_metric_reconf(struct isis_area *area)
{
  struct isis_circuit *circuit;
  struct listnode *node, *nnode;

  if (area->circuit_list)
  {
    for (ALL_LIST_ELEMENTS(area->circuit_list, node, nnode, circuit))
    {
      if (circuit == NULL)
      {
        continue;
      }
      if (circuit->te_metric[0] > 63)
      {
        circuit->te_metric[0] = 10;
        circuit->te_metric[1] = 10;
        circuit->metrics[0].metric_default = 10;
        circuit->metrics[1].metric_default = 10;
      }
    }
  }
}
*/

DEFUN(metric_style,
      metric_style_cmd,
      "metric-style (narrow|transition|wide)",
      "Use old-style (ISO 10589) or new-style packet formats\n"
      "Use old style of TLVs with narrow metric\n"
      "Send and accept both styles of TLVs during transition\n"
      "Use new style of TLVs to carry wider metric\n")
{
  struct isis_area *area;

  area = vty->index;

  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (strncmp(argv[0], "wide", 1) == 0)
  {
    if (!(area->oldmetric == 0 && area->newmetric == 1 && area->compatible == 0))
    {
      area->newmetric = 1;
      area->oldmetric = 0;
      area->compatible = 0;
      if (area->ref_bandwidth_ipv4 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
      if (area->ref_bandwidth_ipv6 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

      lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
    }
  }
  else if (strncmp(argv[0], "tra", 3) == 0)
  {
    if (!(area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1))
    {
      area->newmetric = 1;
      area->oldmetric = 1;
      area->compatible = 1;
      if (area->ref_bandwidth_ipv4 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
      if (area->ref_bandwidth_ipv6 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

      lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
    }
  }
  else
  {
    if (strncmp(argv[0], "n", 1) == 0)
    {
      if (!(area->oldmetric == 1 && area->newmetric == 0 && area->compatible == 0))
      {
        area->newmetric = 0;
        area->oldmetric = 1;
        area->compatible = 0;
        if (area->ref_bandwidth_ipv4 != 0)
          isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
		if (area->ref_bandwidth_ipv6 != 0)
		  isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

        lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
      }
    }
  }

  return CMD_SUCCESS;
}

DEFUN(metric_style_h3c,
      metric_style_h3c_cmd,
      "cost-style (narrow|wide|wide-compatible|narrow-compatible|compatible)",
      "Configure cost style for ISIS\n"
      "Set cost style to narrow\n"
      "Set cost style to wide\n"
      "Set cost style to wide-compatible\n"
      "Set cost style to narrow-compatible\n"
      "Set cost style to compatible\n")
{
  struct isis_area *area;

  area = vty->index;

  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (strncmp(argv[0], "wide-", 5) == 0)
  {
    if (!(area->oldmetric == 0 && area->newmetric == 1 && area->compatible == 1))
    {
      area->newmetric = 1;
      area->oldmetric = 0;
      area->compatible = 1;
      if (area->ref_bandwidth_ipv4 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
	  if (area->ref_bandwidth_ipv6 != 0)
		isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

      lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
    }
  }
  else if (strncmp(argv[0], "narrow-", 7) == 0)
  {
    if (!(area->oldmetric == 1 && area->newmetric == 0 && area->compatible == 1))
    {
      area->newmetric = 0;
      area->oldmetric = 1;
      area->compatible = 1;
      if (area->ref_bandwidth_ipv4 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
	  if (area->ref_bandwidth_ipv6 != 0)
		isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

      lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
    }
  }
  else if (strncmp(argv[0], "wide", 1) == 0)
  {
    if (!(area->oldmetric == 0 && area->newmetric == 1 && area->compatible == 0))
    {
      area->newmetric = 1;
      area->oldmetric = 0;
      area->compatible = 0;
      if (area->ref_bandwidth_ipv4 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
	  if (area->ref_bandwidth_ipv6 != 0)
		isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

      lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
    }
  }
  else if (strncmp(argv[0], "com", 3) == 0)
  {
    if (!(area->oldmetric == 1 && area->newmetric == 1 && area->compatible == 1))
    {
      area->newmetric = 1;
      area->oldmetric = 1;
      area->compatible = 1;
      if (area->ref_bandwidth_ipv4 != 0)
        isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
	  if (area->ref_bandwidth_ipv6 != 0)
		isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

      lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
    }
  }
  else
  {
    if (strncmp(argv[0], "n", 1) == 0)
    {
      if (!(area->oldmetric == 1 && area->newmetric == 0 && area->compatible == 0))
      {
        area->newmetric = 0;
        area->oldmetric = 1;
        area->compatible = 0;
		if (area->ref_bandwidth_ipv4 != 0)
		  isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
		if (area->ref_bandwidth_ipv6 != 0)
		  isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

        lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
      }
    }
  }

  return CMD_SUCCESS;
}

DEFUN(no_metric_style,
      no_metric_style_cmd,
      "no metric-style",
      NO_STR
      "Use old-style (ISO 10589) or new-style packet formats\n")
{
  struct isis_area *area;

  area = vty->index;

  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (!((area->newmetric == 1) && (area->oldmetric == 0)))
  {
    area->newmetric = 1;
    area->oldmetric = 0;
    area->compatible = 0;
    if (area->ref_bandwidth_ipv4 != 0)
      isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
	if (area->ref_bandwidth_ipv6 != 0)
	  isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

    lsp_regenerate_schedule(area, area->is_type, 1);
  }

  return CMD_SUCCESS;
}

DEFUN(no_metric_style_h3c,
      no_metric_style_h3c_cmd,
      "undo cost-style",
      "Cancel current configuration\n"
      "Configure cost style for ISIS\n")
{
  struct isis_area *area;

  area = vty->index;
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (!((area->newmetric == 0) && (area->oldmetric == 1)))
  {
    area->newmetric = 0;
    area->oldmetric = 1;
    area->compatible = 0;
    if (area->ref_bandwidth_ipv4 != 0)
      isis_set_interface_autocost(area, area->ref_bandwidth_ipv4, AF_INET);
	if (area->ref_bandwidth_ipv6 != 0)
	  isis_set_interface_autocost(area, area->ref_bandwidth_ipv6, AF_INET6);

    lsp_regenerate_schedule(area, area->is_type, 1);
  }

  return CMD_SUCCESS;
}

DEFUN(overload_bit_enable,
      overload_bit_enable_cmd,
      "overload bit enable",
      "Set overload bit to avoid any transit traffic\n"
      "Set overload bit\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  area->overload_bit = LSPBIT_OL;
  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  return CMD_SUCCESS;
}

DEFUN(overload_bit_enable_h3c,
      overload_bit_enable_h3c_cmd,
      "set-overload",
      "Configure the router to advertise itself as overloaded\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  area->overload_bit = LSPBIT_OL;
  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  return CMD_SUCCESS;
}

DEFUN(no_overload_bit_enable,
      no_overload_bit_enable_cmd,
      "no overload bit enable",
      "Reset overload bit to accept transit traffic\n"
      "Reset overload bit\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }
  area->overload_bit = 0;
  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  return CMD_SUCCESS;
}

DEFUN(no_overload_bit_enable_h3c,
      no_overload_bit_enable_h3c_cmd,
      "undo set-overload",
      "Cancel current configuration\n"
      "Configure the router to advertise itself as overloaded\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }
  area->overload_bit = 0;
  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  return CMD_SUCCESS;
}

DEFUN(attached_bit_enable,
      attached_bit_enable_cmd,
      "attached bit enable",
      "Set attached bit to identify as L1/L2 router for inter-area traffic\n"
      "Set attached bit\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  area->attached_bit = LSPBIT_ATT;
  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  return CMD_SUCCESS;
}

DEFUN(no_attached_bit_enable,
      no_attached_bit_enable_cmd,
      "no attached bit enable",
      "Reset attached bit\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  area->attached_bit = 0;
  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  return CMD_SUCCESS;
}

DEFUN(dynamic_hostname,
      dynamic_hostname_cmd,
      "hostname dynamic",
      "Dynamic hostname for IS-IS\n"
      "Dynamic hostname\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (!area->dynhostname)
  {
    area->dynhostname = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);
  }

  return CMD_SUCCESS;
}

DEFUN(no_dynamic_hostname,
      no_dynamic_hostname_cmd,
      "no hostname dynamic",
      NO_STR
      "Dynamic hostname for IS-IS\n"
      "Dynamic hostname\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (area->dynhostname)
  {
    area->dynhostname = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);
  }

  return CMD_SUCCESS;
}

DEFUN(spf_interval,
      spf_interval_cmd,
      "spf interval <1-3600>",
      "Minimum interval between SPF calculations\n"
      "Minimum interval between consecutive SPFs in seconds\n")
{
  struct isis_area *area;
  u_int16_t interval;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 1)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area->min_spf_interval[0] = interval;
  area->min_spf_interval[1] = interval;

  return CMD_SUCCESS;
}

DEFUN(spf_interval_h3c,
      spf_interval_h3c_cmd,
      "timer spf <1-3600>",
      "Set ISIS timer\n"
      "The timer for SPF calculations\n"
      "Maximum interval between ISPFs (Seconds)\n")
{
  struct isis_area *area;
  u_int16_t interval;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 1)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    //vty_out(vty,"Ivalid Command%s",VTY_NEWLINE);
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area->min_spf_interval[0] = interval;
  area->min_spf_interval[1] = interval;

  return CMD_SUCCESS;
}

DEFUN(no_spf_interval,
      no_spf_interval_cmd,
      "no spf interval",
      NO_STR
      "Minimum interval between SPF calculations\n")
{
  struct isis_area *area;

  area = vty->index;

  area->min_spf_interval[0] = MINIMUM_SPF_INTERVAL;
  area->min_spf_interval[1] = MINIMUM_SPF_INTERVAL;

  return CMD_SUCCESS;
}

DEFUN(no_spf_interval_h3c,
      no_spf_interval_h3c_cmd,
      "undo timer spf",
      "Cancel current setting\n"
      "Set ISIS timer\n"
      "The timer for SPF calculations\n")
{
  struct isis_area *area;

  area = vty->index;

  area->min_spf_interval[0] = MINIMUM_SPF_INTERVAL;
  area->min_spf_interval[1] = MINIMUM_SPF_INTERVAL;

  return CMD_SUCCESS;
}

#if 0
DEFUN (spf_interval_l1,
       spf_interval_l1_cmd,
       "spf-interval level-1 <1-120>",
       "Minimum interval between SPF calculations\n"
       "Set interval for level 1 only\n"
       "Minimum interval between consecutive SPFs in seconds\n")
{
  struct isis_area *area;
  u_int16_t interval;

  area = vty->index;
  interval = atoi (argv[0]);
  area->min_spf_interval[0] = interval;

  return CMD_SUCCESS;
}

DEFUN (no_spf_interval_l1,
       no_spf_interval_l1_cmd,
       "no spf-interval level-1",
       NO_STR
       "Minimum interval between SPF calculations\n"
       "Set interval for level 1 only\n")
{
  struct isis_area *area;

  area = vty->index;

  area->min_spf_interval[0] = MINIMUM_SPF_INTERVAL;

  return CMD_SUCCESS;
}

ALIAS (no_spf_interval,
       no_spf_interval_l1_arg_cmd,
       "no spf-interval level-1 <1-120>",
       NO_STR
       "Minimum interval between SPF calculations\n"
       "Set interval for level 1 only\n"
       "Minimum interval between consecutive SPFs in seconds\n")

DEFUN (spf_interval_l2,
       spf_interval_l2_cmd,
       "spf-interval level-2 <1-120>",
       "Minimum interval between SPF calculations\n"
       "Set interval for level 2 only\n"
       "Minimum interval between consecutive SPFs in seconds\n")
{
  struct isis_area *area;
  u_int16_t interval;

  area = vty->index;
  interval = atoi (argv[0]);
  area->min_spf_interval[1] = interval;

  return CMD_SUCCESS;
}

DEFUN (no_spf_interval_l2,
       no_spf_interval_l2_cmd,
       "no spf-interval level-2",
       NO_STR
       "Minimum interval between SPF calculations\n"
       "Set interval for level 2 only\n")
{
  struct isis_area *area;

  area = vty->index;

  area->min_spf_interval[1] = MINIMUM_SPF_INTERVAL;

  return CMD_SUCCESS;
}

ALIAS (no_spf_interval,
       no_spf_interval_l2_arg_cmd,
       "no spf-interval level-2 <1-120>",
       NO_STR
       "Minimum interval between SPF calculations\n"
       "Set interval for level 2 only\n"
       "Minimum interval between consecutive SPFs in seconds\n")
#endif
static int
set_lsp_max_lifetime(struct vty *vty, struct isis_area *area,
                     uint16_t interval, int level)
{
  int lvl;
  int set_refresh_interval[ISIS_LEVELS] = {0, 0};
  uint16_t refresh_interval;

  refresh_interval = interval - 300;

  for (lvl = IS_LEVEL_1; lvl <= IS_LEVEL_2; lvl++)
  {
    if (!(lvl & level))
      continue;
    if (refresh_interval < area->lsp_refresh[lvl - 1])
    {
      vty_warning_out(vty, "Level %d Max LSP lifetime %us must be 300s greater than "
                           "the configured LSP refresh interval %us%s",
                      lvl, interval, area->lsp_refresh[lvl - 1], VTY_NEWLINE);
      vty_warning_out(vty, "Automatically reducing level %d LSP refresh interval "
                           "to %us%s",
                      lvl, refresh_interval, VTY_NEWLINE);
      set_refresh_interval[lvl - 1] = 1;

      if (refresh_interval <= area->lsp_gen_interval[lvl - 1])
      {
        vty_warning_out(vty, "LSP refresh interval %us must be greater than "
                             "the configured LSP gen interval %us%s",
                        refresh_interval, area->lsp_gen_interval[lvl - 1],
                        VTY_NEWLINE);
        return CMD_SUCCESS;
      }
    }
  }

  for (lvl = IS_LEVEL_1; lvl <= IS_LEVEL_2; lvl++)
  {
    if (!(lvl & level))
      continue;
    area->max_lsp_lifetime[lvl - 1] = interval;
    /* Automatically reducing lsp_refresh_interval to interval - 300 */
    if (set_refresh_interval[lvl - 1])
    {
      /*save it first ,recover is when no lifetime directly */
      area->lsp_refresh_ago = area->lsp_refresh[lvl - 1];
      area->lsp_refresh[lvl - 1] = refresh_interval;
    }
  }

  lsp_regenerate_schedule(area, level, 1);

  return CMD_SUCCESS;
}

DEFUN(lsp_lifetime,
      lsp_lifetime_cmd,
      "lsp lifetime <360-3600>",
      "IS-IS link state database\n"
      "Maximum LSP lifetime\n"
      "LSP lifetime in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 360)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <360-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <360-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_max_lifetime(vty, area, interval, level);
}

DEFUN(lsp_lifetime_h3c,
      lsp_lifetime_h3c_cmd,
      "timer lsp-max-age <360-3600>",
      "Set ISIS timer\n"
      "Set maximum lifetime of LSP\n"
      "Maximum lifetime of LSP (Seconds)\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 360)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <360-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <360-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_max_lifetime(vty, area, interval, level);
}

static int
set_lsp_refresh_interval(struct vty *vty, struct isis_area *area,
                         uint16_t interval, int level)
{
  int lvl;

  for (lvl = IS_LEVEL_1; lvl <= IS_LEVEL_2; ++lvl)
  {
    if (!(lvl & level))
      continue;
    if (interval <= area->lsp_gen_interval[lvl - 1])
    {
      vty_warning_out(vty, "LSP refresh interval %us must be greater than "
                           "the configured LSP gen interval %us%s",
                      interval, area->lsp_gen_interval[lvl - 1],
                      VTY_NEWLINE);
      return CMD_SUCCESS;
    }
    if (interval > (area->max_lsp_lifetime[lvl - 1] - 300))
    {
      vty_warning_out(vty, "LSP refresh interval %us must be less than "
                           "the configured LSP lifetime %us less 300%s",
                      interval, area->max_lsp_lifetime[lvl - 1],
                      VTY_NEWLINE);
      return CMD_SUCCESS;
    }
  }

  for (lvl = IS_LEVEL_1; lvl <= IS_LEVEL_2; ++lvl)
  {
    if (!(lvl & level))
      continue;
    area->lsp_refresh[lvl - 1] = interval;
  }
  lsp_regenerate_schedule(area, level, 1);

  return CMD_SUCCESS;
}

DEFUN(no_lsp_lifetime,
      no_lsp_lifetime_cmd,
      "no lsp lifetime",
      NO_STR
      "IS-IS link state database\n"
      "LSP lifetime in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_LSP_LIFETIME;
  level = IS_LEVEL_1 | IS_LEVEL_2;

  set_lsp_max_lifetime(vty, area, interval, level);

  /*recover refresh interval*/
  if (area->lsp_refresh_ago > 1 && area->lsp_refresh_ago < 3600)
  {
    set_lsp_refresh_interval(vty, area, area->lsp_refresh_ago, level);
    area->lsp_refresh_ago = 0;
  }

  return CMD_SUCCESS;
}

DEFUN(no_lsp_lifetime_h3c,
      no_lsp_lifetime_h3c_cmd,
      "undo timer lsp-max-age",
      "Cancel current setting\n"
      "Set ISIS timer\n"
      "Set maximum lifetime of LSP\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_LSP_LIFETIME;
  level = IS_LEVEL_1 | IS_LEVEL_2;

  set_lsp_max_lifetime(vty, area, interval, level);

  /*recover refresh interval*/
  if (area->lsp_refresh_ago > 1 && area->lsp_refresh_ago < 3600)
  {
    set_lsp_refresh_interval(vty, area, area->lsp_refresh_ago, level);
    area->lsp_refresh_ago = 0;
  }

  return CMD_SUCCESS;
}

#if 0
DEFUN (max_lsp_lifetime_l1,
       max_lsp_lifetime_l1_cmd,
       "max-lsp-lifetime level-1 <350-65535>",
       "Maximum LSP lifetime for Level 1 only\n"
       "LSP lifetime for Level 1 only in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi (argv[0]);
  level = IS_LEVEL_1;
  return set_lsp_max_lifetime (vty, area, interval, level);
}

DEFUN (no_max_lsp_lifetime_l1,
       no_max_lsp_lifetime_l1_cmd,
       "no max-lsp-lifetime level-1",
       NO_STR
       "LSP lifetime for Level 1 only in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_LSP_LIFETIME;
  level = IS_LEVEL_1;
  return set_lsp_max_lifetime (vty, area, interval, level);
}

ALIAS (no_max_lsp_lifetime_l1,
       no_max_lsp_lifetime_l1_arg_cmd,
       "no max-lsp-lifetime level-1 <350-65535>",
       NO_STR
       "Maximum LSP lifetime for Level 1 only\n"
       "LSP lifetime for Level 1 only in seconds\n")

DEFUN (max_lsp_lifetime_l2,
       max_lsp_lifetime_l2_cmd,
       "max-lsp-lifetime level-2 <350-65535>",
       "Maximum LSP lifetime for Level 2 only\n"
       "LSP lifetime for Level 2 only in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi (argv[0]);
  level = IS_LEVEL_2;
  return set_lsp_max_lifetime (vty, area, interval, level);
}

DEFUN (no_max_lsp_lifetime_l2,
       no_max_lsp_lifetime_l2_cmd,
       "no max-lsp-lifetime level-2",
       NO_STR
       "LSP lifetime for Level 2 only in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_LSP_LIFETIME;
  level = IS_LEVEL_2;
  return set_lsp_max_lifetime (vty, area, interval, level);
}

ALIAS (no_max_lsp_lifetime_l2,
       no_max_lsp_lifetime_l2_arg_cmd,
       "no max-lsp-lifetime level-2 <350-65535>",
       NO_STR
       "Maximum LSP lifetime for Level 2 only\n"
       "LSP lifetime for Level 2 only in seconds\n")
#endif

DEFUN(lsp_refresh_interval,
      lsp_refresh_interval_cmd,
      "lsp refresh-interval <1-3600>",
      "IS-IS link state database\n"
      "LSP refresh interval\n"
      "LSP refresh interval in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 1)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_refresh_interval(vty, area, interval, level);
}

DEFUN(lsp_refresh_interval_h3c,
      lsp_refresh_interval_h3c_cmd,
      "timer lsp-refresh <1-3600>",
      "Set ISIS timer\n"
      "Set LSP refresh interval\n"
      "LSP refresh time (Seconds)\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = atoi(argv[0]);

  if (interval < 1)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval > 3600)
  {
    vty_warning_out(vty, "Invalid Interval %d - should be <1-3600>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_refresh_interval(vty, area, interval, level);
}

DEFUN(no_lsp_refresh_interval,
      no_lsp_refresh_interval_cmd,
      "no lsp refresh-interval",
      NO_STR
      "IS-IS link state database\n"
      "LSP refresh interval in seconds\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_MAX_LSP_GEN_INTERVAL;
  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_refresh_interval(vty, area, interval, level);
}

DEFUN(no_lsp_refresh_interval_h3c,
      no_lsp_refresh_interval_h3c_cmd,
      "undo timer lsp-refresh ",
      "Cancel current setting\n"
      "Set ISIS timer\n"
      "Set LSP refresh interval\n")
{
  struct isis_area *area;
  uint16_t interval;
  int level;

  area = vty->index;
  interval = DEFAULT_MAX_LSP_GEN_INTERVAL;
  level = IS_LEVEL_1 | IS_LEVEL_2;
  return set_lsp_refresh_interval(vty, area, interval, level);
}

DEFUN(log_adj_changes,
      log_adj_changes_cmd,
      "log-adjacency-changes",
      "Log changes in adjacency state\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  area->log_adj_changes = 1;

  return CMD_SUCCESS;
}

DEFUN(no_log_adj_changes,
      no_log_adj_changes_cmd,
      "no log-adjacency-changes",
      "Stop logging changes in adjacency state\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  area->log_adj_changes = 0;

  return CMD_SUCCESS;
}

static int
lsp_statistics_all(struct vty *vty, dict_t *lspdb, char detail, char dynhost)
{
  dnode_t *node = dict_first(lspdb), *next;
  int lsp_count = 0;

  if (detail == ISIS_UI_LEVEL_BRIEF)
  {
    while (node != NULL)
    {
      /* I think it is unnecessary, so I comment it out */
      /* dict_contains (lspdb, node); */
      next = dict_next(lspdb, node);
      node = next;
      lsp_count++;
    }
  }
  else if (detail == ISIS_UI_LEVEL_DETAIL)
  {
    while (node != NULL)
    {
      next = dict_next(lspdb, node);
      node = next;
      lsp_count++;
    }
  }

  return lsp_count;
}

DEFUN(show_isis_statistics,
      show_isis_statistics_cmd,
      "show isis statistics",
      SHOW_STR
      "ISIS information\n"
      "isis packet,lsp,route statistic\n")
{
  struct listnode *node;
  struct isis_area *area;
  struct route_node *rn;
  int level;
  int lsp_count = 0;
  int route_total = 0;

  if (listcount(isis->area_list) == 0)
  {
    memset(&isis_sta, 0, sizeof(struct isis_statistics));
  }

  vty_out(vty, "=============isis packet==============%s", VTY_NEWLINE);
  vty_out(vty, "packet,send total:%d,recv total:%d%s", isis_sta.isis_send, isis_sta.isis_rcv, VTY_NEWLINE);

  vty_out(vty, "=========isis packet detail===========%s", VTY_NEWLINE);
  vty_out(vty, "the lsp  packets send total:%d  recv total:%d  %s", isis_sta.isis_lsp_send, isis_sta.isis_lsp_recv, VTY_NEWLINE);
  vty_out(vty, "the csnp packets:send total:%d  recv total:%d  %s", isis_sta.isis_csnp_send, isis_sta.isis_csnp_recv, VTY_NEWLINE);
  vty_out(vty, "the psnp packets:send total:%d  recv total:%d  %s", isis_sta.isis_psnp_send, isis_sta.isis_psnp_recv, VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    if ((area->is_type & IS_LEVEL_1) &&
        (area->route_table[ISIS_LEVEL1 - 1]))
    {
      for (rn = route_top(area->route_table[ISIS_LEVEL1 - 1]); rn; rn = route_next(rn))
      {
        if (rn->info == NULL)
          continue;

        route_total++;
      }
    }

    if ((area->is_type & IS_LEVEL_2) &&
        (area->route_table[ISIS_LEVEL2 - 1]))
    {
      for (rn = route_top(area->route_table[ISIS_LEVEL2 - 1]); rn; rn = route_next(rn))
      {
        if (rn->info == NULL)
          continue;

        route_total++;
      }
    }
  }

  if (listcount(isis->area_list) == 0)
  {
    memset(&isis_sta, 0, sizeof(struct isis_statistics));
  }

  vty_out(vty, "=========  isis route ===========%s", VTY_NEWLINE);
  vty_out(vty, "the route total:%d   %s", route_total, VTY_NEWLINE);

  vty_out(vty, "=========  spf duration =========%s", VTY_NEWLINE);
  vty_out(vty, "spf duration:%d %s", (u_int32_t)isis_sta.duration_time, VTY_NEWLINE);

  vty_out(vty, "=========  isis LSDB statistics =======%s", VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    for (level = 0; level < ISIS_LEVELS; level++)
    {
      if (area->lspdb[level] && dict_count(area->lspdb[level]) > 0)
      {
        lsp_count = lsp_statistics_all(vty, area->lspdb[level], ISIS_UI_LEVEL_BRIEF,
                                       area->dynhostname) +
                    lsp_count;
      }
    }
  }

  if (listcount(isis->area_list) == 0)
  {
    memset(&isis_sta, 0, sizeof(struct isis_statistics));
  }

  vty_out(vty, "the LSP of LSDB total:%d   %s", lsp_count, VTY_NEWLINE);

  vty_out(vty, "the send to route merge add total:%d %s", isis_sta.isis_route_add_merge, VTY_NEWLINE);
  vty_out(vty, "the send to route merge del total:%d %s", isis_sta.isis_route_del_merge, VTY_NEWLINE);

  return CMD_SUCCESS;
}

DEFUN(show_isis_statistics_h3c,
      show_isis_statistics_h3c_cmd,
      "display isis statistics",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "Statistics information\n")
{
  struct listnode *node;
  struct isis_area *area;
  struct route_node *rn;
  int level;
  int lsp_count = 0;
  int route_total = 0;

  if (listcount(isis->area_list) == 0)
  {
    memset(&isis_sta, 0, sizeof(struct isis_statistics));
  }

  vty_out(vty, "=============isis packet==============%s", VTY_NEWLINE);
  vty_out(vty, "packet,send total:%d,recv total:%d%s", isis_sta.isis_send, isis_sta.isis_rcv, VTY_NEWLINE);

  vty_out(vty, "=========isis packet detail===========%s", VTY_NEWLINE);
  vty_out(vty, "the lsp  packets send total:%d  recv total:%d  %s", isis_sta.isis_lsp_send, isis_sta.isis_lsp_recv, VTY_NEWLINE);
  vty_out(vty, "the csnp packets:send total:%d  recv total:%d  %s", isis_sta.isis_csnp_send, isis_sta.isis_csnp_recv, VTY_NEWLINE);
  vty_out(vty, "the psnp packets:send total:%d  recv total:%d  %s", isis_sta.isis_psnp_send, isis_sta.isis_psnp_recv, VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    if ((area->is_type & IS_LEVEL_1) &&
        (area->route_table[ISIS_LEVEL1 - 1]))
    {
      for (rn = route_top(area->route_table[ISIS_LEVEL1 - 1]); rn; rn = route_next(rn))
      {
        if (rn->info == NULL)
          continue;

        route_total++;
      }
    }

    if ((area->is_type & IS_LEVEL_2) &&
        (area->route_table[ISIS_LEVEL2 - 1]))
    {
      for (rn = route_top(area->route_table[ISIS_LEVEL2 - 1]); rn; rn = route_next(rn))
      {
        if (rn->info == NULL)
          continue;

        route_total++;
      }
    }
  }

  if (listcount(isis->area_list) == 0)
  {
    memset(&isis_sta, 0, sizeof(struct isis_statistics));
  }

  vty_out(vty, "=========  isis route ===========%s", VTY_NEWLINE);
  vty_out(vty, "the route total:%d   %s", route_total, VTY_NEWLINE);

  vty_out(vty, "=========  spf duration =========%s", VTY_NEWLINE);
  vty_out(vty, "spf duration:%d %s", (u_int32_t)isis_sta.duration_time, VTY_NEWLINE);

  vty_out(vty, "=========  isis LSDB statistics =======%s", VTY_NEWLINE);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    for (level = 0; level < ISIS_LEVELS; level++)
    {
      if (area->lspdb[level] && dict_count(area->lspdb[level]) > 0)
      {
        lsp_count = lsp_statistics_all(vty, area->lspdb[level], ISIS_UI_LEVEL_BRIEF,
                                       area->dynhostname) +
                    lsp_count;
      }
    }
  }

  if (listcount(isis->area_list) == 0)
  {
    memset(&isis_sta, 0, sizeof(struct isis_statistics));
  }

  vty_out(vty, "the LSP of LSDB total:%d   %s", lsp_count, VTY_NEWLINE);

  vty_out(vty, "the send to route merge add total:%d %s", isis_sta.isis_route_add_merge, VTY_NEWLINE);
  vty_out(vty, "the send to route merge del total:%d %s", isis_sta.isis_route_del_merge, VTY_NEWLINE);

  return CMD_SUCCESS;
}

void isis_show_route(struct vty *vty, struct route_node *rn, struct isis_route_info *route_info, struct isis_area *area)
{
  struct isis_nexthop *nexthop;
  struct listnode *node;
  char buf[BUFSIZ];

  if (route_info == NULL)
    return;

//if(route_info->depth == 1)
//return;
#if 0
	if(CHECK_FLAG(route_info->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
	{
	    vty_out(vty,"I      ");
		/* Prefix information. */
		vty_out (vty, "%s",prefix2str (&rn->p, buf, sizeof(buf)));

		/* Distance and metric display. */
		vty_out (vty, " [%d/%d]", area->distance, route_info->cost);
		vty_out (vty,"  Directly connected");
		vty_out (vty, "%s", VTY_NEWLINE);
	}
	else
#endif
  {
    for (ALL_LIST_ELEMENTS_RO(route_info->nexthops, node, nexthop))
    {
      vty_out(vty, "       ");
      /* Prefix information. */
      vty_out(vty, "%s", prefix2str(&rn->p, buf, sizeof(buf)));

      /* Distance and metric display. */
      vty_out(vty, " [%d/%u]", area->distance4,
              route_info->cost);

      if (route_info->nexthops)
        vty_out(vty, " via %s", inet_ntoa(nexthop->ip));

      vty_out(vty, "%s", VTY_NEWLINE);
    }
  }
}

void show_l1_route(struct vty *vty, struct isis_area *area)
{
  struct route_node *rn;
  struct isis_route_info *route_info;

  vty_out(vty, "-----------------level-1 route table-----------------%s", VTY_NEWLINE);

  for (rn = route_top(area->route_table[ISIS_LEVEL1 - 1]); rn; rn = route_next(rn))
  {
    route_info = rn->info;

    isis_show_route(vty, rn, route_info, area);
  }

  vty_out(vty, "%s", VTY_NEWLINE);
}
void show_l2_route(struct vty *vty, struct isis_area *area)
{
  struct route_node *rn;
  struct isis_route_info *route_info;

  vty_out(vty, "-----------------level-2 route table-----------------%s", VTY_NEWLINE);

  for (rn = route_top(area->route_table[ISIS_LEVEL2 - 1]); rn; rn = route_next(rn))
  {
    route_info = rn->info;

    isis_show_route(vty, rn, route_info, area);
  }

  vty_out(vty, "%s", VTY_NEWLINE);
}

#if 0
void show_redist_route(struct vty *vty, struct isis_area *area, int level)
{
	struct route_table *er_table;
	struct route_node *rn;
	struct prefix_ipv4 *ipv4;
	struct isis_ext_info *info;
	char buf[BUFSIZ];

	er_table = get_ext_reach(area, AF_INET, level);

	if (!er_table)
	  return;

	vty_out(vty, "isis instance %s level-%d:%s", area->area_tag, level, VTY_NEWLINE);

	vty_out(vty, "Type      ");
	vty_out(vty, "Prefix      ");
	vty_out(vty, "Distance      ");
	vty_out(vty, "Cost      ");
	vty_out(vty, "%s", VTY_NEWLINE);

	for (rn = route_top(er_table); rn; rn = route_next(rn))
	{
		if (!rn->info)
		  continue;

		ipv4 = (struct prefix_ipv4*)&rn->p;
		info = rn->info;

		switch(info->origin)
	    {
		  case(ROUTE_PROTO_STATIC):
			  vty_out(vty, "static");
			  break;
		  case(ROUTE_PROTO_CONNECT):
			  vty_out(vty, "connect");
			  break;
		  case(ROUTE_PROTO_ARP):
			  vty_out(vty, "arp");
			  break;
		  case(ROUTE_PROTO_OSPF):
			  vty_out(vty, "ospf");
			  break;
		  case(ROUTE_PROTO_ISIS):
			  vty_out(vty, "isis");
			  break;
		  case(ROUTE_PROTO_RIP):
			  vty_out(vty, "rip");
			  break;
		  case(ROUTE_PROTO_IBGP):
			  vty_out(vty, "ibgp");
			  break;
		  case(ROUTE_PROTO_EBGP):
			  vty_out(vty, "ebgp");
			  break;
		  default:
		  	  break;
	    }
		vty_out (vty, "   %s       ", prefix2str (&rn->p, buf, sizeof(buf)));
		vty_out (vty, "%d         ", info->distance);
		vty_out (vty, "%d       ", info->metric);
		vty_out(vty, "%s", VTY_NEWLINE);
	}
}
#endif

DEFUN(show_isis_instance_route,
      show_isis_instance_route_cmd,
      "show isis instance <1-255> route",
      SHOW_STR
      "ISIS information\n"
      "ISIS instance\n"
      "Instance number\n"
      "ISIS routing table\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_warning_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  vty_out(vty, "isis instance%s:%s", area->area_tag, VTY_NEWLINE);

  if ((area->is_type & IS_LEVEL_1) &&
      (area->route_table[ISIS_LEVEL1 - 1]))
    show_l1_route(vty, area);

  if ((area->is_type & IS_LEVEL_2) &&
      (area->route_table[ISIS_LEVEL2 - 1]))
    show_l2_route(vty, area);

  return CMD_SUCCESS;
}

DEFUN(show_isis_instance_route_h3c,
      show_isis_instance_route_h3c_cmd,
      "display isis route <1-255>",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "Process ID for display\n"
      "ISIS route table\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_warning_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  vty_out(vty, "isis instance%s:%s", area->area_tag, VTY_NEWLINE);

  if ((area->is_type & IS_LEVEL_1) &&
      (area->route_table[ISIS_LEVEL1 - 1]))
    show_l1_route(vty, area);

  if ((area->is_type & IS_LEVEL_2) &&
      (area->route_table[ISIS_LEVEL2 - 1]))
    show_l2_route(vty, area);

  return CMD_SUCCESS;
}

DEFUN(show_isis_route,
      show_isis_route_cmd,
      "show isis route",
      SHOW_STR
      "ISIS information\n"
      "ISIS routing table\n")
{
  struct isis_area *area;
  struct listnode *node;

  if (isis->area_list == NULL)
  {
    vty_warning_out(vty, "no isis instance!");
    return CMD_SUCCESS;
  }

  //vty_out(vty, SHOW_ISIS_ROUTE_V4_HEADER);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "isis instance %s:%s", area->area_tag, VTY_NEWLINE);
    if ((area->is_type & IS_LEVEL_1) &&
        (area->route_table[ISIS_LEVEL1 - 1]))
      show_l1_route(vty, area);

    if ((area->is_type & IS_LEVEL_2) &&
        (area->route_table[ISIS_LEVEL2 - 1]))
      show_l2_route(vty, area);
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_route_h3c,
      show_isis_route_h3c_cmd,
      "display isis route",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "ISIS route table\n")
{
  struct isis_area *area;
  struct listnode *node;

  if (isis->area_list == NULL)
  {
    vty_warning_out(vty, "no isis instance!");
    return CMD_SUCCESS;
  }

  //vty_out(vty, SHOW_ISIS_ROUTE_V4_HEADER);

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "isis instance %s:%s", area->area_tag, VTY_NEWLINE);
    if ((area->is_type & IS_LEVEL_1) &&
        (area->route_table[ISIS_LEVEL1 - 1]))
      show_l1_route(vty, area);

    if ((area->is_type & IS_LEVEL_2) &&
        (area->route_table[ISIS_LEVEL2 - 1]))
      show_l2_route(vty, area);
  }
  return CMD_SUCCESS;
}

void isis_show_ipv6_route(struct vty *vty, struct route_node *rn, struct isis_route_info *route_info,
                          struct isis_area *area)
{
  struct isis_nexthop6 *nexthop6;
  struct listnode *node;
  u_char ip6[INET6_ADDRSTRLEN];
  struct prefix_ipv6 ipv6_temp;
  char buf[BUFSIZ];

  if (route_info == NULL)
    return;

  memset(&ipv6_temp, 0, sizeof(struct prefix_ipv6));

  zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "%s()%d", __FUNCTION__, __LINE__);
#if 0
	if(CHECK_FLAG(route_info->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
	{
	    vty_out(vty,"I      ");
		/* Prefix information. */
		vty_out (vty, "%s",prefix2str (&rn->p, buf, sizeof(buf)));

		/* Distance and metric display. */
		vty_out (vty, " [%d/%d]", area->distance, route_info->cost);
		vty_out (vty,"  Directly connected");
		vty_out (vty, "%s", VTY_NEWLINE);
	}
	else
#endif
  {
    for (ALL_LIST_ELEMENTS_RO(route_info->nexthops6, node, nexthop6))
    {
      /* Prefix information. */
      memcpy(&ipv6_temp, (struct prefix_ipv6 *)&rn->p, sizeof(struct prefix_ipv6));
      apply_mask_ipv6(&ipv6_temp);

      vty_out(vty, "%s", prefix2str(&ipv6_temp, buf, sizeof(buf)));

      /* Distance and metric display. */
      vty_out(vty, " [%d/%u]", area->distance6, route_info->cost);

      inet_ntop(AF_INET6, &nexthop6->ip6, (char *)ip6, INET6_ADDRSTRLEN);

      vty_out(vty, " via %s", ip6);

      vty_out(vty, "%s", VTY_NEWLINE);
    }
  }
}

void show_ipv6_route(struct vty *vty, struct isis_area *area, int level)
{
  struct route_node *rn;
  struct isis_route_info *route_info;

  vty_out(vty, "-----------------level-%d ipv6 route table-----------------%s", level, VTY_NEWLINE);

  for (rn = route_top(area->route_table6[level - 1]); rn; rn = route_next(rn))
  {
    route_info = rn->info;

    isis_show_ipv6_route(vty, rn, route_info, area);
  }

  vty_out(vty, "%s", VTY_NEWLINE);
}

DEFUN(show_isis_route_ipv6,
      show_isis_route_ipv6_cmd,
      "show isis ipv6 route",
      SHOW_STR
      "ISIS information\n"
      "ISIS routing table\n")
{
  struct isis_area *area;
  struct listnode *node;

  if (isis->area_list == NULL)
  {
    vty_out(vty, "no isis instance!");
    return CMD_SUCCESS;
  }

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "isis instance%s:%s", area->area_tag, VTY_NEWLINE);
    if ((area->is_type & IS_LEVEL_1) &&
        (area->route_table6[ISIS_LEVEL1 - 1]))
    {
      show_ipv6_route(vty, area, IS_LEVEL_1);
    }

    if ((area->is_type & IS_LEVEL_2) &&
        (area->route_table6[ISIS_LEVEL2 - 1]))
      show_ipv6_route(vty, area, ISIS_LEVEL2);
  }

  return CMD_SUCCESS;
}

DEFUN(show_isis_route_ipv6_h3c,
      show_isis_route_ipv6_h3c_cmd,
      "display isis ipv6 route",
      "Display current system information\n"
      "ISIS status and configuration information\n"
      "ISIS routing table\n")
{
  struct isis_area *area;
  struct listnode *node;

  if (isis->area_list == NULL)
  {
    vty_out(vty, "no isis instance!");
    return CMD_SUCCESS;
  }

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    vty_out(vty, "isis instance%s:%s", area->area_tag, VTY_NEWLINE);
    if ((area->is_type & IS_LEVEL_1) &&
        (area->route_table6[ISIS_LEVEL1 - 1]))
    {
      show_ipv6_route(vty, area, IS_LEVEL_1);
    }

    if ((area->is_type & IS_LEVEL_2) &&
        (area->route_table6[ISIS_LEVEL2 - 1]))
      show_ipv6_route(vty, area, ISIS_LEVEL2);
  }
  return CMD_SUCCESS;
}

DEFUN(show_isis_redist_route,
      show_isis_redist_route_cmd,
      "show isis instance <1-255> redist route",
      SHOW_STR
      "ISIS information\n"
      "ISIS instance\n"
      "ISO Routing area tag\n"
      "ISIS redist information\n"
      "ISIS routing table\n")
{
  struct isis_area *area;

  if (atoi(argv[0]) < 1)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-255>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-255>%s",
                    atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_warning_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  return CMD_SUCCESS;
}

int summary_ipv6_add(struct prefix_ipv6 *p, struct isis_area *area, int level)
{
  struct listnode *snode;
  struct ipv6_summary *sreach;
  struct list *list;

  if (level == 1)
    list = area->summary_ipv6_l1;
  else if (level == 2)
    list = area->summary_ipv6_l2;
  else
    return 0;

  if (listcount(list) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(list, snode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
        return 0;

      if (prefix_match((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
        return 0;

      if (prefix_match((struct prefix *)p, (struct prefix *)(&sreach->summary_p)))
      {
        sreach->summary_p = *p;
        return 1;
      }
    }
  }

  sreach = XMALLOC(MTYPE_ISIS_TLV, sizeof(struct ipv6_summary));
  memset(sreach, 0, sizeof(struct ipv6_summary));

  sreach->sflag = 0;
  sreach->metric = 0xFFFFFFFF;
  sreach->summary_p = *p;
  sreach->level = level;
  sreach->ext = 0;

  listnode_add(list, sreach);
  return 1;
}

int summary_add(struct prefix_ipv4 *p, struct isis_area *area, int level)
{
  struct listnode *snode;
  struct ip_summary *sreach;
  struct list *list;

  if (level == 1)
    list = area->summary_l1;
  else if (level == 2)
    list = area->summary_l2;
  else
    return 0;

  if (listcount(list) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(list, snode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
        return 0;

      if (prefix_match((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
        return 0;

      if (prefix_match((struct prefix *)p, (struct prefix *)(&sreach->summary_p)))
      {
        sreach->summary_p = *p;
        return 1;
      }
    }
  }

  sreach = XMALLOC(MTYPE_ISIS_TLV, sizeof(struct ip_summary));
  sreach->sflag = 0;
  sreach->metric = 0xFFFFFFFF;
  sreach->summary_p = *p;
  sreach->level = level;
  listnode_add(list, sreach);
  return 1;
}

void summary_ipv6_regenerate(struct isis_area *area, int level, struct prefix_ipv6 *p)
{
  struct listnode *snode, *lnode;
  struct ipv6_summary *sreach, *lreach;
  struct list *list;

  if (level == 1)
    list = area->summary_ipv6_l1;
  else if (level == 2)
    list = area->summary_ipv6_l2;
  else
    return;

  if (listcount(list) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(list, snode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
      {
        listnode_delete(list, sreach);
        for (ALL_LIST_ELEMENTS_RO(area->summary_ipv6, lnode, lreach))
        {
          if ((lreach->level & level) && prefix_match((struct prefix *)p, (struct prefix *)(&lreach->summary_p)))
            summary_ipv6_add(&lreach->summary_p, area, level);
        }
        summary_ipv6_lsp_list_regenerate(area, level, p);
        break;
      }
    }
  }
}

void summary_regenerate(struct isis_area *area, int level, struct prefix_ipv4 *p)
{
  struct listnode *snode, *lnode;
  struct ip_summary *sreach, *lreach;
  struct list *list;

  if (level == 1)
    list = area->summary_l1;
  else if (level == 2)
    list = area->summary_l2;
  else
    return;

  if (listcount(list) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(list, snode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
      {
        listnode_delete(list, sreach);
        for (ALL_LIST_ELEMENTS_RO(area->summary, lnode, lreach))
        {
          if ((lreach->level & level) && prefix_match((struct prefix *)p, (struct prefix *)(&lreach->summary_p)))
            summary_add(&lreach->summary_p, area, level);
        }
        summary_lsp_list_regenerate(area, level, p);
        break;
      }
    }
  }
}

int summary_ipv6_del(struct prefix_ipv6 *p, struct isis_area *area, int level)
{
  struct listnode *snode, *snnode;
  struct ipv6_summary *sreach;
  struct list *list;
  int flag = 0;

  if (level == 1)
    list = area->summary_ipv6_l1;
  else if (level == 2)
    list = area->summary_ipv6_l2;
  else
    return 0;

  if (listcount(list) > 0)
  {
    for (ALL_LIST_ELEMENTS(list, snode, snnode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
      {
        listnode_delete(list, sreach);
        flag = 1;
      }
    }
  }

  if (listcount(area->summary_ipv6))
  {
    for (ALL_LIST_ELEMENTS(area->summary_ipv6, snode, snnode, sreach))
    {
      if (prefix_match((struct prefix *)p, (struct prefix *)(&sreach->summary_p)) && (level & sreach->level))
      {
        summary_ipv6_add(&sreach->summary_p, area, level);
      }
    }
  }

  return flag;
}

int summary_del(struct prefix_ipv4 *p, struct isis_area *area, int level)
{
  //struct route_node *rnode;
  struct listnode *snode, *snnode;
  struct ip_summary *sreach;
  struct list *list;
  int flag = 0;

  if (level == 1)
    list = area->summary_l1;
  else if (level == 2)
    list = area->summary_l2;
  else
    return 0;

  if (listcount(list) > 0)
  {
    for (ALL_LIST_ELEMENTS(list, snode, snnode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
      {
        listnode_delete(list, sreach);
        flag = 1;
      }
    }
  }

  if (listcount(area->summary))
  {
    for (ALL_LIST_ELEMENTS(area->summary, snode, snnode, sreach))
    {
      if (prefix_match((struct prefix *)p, (struct prefix *)(&sreach->summary_p)) && (level & sreach->level))
      {
        summary_add(&sreach->summary_p, area, level);
      }
    }
  }

  return flag;
}

int summary_list_add(struct prefix_ipv4 *p, struct isis_area *area, int level)
{
  //struct route_node *rnode;
  struct listnode *snode;
  struct ip_summary *sreach;
  //struct list *list;
  int level_s;

  if (listcount(area->summary) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(area->summary, snode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
      {
        if (sreach->level == IS_LEVEL_1_AND_2)
        {
          if (sreach->level == level)
            return 0;
          sreach->level = level;
          summary_regenerate(area, IS_LEVEL_1_AND_2 - level, p);
          return 0;
        }
        else
        {
          if (sreach->level == level)
            return 0;
          if (level == IS_LEVEL_1_AND_2)
          {
            level_s = level - sreach->level;
            sreach->level = level;
            if (summary_add(p, area, level_s))
              summary_lsp_list_regenerate(area, level_s, p);
            return 0;
          }
          else
          {
            //sreach->level =  sreach->level | level;
            //sreach->level = level;
            level_s = sreach->level; //delete level
            sreach->level = level;   //add level

            summary_regenerate(area, level_s, p);
            if (summary_add(p, area, level))
              summary_lsp_list_regenerate(area, level, p);
            return 0;
          }
        }
      }
    }
  }

  sreach = XMALLOC(MTYPE_ISIS_TLV, sizeof(struct ip_summary));
  sreach->sflag = 0;
  sreach->summary_p = *p;
  sreach->level = level;
  sreach->metric = 0xFFFFFFFF;
  listnode_add(area->summary, sreach);

  if (level != IS_LEVEL_1_AND_2)
  {
    if (summary_add(p, area, level))
      summary_lsp_list_regenerate(area, level, p);
  }
  else
  {
    if (summary_add(p, area, IS_LEVEL_1))
      summary_lsp_list_regenerate(area, IS_LEVEL_1, p);
    if (summary_add(p, area, IS_LEVEL_2))
      summary_lsp_list_regenerate(area, IS_LEVEL_2, p);
  }
  return 1;
}

int summary_ipv6_list_add(struct prefix_ipv6 *p, struct isis_area *area, int level)
{
  struct listnode *snode;
  struct ipv6_summary *sreach;
  int level_s;

  if (listcount(area->summary_ipv6) > 0)
  {
    for (ALL_LIST_ELEMENTS_RO(area->summary_ipv6, snode, sreach))
    {
      if (!prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)p))
      {
        if (sreach->level == IS_LEVEL_1_AND_2)
        {
          if (sreach->level == level)
            return 0;
          sreach->level = level;
          summary_ipv6_regenerate(area, IS_LEVEL_1_AND_2 - level, p);
          return 0;
        }
        else
        {
          if (sreach->level == level)
            return 0;
          if (level == IS_LEVEL_1_AND_2)
          {
            level_s = level - sreach->level;
            sreach->level = level;
            if (summary_ipv6_add(p, area, level_s))
              summary_ipv6_lsp_list_regenerate(area, level_s, p);
            return 0;
          }
          else
          {
            level_s = sreach->level; //delete level
            sreach->level = level;   //add level

            summary_ipv6_regenerate(area, level_s, p);
            if (summary_ipv6_add(p, area, level))
              summary_ipv6_lsp_list_regenerate(area, level, p);
            return 0;
          }
        }
      }
    }
  }

  sreach = XMALLOC(MTYPE_ISIS_TLV, sizeof(struct ipv6_summary));
  memset(sreach, 0, sizeof(struct ipv6_summary));

  sreach->sflag = 0;
  sreach->summary_p = *p;
  sreach->level = level;
  sreach->ext = 0;
  sreach->metric = 0xFFFFFFFF;
  listnode_add(area->summary_ipv6, sreach);

  if (level != IS_LEVEL_1_AND_2)
  {
    if (summary_ipv6_add(p, area, level))
      summary_ipv6_lsp_list_regenerate(area, level, p);
  }
  else
  {
    if (summary_ipv6_add(p, area, IS_LEVEL_1))
      summary_ipv6_lsp_list_regenerate(area, IS_LEVEL_1, p);
    if (summary_ipv6_add(p, area, IS_LEVEL_2))
      summary_ipv6_lsp_list_regenerate(area, IS_LEVEL_2, p);
  }
  return 1;
}

int summary_extend_check(struct isis_area *area, int level, struct prefix_ipv4 *p)
{
  struct route_table *table = NULL;
  struct route_node *rnode;
  struct prefix *ipv4;
  int rflag = 0;

  /*    two route_table should be both summary 
	 (the redist_route table and the interface_route table )*/
  table = get_ext_reach(area, AF_INET, level);

  if (table)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;
      ipv4 = (struct prefix *)&rnode->p;
      if (prefix_match((struct prefix *)p, ipv4))
        rflag++;
    }
  }

  table = area->route_table[level - 1];
  if (table)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;
      ipv4 = (struct prefix *)&rnode->p;
      if (prefix_match((struct prefix *)p, ipv4))
        rflag++;
    }
  }
  return rflag;
}

int summary_ipv6_extend_check(struct isis_area *area, int level, struct prefix_ipv6 *p)
{
  struct route_table *table = NULL;
  struct route_node *rnode;
  struct prefix *ipv6;
  int rflag = 0;

  /*    two route_table should be both summary 
	 (the redist_route table and the interface_route table )*/
  table = get_ext_reach(area, AF_INET6, level);

  if (table)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;
      ipv6 = (struct prefix *)&rnode->p;
      if (prefix_match((struct prefix *)p, ipv6))
        rflag++;
    }
  }

  table = area->route_table[level - 1];
  if (table)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;
      ipv6 = (struct prefix *)&rnode->p;
      if (prefix_match((struct prefix *)p, ipv6))
        rflag++;
    }
  }
  return rflag;
}

int summary_route_table_check(struct isis_area *area, int level, struct prefix_ipv4 *p)
{
  struct route_table *table = NULL;
  struct route_node *rnode;
  struct prefix *ipv4;
  int rflag = 0;

  table = area->route_table[level - 1];

  if (table)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;

      ipv4 = (struct prefix *)&rnode->p;
      if (prefix_match((struct prefix *)p, ipv4))
        rflag++;
    }
  }
  return rflag++;
}

int summary_ipv6_route_table_check(struct isis_area *area, int level, struct prefix_ipv6 *p)
{
  struct route_table *table = NULL;
  struct route_node *rnode;
  struct prefix *ipv6;
  int rflag = 0;

  table = area->route_table6[level - 1];

  if (table)
  {
    for (rnode = route_top(table); rnode; rnode = route_next(rnode))
    {
      if (rnode->info == NULL)
        continue;

      ipv6 = (struct prefix *)&rnode->p;
      if (prefix_match((struct prefix *)p, ipv6))
        rflag++;
    }
  }
  return rflag++;
}

void summary_lsp_list_regenerate(struct isis_area *area, int level, struct prefix_ipv4 *p)
{
  int rflag = 0;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    if (area->is_type == IS_LEVEL_1 && level == 2)
      return;
    if (area->is_type == IS_LEVEL_2 && level == 1)
      return;

    rflag += summary_extend_check(area, level, p);

    if (rflag > 0)
      lsp_regenerate_schedule(area, level, 0);
  }

  if (area->is_type == IS_LEVEL_1_AND_2)
  {
    if (level == IS_LEVEL_1)
    {
      if (area->area_import_to_l1)
      {
        rflag += summary_route_table_check(area, IS_LEVEL_2, p);
        rflag += summary_extend_check(area, IS_LEVEL_1, p);
      }
      else
      {
        rflag += summary_route_table_check(area, IS_LEVEL_1, p);
        rflag += summary_extend_check(area, IS_LEVEL_1, p);
      }

      if (rflag > 0)
      {
        lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);
      }
    }
    if (level == IS_LEVEL_2)
    {
      if (area->area_import_to_l2)
      {
        rflag += summary_route_table_check(area, IS_LEVEL_1, p);
        rflag += summary_extend_check(area, IS_LEVEL_2, p);
      }
      else
      {
        rflag += summary_route_table_check(area, IS_LEVEL_2, p);
        rflag += summary_extend_check(area, IS_LEVEL_2, p);
      }

      if (rflag > 0)
      {
        lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);
      }
    }
  }
}

void summary_ipv6_lsp_list_regenerate(struct isis_area *area, int level, struct prefix_ipv6 *p)
{
  int rflag = 0;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    if (area->is_type == IS_LEVEL_1 && level == 2)
      return;
    if (area->is_type == IS_LEVEL_2 && level == 1)
      return;

    rflag += summary_ipv6_extend_check(area, level, p);

    if (rflag > 0)
      lsp_regenerate_schedule(area, level, 0);
  }

  if (area->is_type == IS_LEVEL_1_AND_2)
  {
    if (level == IS_LEVEL_1)
    {
      if (area->area_import_to_l1_v6)
      {
        rflag += summary_ipv6_route_table_check(area, IS_LEVEL_2, p);
        rflag += summary_ipv6_extend_check(area, IS_LEVEL_1, p);
      }
      else
      {
        rflag += summary_ipv6_route_table_check(area, IS_LEVEL_1, p);
        rflag += summary_ipv6_extend_check(area, IS_LEVEL_1, p);
      }

      if (rflag > 0)
      {
        lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);
      }
    }
    if (level == IS_LEVEL_2)
    {
      if (area->area_import_to_l2_v6)
      {
        rflag += summary_ipv6_route_table_check(area, IS_LEVEL_1, p);
        rflag += summary_ipv6_extend_check(area, IS_LEVEL_2, p);
      }
      else
      {
        rflag += summary_ipv6_route_table_check(area, IS_LEVEL_2, p);
        rflag += summary_ipv6_extend_check(area, IS_LEVEL_2, p);
      }

      if (rflag > 0)
      {
        lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 0);
      }
    }
  }
}

void isis_password_encode(char *dpw, char *spw)
{
  unsigned int i;

  for (i = 0; i < strlen(spw); i++)
    *(dpw + i) = *(spw + i) + 10;
}

void isis_password_decode(char *dpw, char *spw)
{
  unsigned int i;

  for (i = 0; i < strlen(spw); i++)
    *(dpw + i) = *(spw + i) - 10;
}

static int
ip_address_and_mask_to_prefix(struct vty *vty, struct prefix_ipv4 *target_p, const char *ip_addr, const char *netmask)
{
  int ret = 0;
  u_char mask_len = 0;
  struct in_addr temp_mask;

  //get ip
  VTY_CHECK_ISIS_ADDRESS("ip address", ip_addr);                 /*check A.B.C.D*/
  VTY_GET_IPV4_ADDRESS("ip address", target_p->prefix, ip_addr); /* Change format */

  //get mask
  if (strlen(netmask) > 2) //if A.B.C.D format
  {
    ret = inet_aton(netmask, &temp_mask);
    if (!ret)
    {
      vty_error_out(vty, "Please specify valid netmask as a.b.c.d or 0~32%s",
                    VTY_NEWLINE);
      return CMD_WARNING;
    }
    mask_len = ip_masklen(temp_mask);
  }
  else //0~32 format
  {
    VTY_GET_INTEGER_RANGE("ip netmask", mask_len, netmask, 0, IPV4_MAX_PREFIXLEN);
  }

  if (mask_len > IPV4_MAX_PREFIXLEN)
  {
    vty_error_out(vty, "Please specify valid netmask length.%s",
                  VTY_NEWLINE);
    return CMD_WARNING;
  }
  target_p->prefixlen = mask_len;
  target_p->family = AF_INET;

  return CMD_SUCCESS;
}

static int
ipv6_address_and_mask_to_prefix(struct vty *vty, struct prefix_ipv6 *p, const char *ipv6_addr, const char *netmask)
{
  u_char plen = 0;

  if (0 == inet_pton(AF_INET6, ipv6_addr, &p->prefix))
    return -1;

  plen = (u_char)atoi(netmask);

  if (plen > IPV6_MAX_BITLEN)
    return -1;

  p->prefixlen = plen;
  p->family = AF_INET6;

  return 0;
}

DEFUN(router_summary,
      router_summary_cmd,
      "route summary A.B.C.D/M (level-1|level-2|level-1-2) ",
      "route information\n"
      "Summary Information\n"
      "IP Infomation\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv4 p;
  int level;

  VTY_GET_IPV4_PREFIX("route summary", p, argv[0]);

  if (!strcmp("level-1", argv[1]))
    level = 1;
  else if (!strcmp("level-2", argv[1]))
    level = 2;
  else if (!strcmp("level-1-2", argv[1]))
    level = 3;
  else
    return CMD_SUCCESS;
  summary_list_add(&p, area, level);
  return CMD_SUCCESS;
}

DEFUN(router_summary_ipv6,
      router_summary_ipv6_cmd,
      "ipv6 summary X:X::X:X/M (level-1|level-2|level-1-2) ",
      "Configure IPv6 commands for ISIS\n"
      "Configure summary address for IPv6\n"
      "IPv6 address\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv6 p;
  int level;

  ISIS_GET_IPV6_PREFIX("ipv6 route summary", p, argv[0]);

  if (!strcmp("level-1", argv[1]))
    level = 1;
  else if (!strcmp("level-2", argv[1]))
    level = 2;
  else if (!strcmp("level-1-2", argv[1]))
    level = 3;
  else
    return CMD_SUCCESS;
  summary_ipv6_list_add(&p, area, level);
  return CMD_SUCCESS;
}

DEFUN(router_summary_ipv6_h3c,
      router_summary_ipv6_h3c_cmd,
      "summary X:X::X:X <0-128> (level-1|level-2|level-1-2) ",
      "Configure summary address for IPv6\n"
      "IPv6 address\n"
      "IPv6 address mask length\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv6 p;
  int level;

  if (0 != ipv6_address_and_mask_to_prefix(vty, &p, argv[0], argv[1]))
  {
    vty_out(vty, "%% Invalid value%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (!strcmp("level-1", argv[2]))
    level = 1;
  else if (!strcmp("level-2", argv[2]))
    level = 2;
  else if (!strcmp("level-1-2", argv[2]))
    level = 3;
  else
    return CMD_SUCCESS;

  summary_ipv6_list_add(&p, area, level);
  return CMD_SUCCESS;
}

DEFUN(router_summary_h3c,
      router_summary_h3c_cmd,
      "summary A.B.C.D (A.B.C.D|<0-32>) (level-1|level-2|level-1-2)",
      "Configure summary address\n"
      "Summary address\n"
      "IP address mask\n"
      "Summary address mask length\n"
      "level-1\n"
      "level-2\n"
      "level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv4 p;
  int level;

  if (ip_address_and_mask_to_prefix(vty, &p, argv[0], argv[1]))
    return CMD_WARNING;

  if (!strcmp("level-1", argv[2]))
    level = 1;
  else if (!strcmp("level-2", argv[2]))
    level = 2;
  else if (!strcmp("level-1-2", argv[2]))
    level = 3;
  else
    return CMD_SUCCESS;

  summary_list_add(&p, area, level);
  return CMD_SUCCESS;
}

DEFUN(no_router_summary_ipv6,
      no_router_summary_ipv6_cmd,
      "undo ipv6 summary X:X::X:X/M (level-1|level-2|level-1-2)",
      "Cancel current setting\n"
      "Configure IPv6 commands for ISIS\n"
      "Configure summary address for IPv6\n"
      "IPv6 address\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv6 p;
  struct ipv6_summary *sreach;
  struct listnode *snode, *snnode;
  int level;
  int flag = 0;

  ISIS_GET_IPV6_PREFIX("ipv6 route summary", p, argv[0]);

  if (!strcmp("level-1", argv[1]))
    level = 1;
  else if (!strcmp("level-2", argv[1]))
    level = 2;
  else if (!strcmp("level-1-2", argv[1]))
    level = 3;
  else
    return CMD_SUCCESS;

  if (listcount(area->summary_ipv6) > 0)
  {
    for (ALL_LIST_ELEMENTS(area->summary_ipv6, snode, snnode, sreach))
    {
      if (prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)&p))
        continue;

      if (sreach->level != level)
      {
        vty_warning_out(vty, "Level dismatch!%s", VTY_NEWLINE);
        return 0;
      }

      listnode_delete(area->summary_ipv6, sreach);
      flag = 1;
    }
  }

  if (flag)
  {
    if (level == 3)
    {
      if (summary_ipv6_del(&p, area, IS_LEVEL_1))
      {
        summary_ipv6_lsp_list_regenerate(area, IS_LEVEL_1, &p);
      }

      if (summary_ipv6_del(&p, area, IS_LEVEL_2))
      {
        summary_ipv6_lsp_list_regenerate(area, IS_LEVEL_2, &p);
      }
    }
    else
    {
      if (summary_ipv6_del(&p, area, level))
      {
        summary_ipv6_lsp_list_regenerate(area, level, &p);
      }
    }
  }
  else
  {
    vty_warning_out(vty, "address dismatch!%s", VTY_NEWLINE);
  }
  return CMD_SUCCESS;
}

DEFUN(no_router_summary_ipv6_h3c,
      no_router_summary_ipv6_h3c_cmd,
      "undo summary X:X::X:X <0-128> (level-1|level-2|level-1-2)",
      "Cancel current setting\n"
      "Configure summary address for IPv6\n"
      "IPv6 address\n"
      "IPv6 address mask length\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv6 p;
  struct ipv6_summary *sreach;
  struct listnode *snode, *snnode;
  int level;
  int flag = 0;

  if (0 != ipv6_address_and_mask_to_prefix(vty, &p, argv[0], argv[1]))
  {
    vty_out(vty, "%% Invalid value%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (!strcmp("level-1", argv[2]))
    level = 1;
  else if (!strcmp("level-2", argv[2]))
    level = 2;
  else if (!strcmp("level-1-2", argv[2]))
    level = 3;
  else
    return CMD_SUCCESS;

  if (listcount(area->summary_ipv6) > 0)
  {
    for (ALL_LIST_ELEMENTS(area->summary_ipv6, snode, snnode, sreach))
    {
      if (prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)&p))
        continue;

      if (sreach->level != level)
      {
        vty_warning_out(vty, "Level dismatch!%s", VTY_NEWLINE);
        return 0;
      }

      listnode_delete(area->summary_ipv6, sreach);
      flag = 1;
    }
  }

  if (flag)
  {
    if (level == 3)
    {
      if (summary_ipv6_del(&p, area, IS_LEVEL_1))
      {
        summary_ipv6_lsp_list_regenerate(area, IS_LEVEL_1, &p);
      }

      if (summary_ipv6_del(&p, area, IS_LEVEL_2))
      {
        summary_ipv6_lsp_list_regenerate(area, IS_LEVEL_2, &p);
      }
    }
    else
    {
      if (summary_ipv6_del(&p, area, level))
      {
        summary_ipv6_lsp_list_regenerate(area, level, &p);
      }
    }
  }
  else
  {
    vty_warning_out(vty, "address dismatch!%s", VTY_NEWLINE);
  }
  return CMD_SUCCESS;
}

DEFUN(no_router_summary,
      no_router_summary_cmd,
      "no route summary A.B.C.D/M (level-1|level-2|level-1-2)",
      NO_STR
      "route information\n"
      "Summary Information\n"
      "IP Infomation\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv4 p;
  struct ip_summary *sreach;
  struct listnode *snode, *snnode;
  int level;
  int flag = 0;

  VTY_GET_IPV4_PREFIX("route summary", p, argv[0]);

  if (!strcmp("level-1", argv[1]))
    level = 1;
  else if (!strcmp("level-2", argv[1]))
    level = 2;
  else if (!strcmp("level-1-2", argv[1]))
    level = 3;
  else
    return CMD_SUCCESS;

  if (listcount(area->summary) > 0)
  {
    for (ALL_LIST_ELEMENTS(area->summary, snode, snnode, sreach))
    {
      if (prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)&p))
        continue;

      if (sreach->level != level)
      {
        vty_warning_out(vty, "Level dismatch!%s", VTY_NEWLINE);
        return 0;
      }

      listnode_delete(area->summary, sreach);
      flag = 1;
    }
  }

  if (flag)
  {
    if (level == 3)
    {
      if (summary_del(&p, area, IS_LEVEL_1))
      {
        summary_lsp_list_regenerate(area, IS_LEVEL_1, &p);
      }

      if (summary_del(&p, area, IS_LEVEL_2))
      {
        summary_lsp_list_regenerate(area, IS_LEVEL_2, &p);
      }
    }
    else
    {
      if (summary_del(&p, area, level))
      {
        summary_lsp_list_regenerate(area, level, &p);
      }
    }
  }
  return CMD_SUCCESS;
}

DEFUN(no_router_summary_h3c,
      no_router_summary_h3c_cmd,
      "undo summary A.B.C.D (A.B.C.D|<0-32>) (level-1|level-2|level-1-2)",
      "Cancel current setting\n"
      "Configure summary address\n"
      "Summary address\n"
      "Summary address mask length\n"
      "IP address mask\n"
      "level-1\n"
      "level-2\n"
      "level-1-2\n")
{
  struct isis_area *area = vty->index;
  struct prefix_ipv4 p;
  struct ip_summary *sreach;
  struct listnode *snode, *snnode;
  int level;
  int flag = 0;

  if (ip_address_and_mask_to_prefix(vty, &p, argv[0], argv[1]))
    return CMD_WARNING;

  if (!strcmp("level-1", argv[2]))
    level = 1;
  else if (!strcmp("level-2", argv[2]))
    level = 2;
  else if (!strcmp("level-1-2", argv[2]))
    level = 3;
  else
    return CMD_SUCCESS;

  if (listcount(area->summary) > 0)
  {
    for (ALL_LIST_ELEMENTS(area->summary, snode, snnode, sreach))
    {
      if (prefix_cmp((struct prefix *)(&sreach->summary_p), (struct prefix *)&p))
        continue;

      if (sreach->level != level)
      {
        vty_warning_out(vty, "Level dismatch!%s", VTY_NEWLINE);
        return 0;
      }

      listnode_delete(area->summary, sreach);
      flag = 1;
    }
  }

  if (flag)
  {
    if (level == 3)
    {
      if (summary_del(&p, area, IS_LEVEL_1))
      {
        summary_lsp_list_regenerate(area, IS_LEVEL_1, &p);
      }

      if (summary_del(&p, area, IS_LEVEL_2))
      {
        summary_lsp_list_regenerate(area, IS_LEVEL_2, &p);
      }
    }
    else
    {
      if (summary_del(&p, area, level))
      {
        summary_lsp_list_regenerate(area, level, &p);
      }
    }
  }
  return CMD_SUCCESS;
}

DEFUN(router_import_into_level2,
      router_import_into_level2_cmd,
      "route-import level-1 into level-2",
      "import routes from other area\n"
      "ISIS level-1 (source)\n"
      "import into\n"
      "ISIS level-2 (destination)\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l2 == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l2 = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(router_import_into_level2_h3c,
      router_import_into_level2_h3c_cmd,
      "import-route isis level-1 into level-2",
      "Import routes from other protocols into ISIS\n"
      "ISIS level-2 (source)\n"
      "Import into level-2\n"
      "ISIS level-1 (destination)\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l2 == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l2 = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(router_import_v6_into_level2_h3c,
      router_import_v6_into_level2_h3c_cmd,
      "import-route isisv6 level-1 into level-2",
      "Import routes from other protocols into ISIS\n"
      "ISIS level-2 (source)\n"
      "Import into level-2\n"
      "ISIS level-1 (destination)\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l2_v6 == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l2_v6 = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(router_import_into_level1,
      router_import_into_level1_cmd,
      "route-import level-2 into level-1",
      "import routes from other area\n"
      "ISIS level-2 (source)\n"
      "import into\n"
      "ISIS level-1 (destination)\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l1 == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l1 = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(router_import_into_level1_h3c,
      router_import_into_level1_h3c_cmd,
      "import-route isis level-2 into level-1",
      "Import routes from other protocols into ISIS\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Level-2\n"
      "Import into level-1\n"
      "Level-1\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l1 == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l1 = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(router_import_v6_into_level1_h3c,
      router_import_v6_into_level1_h3c_cmd,
      "import-route isisv6 level-2 into level-1",
      "Import routes from other protocols into ISIS\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Level-2\n"
      "Import into level-1\n"
      "Level-1\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l1_v6 == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l1_v6 = 1;
    lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(no_router_import_into_level2,
      no_router_import_into_level2_cmd,
      "no route-import level-1 into level-2",
      NO_STR
      "import routes from other area\n"
      "ISIS level-1 (source)\n"
      "import into\n"
      "ISIS level-2 (destination)\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l2 == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l2 = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(no_router_import_into_level2_h3c,
      no_router_import_into_level2_h3c_cmd,
      "undo import-route isis level-1 into level-2",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      "ISIS level-2 (source)\n"
      "Import into level-2\n"
      "ISIS level-1 (destination)\n")

{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l2 == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l2 = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(no_router_import_v6_into_level2_h3c,
      no_router_import_v6_into_level2_h3c_cmd,
      "undo import-route isisv6 level-1 into level-2",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      "ISIS level-2 (source)\n"
      "Import into level-2\n"
      "ISIS level-1 (destination)\n")

{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l2_v6 == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l2_v6 = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(no_router_import_into_level1,
      no_router_import_into_level1_cmd,
      "no route-import level-2 into level-1",
      NO_STR
      "import routes from other area\n"
      "ISIS level-2 (source)\n"
      "import into\n"
      "ISIS level-1 (destination)\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l1 == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l1 = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(no_router_import_into_level1_h3c,
      no_router_import_into_level1_h3c_cmd,
      "undo import-route isis level-2 into level-1",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Level-2\n"
      "Import into level-1\n"
      "Level-1\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l1 == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l1 = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(no_router_import_v6_into_level1_h3c,
      no_router_import_v6_into_level1_h3c_cmd,
      "undo import-route isisv6 level-2 into level-1",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Level-2\n"
      "Import into level-1\n"
      "Level-1\n")
{
  struct isis_area *area = vty->index;

  if (area->is_type != IS_LEVEL_1_AND_2)
  {
    vty_warning_out(vty, "Should be configured in level-1-2!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (area->area_import_to_l1_v6 == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->area_import_to_l1_v6 = 0;
    lsp_regenerate_schedule(area, IS_LEVEL_1, 0);
    return CMD_SUCCESS;
  }
}

DEFUN(graceful_restart,
      graceful_restart_cmd,
      "graceful-restart",
      "Graceful restart enable\n")
{
  struct isis_area *area = vty->index;

  if (area->gr == 1)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->gr = 1;
    return CMD_SUCCESS;
  }
}

DEFUN(no_graceful_restart,
      no_graceful_restart_cmd,
      "no graceful-restart",
      NO_STR
      "Graceful restart enable")
{
  struct isis_area *area = vty->index;

  if (area->gr == 0)
  {
    return CMD_SUCCESS;
  }
  else
  {
    area->gr = 0;
    return CMD_SUCCESS;
  }
}

DEFUN(area_passwd_md5,
      area_passwd_md5_cmd,
      "area-password md5 WORD",
      "Configure the authentication password for an area\n"
      "Authentication type\n"
      "Area password length 1-254\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;
  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (argc > 1)
  {
    SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_SEND);
    if (strncmp(argv[1], "v", 1) == 0)
      SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);
    else
      UNSET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);
  }
  else
  {
    UNSET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_SEND);
    UNSET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);
  }

  /*there is password already,now  change the password */
  if (area->area_passwd.type != ISIS_PASSWD_TYPE_HMAC_MD5 ||
      area->area_passwd.len != len ||
      memcmp(area->area_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL1 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL1 - 1]);
      while (dnode)
      {
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "%-20s[%d] area-auth md5 print", __FUNCTION__, __LINE__);
        next = dict_next(area->lspdb[ISIS_LEVEL1 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL1 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->area_passwd.len = (u_char)len;
  area->area_passwd.type = ISIS_PASSWD_TYPE_HMAC_MD5;

  if (vty->config_read_flag != 1)
  {
    memset(area->area_passwd.passwd, 0, 255);
    memset(area->area_passwd.en_passwd, 0, 255);
    strncpy((char *)area->area_passwd.passwd, argv[0], len);
    isis_password_encode((char *)area->area_passwd.en_passwd, (char *)area->area_passwd.passwd);
  }
  else
  {
    memset(area->area_passwd.passwd, 0, 255);
    memset(area->area_passwd.en_passwd, 0, 255);
    strncpy((char *)area->area_passwd.en_passwd, argv[0], len);
    isis_password_decode((char *)area->area_passwd.passwd, (char *)area->area_passwd.en_passwd);
  }

  /*if the area type is no level-1 or level 1-2 , it's no necessary to schedule */
  if (area->is_type & ISIS_LEVEL1)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);
    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      /* sending l1 csnp when the circuit is level-1 or level 1-2 */
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL1)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL1Csnp", circuit->t_send_csnp[0], send_l1_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[0], CSNP_JITTER));
      }
    }
  }
  return CMD_SUCCESS;
}

DEFUN(area_passwd_md5_h3c,
      area_passwd_md5_h3c_cmd,
      "area-authentication-mode md5 WORD",
      "Set the type and password of routing area authentication\n"
      "MD5 authentication type\n"
      "Area password length 1-254\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;
  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_SEND);
  SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);

  /*there is password already,now  change the password */
  if (area->area_passwd.type != ISIS_PASSWD_TYPE_HMAC_MD5 ||
      area->area_passwd.len != len ||
      memcmp(area->area_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL1 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL1 - 1]);
      while (dnode)
      {
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "%-20s[%d] area-auth md5 print", __FUNCTION__, __LINE__);
        next = dict_next(area->lspdb[ISIS_LEVEL1 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL1 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->area_passwd.len = (u_char)len;
  area->area_passwd.type = ISIS_PASSWD_TYPE_HMAC_MD5;

  if (vty->config_read_flag != 1)
  {
    memset(area->area_passwd.passwd, 0, 255);
    memset(area->area_passwd.en_passwd, 0, 255);
    strncpy((char *)area->area_passwd.passwd, argv[0], len);
    isis_password_encode((char *)area->area_passwd.en_passwd, (char *)area->area_passwd.passwd);
  }
  else
  {
    memset(area->area_passwd.passwd, 0, 255);
    memset(area->area_passwd.en_passwd, 0, 255);
    strncpy((char *)area->area_passwd.en_passwd, argv[0], len);
    isis_password_decode((char *)area->area_passwd.passwd, (char *)area->area_passwd.en_passwd);
  }

  /*if the area type is no level-1 or level 1-2 , it's no necessary to schedule */
  if (area->is_type & ISIS_LEVEL1)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);
    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      /* sendind l1 csnp when the circuit is level-1 or level 1-2 */
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL1)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL1Csnp", circuit->t_send_csnp[0], send_l1_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[0], CSNP_JITTER));
      }
    }
  }

  return CMD_SUCCESS;
}

ALIAS(area_passwd_md5,
      area_passwd_md5_snpauth_cmd,
      "area-password md5 WORD authenticate snp (send-only|validate)",
      "Configure the authentication password for an area\n"
      "Authentication type\n"
      "Area password length 1-254\n"
      "Authentication\n"
      "SNP PDUs\n"
      "Send but do not check PDUs on receiving\n"
      "Send and check PDUs on receiving\n")

DEFUN(area_passwd_clear,
      area_passwd_clear_cmd,
      "area-password clear WORD",
      "Configure the authentication password for an area\n"
      "Authentication type\n"
      "Area password length 1-254\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;

  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (argc > 1)
  {
    SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_SEND);
    if (strncmp(argv[1], "v", 1) == 0)
      SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);
    else
      UNSET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);
  }
  else
  {
    UNSET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_SEND);
    UNSET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);
  }

  if (area->area_passwd.type != ISIS_PASSWD_TYPE_CLEARTXT ||
      area->area_passwd.len != len ||
      memcmp(area->area_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL1 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL1 - 1]);
      while (dnode)
      {
        next = dict_next(area->lspdb[ISIS_LEVEL1 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL1 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->area_passwd.len = (u_char)len;
  area->area_passwd.type = ISIS_PASSWD_TYPE_CLEARTXT;
  memset(area->area_passwd.passwd, 0, 255);
  strncpy((char *)area->area_passwd.passwd, argv[0], len);

  /*if the area type is no level-1 or level 1-2 , it's no necessary to schedule */
  if (area->is_type & ISIS_LEVEL1)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);
    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      /* sendind l1 csnp when the circuit is level-1 or level 1-2 */
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL1)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL1Csnp", circuit->t_send_csnp[0], send_l1_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[0], CSNP_JITTER));
      }
    }
  }

  return CMD_SUCCESS;
}

DEFUN(area_passwd_clear_h3c,
      area_passwd_clear_h3c_cmd,
      "area-authentication-mode simple WORD",
      "Set the type and password of routing area authentication\n"
      "Plaintext authentication type\n"
      "Area password length 1-254\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;

  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_SEND);
  SET_FLAG(area->area_passwd.snp_auth, SNP_AUTH_RECV);

  if (area->area_passwd.type != ISIS_PASSWD_TYPE_CLEARTXT ||
      area->area_passwd.len != len ||
      memcmp(area->area_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL1 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL1 - 1]);
      while (dnode)
      {
        next = dict_next(area->lspdb[ISIS_LEVEL1 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL1 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->area_passwd.len = (u_char)len;
  area->area_passwd.type = ISIS_PASSWD_TYPE_CLEARTXT;
  memset(area->area_passwd.passwd, 0, 255);
  strncpy((char *)area->area_passwd.passwd, argv[0], len);

  /*if the area type is no level-1 or level 1-2 , it's no necessary to schedule */
  if (area->is_type & ISIS_LEVEL1)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);
    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      /* sendind l1 csnp when the circuit is level-1 or level 1-2 */
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL1)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL1Csnp", circuit->t_send_csnp[0], send_l1_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[0], CSNP_JITTER));
      }
    }
  }

  return CMD_SUCCESS;
}

ALIAS(area_passwd_clear,
      area_passwd_clear_snpauth_cmd,
      "area-password clear WORD authenticate snp (send-only|validate)",
      "Configure the authentication password for an area\n"
      "Authentication type\n"
      "Area password length 1-254\n"
      "Authentication\n"
      "SNP PDUs\n"
      "Send but do not check PDUs on receiving\n"
      "Send and check PDUs on receiving\n")

DEFUN(no_area_passwd,
      no_area_passwd_cmd,
      "no area-password",
      NO_STR
      "Configure the authentication password for an area\n")
{
  struct isis_area *area;

  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  if (area->area_passwd.len == 0)
  {
    vty_warning_out(vty, "No area-password in this instance%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(&area->area_passwd, 0, sizeof(struct isis_passwd));

  lsp_regenerate_schedule(area, area->is_type, 1);

  return CMD_SUCCESS;
}

DEFUN(no_area_passwd_h3c,
      no_area_passwd_h3c_cmd,
      "undo area-authentication-mode",
      "Cancel current configuration\n"
      "Set the type and password of routing area authentication\n")
{
  struct isis_area *area;

  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  if (area->area_passwd.len == 0)
  {
    vty_warning_out(vty, "No area-password in this instance%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(&area->area_passwd, 0, sizeof(struct isis_passwd));
  lsp_regenerate_schedule(area, area->is_type, 1);

  return CMD_SUCCESS;
}

DEFUN(domain_passwd_md5,
      domain_passwd_md5_cmd,
      "domain-password md5 WORD",
      "Set the authentication password for a routing domain\n"
      "Authentication type\n"
      "Routing domain password length 1-254\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;

  area = vty->index;
  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (argc > 1)
  {
    SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_SEND);
    if (strncmp(argv[1], "v", 1) == 0)
      SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);
    else
      UNSET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);
  }
  else
  {
    UNSET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_SEND);
    UNSET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);
  }

  /*there is password already,now  change the password */
  if (area->domain_passwd.type != ISIS_PASSWD_TYPE_HMAC_MD5 ||
      area->domain_passwd.len != len ||
      memcmp(area->domain_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL2 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL2 - 1]);
      while (dnode)
      {
        next = dict_next(area->lspdb[ISIS_LEVEL2 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL2 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->domain_passwd.len = (u_char)len;
  area->domain_passwd.type = ISIS_PASSWD_TYPE_HMAC_MD5;

  if (vty->config_read_flag != 1)
  {
    memset(area->domain_passwd.passwd, 0, 255);
    memset(area->domain_passwd.en_passwd, 0, 255);
    strncpy((char *)area->domain_passwd.passwd, argv[0], len);
    isis_password_encode((char *)area->domain_passwd.en_passwd, (char *)area->domain_passwd.passwd);
  }

  if (vty->config_read_flag == 1)
  {
    memset(area->domain_passwd.passwd, 0, 255);
    memset(area->domain_passwd.en_passwd, 0, 255);
    strncpy((char *)area->domain_passwd.en_passwd, argv[0], len);
    isis_password_decode((char *)area->domain_passwd.passwd, (char *)area->domain_passwd.en_passwd);
  }

  if (area->is_type & ISIS_LEVEL2)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);

    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL2)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL2Csnp", circuit->t_send_csnp[1], send_l2_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[1], CSNP_JITTER));
      }
    }
  }
  return CMD_SUCCESS;
}

ALIAS(domain_passwd_md5,
      domain_passwd_md5_snpauth_cmd,
      "domain-password md5 WORD authenticate snp (send-only|validate)",
      "Set the authentication password for a routing domain\n"
      "Authentication type\n"
      "Routing domain password length 1-254\n"
      "Authentication\n"
      "SNP PDUs\n"
      "Send but do not check PDUs on receiving\n"
      "Send and check PDUs on receiving\n")

DEFUN(domain_passwd_md5_h3c,
      domain_passwd_md5_h3c_cmd,
      "domain-authentication-mode md5 WORD",
      "Set the type and password of routing domain authentication\n"
      "MD5 authentication type\n"
      "Authentication password length <1-254>\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;

  area = vty->index;
  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_SEND);
  SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);

  /*there is password already,now  change the password */
  if (area->domain_passwd.type != ISIS_PASSWD_TYPE_HMAC_MD5 ||
      area->domain_passwd.len != len ||
      memcmp(area->domain_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL2 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL2 - 1]);
      while (dnode)
      {
        next = dict_next(area->lspdb[ISIS_LEVEL2 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL2 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->domain_passwd.len = (u_char)len;
  area->domain_passwd.type = ISIS_PASSWD_TYPE_HMAC_MD5;

  if (vty->config_read_flag != 1)
  {
    memset(area->domain_passwd.passwd, 0, 255);
    memset(area->domain_passwd.en_passwd, 0, 255);
    strncpy((char *)area->domain_passwd.passwd, argv[0], len);
    isis_password_encode((char *)area->domain_passwd.en_passwd, (char *)area->domain_passwd.passwd);
  }

  if (vty->config_read_flag == 1)
  {
    memset(area->domain_passwd.passwd, 0, 255);
    memset(area->domain_passwd.en_passwd, 0, 255);
    strncpy((char *)area->domain_passwd.en_passwd, argv[0], len);
    isis_password_decode((char *)area->domain_passwd.passwd, (char *)area->domain_passwd.en_passwd);
  }

  if (area->is_type & ISIS_LEVEL2)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);

    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL2)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL2Csnp", circuit->t_send_csnp[1], send_l2_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[1], CSNP_JITTER));
      }
    }
  }

  return CMD_SUCCESS;
}

DEFUN(domain_passwd_clear,
      domain_passwd_clear_cmd,
      "domain-password clear WORD",
      "Set the authentication password for a routing domain\n"
      "Authentication type\n"
      "Routing domain password length 1-254\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;

  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (argc > 1)
  {
    SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_SEND);
    if (strncmp(argv[1], "v", 1) == 0)
      SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);
    else
      UNSET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);
  }
  else
  {
    UNSET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_SEND);
    UNSET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);
  }

  /*there is password already,now  change the password */
  if (area->domain_passwd.type != ISIS_PASSWD_TYPE_CLEARTXT ||
      area->domain_passwd.len != len ||
      memcmp(area->domain_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL2 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL2 - 1]);
      while (dnode)
      {
        next = dict_next(area->lspdb[ISIS_LEVEL2 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL2 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->domain_passwd.len = (u_char)len;
  area->domain_passwd.type = ISIS_PASSWD_TYPE_CLEARTXT;
  memset(area->domain_passwd.passwd, 0, 255);
  strncpy((char *)area->domain_passwd.passwd, argv[0], len);

  if (area->is_type & ISIS_LEVEL2)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);

    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL2)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL2Csnp", circuit->t_send_csnp[1], send_l2_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[1], CSNP_JITTER));
      }
    }
  }

  return CMD_SUCCESS;
}

ALIAS(domain_passwd_clear,
      domain_passwd_clear_snpauth_cmd,
      "domain-password clear WORD authenticate snp (send-only|validate)",
      "Set the authentication password for a routing domain\n"
      "Authentication type\n"
      "Routing domain password length 1-254\n"
      "Authentication\n"
      "SNP PDUs\n"
      "Send but do not check PDUs on receiving\n"
      "Send and check PDUs on receiving\n")

DEFUN(domain_passwd_clear_h3c,
      domain_passwd_clear_h3c_cmd,
      "domain-authentication-mode simple WORD",
      "Set the type and password of routing domain authentication\n"
      "Plaintext authentication type\n"
      "Authentication password length <1-254>\n")
{
  struct isis_area *area;
  struct isis_circuit *circuit;
  struct listnode *cnode;
  int len;
  dnode_t *dnode, *next;
  struct isis_lsp *lsp;

  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long area password (>254)%s", VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_SEND);
  SET_FLAG(area->domain_passwd.snp_auth, SNP_AUTH_RECV);

  /*there is password already,now  change the password */
  if (area->domain_passwd.type != ISIS_PASSWD_TYPE_CLEARTXT ||
      area->domain_passwd.len != len ||
      memcmp(area->domain_passwd.passwd, argv[0], len))
  {
    if (area->lspdb[ISIS_LEVEL2 - 1] != NULL)
    {
      dnode = dict_first(area->lspdb[ISIS_LEVEL2 - 1]);
      while (dnode)
      {
        next = dict_next(area->lspdb[ISIS_LEVEL2 - 1], dnode);
        lsp = dnode_get(dnode);

        if (memcmp(lsp->lsp_header->lsp_id, area->sysid, ISIS_SYS_ID_LEN))
        {
          if (argc > 1)
          {
            lsp_destroy(lsp);
            dict_delete_free(area->lspdb[ISIS_LEVEL2 - 1], dnode);
            dnode = next;
            continue;
          }
          lsp_clear_data(lsp);
          lsp->lsp_header->seq_num = 0;
          lsp->lsp_header->pdu_len = 0;
        }
        dnode = next;
      }
    }
  }
  else
    return CMD_SUCCESS;

  area->domain_passwd.len = (u_char)len;
  area->domain_passwd.type = ISIS_PASSWD_TYPE_CLEARTXT;
  memset(area->domain_passwd.passwd, 0, 255);
  strncpy((char *)area->domain_passwd.passwd, argv[0], len);

  lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);

  if (area->is_type & ISIS_LEVEL2)
  {
    lsp_regenerate_schedule(area, area->is_type, 1);

    for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
    {
      if (circuit->circ_type == CIRCUIT_T_P2P && circuit->is_type & ISIS_LEVEL2)
      {
        THREAD_TIMER_ON_NEW((char *)"ISISSendL2Csnp", circuit->t_send_csnp[1], send_l2_csnp, circuit,
                            isis_jitter(circuit->csnp_interval[1], CSNP_JITTER));
      }
    }
  }

  return CMD_SUCCESS;
}

DEFUN(no_domain_passwd,
      no_domain_passwd_cmd,
      "no domain-password",
      NO_STR
      "Set the authentication password for a routing domain\n")
{
  struct isis_area *area;
  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  if (area->domain_passwd.len == 0)
  {
    vty_warning_out(vty, "No domain-password in this instance%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(&area->domain_passwd, 0, sizeof(struct isis_passwd));
  lsp_regenerate_schedule(area, area->is_type, 1);

  return CMD_SUCCESS;
}

DEFUN(no_domain_passwd_h3c,
      no_domain_passwd_h3c_cmd,
      "undo domain-authentication-mode",
      "Cancel current configuration\n"
      "Set the type and password of routing domain authentication\n")
{
  struct isis_area *area;
  area = vty->index;

  if (!area)
  {
    vty_warning_out(vty, "Can't find IS-IS instance%s", VTY_NEWLINE);
    return CMD_ERR_NO_MATCH;
  }

  if (area->domain_passwd.len == 0)
  {
    vty_warning_out(vty, "No domain-password in this instance%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(&area->domain_passwd, 0, sizeof(struct isis_passwd));
  lsp_regenerate_schedule(area, area->is_type, 1);

  return CMD_SUCCESS;
}

DEFUN(isis_show_timerinfo,
      isis_show_timerinfo_cmd,
      "show isis timer",
      SHOW_STR
      "isis module information\n"
      "isis module timer\n")
{
  int iInLn = 1000;
  char bInfo[1000] = {0};

  iInLn = high_pre_timer_info(bInfo, iInLn);

  if (iInLn > 0)
    vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
  else
    vty_error_out(vty, "Show isis module timer infor error. %s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

const struct message isis_debug_name[] =
    {
        {.key = ISIS_DEBUG_TYPE_HELLO, .str = "hello"},
        {.key = ISIS_DEBUG_TYPE_SPF, .str = "spf"},
        {.key = ISIS_DEBUG_TYPE_LSP, .str = "lsp"},
        {.key = ISIS_DEBUG_TYPE_MSG, .str = "message"},
        {.key = ISIS_DEBUG_TYPE_EVENTS, .str = "events"},
        {.key = ISIS_DEBUG_TYPE_ROUTE, .str = "route"},
        {.key = ISIS_DEBUG_TYPE_CSNP, .str = "csnp"},
        {.key = ISIS_DEBUG_TYPE_PSNP, .str = "psnp"},
        {.key = ISIS_DEBUG_TYPE_SEND, .str = "send"},
        {.key = ISIS_DEBUG_TYPE_RECEIVE, .str = "receive"},
        {.key = ISIS_DEBUG_TYPE_TLV, .str = "tlv"},
        {.key = ISIS_DEBUG_TYPE_ALL, .str = "all"}};

DEFUN(isis_change_all_cost,
      isis_change_all_cost_cmd,
      "circuit-cost <1-16777215>",
      "Set the global default cost for all the interfaces\n"
      "Cost value\n")
{
  uint32_t met = 0;
  uint8_t flag = 0;
  struct isis_area *area = NULL;
  struct listnode *cnode = NULL;
  struct isis_circuit *circuit = NULL;

  area = vty->index;
  met = (uint32_t)atoi(argv[0]);

  /* scan all of the circuits to change the cost */
  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
  {
    if (!circuit)
      continue;

    if (!circuit->ip_router && !circuit->ipv6_router)
      continue;

    if (area->newmetric == 1)
    {
      if (circuit->te_metric[0] != met || circuit->te_metric[1] != met)
      {
        circuit->te_metric[0] = met;
        circuit->te_metric[1] = met;
        /* the flag is used for deciding whether should fresh the lsp */
        flag = 1;
      }
    }

    if (area->oldmetric == 1)
    {
      if (met > 63)
      {
        vty_warning_out(vty, "IS-IS truncate the cost values which exceed 63 to 63. %s", VTY_NEWLINE);
        if (circuit->metrics[0].metric_default != 63 || circuit->metrics[1].metric_default != 63)
        {
          circuit->metrics[0].metric_default = 63;
          circuit->metrics[1].metric_default = 63;
          /* the flag is used for deciding whether should fresh the lsp */
          flag = 1;
        }
      }
      else
      {
        if (circuit->metrics[0].metric_default != met || circuit->metrics[1].metric_default != met)
        {
          circuit->metrics[0].metric_default = met;
          circuit->metrics[1].metric_default = met;
          /* the flag is used for deciding whether should fresh the lsp */
          flag = 1;
        }
      }
    }
  }

  if (flag != 0)
    lsp_regenerate_schedule(area, area->is_type, 0);

  return CMD_SUCCESS;
}

DEFUN(isis_change_all_cost_v6,
      isis_change_all_cost_v6_cmd,
      "circuit-cost <1-16777215>",
      "Set the global default cost for all the interfaces\n"
      "Cost value\n")
{
  uint32_t met = 0;
  uint8_t flag = 0;
  struct isis_area *area = NULL;
  struct listnode *cnode = NULL;
  struct isis_circuit *circuit = NULL;

  area = vty->index;
  met = (uint32_t)atoi(argv[0]);

  /* scan all of the circuits to change the cost */
  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
  {
    if (!circuit)
      continue;

    if (!circuit->ip_router && !circuit->ipv6_router)
      continue;

    if (area->newmetric == 1)
    {
      if (circuit->te_metric_v6[0] != met || circuit->te_metric_v6[1] != met)
      {
        circuit->te_metric_v6[0] = met;
        circuit->te_metric_v6[1] = met;
        /* the flag is used for deciding whether should fresh the lsp */
        flag = 1;
      }
    }

    if (area->oldmetric == 1)
    {
      if (met > 63)
      {
        vty_warning_out(vty, "IS-IS truncate the cost values which exceed 63 to 63. %s", VTY_NEWLINE);
        if (circuit->metrics_v6[0].metric_default != 63 || circuit->metrics_v6[1].metric_default != 63)
        {
          circuit->metrics_v6[0].metric_default = 63;
          circuit->metrics_v6[1].metric_default = 63;
          /* the flag is used for deciding whether should fresh the lsp */
          flag = 1;
        }
      }
      else
      {
        if (circuit->metrics_v6[0].metric_default != met || circuit->metrics_v6[1].metric_default != met)
        {
          circuit->metrics_v6[0].metric_default = met;
          circuit->metrics_v6[1].metric_default = met;
          /* the flag is used for deciding whether should fresh the lsp */
          flag = 1;
        }
      }
    }
  }

  if (flag != 0)
    lsp_regenerate_schedule(area, area->is_type, 0);

  return CMD_SUCCESS;
}

DEFUN(no_isis_change_all_cost,
      no_isis_change_all_cost_cmd,
      "undo circuit-cost",
      "Cancel current setting\n"
      "Set the global default cost for all the interfaces\n")
{
  uint8_t flag = 0;
  struct isis_area *area = NULL;
  struct listnode *cnode = NULL;
  struct isis_circuit *circuit = NULL;

  area = vty->index;

  /* scan all of the circuits to change the cost */

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
  {
    if (!circuit)
      continue;

    if (!circuit->ip_router && !circuit->ipv6_router)
      continue;

    if (area->newmetric == 1)
    {
      if (circuit->te_metric[0] != DEFAULT_CIRCUIT_METRIC || circuit->te_metric[1] != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->te_metric[0] = DEFAULT_CIRCUIT_METRIC;
        circuit->te_metric[1] = DEFAULT_CIRCUIT_METRIC;
        /* the flag is used for deciding whether should fresh the lsp */
        flag = 1;
      }
    }

    if (area->oldmetric == 1)
    {
      if (circuit->metrics[0].metric_default != DEFAULT_CIRCUIT_METRIC || circuit->metrics[1].metric_default != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->metrics[0].metric_default = DEFAULT_CIRCUIT_METRIC;
        circuit->metrics[1].metric_default = DEFAULT_CIRCUIT_METRIC;
        /* the flag is used for deciding whether should fresh the lsp */
        flag = 1;
      }
    }
  }

  if (flag != 0)
    lsp_regenerate_schedule(area, area->is_type, 0);

  return CMD_SUCCESS;
}

DEFUN(no_isis_change_all_cost_v6,
      no_isis_change_all_cost_v6_cmd,
      "undo circuit-cost",
      "Cancel current setting\n"
      "Set the global default cost for all the interfaces\n")
{
  uint8_t flag = 0;
  struct isis_area *area = NULL;
  struct listnode *cnode = NULL;
  struct isis_circuit *circuit = NULL;

  area = vty->index;

  /* scan all of the circuits to change the cost */

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
  {
    if (!circuit)
      continue;

    if (!circuit->ip_router && !circuit->ipv6_router)
      continue;

    if (area->newmetric == 1)
    {
      if (circuit->te_metric_v6[0] != DEFAULT_CIRCUIT_METRIC || circuit->te_metric_v6[1] != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->te_metric_v6[0] = DEFAULT_CIRCUIT_METRIC;
        circuit->te_metric_v6[1] = DEFAULT_CIRCUIT_METRIC;
        /* the flag is used for deciding whether should fresh the lsp */
        flag = 1;
      }
    }

    if (area->oldmetric == 1)
    {
      if (circuit->metrics_v6[0].metric_default != DEFAULT_CIRCUIT_METRIC || circuit->metrics_v6[1].metric_default != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->metrics_v6[0].metric_default = DEFAULT_CIRCUIT_METRIC;
        circuit->metrics_v6[1].metric_default = DEFAULT_CIRCUIT_METRIC;
        /* the flag is used for deciding whether should fresh the lsp */
        flag = 1;
      }
    }
  }

  if (flag != 0)
    lsp_regenerate_schedule(area, area->is_type, 0);

  return CMD_SUCCESS;
}

DEFUN(isis_debug_monitor,
      isis_debug_monitor_cmd,
      "debug isis (enable|disable) (hello|spf|lsp|message|events|route|neighbor|psnp|csnp|send|receive|tlv|all)",
      "Debug information to moniter\n"
      "Programe name\n"
      "Enable statue\n"
      "Disatble statue\n"
      "Type name of packets hello\n"
      "Type name of spf messege\n"
      "Type name of lsp messege\n"
      "Type name of message log queue \n"
      "Type name of events log queue \n"
      "Type name of route status\n"
      "Type name of neighbor status\n"
      "Type name of packets psnp\n"
      "Type name of packets csnp\n"
      "Type name of packets send\n"
      "Type name of packets receive\n"
      "Type name of isis tlv\n"
      "Type name of all debug\n")
{
  unsigned int zlog_num;

  for (zlog_num = 0; zlog_num < array_size(isis_debug_name); zlog_num++)
  {
    if (!strncmp(argv[1], isis_debug_name[zlog_num].str, 3))
    {
      zlog_debug_set(vty, isis_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

      return CMD_SUCCESS;
    }
  }

  vty_out(vty, "No debug typd find %s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

#define AUTO_NARROW_METRIC 20
int isis_set_interface_autocost(struct isis_area *area, uint32_t refbw, int afi)
{
  uint8_t flag = 0;
  u_int32_t newcost = 0;
  struct listnode *node = NULL;
  struct isis_circuit *circuit = NULL;

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
  {
    if (area->oldmetric == 1)
    {
      if (afi == AF_INET)
      {
        /* whatever the param is , 20 is the cost for narrow , just like h3c*/
        if (circuit->metrics[0].metric_default != AUTO_NARROW_METRIC ||
            circuit->metrics[1].metric_default != AUTO_NARROW_METRIC)
        {
//          if (circuit->interface != NULL)
//            circuit->interface->bandwidth = refbw;
          circuit->metrics[0].metric_default = AUTO_NARROW_METRIC;
          circuit->metrics[1].metric_default = AUTO_NARROW_METRIC;
          circuit->ipv4_auto_cost_flag = 1;
          flag = 1;
        }
      }

      if (afi == AF_INET6)
      {
        if (circuit->metrics_v6[0].metric_default != AUTO_NARROW_METRIC ||
            circuit->metrics_v6[1].metric_default != AUTO_NARROW_METRIC)
        {
 //         if (circuit->interface != NULL)
 //           circuit->interface->bandwidth = refbw;
          circuit->metrics_v6[0].metric_default = AUTO_NARROW_METRIC;
          circuit->metrics_v6[1].metric_default = AUTO_NARROW_METRIC;
          circuit->ipv6_auto_cost_flag = 1;
          flag = 1;
        }
      }
    }

    if (area->newmetric == 1)
    {
      /* the max is 16777214 just lik h3c , calculate the proportion and multiply the default */
      if (refbw / ISIS_DEFAULT_BANDWIDTH * DEFAULT_CIRCUIT_METRIC >= 16777214)
        newcost = 16777214;
      else if (refbw / ISIS_DEFAULT_BANDWIDTH >= 1)
        newcost = refbw / ISIS_DEFAULT_BANDWIDTH * DEFAULT_CIRCUIT_METRIC;
      else
        newcost = 1;

      if (afi == AF_INET)
      {
        if (circuit->te_metric[0] != newcost || circuit->te_metric[1] != newcost)
        {
//          if (circuit->interface != NULL)
//            circuit->interface->bandwidth = refbw;
          circuit->te_metric[0] = newcost;
          circuit->te_metric[1] = newcost;
          circuit->ipv4_auto_cost_flag = 1;
          flag = 1;
        }
      }

      if (afi == AF_INET6)
      {
        if (circuit->te_metric_v6[0] != newcost || circuit->te_metric_v6[1] != newcost)
        {
//          if (circuit->interface != NULL)
//            circuit->interface->bandwidth = refbw;
          circuit->te_metric_v6[0] = newcost;
          circuit->te_metric_v6[1] = newcost;
          circuit->ipv6_auto_cost_flag = 1;
          flag = 1;
        }
      }
    }
  }
  return flag;
}

DEFUN(isis_bandwidth_reference,
      isis_bandwidth_reference_cmd,
      "bandwidth-reference <1-2147483648>",
      "Set the bandwidth reference\n"
      "The reference bandwidth (Mbits/s)\n")
{
  u_int32_t refbw = 0;
  struct isis_area *area = vty->index;

  if (!area)
    return CMD_WARNING;

  refbw = strtol(argv[0], NULL, 10);

  /* If reference bandwidth is changed. */
  if (refbw == area->ref_bandwidth_ipv4)
  {
    return CMD_SUCCESS;
  }

  area->ref_bandwidth_ipv4 = refbw;

  if (isis_set_interface_autocost(area, refbw, AF_INET) == 1)
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}

DEFUN(isis_bandwidth_reference_v6,
      isis_bandwidth_reference_v6_cmd,
      "bandwidth-reference <1-2147483648>",
      "Set the bandwidth reference\n"
      "The reference bandwidth (Mbits/s)\n")
{
  u_int32_t refbw = 0;
  struct isis_area *area = vty->index;

  if (!area)
    return CMD_WARNING;

  refbw = strtol(argv[0], NULL, 10);

  /* If reference bandwidth is changed. */
  if (refbw == area->ref_bandwidth_ipv6)
  {
    return CMD_SUCCESS;
  }

  area->ref_bandwidth_ipv6 = refbw;

  if (isis_set_interface_autocost(area, refbw, AF_INET6) == 1)
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}

DEFUN(undo_isis_bandwidth_reference,
      undo_isis_bandwidth_reference_cmd,
      "undo bandwidth-reference",
      "Cancel current setting\n"
      "Set the bandwidth reference\n")
{
  uint8_t flag = 0;
  struct isis_area *area = vty->index;
  struct listnode *node = NULL;
  struct isis_circuit *circuit = NULL;

  /* If reference bandwidth is changed. */
  if (0 == area->ref_bandwidth_ipv4)
  {
    return CMD_SUCCESS;
  }

  area->ref_bandwidth_ipv4 = 0;

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
  {
    if (area->oldmetric == 1)
    {
      if (circuit->metrics[0].metric_default != DEFAULT_CIRCUIT_METRIC ||
          circuit->metrics[1].metric_default != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->metrics[0].metric_default = DEFAULT_CIRCUIT_METRIC;
        circuit->metrics[1].metric_default = DEFAULT_CIRCUIT_METRIC;
        circuit->ipv4_auto_cost_flag = 0;
        flag = 1;
      }
    }

    if (area->newmetric == 1)
    {
      if (circuit->te_metric[0] != DEFAULT_CIRCUIT_METRIC || circuit->te_metric[1] != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->te_metric[0] = DEFAULT_CIRCUIT_METRIC;
        circuit->te_metric[1] = DEFAULT_CIRCUIT_METRIC;
        circuit->ipv4_auto_cost_flag = 0;
        flag = 1;
      }
    }
  }

  if (flag == 1)
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}

DEFUN(undo_isis_bandwidth_reference_v6,
      undo_isis_bandwidth_reference_v6_cmd,
      "undo bandwidth-reference",
      "Cancel current setting\n"
      "Set the bandwidth reference\n")
{
  uint8_t flag = 0;
  struct isis_area *area = vty->index;
  struct listnode *node = NULL;
  struct isis_circuit *circuit = NULL;

  /* If reference bandwidth is changed. */
  if (0 == area->ref_bandwidth_ipv6)
  {
    return CMD_SUCCESS;
  }

  area->ref_bandwidth_ipv6 = 0;

  for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
  {
    if (area->oldmetric == 1)
    {
      if (circuit->metrics_v6[0].metric_default != DEFAULT_CIRCUIT_METRIC ||
          circuit->metrics_v6[1].metric_default != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->metrics_v6[0].metric_default = DEFAULT_CIRCUIT_METRIC;
        circuit->metrics_v6[1].metric_default = DEFAULT_CIRCUIT_METRIC;
        circuit->ipv6_auto_cost_flag = 0;
        flag = 1;
      }
    }

    if (area->newmetric == 1)
    {
      if (circuit->te_metric_v6[0] != DEFAULT_CIRCUIT_METRIC || circuit->te_metric_v6[1] != DEFAULT_CIRCUIT_METRIC)
      {
        circuit->te_metric_v6[0] = DEFAULT_CIRCUIT_METRIC;
        circuit->te_metric_v6[1] = DEFAULT_CIRCUIT_METRIC;
        circuit->ipv6_auto_cost_flag = 0;
        flag = 1;
      }
    }
  }

  if (flag == 1)
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}


#ifdef TOPOLOGY_GENERATE

DEFUN(topology_generate_grid,
      topology_generate_grid_cmd,
      "topology generate grid <1-100> <1-100> <1-65000> [param] [param] "
      "[param]",
      "Topology generation for IS-IS\n"
      "Topology generation\n"
      "Grid topology\n"
      "X parameter of the grid\n"
      "Y parameter of the grid\n"
      "Random seed\n"
      "Optional param 1\n"
      "Optional param 2\n"
      "Optional param 3\n"
      "Topology\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (!spgrid_check_params(vty, argc, argv))
  {
    if (area->topology)
      list_delete(area->topology);
    area->topology = NULL;
    area->topology = list_new();
    memcpy(area->top_params, vty->buf, 200);
    gen_spgrid_topology(vty, area->topology);
    remove_topology_lsps(area);
    generate_topology_lsps(area);
    /* Regenerate L1 LSP to get two way connection to the generated
       * topology. */
    lsp_regenerate_schedule(area, IS_LEVEL_1 | IS_LEVEL_2, 1);
  }

  return CMD_SUCCESS;
}

/*
DEFUN(show_isis_generated_topology,
      show_isis_generated_topology_cmd,
      "show isis generated-topologies",
      SHOW_STR
      "ISIS network information\n"
      "Show generated topologies\n")
{
  struct isis_area *area;
  struct listnode *node;
  struct listnode *node2;
  struct arc *arc;

  for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
  {
    if (!area->topology)
      continue;

    vty_out(vty, "Topology for isis area: %s%s", area->area_tag,
            VTY_NEWLINE);
    vty_out(vty, "From node     To node     Distance%s", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(area->topology, node2, arc))
      vty_out(vty, "%9ld %11ld %12ld%s", arc->from_node, arc->to_node,
              arc->distance, VTY_NEWLINE);
  }
  return CMD_SUCCESS;
}
*/

/* Base IS for topology generation. */
DEFUN(topology_baseis,
      topology_baseis_cmd,
      "topology base-is WORD",
      "Topology generation for IS-IS\n"
      "A Network IS Base for this topology\n"
      "XXXX.XXXX.XXXX Network entity title (NET)\n")
{
  struct isis_area *area;
  u_char buff[ISIS_SYS_ID_LEN];

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  if (sysid2buff(buff, argv[0]))
    sysid2buff(area->topology_baseis, argv[0]);

  return CMD_SUCCESS;
}

DEFUN(no_topology_baseis,
      no_topology_baseis_cmd,
      "no topology base-is WORD",
      NO_STR
      "Topology generation for IS-IS\n"
      "A Network IS Base for this topology\n"
      "XXXX.XXXX.XXXX Network entity title (NET)\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  memcpy(area->topology_baseis, DEFAULT_TOPOLOGY_BASEIS, ISIS_SYS_ID_LEN);
  return CMD_SUCCESS;
}

ALIAS(no_topology_baseis,
      no_topology_baseis_noid_cmd,
      "no topology base-is",
      NO_STR
      "Topology generation for IS-IS\n"
      "A Network IS Base for this topology\n")

DEFUN(topology_basedynh,
      topology_basedynh_cmd,
      "topology base-dynh WORD",
      "Topology generation for IS-IS\n"
      "Dynamic hostname base for this topology\n"
      "Dynamic hostname base\n")
{
  struct isis_area *area;

  area = vty->index;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return CMD_WARNING;
  }

  /* I hope that it's enough. */
  area->topology_basedynh = strndup(argv[0], 16);
  return CMD_SUCCESS;
}

#endif /* TOPOLOGY_GENERATE */

/* IS-IS configuration write function */
int isis_config_write(struct vty *vty)
{
  int write = 0;

  if (isis != NULL)
  {
    struct isis_area *area;
    struct listnode *node, *node2;
    struct ip_summary *summary;
    struct ipv6_summary *summary_ipv6;
    char buf[IPV6_ADDR_STRLEN] = "";

    for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
    {
      /* ISIS - Area name */
      vty_out(vty, "isis instance %s", area->area_tag);

      if (area->vpn != 0)
      {
        vty_out(vty, " vpn-instance %d", area->vpn);
      }

      vty_out(vty, "%s", VTY_NEWLINE);
      write++;
      /* ISIS - Net */
      if (listcount(area->area_addrs) > 0)
      {
        struct area_addr *area_addr;
        for (ALL_LIST_ELEMENTS_RO(area->area_addrs, node2, area_addr))
        {
          vty_out(vty, " net entity %s%s",
                  isonet_print(area_addr->area_addr,
                               area_addr->addr_len + ISIS_SYS_ID_LEN +
                                   1),
                  VTY_NEWLINE);
          write++;
        }
      }

      if (area->ref_bandwidth_ipv4 != 0)
      {
        vty_out(vty, " bandwidth-reference %u %s", area->ref_bandwidth_ipv4, VTY_NEWLINE);
        write++;
      }

      /* ISIS area-password auth */
      if (area->area_passwd.type == ISIS_PASSWD_TYPE_CLEARTXT)
      {
        switch (area->area_passwd.snp_auth)
        {
        case SNP_AUTH_SEND | SNP_AUTH_RECV:
        {
          vty_out(vty, " area-password clear %s authenticate snp validate %s", area->area_passwd.passwd, VTY_NEWLINE);
          write++;
          break;
        }
        case SNP_AUTH_SEND:
        {
          vty_out(vty, " area-password clear %s authenticate snp send-only %s", area->area_passwd.passwd, VTY_NEWLINE);
          write++;
          break;
        }
        default:
        {
          vty_out(vty, " area-password clear %s %s", area->area_passwd.passwd, VTY_NEWLINE);
          write++;
          break;
        }
        }
      }

      if (area->area_passwd.type == ISIS_PASSWD_TYPE_HMAC_MD5)
      {
        switch (area->area_passwd.snp_auth)
        {
        case SNP_AUTH_SEND | SNP_AUTH_RECV:
        {
          vty_out(vty, " area-password md5 %s authenticate snp validate %s", area->area_passwd.en_passwd, VTY_NEWLINE);
          write++;
          break;
        }
        case SNP_AUTH_SEND:
        {
          vty_out(vty, " area-password md5 %s authenticate snp send-only %s", area->area_passwd.en_passwd, VTY_NEWLINE);
          write++;
          break;
        }
        default:
        {
          vty_out(vty, " area-password md5 %s %s", area->area_passwd.en_passwd, VTY_NEWLINE);
          write++;
          break;
        }
        }
      }

      /*domain-password auth */
      if (area->domain_passwd.type == ISIS_PASSWD_TYPE_CLEARTXT)
      {
        switch (area->domain_passwd.snp_auth)
        {
        case SNP_AUTH_SEND | SNP_AUTH_RECV:
        {
          vty_out(vty, " domain-password clear %s authenticate snp validate %s", area->domain_passwd.passwd, VTY_NEWLINE);
          write++;
          break;
        }
        case SNP_AUTH_SEND:
        {
          vty_out(vty, " domain-password clear %s authenticate snp send-only %s", area->domain_passwd.passwd, VTY_NEWLINE);
          write++;
          break;
        }
        default:
        {
          vty_out(vty, " domain-password clear %s %s", area->domain_passwd.passwd, VTY_NEWLINE);
          write++;
          break;
        }
        }
      }

      if (area->domain_passwd.type == ISIS_PASSWD_TYPE_HMAC_MD5)
      {
        switch (area->domain_passwd.snp_auth)
        {
        case SNP_AUTH_SEND | SNP_AUTH_RECV:
        {
          vty_out(vty, " domain-password md5 %s authenticate snp validate %s", area->domain_passwd.en_passwd, VTY_NEWLINE);
          write++;
          break;
        }
        case SNP_AUTH_SEND:
        {
          vty_out(vty, " domain-password md5 %s authenticate snp send-only %s", area->domain_passwd.en_passwd, VTY_NEWLINE);
          write++;
          break;
        }
        default:
        {
          vty_out(vty, " domain-password md5 %s %s", area->domain_passwd.en_passwd, VTY_NEWLINE);
          write++;
          break;
        }
        }
      }

      /* ISIS - Dynamic hostname - Defaults to true so only display if
	 * false. */
      if (!area->dynhostname)
      {
        vty_out(vty, " no hostname dynamic%s", VTY_NEWLINE);
        write++;
      }
      /* ISIS - Metric-Style - when true displays wide */
      if (area->newmetric)
      {
        if (area->oldmetric)
        {
          if (area->compatible)
            vty_out(vty, " cost-style compatible%s", VTY_NEWLINE);
        }
        else
        {
          if (area->compatible)
            vty_out(vty, " cost-style wide-compatible%s", VTY_NEWLINE);
          //			else
          //				vty_out (vty, " metric-style wide%s", VTY_NEWLINE);
        }
        write++;
      }
      else
      {
        if (!area->compatible)
          vty_out(vty, " metric-style narrow%s", VTY_NEWLINE);
        else
          vty_out(vty, " cost-style narrow-compatible%s", VTY_NEWLINE);
        write++;
      }
      /* ISIS - overload-bit */
      if (area->overload_bit)
      {
        vty_out(vty, " overload bit enable%s", VTY_NEWLINE);
        write++;
      }
      /* ISIS - Area is-type (level-1-2 is default) */
      if (area->is_type == IS_LEVEL_1)
      {
        vty_out(vty, " level level-1%s", VTY_NEWLINE);
        write++;
      }
      else if (area->is_type == IS_LEVEL_2)
      {
        vty_out(vty, " level level-2%s", VTY_NEWLINE);
        write++;
      }
      write += isis_redist_config_write(vty, area, AF_INET);
      //write += isis_redist_config_write(vty, area, AF_INET6);
      /* ISIS - Lsp generation interval */
      if (area->lsp_gen_interval[0] == area->lsp_gen_interval[1])
      {
        if (area->lsp_gen_interval[0] != DEFAULT_MIN_LSP_GEN_INTERVAL)
        {
          vty_out(vty, " lsp gen-interval %d%s",
                  area->lsp_gen_interval[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        if (area->lsp_gen_interval[0] != DEFAULT_MIN_LSP_GEN_INTERVAL)
        {
          vty_out(vty, " lsp gen-interval level-1 %d%s",
                  area->lsp_gen_interval[0], VTY_NEWLINE);
          write++;
        }
        if (area->lsp_gen_interval[1] != DEFAULT_MIN_LSP_GEN_INTERVAL)
        {
          vty_out(vty, " lsp gen-interval level-2 %d%s",
                  area->lsp_gen_interval[1], VTY_NEWLINE);
          write++;
        }
      }
      /* ISIS - LSP lifetime */
      if (area->max_lsp_lifetime[0] == area->max_lsp_lifetime[1])
      {
        if (area->max_lsp_lifetime[0] != DEFAULT_LSP_LIFETIME)
        {
          vty_out(vty, " lsp lifetime %u%s", area->max_lsp_lifetime[0],
                  VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        if (area->max_lsp_lifetime[0] != DEFAULT_LSP_LIFETIME)
        {
          vty_out(vty, " lsp lifetime level-1 %u%s",
                  area->max_lsp_lifetime[0], VTY_NEWLINE);
          write++;
        }
        if (area->max_lsp_lifetime[1] != DEFAULT_LSP_LIFETIME)
        {
          vty_out(vty, " lsp lifetime level-2 %u%s",
                  area->max_lsp_lifetime[1], VTY_NEWLINE);
          write++;
        }
      }
      /* ISIS - LSP refresh interval */
      if (area->lsp_refresh[0] == area->lsp_refresh[1])
      {
        if (area->lsp_refresh[0] != DEFAULT_MAX_LSP_GEN_INTERVAL)
        {
          vty_out(vty, " lsp refresh-interval %u%s", area->lsp_refresh[0],
                  VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        if (area->lsp_refresh[0] != DEFAULT_MAX_LSP_GEN_INTERVAL)
        {
          vty_out(vty, " lsp refresh-interval level-1 %u%s",
                  area->lsp_refresh[0], VTY_NEWLINE);
          write++;
        }
        if (area->lsp_refresh[1] != DEFAULT_MAX_LSP_GEN_INTERVAL)
        {
          vty_out(vty, " lsp refresh-interval level-2 %u%s",
                  area->lsp_refresh[1], VTY_NEWLINE);
          write++;
        }
      }
      if (area->lsp_mtu != DEFAULT_LSP_MTU)
      {
        vty_out(vty, " lsp length %u%s", area->lsp_mtu, VTY_NEWLINE);
        write++;
      }

      /* Minimum SPF interval. */
      if (area->min_spf_interval[0] == area->min_spf_interval[1])
      {
        if (area->min_spf_interval[0] != MINIMUM_SPF_INTERVAL)
        {
          vty_out(vty, " spf interval %d%s",
                  area->min_spf_interval[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        if (area->min_spf_interval[0] != MINIMUM_SPF_INTERVAL)
        {
          vty_out(vty, " spf interval level-1 %d%s",
                  area->min_spf_interval[0], VTY_NEWLINE);
          write++;
        }
        if (area->min_spf_interval[1] != MINIMUM_SPF_INTERVAL)
        {
          vty_out(vty, " spf interval level-2 %d%s",
                  area->min_spf_interval[1], VTY_NEWLINE);
          write++;
        }
      }

      /*  
      if (area->distance != DEFAULT_DISTANCE)
      {
        vty_out(vty, " preference %d%s", area->distance, VTY_NEWLINE);
        write++;
      }
*/

      if (area->area_import_to_l2 == 0)
      {
        vty_out(vty, " no route-import level-1 into level-2%s", VTY_NEWLINE);
        write++;
      }

      if (area->area_import_to_l1 == 1)
      {
        vty_out(vty, " route-import level-2 into level-1%s", VTY_NEWLINE);
        write++;
      }

      if (listcount(area->summary) > 0)
      {
        for (ALL_LIST_ELEMENTS_RO(area->summary, node2, summary))
        {
          vty_out(vty, " route summary");
          vty_out(vty, " %s/%d", inet_ntoa(summary->summary_p.prefix),
                  summary->summary_p.prefixlen);

          if (summary->level == 1 || summary->level == 2)
            vty_out(vty, " level-%d%s", summary->level, VTY_NEWLINE);
          else
            vty_out(vty, " level-1-2%s", VTY_NEWLINE);
          write++;
        }
      }

      vty_out(vty, " address-family ipv4%s", VTY_NEWLINE);
      write++;

      if (area->distance4 != DEFAULT_DISTANCE)
      {
        vty_out(vty, "  preference %d%s", area->distance4, VTY_NEWLINE);
        write++;
      }

      vty_out(vty, " address-family ipv6%s", VTY_NEWLINE);
      write++;
      if (area->distance6 != DEFAULT_DISTANCE)
      {
        vty_out(vty, "  preference %d%s", area->distance6, VTY_NEWLINE);
        write++;
      }

      if (area->ref_bandwidth_ipv6 != 0)
      {
        vty_out(vty, "  bandwidth-reference %u %s", area->ref_bandwidth_ipv6, VTY_NEWLINE);
        write++;
      }

      if (area->area_import_to_l2_v6 == 0)
      {
        vty_out(vty, "  undo import-route isisv6 level-1 into level-2%s", VTY_NEWLINE);
        write++;
      }

      if (area->area_import_to_l1_v6 == 1)
      {
        vty_out(vty, "  route-import isisv6 level-2 into level-1%s", VTY_NEWLINE);
        write++;
      }

      if (listcount(area->summary_ipv6) > 0)
      {
        for (ALL_LIST_ELEMENTS_RO(area->summary_ipv6, node2, summary_ipv6))
        {
          vty_out(vty, "  summary");
          inet_ntop6((void *)&(summary_ipv6->summary_p.prefix), buf, IPV6_ADDR_STRLEN);
          vty_out(vty, " %s %d", buf, summary_ipv6->summary_p.prefixlen);

          if (summary_ipv6->level == 1 || summary_ipv6->level == 2)
            vty_out(vty, " level-%d%s", summary_ipv6->level, VTY_NEWLINE);
          else
            vty_out(vty, " level-1-2%s", VTY_NEWLINE);
          write++;
        }
      }

      /* Authentication passwords. */

#ifdef TOPOLOGY_GENERATE
      if (memcmp(area->topology_baseis, DEFAULT_TOPOLOGY_BASEIS,
                 ISIS_SYS_ID_LEN))
      {
        vty_out(vty, " topology base-is %s%s",
                sysid_print((u_char *)area->topology_baseis), VTY_NEWLINE);
        write++;
      }
      if (area->topology_basedynh)
      {
        vty_out(vty, " topology base-dynh %s%s",
                area->topology_basedynh, VTY_NEWLINE);
        write++;
      }
      /* We save the whole command line here. */
      if (strlen(area->top_params))
      {
        vty_out(vty, " %s%s", area->top_params, VTY_NEWLINE);
        write++;
      }
#endif /* TOPOLOGY_GENERATE */
    }
  }

  return write;
}

struct cmd_node isis_node = {
    ISIS_NODE,
    "%s(config-isis)# ",
    1};

struct cmd_node isis_family_v4_node = {
    ISIS_FAMILY_V4_NODE,
    "%s(config-isis-ipv4)# ",
    1};

struct cmd_node isis_family_v6_node = {
    ISIS_FAMILY_V6_NODE,
    "%s(config-isis-ipv6)# ",
    1};

void isis_init()
{
  install_node(&isis_node, isis_config_write);
  install_node(&isis_family_v4_node, NULL);
  install_node(&isis_family_v6_node, NULL);
  //install_node (&debug_node, config_write_debug);

  install_default(ISIS_NODE);
  install_default(ISIS_FAMILY_V4_NODE);
  install_default(ISIS_FAMILY_V6_NODE);

  install_element(ISIS_NODE, &isis_change_all_cost_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_isis_change_all_cost_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &isis_change_all_cost_v6_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_isis_change_all_cost_v6_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &net_entity_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_net_entity_cmd, CMD_SYNC);

  install_element(CONFIG_NODE, &isis_instance_cmd, CMD_SYNC);
  install_element(CONFIG_NODE, &no_isis_instance_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &level_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_level_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &metric_style_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_metric_style_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_gen_interval_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_gen_interval_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_refresh_interval_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_refresh_interval_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_lifetime_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_lifetime_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_length_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_length_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &spf_interval_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_spf_interval_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &overload_bit_enable_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_overload_bit_enable_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &router_summary_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_router_summary_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &router_summary_ipv6_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_router_summary_ipv6_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V6_NODE, &router_summary_ipv6_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_router_summary_ipv6_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &isis_address_family_ipv4_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &isis_address_family_ipv4_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &isis_address_family_ipv6_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &isis_address_family_ipv6_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &router_import_into_level1_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &router_import_into_level2_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_router_import_into_level1_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_router_import_into_level2_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &area_passwd_md5_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &area_passwd_md5_snpauth_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &area_passwd_clear_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &area_passwd_clear_snpauth_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_area_passwd_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &domain_passwd_md5_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &domain_passwd_md5_snpauth_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &domain_passwd_clear_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &domain_passwd_clear_snpauth_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_domain_passwd_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &net_entity_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_net_entity_h3c_cmd, CMD_SYNC);

  install_element(CONFIG_NODE, &isis_instance_h3c_cmd, CMD_SYNC);
  install_element(CONFIG_NODE, &no_isis_instance_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &level_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_level_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &metric_style_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_metric_style_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_gen_interval_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_gen_interval_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_refresh_interval_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_refresh_interval_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_lifetime_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_lifetime_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &lsp_length_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_lsp_length_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &spf_interval_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_spf_interval_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &overload_bit_enable_h3c_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_overload_bit_enable_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &distance_cmd, CMD_SYNC);
  install_element(ISIS_NODE, &no_distance_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V4_NODE, &ipv4_distance_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &no_ipv4_distance_h3c_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V6_NODE, &ipv6_distance_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_ipv6_distance_h3c_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V4_NODE, &router_summary_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &no_router_summary_h3c_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V4_NODE, &router_import_into_level1_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &router_import_into_level2_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &no_router_import_into_level1_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V4_NODE, &no_router_import_into_level2_h3c_cmd, CMD_SYNC);

  install_element(ISIS_FAMILY_V6_NODE, &router_import_v6_into_level1_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &router_import_v6_into_level2_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_router_import_v6_into_level1_h3c_cmd, CMD_SYNC);
  install_element(ISIS_FAMILY_V6_NODE, &no_router_import_v6_into_level2_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &area_passwd_md5_h3c_cmd, CMD_SYNC);
  //install_element (ISIS_NODE, &area_passwd_md5_snpauth_h3c_cmd,CMD_SYNC);
  install_element(ISIS_NODE, &area_passwd_clear_h3c_cmd, CMD_SYNC);
  //install_element (ISIS_NODE, &area_passwd_clear_snpauth_h3c_cmd,CMD_SYNC);
  install_element(ISIS_NODE, &no_area_passwd_h3c_cmd, CMD_SYNC);

  install_element(ISIS_NODE, &domain_passwd_md5_h3c_cmd, CMD_SYNC);
  //install_element (ISIS_NODE, &domain_passwd_md5_snpauth_h3c_cmd,CMD_SYNC);
  install_element(ISIS_NODE, &domain_passwd_clear_h3c_cmd, CMD_SYNC);
  //install_element (ISIS_NODE, &domain_passwd_clear_snpauth_h3c_cmd,CMD_SYNC);
  install_element(ISIS_NODE, &no_domain_passwd_h3c_cmd, CMD_SYNC);

  /*                           show                      */

  install_element(CONFIG_NODE, &show_isis_interface_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_interface_arg_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_loopback_interface_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_trunk_interface_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_vlanif_interface_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_instance_interface_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_neighbor_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_neighbor_detail_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_instance_neighbor_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_instance_neighbor_detail_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &clear_isis_neighbor_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &clear_isis_neighbor_arg_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_database_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_lsp_arg_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_database_detail_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_database_instance_brief_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_database_instance_detail_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_debug_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_route_cmd, CMD_LOCAL);
  //install_element (CONFIG_NODE,&show_isis_redist_route_cmd);
  install_element(CONFIG_NODE, &show_isis_instance_route_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_route_ipv6_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_interface_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_interface_arg_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_loopback_interface_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_trunk_interface_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_vlanif_interface_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_instance_interface_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_neighbor_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_neighbor_detail_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_instance_neighbor_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_instance_neighbor_detail_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &clear_isis_neighbor_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &clear_isis_neighbor_arg_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_database_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_lsp_arg_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_database_detail_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_database_instance_brief_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_database_instance_detail_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_debug_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_route_cmd, CMD_LOCAL);
  //install_element (CONFIG_NODE,&show_isis_redist_route_cmd);
  install_element(ISIS_NODE, &show_isis_instance_route_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_route_ipv6_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_interface_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_interface_arg_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_loopback_interface_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_trunk_interface_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_vlanif_interface_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_interface_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_neighbor_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_neighbor_detail_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_detail_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &clear_isis_neighbor_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &clear_isis_neighbor_arg_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_database_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_lsp_arg_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_database_detail_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_database_instance_brief_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_database_instance_detail_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_debug_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_route_cmd, CMD_LOCAL);
  //install_element (CONFIG_NODE,&show_isis_redist_route_cmd);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_route_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_route_ipv6_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_interface_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_interface_arg_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_loopback_interface_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_vlanif_interface_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_instance_interface_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_neighbor_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_neighbor_detail_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_instance_neighbor_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_instance_neighbor_detail_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_database_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_database_detail_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_database_instance_brief_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_database_instance_detail_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_route_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_instance_route_h3c_cmd, CMD_LOCAL);
  install_element(CONFIG_NODE, &show_isis_route_ipv6_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &isis_show_timerinfo_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_interface_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_interface_arg_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_loopback_interface_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_vlanif_interface_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_instance_interface_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_neighbor_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_neighbor_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_instance_neighbor_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_instance_neighbor_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_database_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_database_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_database_instance_brief_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_database_instance_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &show_isis_route_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_instance_route_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_route_ipv6_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &isis_show_timerinfo_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_interface_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_interface_arg_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_loopback_interface_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_vlanif_interface_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_interface_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_neighbor_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_neighbor_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_database_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_database_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_database_instance_brief_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_database_instance_detail_h3c_cmd, CMD_LOCAL);

  install_element(ISIS_FAMILY_V4_NODE, &show_isis_route_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_instance_route_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_route_ipv6_h3c_cmd, CMD_LOCAL);

  /*              debug                */
  /*
        install_element (CONFIG_NODE, &debug_isis_packet_dump_cmd,CMD_LOCAL);
        install_element (CONFIG_NODE, &no_debug_isis_packet_dump_cmd,CMD_LOCAL);

        install_element (CONFIG_NODE, &debug_isis_spf_cmd,CMD_LOCAL);
        install_element (CONFIG_NODE, &no_debug_isis_spf_cmd,CMD_LOCAL);

        install_element (CONFIG_NODE, &debug_isis_lsp_cmd,CMD_LOCAL);
        install_element (CONFIG_NODE, &no_debug_isis_lsp_cmd,CMD_LOCAL);

        install_element (CONFIG_NODE, &debug_isis_events_cmd,CMD_LOCAL);
        install_element (CONFIG_NODE, &no_debug_isis_events_cmd,CMD_LOCAL);
    */
  install_element(CONFIG_NODE, &show_isis_statistics_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_statistics_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_statistics_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &show_isis_statistics_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &show_isis_statistics_h3c_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V4_NODE, &show_isis_statistics_h3c_cmd, CMD_LOCAL);

  install_element(CONFIG_NODE, &isis_debug_monitor_cmd, CMD_LOCAL);

  install_element(ISIS_NODE, &isis_bandwidth_reference_cmd, CMD_LOCAL);
  install_element(ISIS_NODE, &undo_isis_bandwidth_reference_cmd, CMD_LOCAL);
  
  install_element(ISIS_FAMILY_V6_NODE, &isis_bandwidth_reference_v6_cmd, CMD_LOCAL);
  install_element(ISIS_FAMILY_V6_NODE, &undo_isis_bandwidth_reference_v6_cmd, CMD_LOCAL);
}
