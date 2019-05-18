/*
 * IS-IS Rout(e)ing protocol - isis_circuit.h
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
#ifdef GNU_LINUX
#include <net/ethernet.h>
#else
#include <netinet/if_ether.h>
#endif

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN ETHERADDRL
#endif

#include "log.h"
#include "memory.h"
#include "if.h"
#include "linklist.h"
#include "command.h"
#include "thread.h"
#include "hash.h"
#include "prefix.h"
#include "stream.h"
#include "vrf.h"

#include "isisd/dict.h"
#include "isisd/include-netbsd/iso.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_tlv.h"
#include "isisd/isis_lsp.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_network.h"
#include "isisd/isis_misc.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_adjacency.h"
#include "isisd/isis_dr.h"
#include "isisd/isisd.h"
#include "isisd/isis_csm.h"
#include "isisd/isis_events.h"

#include "module_id.h"
#include "ifm_common.h"
//#include "if_dl.h"
/*
 * Prototypes.
 */
extern int hello_init(struct isis_circuit *circuit);
extern int isis_zebra_if_address_add(struct ifm_event *pevent, vrf_id_t vrf_id);

int isis_interface_config_write(struct vty *);
int isis_if_new_hook(struct interface *);
int isis_if_delete_hook(struct interface *);

struct isis_circuit *
isis_circuit_conf_default(struct isis_circuit *circuit)
{
  int i;
  /*
   * Default values
   */
  circuit->is_type = IS_LEVEL_1_AND_2;
  circuit->flags = 0;
  circuit->pad_hellos = 1;
  for (i = 0; i < 2; i++)
  {
    circuit->hello_interval[i] = DEFAULT_HELLO_INTERVAL;
    circuit->hello_multiplier[i] = DEFAULT_HELLO_MULTIPLIER;
    circuit->csnp_interval[i] = DEFAULT_CSNP_INTERVAL;
    circuit->psnp_interval[i] = DEFAULT_PSNP_INTERVAL;
    circuit->priority[i] = DEFAULT_PRIORITY;

    circuit->metrics[i].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics[i].metric_expense = METRICS_UNSUPPORTED;
    circuit->metrics[i].metric_error = METRICS_UNSUPPORTED;
    circuit->metrics[i].metric_delay = METRICS_UNSUPPORTED;
    circuit->te_metric[i] = DEFAULT_CIRCUIT_METRIC;

    circuit->metrics_v6[i].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics_v6[i].metric_expense = METRICS_UNSUPPORTED;
    circuit->metrics_v6[i].metric_error = METRICS_UNSUPPORTED;
    circuit->metrics_v6[i].metric_delay = METRICS_UNSUPPORTED;
    circuit->te_metric_v6[i] = DEFAULT_CIRCUIT_METRIC;
  }
  circuit->circ_type = CIRCUIT_T_BROADCAST;
  circuit->circ_type_config = CIRCUIT_T_BROADCAST;
  circuit->is_passive = 0;
  memset(&circuit->passwd, 0, sizeof(struct isis_passwd));

  circuit->rcv_flag = 1;

  return circuit;
}

/*be used in change the interface network*/
struct isis_circuit *
isis_circuit_conf_ago(struct isis_circuit *circuit)
{
  int i;
  /*
	 * Default values
	 */
  //circuit->is_type = IS_LEVEL_1_AND_2;
  circuit->flags = 0;
  circuit->pad_hellos = 1;
  for (i = 0; i < 2; i++)
  {
    circuit->hello_interval[i] = DEFAULT_HELLO_INTERVAL;
    circuit->hello_multiplier[i] = DEFAULT_HELLO_MULTIPLIER;
    circuit->csnp_interval[i] = DEFAULT_CSNP_INTERVAL;
    circuit->psnp_interval[i] = DEFAULT_PSNP_INTERVAL;
    circuit->priority[i] = DEFAULT_PRIORITY;
    //circuit->metrics[i].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics[i].metric_expense = METRICS_UNSUPPORTED;
    circuit->metrics[i].metric_error = METRICS_UNSUPPORTED;
    circuit->metrics[i].metric_delay = METRICS_UNSUPPORTED;

    circuit->metrics_v6[i].metric_expense = METRICS_UNSUPPORTED;
    circuit->metrics_v6[i].metric_error = METRICS_UNSUPPORTED;
    circuit->metrics_v6[i].metric_delay = METRICS_UNSUPPORTED;
    //circuit->te_metric[i] = DEFAULT_CIRCUIT_METRIC;
  }
  circuit->circ_type = CIRCUIT_T_BROADCAST;
  circuit->circ_type_config = CIRCUIT_T_BROADCAST;
  circuit->is_passive = 0;
  memset(&circuit->passwd, 0, sizeof(struct isis_passwd));

  circuit->rcv_flag = 1;

  return circuit;
}

struct isis_circuit *
isis_circuit_new()
{
  struct isis_circuit *circuit;
  int i;

  circuit = XCALLOC(MTYPE_ISIS_CIRCUIT, sizeof(struct isis_circuit));
  
  if (circuit == NULL)
  {
    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Can't malloc isis circuit");
    return NULL;
  }
  memset(circuit , 0 , sizeof(struct isis_circuit));

  /*
   * Default values
   */
  circuit->is_type = IS_LEVEL_1_AND_2;
  circuit->flags = 0;
  circuit->pad_hellos = 1;
  for (i = 0; i < 2; i++)
  {
    circuit->hello_interval[i] = DEFAULT_HELLO_INTERVAL;
    circuit->hello_multiplier[i] = DEFAULT_HELLO_MULTIPLIER;
    circuit->csnp_interval[i] = DEFAULT_CSNP_INTERVAL;
    circuit->psnp_interval[i] = DEFAULT_PSNP_INTERVAL;
    circuit->priority[i] = DEFAULT_PRIORITY;

    circuit->metrics[i].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics[i].metric_expense = METRICS_UNSUPPORTED;
    circuit->metrics[i].metric_error = METRICS_UNSUPPORTED;
    circuit->metrics[i].metric_delay = METRICS_UNSUPPORTED;
    circuit->te_metric[i] = DEFAULT_CIRCUIT_METRIC;

    circuit->metrics_v6[i].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics_v6[i].metric_expense = METRICS_UNSUPPORTED;
    circuit->metrics_v6[i].metric_error = METRICS_UNSUPPORTED;
    circuit->metrics_v6[i].metric_delay = METRICS_UNSUPPORTED;
    circuit->te_metric_v6[i] = DEFAULT_CIRCUIT_METRIC;
  }

  circuit->rcv_flag = 1;

  return circuit;
}

void isis_circuit_del(struct isis_circuit *circuit)
{
  if (!circuit)
    return;

  isis_circuit_if_unbind(circuit, circuit->interface);

  /* and lastly the circuit itself */
  XFREE(MTYPE_ISIS_CIRCUIT, circuit);

  return;
}

struct prefix *
isis_ip_slave_judge(struct isis_circuit *circuit)
{
  struct listnode *pnode = NULL;
  struct connected *ifc = NULL;
  struct prefix *prefix_address = NULL;

  for (ALL_LIST_ELEMENTS_RO(circuit->interface->connected, pnode, ifc))
  {
    if (ifc->flags != ZEBRA_IFA_SLAVE)
    {
      prefix_address = ifc->address;
      return prefix_address;
    }
  }
  zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "-%30s[%d]:ERROR:Ip without slave is not found!!", __FUNCTION__, __LINE__);
  return NULL;
}

void isis_circuit_configure(struct isis_circuit *circuit, struct isis_area *area)
{
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }
  circuit->area = area;
  circuit->rcv_flag = 1;
  /*
   * Whenever the is-type of an area is changed, the is-type of each circuit
   * in that area is updated to a non-empty subset of the area is-type.
   * Inversely, when configuring a new circuit, this property should be
   * ensured as well.
   */
  //if (area->is_type != IS_LEVEL_1_AND_2)
  circuit->is_type = area->is_type;

  /*
   * Add the circuit into area
   */
  listnode_add(area->circuit_list, circuit);

  circuit->idx = flags_get_index(&area->flags);

  return;
}

void isis_circuit_deconfigure(struct isis_circuit *circuit, struct isis_area *area)
{
  /* Free the index of SRM and SSN flags */
  flags_free_index(&area->flags, circuit->idx);
  circuit->idx = 0;
  /* Remove circuit from area */
  //assert (circuit->area == area);

  if (circuit->area != area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }
  listnode_delete(area->circuit_list, circuit);
  circuit->area = NULL;

  return;
}

struct isis_circuit *
circuit_lookup_by_ifp(struct interface *ifp, struct list *list)
{
  struct isis_circuit *circuit = NULL;
  struct listnode *node;

  if (!list)
    return NULL;

  for (ALL_LIST_ELEMENTS_RO(list, node, circuit))
    if (circuit->interface == ifp)
    {
      //assert (ifp->info == circuit);
      if (ifp->info != circuit)
      {
        zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
        return NULL;
      }
      return circuit;
    }

  return NULL;
}

struct isis_circuit *
circuit_scan_by_ifp(struct interface *ifp)
{
  struct isis_area *area;
  struct listnode *node;
  struct isis_circuit *circuit;

  if (ifp->info)
    return (struct isis_circuit *)ifp->info;

  if (isis->area_list)
  {
    for (ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
    {
      circuit = circuit_lookup_by_ifp(ifp, area->circuit_list);
      if (circuit)
        return circuit;
    }
  }
  return circuit_lookup_by_ifp(ifp, isis->init_circ_list);
}

static struct isis_circuit *
isis_circuit_lookup(struct vty *vty)
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  char ifname_tmp[INTERFACE_NAMSIZ];
  uint32_t ifindex;
  vrf_id_t vrf_id = VRF_DEFAULT;

  //ifp = (struct interface *) vty->index;
  ifindex = (uint32_t)vty->index;

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp,
                               strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                               vrf_id);
  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return NULL;
  }

  circuit = circuit_scan_by_ifp(ifp);
  if (!circuit)
  {
    //vty_out (vty, "ISIS is not enabled on circuit %s%s",
    //ifp->name, VTY_NEWLINE);
    return NULL;
  }

  return circuit;
}

void isis_circuit_add_addr(struct isis_circuit *circuit,
                           struct connected *connected)
{
  struct listnode *node;
  struct prefix_ipv4 *ipv4;
  char buf[BUFSIZ];
  memset(&buf, 0, BUFSIZ);

  struct prefix_ipv6 *ipv6;

  /*if ipv4,add the ip to circuit->ip_addrs */
  if (connected->address->family == AF_INET)
  {
    u_int32_t addr = connected->address->u.prefix4.s_addr;
    addr = ntohl(addr);
    if (IPV4_NET0(addr) ||
        IPV4_NET127(addr) ||
        IN_CLASSD(addr) ||
        IPV4_LINKLOCAL(addr))
      return;

    /*whether the ip exit already*/
    for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, node, ipv4))
    {
      if (prefix_same((struct prefix *)ipv4, connected->address))
        return;
    }

    ipv4 = prefix_ipv4_new();
    ipv4->prefixlen = connected->address->prefixlen;
    ipv4->prefix = connected->address->u.prefix4;
    listnode_add(circuit->ip_addrs, ipv4);
    if (circuit->area)
    {
      /*here we should send a hello forcible*/
      if (circuit->is_type & IS_LEVEL_1)
        send_hello(circuit, IS_LEVEL_1);
      if (circuit->is_type & IS_LEVEL_2)
        send_hello(circuit, IS_LEVEL_2);

      lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);
    }

    prefix2str(connected->address, buf, BUFSIZ);
    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Added IP address %s to circuit %d", buf,
               circuit->circuit_id);
  }

  else if (connected->address->family == AF_INET6)
  {
    if (IN6_IS_ADDR_LOOPBACK(&connected->address->u.prefix6))
      return;

    for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_link, node, ipv6))
      if (prefix_same((struct prefix *)ipv6, connected->address))
        return;

    for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_non_link, node, ipv6))
      if (prefix_same((struct prefix *)ipv6, connected->address))
        return;

    ipv6 = prefix_ipv6_new();
    ipv6->prefixlen = connected->address->prefixlen;
    ipv6->prefix = connected->address->u.prefix6;

    if (IN6_IS_ADDR_LINKLOCAL(&ipv6->prefix))
    {
      listnode_add(circuit->ipv6_link, ipv6);
    }
    else
    {
      listnode_add(circuit->ipv6_non_link, ipv6);
    }
    if (circuit->area)
    {
      lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);
    }

    prefix2str(connected->address, buf, BUFSIZ);
    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Added IPv6 address %s to circuit %d", buf,
               circuit->circuit_id);
  }
  return;
}

void isis_circuit_del_addr(struct isis_circuit *circuit,
                           struct connected *connected)
{
  struct prefix_ipv4 *ipv4, *ip = NULL;
  struct listnode *node;
  u_char buf[BUFSIZ];

  struct prefix_ipv6 *ipv6, *ip6 = NULL;
  int found = 0;

  memset(&buf, 0, BUFSIZ);

  /*for ipv4*/
  if (connected->address->family == AF_INET)
  {
    ipv4 = prefix_ipv4_new();
    ipv4->prefixlen = connected->address->prefixlen;
    ipv4->prefix = connected->address->u.prefix4;

    for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, node, ip))
    {
      if (prefix_same((struct prefix *)ip, (struct prefix *)ipv4))
        break;
    }

    if (ip)
    {
      listnode_delete(circuit->ip_addrs, ip);
      if (circuit->area)
        lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);
    }

    else
    {
      prefix2str(connected->address, (char *)buf, BUFSIZ);
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Nonexitant ip address %s removal attempt from \
                      circuit %d",
                 buf, circuit->circuit_id);
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Current ip addresses on %s:", circuit->interface->name);
      for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, node, ip))
      {
        prefix2str((struct prefix *)ip, (char *)buf, BUFSIZ);
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "  %s", buf);
      }
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "End of addresses");
    }

    prefix_ipv4_free(ipv4);
  }

  else if (connected->address->family == AF_INET6)
  {
    ipv6 = prefix_ipv6_new();
    ipv6->prefixlen = connected->address->prefixlen;
    ipv6->prefix = connected->address->u.prefix6;

    if (IN6_IS_ADDR_LINKLOCAL(&ipv6->prefix))
    {
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_link, node, ip6))
      {
        if (prefix_same((struct prefix *)ip6, (struct prefix *)ipv6))
          break;
      }
      if (ip6)
      {
        listnode_delete(circuit->ipv6_link, ip6);
        found = 1;
      }
    }
    else
    {
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_non_link, node, ip6))
      {
        if (prefix_same((struct prefix *)ip6, (struct prefix *)ipv6))
          break;
      }
      if (ip6)
      {
        listnode_delete(circuit->ipv6_non_link, ip6);
        found = 1;
      }
    }

    if (!found)
    {
      prefix2str(connected->address, (char *)buf, BUFSIZ);
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Nonexitant ip address %s removal attempt from \
		      circuit %d",
                 buf, circuit->circuit_id);
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "Current ip addresses on %s:", circuit->interface->name);
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_link, node, ip6))
      {
        prefix2str((struct prefix *)ip6, (char *)buf, BUFSIZ);
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "  %s", buf);
      }
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, " -----");
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_non_link, node, ip6))
      {
        prefix2str((struct prefix *)ip6, (char *)buf, BUFSIZ);
        zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "  %s", buf);
      }
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "End of addresses");
    }
    else if (circuit->area)
      lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);

    prefix_ipv6_free(ipv6);
  }

  return;
}

static u_char
isis_circuit_id_gen(struct interface *ifp)
{
  u_char id = 0;
  //char ifname[16];
  //unsigned int i;
  //int start = -1, end = -1;

  /*
   * Get a stable circuit id from ifname. This makes
   * the ifindex from flapping when netdevs are created
   * and deleted on the fly. Note that this circuit id
   * is used in pseudo lsps so it is better to be stable.
   * The following code works on any reasonanle ifname
   * like: eth 0/0/1 or trk-1.1 etc.
   */
#if 0
  for (i = 0; i < strlen (ifp->name); i++)
  {
      if (isdigit((unsigned char)ifp->name[i]))
      {
          if (start <= 0)
          {
              start = i;
              end = i + 1;
          }
          else
          {
              end = i + 1;
          }
      }
      else if (start > 0)
        break;
  }

#endif
  if (IFM_IS_SUBPORT(ifp->ifindex))
  {
    id = (u_char)(IFM_PORT_ID_GET(ifp->ifindex) + IFM_SUBPORT_ID_GET(ifp->ifindex) + 80);
  }
  else
  {
    id = (u_char)(IFM_PORT_ID_GET(ifp->ifindex) + 1);
  }

  //  if ((start >= 0) && (end >= start) && (end - start) < 16)
  //  {
  //      memset (ifname, 0, 16);
  //      strncpy (ifname, &ifp->name[start], end - start);
  //      id = (u_char)atoi(ifname);
  //  }

  /* Try to be unique. */
  if (!id)
    id = (u_char)((ifp->ifindex & 0xff) | 0x80);

  return id;
}

void isis_circuit_if_add(struct isis_circuit *circuit, struct interface *ifp)
{
  struct listnode *node, *nnode;
  struct connected *conn;

  circuit->circuit_id = isis_circuit_id_gen(ifp);

  isis_circuit_if_bind(circuit, ifp);
  /*  isis_circuit_update_addrs (circuit, ifp); */

  if (if_is_broadcast(ifp))
  {
    if (circuit->circ_type_config == CIRCUIT_T_P2P)
      circuit->circ_type = CIRCUIT_T_P2P;
    else
      circuit->circ_type = CIRCUIT_T_BROADCAST;
  }
  else if (if_is_pointopoint(ifp))
  {
    circuit->circ_type = CIRCUIT_T_P2P;
  }
  else if (if_is_loopback(ifp))
  {
    circuit->circ_type = CIRCUIT_T_LOOPBACK;
    circuit->is_passive = 1;
  }
  else
  {
    /* It's normal in case of loopback etc. */
    //if (isis->debugs & DEBUG_EVENTS|| isis->debugs & ISIS_DEBUG_TYPE_ALL)
    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "isis_circuit_if_add: unsupported media");
    circuit->circ_type = CIRCUIT_T_UNKNOWN;
  }

  circuit->ip_addrs = list_new();
  circuit->ipv6_link = list_new();
  circuit->ipv6_non_link = list_new();

  for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, conn))
  {
    isis_circuit_add_addr(circuit, conn);
  }

  return;
}

void isis_circuit_if_del(struct isis_circuit *circuit, struct interface *ifp)
{
  struct listnode *node, *nnode;
  struct connected *conn;

  //assert (circuit->interface == ifp);
  if (circuit->interface != ifp)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  /* destroy addresses */
  for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, conn))
    isis_circuit_del_addr(circuit, conn);

  if (circuit->ip_addrs)
  {
    //assert (listcount(circuit->ip_addrs) == 0);
    if (listcount(circuit->ip_addrs) != 0)
    {
      zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
      return;
    }
    list_delete(circuit->ip_addrs);
    circuit->ip_addrs = NULL;
  }

#if 1
  if (circuit->ipv6_link)
  {
    //assert (listcount(circuit->ipv6_link) == 0);
    if (listcount(circuit->ipv6_link) != 0)
    {
      zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
      return;
    }

    list_delete(circuit->ipv6_link);
    circuit->ipv6_link = NULL;
  }

  if (circuit->ipv6_non_link)
  {
    //assert (listcount(circuit->ipv6_non_link) == 0);
    if (listcount(circuit->ipv6_non_link) != 0)
    {
      zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
      return;
    }

    list_delete(circuit->ipv6_non_link);
    circuit->ipv6_non_link = NULL;
  }
#endif /* 0 */

  circuit->circ_type = CIRCUIT_T_UNKNOWN;
  circuit->circuit_id = 0;

  return;
}

void isis_circuit_if_bind(struct isis_circuit *circuit, struct interface *ifp)
{

  //assert (circuit != NULL);
  if ((circuit == NULL))
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  //assert (ifp != NULL);
  if (ifp == NULL)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  if (circuit->interface)
  { //assert (circuit->interface == ifp);
    if (circuit->interface != ifp)
    {
      zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
      return;
    }
  }
  else
    circuit->interface = ifp;
  if (ifp->info)
  {
    //assert (ifp->info == circuit);
    if (ifp->info != circuit)
    {
      zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
      return;
    }
  }
  else
    ifp->info = circuit;
}

void isis_circuit_if_unbind(struct isis_circuit *circuit, struct interface *ifp)
{
  //assert (circuit != NULL);
  if (circuit == NULL)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  //assert (ifp != NULL);
  if (ifp == NULL)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  //assert (circuit->interface == ifp);
  if (circuit->interface != ifp)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  //assert (ifp->info == circuit);
  if (ifp->info != circuit)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  circuit->interface = NULL;
  ifp->info = NULL;
}

static void
isis_circuit_update_all_srmflags(struct isis_circuit *circuit, int is_set)
{
  struct isis_area *area;
  struct isis_lsp *lsp;
  dnode_t *dnode, *dnode_next;
  int level;

  //assert (circuit);
  if (!circuit)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  area = circuit->area;
  //assert (area);
  if (!area)
  {
    zlog_err("%-20s[%d] error: ", __FUNCTION__, __LINE__);
    return;
  }

  for (level = ISIS_LEVEL1; level <= ISIS_LEVEL2; level++)
  {
    if (level & circuit->is_type)
    {
      if (area->lspdb[level - 1] &&
          dict_count(area->lspdb[level - 1]) > 0)
      {
        for (dnode = dict_first(area->lspdb[level - 1]);
             dnode != NULL; dnode = dnode_next)
        {
          dnode_next = dict_next(area->lspdb[level - 1], dnode);
          lsp = dnode_get(dnode);
          if (is_set)
          {
            ISIS_SET_FLAG(lsp->SRMflags, circuit);
          }
          else
          {
            ISIS_CLEAR_FLAG(lsp->SRMflags, circuit);
          }
        }
      }
    }
  }
}

size_t
isis_circuit_pdu_size(struct isis_circuit *circuit)
{
  return ISO_MTU(circuit);
}

void isis_circuit_stream(struct isis_circuit *circuit, struct stream **stream)
{
  size_t stream_size = isis_circuit_pdu_size(circuit);

  if (!*stream)
  {
    *stream = stream_new(stream_size);
  }
  else
  {
    if (STREAM_SIZE(*stream) != stream_size)
      stream_resize(*stream, stream_size);
    stream_reset(*stream);
  }
}

int isis_circuit_up(struct isis_circuit *circuit)
{
  int retv;

  /* Set the flags for all the lsps of the circuit. */
  isis_circuit_update_all_srmflags(circuit, 1);

  if (circuit->state == C_STATE_UP)
    return ISIS_OK;

  if (circuit->is_passive)
    return ISIS_OK;

  if (circuit->area->lsp_mtu > isis_circuit_pdu_size(circuit))
  {
    zlog_err("Interface MTU %zu on %s is too low to support area lsp mtu %u!",
             isis_circuit_pdu_size(circuit), circuit->interface->name,
             circuit->area->lsp_mtu);
    isis_circuit_down(circuit);
    return ISIS_DISMATCH;
  }
  if (circuit->is_type & IS_LEVEL_1)
  {
    if (circuit->same_sys_psnp[0] == NULL)
    {
      circuit->same_sys_psnp[0] = list_new();
    }
  }

  if (circuit->is_type & IS_LEVEL_2)
  {
    if (circuit->same_sys_psnp[1] == NULL)
    {
      circuit->same_sys_psnp[1] = list_new();
    }
  }

  if (circuit->circ_type == CIRCUIT_T_BROADCAST)
  {
    /*
       * Get the Hardware Address
       */
    if (circuit->interface->hw_addr_len != ETH_ALEN)
    {
      zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "unsupported link layer");
    }
    else
    {
      memcpy(circuit->u.bc.snpa, circuit->interface->hw_addr, ETH_ALEN);
    }

    zlog_debug(ISIS_DEBUG_TYPE_EVENTS, "isis_circuit_if_add: if_id %d, isomtu %d snpa %s",
               circuit->interface->ifindex, ISO_MTU(circuit),
               snpa_print(circuit->u.bc.snpa));

    circuit->u.bc.adjdb[0] = list_new();
    circuit->u.bc.adjdb[1] = list_new();

    /*
       * ISO 10589 - 8.4.1 Enabling of broadcast circuits
       */

    /* initilizing the hello sending threads
       * for a broadcast IF
       */

    /* 8.4.1 a) commence sending of IIH PDUs */

    if (circuit->is_type & IS_LEVEL_1)
    {
      //thread_add_event (master, send_lan_l1_hello, circuit, 0);
      //send_lan_l1_hello(circuit);
      THREAD_TIMER_MSEC_ON_NEW((char *)"ISISSendL1Hello", circuit->u.bc.t_send_lan_hello[0],
                               send_lan_l1_hello, circuit, 500);

      circuit->u.bc.lan_neighs[0] = list_new();
    }

    if (circuit->is_type & IS_LEVEL_2)
    {
      //thread_add_event (master, send_lan_l2_hello, circuit, 0);
      //send_lan_l2_hello(circuit);
      THREAD_TIMER_MSEC_ON_NEW((char *)"ISISSendL2Hello", circuit->u.bc.t_send_lan_hello[1],
                               send_lan_l2_hello, circuit, 500);

      circuit->u.bc.lan_neighs[1] = list_new();
    }

    /* 8.4.1 b) FIXME: solicit ES - 8.4.6 */
    /* 8.4.1 c) FIXME: listen for ESH PDUs */

    /* 8.4.1 d) */
    /* dr election will commence in... */
    if (circuit->is_type & IS_LEVEL_1)
    {
      THREAD_TIMER_ON_NEW((char *)"ISISRunL1Dr", circuit->u.bc.t_run_dr[0], isis_run_dr_l1,
                          circuit, 2 * circuit->hello_interval[0]);
    }
    if (circuit->is_type & IS_LEVEL_2)
    {
      THREAD_TIMER_ON_NEW((char *)"ISISRunL2Dr", circuit->u.bc.t_run_dr[1], isis_run_dr_l2,
                          circuit, 2 * circuit->hello_interval[1]);
    }
  }
  else
  {
    /* initializing the hello send threads
       * for a ptp IF
       */
    circuit->u.p2p.neighbor = NULL;
    //thread_add_event (master, send_p2p_hello, circuit, 0);
    //send_p2p_hello(circuit);
    THREAD_TIMER_MSEC_ON_NEW((char *)"ISISSendP2PHello", circuit->u.p2p.t_send_p2p_hello, send_p2p_hello, circuit, 500);
  }

  /* initializing PSNP timers */
  if (circuit->is_type & IS_LEVEL_1)
  {
    THREAD_TIMER_ON_NEW((char *)"ISISSendL1Psnp", circuit->t_send_psnp[0], send_l1_psnp, circuit,
                        isis_jitter(circuit->psnp_interval[0], PSNP_JITTER));
  }

  if (circuit->is_type & IS_LEVEL_2)
  {
    THREAD_TIMER_ON_NEW((char *)"ISISSendL2Psnp", circuit->t_send_psnp[1], send_l2_psnp, circuit,
                        isis_jitter(circuit->psnp_interval[1], PSNP_JITTER));
  }

  /* unified init for circuits; ignore warnings below this level */
  //retv = isis_sock_init (circuit);

  retv = hello_init(circuit);
  if (retv != ISIS_OK)
  {
    isis_circuit_down(circuit);
    return retv;
  }

  /* initialize the circuit streams after opening connection */
  isis_circuit_stream(circuit, &circuit->rcv_stream);
  isis_circuit_stream(circuit, &circuit->snd_stream);

  circuit->lsp_queue = list_new();
  circuit->lsp_queue_last_cleared = time(NULL);

  return ISIS_OK;
}

void isis_circuit_down(struct isis_circuit *circuit)
{
  if (circuit->state != C_STATE_UP)
    return;

  circuit->rcv_flag = 0;

  /* Clear the flags for all the lsps of the circuit. */
  isis_circuit_update_all_srmflags(circuit, 0);

  if (circuit->circ_type == CIRCUIT_T_BROADCAST)
  {
    /* destroy neighbour lists */
    if (circuit->u.bc.lan_neighs[0])
    {
      list_delete(circuit->u.bc.lan_neighs[0]);
      circuit->u.bc.lan_neighs[0] = NULL;
    }
    if (circuit->u.bc.lan_neighs[1])
    {
      list_delete(circuit->u.bc.lan_neighs[1]);
      circuit->u.bc.lan_neighs[1] = NULL;
    }
    /* destroy adjacency databases */
    if (circuit->u.bc.adjdb[0])
    {
      circuit->u.bc.adjdb[0]->del = isis_delete_adj;
      list_delete(circuit->u.bc.adjdb[0]);
      circuit->u.bc.adjdb[0] = NULL;
    }
    if (circuit->u.bc.adjdb[1])
    {
      circuit->u.bc.adjdb[1]->del = isis_delete_adj;
      list_delete(circuit->u.bc.adjdb[1]);
      circuit->u.bc.adjdb[1] = NULL;
    }
    if (circuit->u.bc.is_dr[0])
    {
      isis_dr_resign(circuit, 1);
      circuit->u.bc.is_dr[0] = 0;
    }
    memset(circuit->u.bc.l1_desig_is, 0, ISIS_SYS_ID_LEN + 1);
    if (circuit->u.bc.is_dr[1])
    {
      isis_dr_resign(circuit, 2);
      circuit->u.bc.is_dr[1] = 0;
    }
    memset(circuit->u.bc.l2_desig_is, 0, ISIS_SYS_ID_LEN + 1);
    memset(circuit->u.bc.snpa, 0, ETH_ALEN);
	memset(circuit->u.bc.run_dr_elect, 0, sizeof(char) * 2);

    THREAD_TIMER_OFF_NEW(circuit->u.bc.t_send_lan_hello[0]);
    circuit->u.bc.t_send_lan_hello[0] = 0;
    THREAD_TIMER_OFF_NEW(circuit->u.bc.t_send_lan_hello[1]);
    circuit->u.bc.t_send_lan_hello[1] = 0;
    THREAD_TIMER_OFF_NEW(circuit->u.bc.t_run_dr[0]);
    circuit->u.bc.t_run_dr[0] = 0;
    THREAD_TIMER_OFF_NEW(circuit->u.bc.t_run_dr[1]);
    circuit->u.bc.t_run_dr[1] = 0;
    THREAD_TIMER_OFF_NEW(circuit->u.bc.t_refresh_pseudo_lsp[0]);
    circuit->u.bc.t_refresh_pseudo_lsp[0] = 0;
    THREAD_TIMER_OFF_NEW(circuit->u.bc.t_refresh_pseudo_lsp[1]);
    circuit->u.bc.t_refresh_pseudo_lsp[1] = 0;

    circuit->lsp_regenerate_pending[0] = 0;
    circuit->lsp_regenerate_pending[1] = 0;
  }
  else if (circuit->circ_type == CIRCUIT_T_P2P)
  {
    isis_delete_adj(circuit->u.p2p.neighbor);
    circuit->u.p2p.neighbor = NULL;
	THREAD_TIMER_OFF_NEW(circuit->u.p2p.t_send_p2p_hello);
	circuit->u.p2p.t_send_p2p_hello = 0;
  }

  /* Cancel all active threads */
  THREAD_TIMER_OFF_NEW(circuit->t_send_csnp[0]);
  circuit->t_send_csnp[0] = 0;
  THREAD_TIMER_OFF_NEW(circuit->t_send_csnp[1]);
  circuit->t_send_csnp[1] = 0;
  THREAD_TIMER_OFF_NEW(circuit->t_send_psnp[0]);
  circuit->t_send_psnp[0] = 0;
  THREAD_TIMER_OFF_NEW(circuit->t_send_psnp[1]);
  circuit->t_send_psnp[1] = 0;

  if (circuit->same_sys_psnp[0])
  {
    list_delete(circuit->same_sys_psnp[0]);
    circuit->same_sys_psnp[0] = NULL;
  }

  if (circuit->same_sys_psnp[1])
  {
    list_delete(circuit->same_sys_psnp[1]);
    circuit->same_sys_psnp[1] = NULL;
  }
  //THREAD_OFF (circuit->t_read);

  if (circuit->lsp_queue)
  {
    circuit->lsp_queue->del = NULL;
    list_delete(circuit->lsp_queue);
    circuit->lsp_queue = NULL;
  }

  /* send one gratuitous hello to spead up convergence */
  if (circuit->is_type & IS_LEVEL_1)
    send_hello(circuit, IS_LEVEL_1);
  if (circuit->is_type & IS_LEVEL_2)
    send_hello(circuit, IS_LEVEL_2);

  circuit->upadjcount[0] = 0;
  circuit->upadjcount[1] = 0;

  /* close the socket */
  //if (circuit->fd)
  //  {
  //     close (circuit->fd);
  //    circuit->fd = 0;
  //   }

  if (circuit->rcv_stream != NULL)
  {
    stream_free(circuit->rcv_stream);
    circuit->rcv_stream = NULL;
  }

  if (circuit->snd_stream != NULL)
  {
    stream_free(circuit->snd_stream);
    circuit->snd_stream = NULL;
  }

  thread_cancel_event(master, circuit);

  return;
}

void circuit_update_nlpids(struct isis_circuit *circuit)
{
  circuit->nlpids.count = 0;

  if (circuit->ip_router)
  {
    circuit->nlpids.nlpids[0] = NLPID_IP;
    circuit->nlpids.count++;
  }
#if 1
  if (circuit->ipv6_router)
  {
    circuit->nlpids.nlpids[circuit->nlpids.count] = NLPID_IPV6;
    circuit->nlpids.count++;
  }
#endif /* 0 */

  return;
}

void isis_circuit_print_vty(struct isis_circuit *circuit, struct vty *vty,
                            char detail)
{
  if (detail == ISIS_UI_LEVEL_BRIEF)
  {
    vty_out(vty, "  %-12s", circuit->interface->name);
    vty_out(vty, "0x%-7x", circuit->circuit_id);
    vty_out(vty, "%-9s", circuit_state2string(circuit->state));
    vty_out(vty, "%-9s", circuit_type2string(circuit->circ_type));
    if (circuit->circ_type != CIRCUIT_T_P2P)
      vty_out(vty, "%-5d", (circuit->interface->mtu - 3));
    else
      vty_out(vty, "%-5d", circuit->interface->mtu);
    vty_out(vty, "%-9s", circuit_t2string(circuit->is_type));
    vty_out(vty, "%s", VTY_NEWLINE);
  }

  if (detail == ISIS_UI_LEVEL_DETAIL)
  {
    struct listnode *node;
    struct prefix *ip_addr;
    u_char buf[BUFSIZ];

    vty_out(vty, "  Interface: %s", circuit->interface->name);
    vty_out(vty, ", State: %s", circuit_state2string(circuit->state));
    if (circuit->is_passive)
      vty_out(vty, ", Passive");
    else
      vty_out(vty, ", Active");
    vty_out(vty, ", Circuit Id: 0x%x", circuit->circuit_id);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "    Type: %s ", circuit_type2string(circuit->circ_type));
    if (circuit->circ_type != CIRCUIT_T_P2P)
      vty_out(vty, "MTU:%-5d", circuit->interface->mtu - 3);
    else
      vty_out(vty, "MTU:%-5d", circuit->interface->mtu);
    vty_out(vty, ", Level: %s", circuit_t2string(circuit->is_type));
    if (circuit->circ_type == CIRCUIT_T_BROADCAST)
      vty_out(vty, ", SNPA: %-10s", snpa_print(circuit->u.bc.snpa));
    vty_out(vty, "%s", VTY_NEWLINE);
    if (circuit->is_type & IS_LEVEL_1)
    {
      vty_out(vty, "    Level-1 Information:%s", VTY_NEWLINE);
      if (circuit->area->newmetric)
        vty_out(vty, "      Metric: %d", circuit->te_metric[0]);
      else
        vty_out(vty, "      Metric: %d", circuit->metrics[0].metric_default);

      if (circuit->area->newmetric)
        vty_out(vty, "      Metric_v6: %d", circuit->te_metric_v6[0]);
      else
        vty_out(vty, "      Metric_v6: %d", circuit->metrics_v6[0].metric_default);

      if (!circuit->is_passive)
      {
        //vty_out (vty, ", Active neighbors: %u%s",
        //circuit->upadjcount[0], VTY_NEWLINE);
        vty_out(vty, "      Hello interval: %u, "
                     "Hello Multiplier: %u %s%s",
                circuit->hello_interval[0],
                circuit->hello_multiplier[0],
                (circuit->pad_hellos ? "(pad)" : "(no-pad)"),
                VTY_NEWLINE);
        vty_out(vty, "      CNSP interval: %u, "
                     "PSNP interval: %u%s",
                circuit->csnp_interval[0],
                circuit->psnp_interval[0], VTY_NEWLINE);
        if (circuit->circ_type == CIRCUIT_T_BROADCAST)
          vty_out(vty, "      LAN Priority: %u, %s%s",
                  circuit->priority[0],
                  (circuit->u.bc.is_dr[0] ? "is DIS" : "is not DIS"), VTY_NEWLINE);
      }
      else
      {
        vty_out(vty, "%s", VTY_NEWLINE);
      }
    }
    if (circuit->is_type & IS_LEVEL_2)
    {
      vty_out(vty, "    Level-2 Information:%s", VTY_NEWLINE);
      if (circuit->area->newmetric)
        vty_out(vty, "      Metric: %d", circuit->te_metric[1]);
      else
        vty_out(vty, "      Metric: %d", circuit->metrics[1].metric_default);

      if (circuit->area->newmetric)
        vty_out(vty, "      Metric_v6: %d", circuit->te_metric_v6[1]);
      else
        vty_out(vty, "      Metric_v6: %d", circuit->metrics_v6[1].metric_default);

      if (!circuit->is_passive)
      {
        //vty_out (vty, ", Active neighbors: %u%s",
        //circuit->upadjcount[1], VTY_NEWLINE);
        vty_out(vty, "      Hello interval: %u, "
                     "Hello Multiplier: %u %s%s",
                circuit->hello_interval[1],
                circuit->hello_multiplier[1],
                (circuit->pad_hellos ? "(pad)" : "(no-pad)"),
                VTY_NEWLINE);
        vty_out(vty, "      CNSP interval: %u, "
                     "PSNP interval: %u%s",
                circuit->csnp_interval[1],
                circuit->psnp_interval[1], VTY_NEWLINE);
        if (circuit->circ_type == CIRCUIT_T_BROADCAST)
          vty_out(vty, "      LAN Priority: %u, %s%s",
                  circuit->priority[1],
                  (circuit->u.bc.is_dr[1] ? "is DIS" : "is not DIS"), VTY_NEWLINE);
      }
      else
      {
        vty_out(vty, "%s", VTY_NEWLINE);
      }
    }
    if (circuit->ip_addrs && listcount(circuit->ip_addrs) > 0)
    {
      vty_out(vty, "    IP Prefix(es):%s", VTY_NEWLINE);
      for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, node, ip_addr))
      {
        struct in_addr ip_addr_1;
        u_char ip4[INET_ADDRSTRLEN];
        ip_addr_1.s_addr = ip_addr->u.prefix4.s_addr;
        ip_addr_1.s_addr = htonl(ip_addr_1.s_addr);
        inet_ntop(AF_INET, &ip_addr_1, (char *)ip4, INET_ADDRSTRLEN);

        vty_out(vty, "      %s%s", ip4, VTY_NEWLINE);
      }
    }
#if 1
    if (circuit->ipv6_link && listcount(circuit->ipv6_link) > 0)
    {
      vty_out(vty, "    IPv6 Link-Locals:%s", VTY_NEWLINE);
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_link, node, ip_addr))
      {
        prefix2str(ip_addr, (char *)buf, BUFSIZ),
            vty_out(vty, "      %s%s", buf, VTY_NEWLINE);
      }
    }
    if (circuit->ipv6_non_link && listcount(circuit->ipv6_non_link) > 0)
    {
      vty_out(vty, "    IPv6 Prefixes:%s", VTY_NEWLINE);
      for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_non_link, node, ip_addr))
      {
        prefix2str(ip_addr, (char *)buf, BUFSIZ),
            vty_out(vty, "      %s%s", buf, VTY_NEWLINE);
      }
    }
#endif

    vty_out(vty, "%s", VTY_NEWLINE);
  }
  return;
}

int isis_interface_config_write(struct vty *vty)
{
  int write = 0;
  struct listnode *node, *node2;
  struct interface *ifp;
  struct isis_area *area;
  struct isis_circuit *circuit;
  int i;

  for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
  {
    /* IF name */
    vty_out(vty, "interface %s%s", ifp->name, VTY_NEWLINE);
    write++;
    /* IF desc */
    if (ifp->desc)
    {
      vty_out(vty, " description %s%s", ifp->desc, VTY_NEWLINE);
      write++;
    }
    /* ISIS Circuit */
    for (ALL_LIST_ELEMENTS_RO(isis->area_list, node2, area))
    {
      circuit = circuit_lookup_by_ifp(ifp, area->circuit_list);
      if (circuit == NULL)
        continue;
      if (circuit->ip_router)
      {
        vty_out(vty, " isis enable instance %s%s", area->area_tag,
                VTY_NEWLINE);
        write++;
      }
      if (circuit->ipv6_router)
      {
        vty_out(vty, " isis enable ipv6 instance %s%s", area->area_tag,
                VTY_NEWLINE);
        write++;
      }
      if (circuit->is_passive)
      {
        vty_out(vty, " isis passive%s", VTY_NEWLINE);
        write++;
      }
      if (circuit->circ_type_config == CIRCUIT_T_P2P)
      {
        vty_out(vty, " isis network p2p%s", VTY_NEWLINE);
        write++;
      }

      /* ISIS - circuit type */
      if (circuit->is_type == IS_LEVEL_1)
      {
        vty_out(vty, " isis circuit-type level-1%s", VTY_NEWLINE);
        write++;
      }
      else
      {
        if (circuit->is_type == IS_LEVEL_2)
        {
          vty_out(vty, " isis circuit-type level-2%s",
                  VTY_NEWLINE);
          write++;
        }
      }

      /* ISIS - CSNP interval */
      if (circuit->csnp_interval[0] == circuit->csnp_interval[1])
      {
        if (circuit->csnp_interval[0] != DEFAULT_CSNP_INTERVAL)
        {
          vty_out(vty, " isis csnp-interval %d%s",
                  circuit->csnp_interval[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        for (i = 0; i < 2; i++)
        {
          if (circuit->csnp_interval[i] != DEFAULT_CSNP_INTERVAL)
          {
            vty_out(vty, " isis csnp-interval %d level-%d%s",
                    circuit->csnp_interval[i], i + 1, VTY_NEWLINE);
            write++;
          }
        }
      }

      /* ISIS - PSNP interval */
      if (circuit->psnp_interval[0] == circuit->psnp_interval[1])
      {
        if (circuit->psnp_interval[0] != DEFAULT_PSNP_INTERVAL)
        {
          vty_out(vty, " isis psnp-interval %d%s",
                  circuit->psnp_interval[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        for (i = 0; i < 2; i++)
        {
          if (circuit->psnp_interval[i] != DEFAULT_PSNP_INTERVAL)
          {
            vty_out(vty, " isis psnp-interval %d level-%d%s",
                    circuit->psnp_interval[i], i + 1, VTY_NEWLINE);
            write++;
          }
        }
      }

      /* ISIS - Hello padding - Defaults to true so only display if false */
      if (circuit->pad_hellos == 0)
      {
        vty_out(vty, " no hello padding enable%s", VTY_NEWLINE);
        write++;
      }

      /* ISIS - Hello interval */
      if (circuit->hello_interval[0] == circuit->hello_interval[1])
      {
        if (circuit->hello_interval[0] != DEFAULT_HELLO_INTERVAL)
        {
          vty_out(vty, " isis hello-interval %d%s",
                  circuit->hello_interval[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        for (i = 0; i < 2; i++)
        {
          if (circuit->hello_interval[i] != DEFAULT_HELLO_INTERVAL)
          {
            vty_out(vty, " isis hello-interval %d level-%d%s",
                    circuit->hello_interval[i], i + 1, VTY_NEWLINE);
            write++;
          }
        }
      }

      /* ISIS - Hello Multiplier */
      if (circuit->hello_multiplier[0] == circuit->hello_multiplier[1])
      {
        if (circuit->hello_multiplier[0] != DEFAULT_HELLO_MULTIPLIER)
        {
          vty_out(vty, " isis hello-multiplier %d%s",
                  circuit->hello_multiplier[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        for (i = 0; i < 2; i++)
        {
          if (circuit->hello_multiplier[i] != DEFAULT_HELLO_MULTIPLIER)
          {
            vty_out(vty, " isis hello-multiplier %d level-%d%s",
                    circuit->hello_multiplier[i], i + 1,
                    VTY_NEWLINE);
            write++;
          }
        }
      }

      /* ISIS - Priority */
      if (circuit->priority[0] == circuit->priority[1])
      {
        if (circuit->priority[0] != DEFAULT_PRIORITY)
        {
          vty_out(vty, " isis priority %d%s",
                  circuit->priority[0], VTY_NEWLINE);
          write++;
        }
      }
      else
      {
        for (i = 0; i < 2; i++)
        {
          if (circuit->priority[i] != DEFAULT_PRIORITY)
          {
            vty_out(vty, " isis priority %d level-%d%s",
                    circuit->priority[i], i + 1, VTY_NEWLINE);
            write++;
          }
        }
      }

      /* ISIS - Metric */
      if (circuit->area && circuit->ipv4_auto_cost_flag == 0)
      {
        if (circuit->te_metric[0] == circuit->te_metric[1])
        {
          if (circuit->te_metric[0] != DEFAULT_CIRCUIT_METRIC)
          {
            vty_out(vty, " isis metric %d%s", circuit->te_metric[0],
                    VTY_NEWLINE);
            write++;
          }
        }
        else
        {
          for (i = 0; i < 2; i++)
          {
            if (circuit->te_metric[i] != DEFAULT_CIRCUIT_METRIC)
            {
              vty_out(vty, " isis metric %d level-%d%s",
                      circuit->te_metric[i], i + 1, VTY_NEWLINE);
              write++;
            }
          }
        }
      }

      /* ISIS - Metric v6*/
      if (circuit->area && circuit->ipv6_auto_cost_flag == 0)
      {
        if (circuit->te_metric_v6[0] == circuit->te_metric_v6[1])
        {
          if (circuit->te_metric_v6[0] != DEFAULT_CIRCUIT_METRIC)
          {
            vty_out(vty, " isis ipv6 cost %d%s", circuit->te_metric_v6[0],
                    VTY_NEWLINE);
            write++;
          }
        }
        else
        {
          for (i = 0; i < 2; i++)
          {
            if (circuit->te_metric_v6[i] != DEFAULT_CIRCUIT_METRIC)
            {
              vty_out(vty, " isis ipv6 cost %d level-%d%s",
                      circuit->te_metric_v6[i], i + 1, VTY_NEWLINE);
              write++;
            }
          }
        }
      }
      
      /* circuit auth*/
      if (circuit->passwd.type == ISIS_PASSWD_TYPE_HMAC_MD5)
      {
        vty_out(vty, " isis password %s md5 %s", circuit->passwd.en_passwd,
                VTY_NEWLINE);
        write++;
      }
      else if (circuit->passwd.type == ISIS_PASSWD_TYPE_CLEARTXT)
      {
        vty_out(vty, " isis password %s%s", circuit->passwd.passwd,
                VTY_NEWLINE);
        write++;
      }
    }
    vty_out(vty, "!%s", VTY_NEWLINE);
  }

  return write;
}

int isis_add_ipv6_locallink_addr(uint32_t ifindex)
{
  u_char pmac[6];
  struct ifm_event pevent;

  if (ifm_get_mac(ifindex, MODULE_ID_ISIS, pmac) != 0)
  {
    zlog_err("%-15s[%d]: ISIS get ifindex %0x mac time out", __FUNCTION__, __LINE__, ifindex);
    return -1;
  }

  memset(&pevent, 0, sizeof(pevent));

  pevent.ifindex = ifindex;
  pevent.ipaddr.type = INET_FAMILY_IPV6;
  pevent.ipaddr.addr.ipv6[0] = 0xfe;
  pevent.ipaddr.addr.ipv6[1] = 0x80;
  pevent.ipaddr.addr.ipv6[2] = 0x00;
  pevent.ipaddr.addr.ipv6[3] = 0x00;
  pevent.ipaddr.addr.ipv6[4] = 0x00;
  pevent.ipaddr.addr.ipv6[5] = 0x00;
  pevent.ipaddr.addr.ipv6[6] = 0x00;
  pevent.ipaddr.addr.ipv6[7] = 0x00;
  pevent.ipaddr.addr.ipv6[8] = pmac[0];
  pevent.ipaddr.addr.ipv6[9] = pmac[1];
  pevent.ipaddr.addr.ipv6[10] = pmac[2];
  pevent.ipaddr.addr.ipv6[11] = 0xff;
  pevent.ipaddr.addr.ipv6[12] = 0xfe;
  pevent.ipaddr.addr.ipv6[13] = pmac[3];
  pevent.ipaddr.addr.ipv6[14] = pmac[4];
  pevent.ipaddr.addr.ipv6[15] = pmac[5];
  pevent.ipaddr.prefixlen = 64;
  isis_zebra_if_address_add(&pevent, VRF_DEFAULT);

  return 0;
}

int isis_if_set_value(uint32_t ifindex, struct interface *ifp, vrf_id_t vrf_id)
{
#if 1
  uchar pmac[6];
  struct ifm_l3 l3;
  struct ifm_l3 l3_unnum;
  uint8_t pdown_flag;
  struct ifm_event pevent;
  uint16_t mtu;

  ifp->ifindex = ifindex;
  ifp->metric = 1;
  ifp->bandwidth = ISIS_DEFAULT_BANDWIDTH;
  ifp->ll_type = ZEBRA_LLT_ETHER;
  ifp->hw_addr_len = 6;

  if (ifm_get_mtu(ifindex, MODULE_ID_ISIS, &mtu) != 0)
  {
    zlog_warn("%-15s[%d]: isis get ifindex %0x mtu timeout", __func__, __LINE__, pevent.ifindex);
    ifp->mtu = 1500;
  }
  else
  {
    ifp->mtu = mtu;
  }

  /* get the mac,ip,link at the start*/
  if (ifm_get_mac(ifindex, MODULE_ID_ISIS, pmac) != 0)
  {
    zlog_err("%-15s[%d]: ISIS get ifindex %0x mac time out", __FUNCTION__, __LINE__, ifindex);
  }
  else
  {
    memcpy(ifp->hw_addr, pmac, 6);
  }

  if (ifm_get_link(ifindex, MODULE_ID_ISIS, &pdown_flag) != 0)
  {
    zlog_err("%-15s[%d]: ISIS get ifindex %0x link status time out", __FUNCTION__, __LINE__, ifindex);
    ifp->status = 1;
    ifp->flags = 0;
  }
  else
  {
    if (pdown_flag == IFNET_LINKUP)
    {
      ifp->status = 0;
      SET_FLAG(ifp->flags, IFF_UP);
      SET_FLAG(ifp->flags, IFF_RUNNING);
      SET_FLAG(ifp->flags, IFF_BROADCAST);
    }
    else
    {
      ifp->status = 1;
      ifp->flags = 0;
    }
  }

  if (if_is_operative(ifp))
  {
    /* Prevent more than one area per circuit */
    isis_csm_state_change(IF_UP_FROM_Z, circuit_scan_by_ifp(ifp), ifp);
  }

  if (ifm_get_l3if(ifindex, MODULE_ID_ISIS, &l3) != 0)
  {
    zlog_err("%-15s[%d]: ISIS get ifindex %0x l3 time out", __FUNCTION__, __LINE__, ifindex);
    return -1;
  }

  ifp->vpn = l3.vpn;
  if (l3.ipv4_flag == IP_TYPE_UNNUMBERED)
  {
    if (0 != ifm_get_l3if(l3.unnumbered_if, MODULE_ID_ISIS, &l3_unnum))
    {
      zlog_err("%-15s[%d]: ISIS get ifindex %0x l3 time out", __FUNCTION__, __LINE__, l3.unnumbered_if);
    }

    if (l3_unnum.ipv4[0].addr != 0)
    {
      pevent.ifindex = ifindex;
      pevent.ipaddr.type = INET_FAMILY_IPV4;
      pevent.ipaddr.addr.ipv4 = l3_unnum.ipv4[0].addr;
      pevent.ipaddr.prefixlen = l3_unnum.ipv4[0].prefixlen;
      pevent.vpn = l3_unnum.vpn;
      isis_zebra_if_address_add(&pevent, vrf_id);

      if (l3_unnum.ipv4[1].addr != 0)
      {
        memset(&pevent, 0, sizeof(struct ifm_event));
        pevent.ifindex = ifindex;
        pevent.ipaddr.type = INET_FAMILY_IPV4;
        pevent.ipaddr.addr.ipv4 = l3_unnum.ipv4[1].addr;
        pevent.ipaddr.prefixlen = l3_unnum.ipv4[1].prefixlen;
        pevent.vpn = l3_unnum.vpn;
        isis_zebra_if_address_add(&pevent, vrf_id);
      }
    }

    if (IFP_IF_IPV6_EXIST(l3_unnum.ipv6[0].addr))
    {
      pevent.ifindex = ifindex;
      pevent.ipaddr.type = INET_FAMILY_IPV6;
      IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3_unnum.ipv6[0].addr);
      pevent.ipaddr.prefixlen = l3_unnum.ipv6[0].prefixlen;
      pevent.vpn = l3_unnum.vpn;

      isis_zebra_if_address_add(&pevent, vrf_id);

      if (IFP_IF_IPV6_EXIST(l3_unnum.ipv6[1].addr))
      {
        memset(&pevent, 0, sizeof(struct ifm_event));
        pevent.ifindex = ifindex;
        pevent.ipaddr.type = INET_FAMILY_IPV6;
        IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3_unnum.ipv6[1].addr);
        pevent.ipaddr.prefixlen = l3_unnum.ipv6[1].prefixlen;
        pevent.vpn = l3_unnum.vpn;

        isis_zebra_if_address_add(&pevent, vrf_id);
      }
    }
  }
  else
  {
    if (l3.ipv4[0].addr != 0)
    {
      pevent.ifindex = ifindex;
      pevent.ipaddr.type = INET_FAMILY_IPV4;
      pevent.ipaddr.addr.ipv4 = l3.ipv4[0].addr;
      pevent.ipaddr.prefixlen = l3.ipv4[0].prefixlen;
      pevent.vpn = l3.vpn;

      isis_zebra_if_address_add(&pevent, vrf_id);

      if (l3.ipv4[1].addr != 0)
      {
        memset(&pevent, 0, sizeof(struct ifm_event));
        pevent.ifindex = ifindex;
        pevent.ipaddr.type = INET_FAMILY_IPV4;
        pevent.ipaddr.addr.ipv4 = l3.ipv4[1].addr;
        pevent.ipaddr.prefixlen = l3.ipv4[1].prefixlen;
        pevent.vpn = l3.vpn;

        isis_zebra_if_address_add(&pevent, vrf_id);
      }
    }

    if (IFP_IF_IPV6_EXIST(l3.ipv6[0].addr))
    {
      pevent.ifindex = ifindex;
      pevent.ipaddr.type = INET_FAMILY_IPV6;

      IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3.ipv6[0].addr);
      pevent.ipaddr.prefixlen = l3.ipv6[0].prefixlen;
      pevent.vpn = l3.vpn;

      isis_zebra_if_address_add(&pevent, vrf_id);

      if (IFP_IF_IPV6_EXIST(l3.ipv6[1].addr))
      {
        memset(&pevent, 0, sizeof(struct ifm_event));
        pevent.ifindex = ifindex;
        pevent.ipaddr.type = INET_FAMILY_IPV6;
        IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3.ipv6[1].addr);
        pevent.ipaddr.prefixlen = l3.ipv6[1].prefixlen;
        pevent.vpn = l3.vpn;

        isis_zebra_if_address_add(&pevent, vrf_id);
      }
    }
  }

#if 0
	    if(l3->ipv4_flag != IP_TYPE_INVALID)
	    {
		  pevent.ifindex = ifindex;
		  pevent.ipaddr.type = INET_FAMILY_IPV4;
		  pevent.ipaddr.addr.ipv4 = l3->ipv4[0].addr;
		  pevent.ipaddr.prefixlen = l3->ipv4[0].prefixlen;
		  pevent.vpn = l3->vpn;
		  isis_zebra_if_address_add (&pevent, vrf_id);

		  pevent.ifindex = ifindex;
		  pevent.ipaddr.type = INET_FAMILY_IPV4;
		  pevent.ipaddr.addr.ipv4 = l3->ipv4[1].addr;
		  pevent.ipaddr.prefixlen = l3->ipv4[1].prefixlen;
		  pevent.vpn = l3->vpn;
		  isis_zebra_if_address_add (&pevent, vrf_id);
		}
		
		if(l3->ipv6_flag != IP_TYPE_INVALID)
		{
		  pevent.ifindex = ifindex;
		  pevent.ipaddr.type = INET_FAMILY_IPV6;
		  memcpy(&pevent.ipaddr.addr.ipv6, &l3->ipv6[0].addr,16);
		  pevent.ipaddr.prefixlen = l3->ipv6[0].prefixlen;
		  pevent.vpn = l3->vpn;
		  isis_zebra_if_address_add (&pevent, vrf_id);

		  pevent.ifindex = ifindex;
		  pevent.ipaddr.type = INET_FAMILY_IPV6;
		  memcpy(&pevent.ipaddr.addr.ipv6, &l3->ipv6[1].addr,16);
		  pevent.ipaddr.prefixlen = l3->ipv6[1].prefixlen;
		  pevent.vpn = l3->vpn;
		  isis_zebra_if_address_add (&pevent, vrf_id);
		}
#endif
#endif

  return 0;
}
struct interface *
isis_interface_add(uint32_t ifindex, vrf_id_t vrf_id)
{
  struct interface *ifp;
  char ifname_tmp[INTERFACE_NAMSIZ];

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp, strnlen(ifname_tmp, INTERFACE_NAMSIZ), vrf_id);
  if (ifp == NULL)
  {
    return NULL;
  }

  isis_if_set_value(ifindex, ifp, vrf_id);

  return ifp;
}

struct interface *
isis_interface_find(struct vty *vty)
{
  struct interface *ifp;
  char ifname_tmp[INTERFACE_NAMSIZ];
  uint32_t ifindex;
  vrf_id_t vrf_id = VRF_DEFAULT;

  ifindex = (uint32_t)vty->index;

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp,
                               strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                               vrf_id);
  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return NULL;
  }
  return ifp;
}

DEFUN(isis_enable_instance,
      isis_enable_instance_cmd,
      "isis enable instance <1-255>",
      "Interface Internet Protocol config commands\n"
      "IP router interface commands\n"
      "IS-IS Routing for IP\n"
      "Routing process tag\n")
{
  vrf_id_t vrf_id = VRF_DEFAULT;
  struct isis_circuit *circuit;
  struct interface *ifp;
  struct isis_area *area;
  uint8_t ifmode;
  int rv;
  uint32_t ifindex;

  int node = vty->node;

  if (atoi(argv[0]) < 1 || atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <1-255>%s", atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);

  if (!area)
  {
    vty_warning_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (listcount(area->area_addrs) == 0)
  {
    vty_warning_out(vty, "Can't find net entity in this instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  ifindex = (uint32_t)vty->index;

  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifp = isis_interface_add(ifindex, vrf_id);
  if (ifp == NULL)
  {
    vty_warning_out(vty, "Get interface failed!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (ifp->vpn != area->vpn)
  {
    vty_warning_out(vty, "The interface and IS-IS process must belong to the same VPN instance.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  circuit = circuit_scan_by_ifp(ifp);

  if (circuit && circuit->area)
  {
    if (circuit->ip_router == 1)
    {
      if (strcmp(circuit->area->area_tag, argv[0]))
      {
        vty_warning_out(vty, "ISIS circuit is already defined on %s%s",
                        circuit->area->area_tag, VTY_NEWLINE);
        return CMD_SUCCESS;
      }
      return CMD_SUCCESS;
    }
    else
    {
      if (strcmp(circuit->area->area_tag, argv[0]))
      {
        vty_warning_out(vty, "ISIS circuit is already defined on %s%s",
                        circuit->area->area_tag, VTY_NEWLINE);
        return CMD_SUCCESS;
      }

      if (circuit->ipv6_router == 1)
      {
        circuit->ip_router = 1;
        circuit->area->ip_circuits++;
        lsp_regenerate_schedule(circuit->area, IS_LEVEL_1_AND_2, 1);
      }

      return CMD_SUCCESS;
    }
  }

  if (area->lsp_mtu > ifp->mtu)
  {
    vty_warning_out(vty, "Interface MTU %zu is too low to support area lsp mtu %u!%s",
                    ifp->mtu, area->lsp_mtu, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = isis_csm_state_change(ISIS_ENABLE, circuit, area);
  if (circuit->state != C_STATE_CONF && circuit->state != C_STATE_UP)
  {
    vty_warning_out(vty, "Couldn't bring up interface, please check log.%s", VTY_NEWLINE);
    rv = CMD_SUCCESS;
  }
  else
  {
    isis_circuit_if_bind(circuit, ifp);
    circuit->ip_router = 1;
    area->ip_circuits++;
    circuit_update_nlpids(circuit);
    /*hello init*/
    if (circuit->interface != NULL)
      hello_init(circuit);
    rv = CMD_SUCCESS;
  }

  vty->node = node;
  vty->index = (void *)ifindex;

  return rv;
}

DEFUN(isis_enable_instance_h3c,
      isis_enable_instance_h3c_cmd,
      "isis enable",
      "Configure interface parameters for ISIS\n"
      "Enable ISIS processing\n")
{
  vrf_id_t vrf_id = VRF_DEFAULT;
  struct isis_circuit *circuit;
  struct interface *ifp;
  struct isis_area *area;
  uint8_t ifmode;
  int rv;
  uint32_t ifindex;
  int area_tag = 1;
  char buf[32];
  int node = vty->node;

  while (area_tag <= 255 && area_tag > 0)
  {
    sprintf(buf, "%d", area_tag);
    area = isis_area_lookup(buf);
    if (area != NULL)
    {
      break;
    }
    area_tag++;
  }

  if (!area)
  {
    vty_warning_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (listcount(area->area_addrs) == 0)
  {
    vty_warning_out(vty, "Can't find net entity in this instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  ifindex = (uint32_t)vty->index;

  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifp = isis_interface_add(ifindex, vrf_id);
  if (ifp == NULL)
  {
    vty_warning_out(vty, "Get interface failed!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (ifp->vpn != area->vpn)
  {
    vty_warning_out(vty, "The interface and IS-IS process must belong to the same VPN instance.%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  circuit = circuit_scan_by_ifp(ifp);

  if (circuit && circuit->area)
  {
    if (circuit->ip_router == 1)
    {
      if (strcmp(circuit->area->area_tag, buf))
      {
        vty_warning_out(vty, "ISIS circuit is already defined on %s%s",
                        circuit->area->area_tag, VTY_NEWLINE);
        return CMD_SUCCESS;
      }
      return CMD_SUCCESS;
    }
    else
    {
      if (strcmp(circuit->area->area_tag, buf))
      {
        vty_warning_out(vty, "ISIS circuit is already defined on %s%s",
                        circuit->area->area_tag, VTY_NEWLINE);
        return CMD_SUCCESS;
      }

      if (circuit->ipv6_router == 1)
      {
        circuit->ip_router = 1;
        circuit->area->ip_circuits++;
        lsp_regenerate_schedule(circuit->area, IS_LEVEL_1_AND_2, 1);
      }

      return CMD_SUCCESS;
    }
  }

  if (area->lsp_mtu > ifp->mtu)
  {
    vty_warning_out(vty, "Interface MTU %zu is too low to support area lsp mtu %u!%s",
                    ifp->mtu, area->lsp_mtu, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = isis_csm_state_change(ISIS_ENABLE, circuit, area);
  if (circuit->state != C_STATE_CONF && circuit->state != C_STATE_UP)
  {
    vty_warning_out(vty, "Couldn't bring up interface, please check log.%s", VTY_NEWLINE);
    rv = CMD_SUCCESS;
  }
  else
  {
    isis_circuit_if_bind(circuit, ifp);
    circuit->ip_router = 1;
    area->ip_circuits++;
    circuit_update_nlpids(circuit);
    /*hello init*/
    if (circuit->interface != NULL)
      hello_init(circuit);
    rv = CMD_SUCCESS;
  }

  vty->node = node;
  vty->index = (void *)ifindex;

  return rv;
}

DEFUN(no_isis_enable_instance,
      no_isis_enable_instance_cmd,
      "no isis enable instance <1-255>",
      NO_STR
      "Interface Internet Protocol config commands\n"
      "IP router interface commands\n"
      "IS-IS Routing for IP\n"
      "Routing process tag\n")
{
  vrf_id_t vrf_id = VRF_DEFAULT;
  struct interface *ifp;
  struct isis_area *area;
  struct isis_circuit *circuit;
  uint8_t ifmode;

  uint32_t ifindex;
  char if_name[INTERFACE_NAMSIZ];

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifm_get_name_by_ifindex(ifindex, if_name);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(if_name, strlen(if_name), vrf_id);

  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);
  if (!area)
  {
    vty_warning_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_lookup_by_ifp(ifp, area->circuit_list);
  if (!circuit)
  {
    vty_warning_out(vty, "ISIS is not enabled on circuit %s%s", ifp->name, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->ip_router = 0;
  area->ip_circuits--;
  if (area->ip_circuits == 0)
  {
    if (area->is_type & IS_LEVEL_1)
      area->force_spf_l1 = 1;

    if (area->is_type & IS_LEVEL_2)
      area->force_spf_l2 = 1;
  }

  if (circuit->ipv6_router == 0 && circuit->ip_router == 0)
  {
    //isis_csm_state_change (ISIS_DISABLE, circuit, area);
    if (isis_csm_state_change(ISIS_DISABLE, circuit, area))
      isis_circuit_conf_default(circuit);
  }
  else
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}

DEFUN(no_isis_enable_instance_h3c,
      no_isis_enable_instance_h3c_cmd,
      "undo isis <1-255>",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Process ID\n")
{
  vrf_id_t vrf_id = VRF_DEFAULT;
  struct interface *ifp;
  struct isis_area *area;
  struct isis_circuit *circuit;
  uint8_t ifmode;

  uint32_t ifindex;
  char if_name[INTERFACE_NAMSIZ];

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifm_get_name_by_ifindex(ifindex, if_name);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(if_name, strlen(if_name), vrf_id);

  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);
  if (!area)
  {
    vty_warning_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_lookup_by_ifp(ifp, area->circuit_list);
  if (!circuit)
  {
    vty_warning_out(vty, "ISIS is not enabled on circuit %s%s", ifp->name, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->ip_router = 0;
  area->ip_circuits--;
  if (area->ip_circuits == 0)
  {
    if (area->is_type & IS_LEVEL_1)
      area->force_spf_l1 = 1;

    if (area->is_type & IS_LEVEL_2)
      area->force_spf_l2 = 1;
  }

  if (area->ipv6_circuits == 0)
  {
    if (area->is_type & IS_LEVEL_1)
      area->force_spf_v6_l1 = 1;

    if (area->is_type & IS_LEVEL_2)
      area->force_spf_v6_l2 = 1;
  }

  if (circuit->ipv6_router == 0 && circuit->ip_router == 0)
  {
    //isis_csm_state_change (ISIS_DISABLE, circuit, area);
    if (isis_csm_state_change(ISIS_DISABLE, circuit, area))
      isis_circuit_conf_default(circuit);
  }
  else
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}

DEFUN(isis_enable_ipv6_instance,
      isis_enable_ipv6_instance_cmd,
      "isis enable ipv6 instance <1-255>",
      "Interface Internet Protocol config commands\n"
      "IP router interface commands\n"
      "IPV6 type\n"
      "IS-IS Routing for IP\n"
      "Routing process tag\n")

{
  vrf_id_t vrf_id = VRF_DEFAULT;
  struct isis_circuit *circuit = NULL;
  struct interface *ifp = NULL;
  struct isis_area *area = NULL;
  uint8_t ifmode;
  int rv;
  uint32_t ifindex;
  int ret = 0;

  if (atoi(argv[0]) < 1 || atoi(argv[0]) > 255)
  {
    vty_warning_out(vty, "Invalid Instance %d - should be <0-127>%s", atoi(argv[0]), VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);
  if (!area)
  {
    vty_warning_out(vty, "Can't find instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (listcount(area->area_addrs) == 0)
  {
    vty_warning_out(vty, "Can't find net entity in this instance!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifp = isis_interface_add(ifindex, vrf_id);
  if (ifp == NULL)
  {
    vty_warning_out(vty, "Get interface failed!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_scan_by_ifp(ifp);
  if (circuit == NULL)
  {
    vty_warning_out(vty, "There is no circuit!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->ipv6_link && listcount(circuit->ipv6_link) == 0)
  {
    ret = isis_add_ipv6_locallink_addr(ifindex);
    if (ret == -1)
    {
      vty_error_out(vty, "get the interface mac timeout!%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  }

  if (circuit->area != NULL)
  {
    if (circuit->ipv6_router == 1)
    {
      if (strcmp(circuit->area->area_tag, argv[0]))
      {
        vty_warning_out(vty, "ISIS circuit is already defined on %s%s",
                        circuit->area->area_tag, VTY_NEWLINE);
        return CMD_SUCCESS;
      }
      return CMD_SUCCESS;
    }
    else
    {
      if (strcmp(circuit->area->area_tag, argv[0]))
      {
        vty_warning_out(vty, "ISIS circuit is already defined on %s%s",
                        circuit->area->area_tag, VTY_NEWLINE);
        return CMD_SUCCESS;
      }

      if (circuit->ip_router == 1)
      {
        circuit->ipv6_router = 1;
        circuit->area->ipv6_circuits++;
        lsp_regenerate_schedule(circuit->area, IS_LEVEL_1_AND_2, 1);
      }
      return CMD_SUCCESS;
    }
  }

  if (circuit->ip_router != 1)
    circuit = isis_csm_state_change(ISIS_ENABLE, circuit, area);

  if (circuit->state != C_STATE_CONF && circuit->state != C_STATE_UP)
  {
    vty_warning_out(vty, "Couldn't bring up interface, please check log.%s", VTY_NEWLINE);
    rv = CMD_SUCCESS;
  }
  else
  {
    isis_circuit_if_bind(circuit, ifp);
    circuit->ipv6_router = 1;
    area->ipv6_circuits++;
    circuit_update_nlpids(circuit);
    rv = CMD_SUCCESS;
  }

  vty->node = INTERFACE_NODE;
  vty->index = (void *)ifindex;

  return rv;
}

DEFUN(no_isis_enable_ipv6_instance,
      no_isis_enable_ipv6_instance_cmd,
      "no isis enable ipv6 instance <1-255>",
      NO_STR
      "IPv6 interface subcommands\n"
      "IPv6 Router interface commands\n"
      "IS-IS Routing for IPv6\n"
      "Routing process tag\n")
{
  vrf_id_t vrf_id = VRF_DEFAULT;
  struct interface *ifp;
  struct isis_area *area;
  struct isis_circuit *circuit;
  uint8_t ifmode;
  uint32_t ifindex;
  char if_name[INTERFACE_NAMSIZ];

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifm_get_name_by_ifindex(ifindex, if_name);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(if_name, strlen(if_name), vrf_id);

  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  area = isis_area_lookup(argv[0]);
  if (!area)
  {
    vty_warning_out(vty, "Can't find ISIS instance %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_lookup_by_ifp(ifp, area->circuit_list);
  if (!circuit)
  {
    vty_warning_out(vty, "ISIS is not enabled on circuit %s%s", ifp->name, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->ipv6_router = 0;
  area->ipv6_circuits--;
  if (area->ipv6_circuits == 0)
  {
    if (area->is_type & IS_LEVEL_1)
      area->force_spf_v6_l1 = 1;

    if (area->is_type & IS_LEVEL_2)
      area->force_spf_v6_l2 = 1;
  }

  if (circuit->ipv6_router == 0 && circuit->ip_router == 0)
    isis_csm_state_change(ISIS_DISABLE, circuit, area);
  else
    lsp_regenerate_schedule(area, IS_LEVEL_1_AND_2, 0);

  return CMD_SUCCESS;
}

DEFUN(isis_passive,
      isis_passive_cmd,
      "isis passive",
      "IS-IS commands\n"
      "Configure the passive mode for interface\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->is_passive == 1)
    return CMD_SUCCESS;

  if (circuit->state != C_STATE_UP)
  {
    circuit->is_passive = 1;
  }
  else
  {
    struct isis_area *area = circuit->area;
    isis_csm_state_change(ISIS_DISABLE, circuit, area);
    circuit->is_passive = 1;
    isis_csm_state_change(ISIS_ENABLE, circuit, area);
  }

  return CMD_SUCCESS;
}

DEFUN(no_isis_passive,
      no_isis_passive_cmd,
      "no isis passive",
      NO_STR
      "IS-IS commands\n"
      "Configure the passive mode for interface\n")
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  //ifp = (struct interface *) vty->index;
  ifp = isis_interface_find(vty);

  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* FIXME: what is wrong with circuit = ifp->info ? */
  circuit = circuit_scan_by_ifp(ifp);
  if (!circuit)
  {
    vty_warning_out(vty, "ISIS is not enabled on circuit %s%s",
                    ifp->name, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (if_is_loopback(ifp))
  {
    vty_warning_out(vty, "Can't set no passive for loopback interface%s",
                    VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->is_passive == 0)
    return CMD_SUCCESS;

  if (circuit->state != C_STATE_UP)
  {
    circuit->is_passive = 0;
  }
  else
  {
    struct isis_area *area = circuit->area;
    isis_csm_state_change(ISIS_DISABLE, circuit, area);
    circuit->is_passive = 0;
    isis_csm_state_change(ISIS_ENABLE, circuit, area);
  }

  return CMD_SUCCESS;
}

DEFUN(isis_circuit_type,
      isis_circuit_type_cmd,
      "isis circuit-type (level-1|level-1-2|level-2)",
      "IS-IS commands\n"
      "Configure circuit type for interface\n"
      "Level-1 only adjacencies are formed\n"
      "Level-1-2 adjacencies are formed\n"
      "Level-2 only adjacencies are formed\n")
{
  int circuit_type;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit_type = string2circuit_t(argv[0]);
  if (!circuit_type)
  {
    vty_warning_out(vty, "Unknown circuit-type %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->state == C_STATE_UP &&
      circuit->area->is_type != IS_LEVEL_1_AND_2 &&
      circuit->area->is_type != circuit_type)
  {
    vty_warning_out(vty, "Invalid circuit level for instance %s.%s",
                    circuit->area->area_tag, VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  isis_event_circuit_type_change(circuit, circuit_type);

  return CMD_SUCCESS;
}

DEFUN(isis_circuit_type_h3c,
      isis_circuit_type_h3c_cmd,
      "isis circuit-level (level-1|level-1-2|level-2)",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS level for the interface\n"
      "Level-1\n"
      "Level-1-2\n"
      "Level-2\n")
{
  int circuit_type;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit_type = string2circuit_t(argv[0]);
  if (!circuit_type)
  {
    vty_warning_out(vty, "Unknown circuit-type %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->state == C_STATE_UP &&
      circuit->area->is_type != IS_LEVEL_1_AND_2 &&
      circuit->area->is_type != circuit_type)
  {
    vty_warning_out(vty, "Invalid circuit level for instance %s.%s",
                    circuit->area->area_tag, VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  isis_event_circuit_type_change(circuit, circuit_type);

  return CMD_SUCCESS;
}

DEFUN(no_isis_circuit_type,
      no_isis_circuit_type_cmd,
      "no isis circuit-type",
      NO_STR
      "IS-IS commands\n"
      "Configure circuit type for interface\n")
{
  int circuit_type;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /*
   * Set the circuits level to its default value
   */
  if (circuit->state == C_STATE_UP)
    circuit_type = circuit->area->is_type;
  else
    circuit_type = IS_LEVEL_1_AND_2;
  isis_event_circuit_type_change(circuit, circuit_type);

  return CMD_SUCCESS;
}

DEFUN(no_isis_circuit_type_h3c,
      no_isis_circuit_type_h3c_cmd,
      "undo isis circuit-level",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS level for the interface\n")
{
  int circuit_type;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /*
   * Set the circuits level to its default value
   */
  if (circuit->state == C_STATE_UP)
    circuit_type = circuit->area->is_type;
  else
    circuit_type = IS_LEVEL_1_AND_2;
  isis_event_circuit_type_change(circuit, circuit_type);

  return CMD_SUCCESS;
}

DEFUN(isis_passwd_md5,
      isis_passwd_md5_cmd,
      "isis password WORD md5",
      "IS-IS commands\n"
      "Configure the authentication password for a circuit\n"
      "Circuit password clear:<1-127>/md5:<1-254>\n"
      "Authentication type\n")
{
  int len;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long circuit password (>254)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  circuit->passwd.len = len;
  circuit->passwd.type = ISIS_PASSWD_TYPE_HMAC_MD5;
  //strncpy ((char *)circuit->passwd.passwd, argv[0], 255);

  if (vty->config_read_flag != 1)
  {
    strncpy((char *)circuit->passwd.passwd, argv[0], 255);
    isis_password_encode((char *)circuit->passwd.en_passwd, (char *)circuit->passwd.passwd);
  }

  if (vty->config_read_flag == 1)
  {
    strncpy((char *)circuit->passwd.en_passwd, argv[0], 255);
    isis_password_decode((char *)circuit->passwd.passwd, (char *)circuit->passwd.en_passwd);
  }

  return CMD_SUCCESS;
}

DEFUN(isis_passwd_md5_h3c,
      isis_passwd_md5_h3c_cmd,
      "isis authentication-mode md5 WORD",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set the type and password of ISIS interface authentication\n"
      "Authentication type\n"
      "Circuit password clear:<1-127>/md5:<1-254>\n")
{
  int len;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  len = strlen(argv[0]);
  if (len > 254)
  {
    vty_warning_out(vty, "Too long circuit password (>254)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  circuit->passwd.len = len;
  circuit->passwd.type = ISIS_PASSWD_TYPE_HMAC_MD5;
  //strncpy ((char *)circuit->passwd.passwd, argv[0], 255);

  if (vty->config_read_flag != 1)
  {
    strncpy((char *)circuit->passwd.passwd, argv[0], 255);
    isis_password_encode((char *)circuit->passwd.en_passwd, (char *)circuit->passwd.passwd);
  }

  if (vty->config_read_flag == 1)
  {
    strncpy((char *)circuit->passwd.en_passwd, argv[0], 255);
    isis_password_decode((char *)circuit->passwd.passwd, (char *)circuit->passwd.en_passwd);
  }

  return CMD_SUCCESS;
}

DEFUN(isis_passwd_clear,
      isis_passwd_clear_cmd,
      "isis password WORD",
      "IS-IS commands\n"
      "Configure the authentication password for a circuit\n"
      "Circuit password clear:<1-127>/md5:<1-254>\n")
{
  int len;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  len = strlen(argv[0]);
  if (len > 127)
  {
    vty_warning_out(vty, "Too long circuit password (>127)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  circuit->passwd.len = len;
  circuit->passwd.type = ISIS_PASSWD_TYPE_CLEARTXT;
  strncpy((char *)circuit->passwd.passwd, argv[0], 255);

  return CMD_SUCCESS;
}

DEFUN(isis_passwd_clear_h3c,
      isis_passwd_clear_h3c_cmd,
      "isis authentication-mode simple WORD",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set the type and password of ISIS interface authentication\n"
      "Authentication type\n"
      "Circuit password clear:<1-127>/md5:<1-254>\n")
{
  int len;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  len = strlen(argv[0]);
  if (len > 127)
  {
    vty_warning_out(vty, "Too long circuit password (>127)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  circuit->passwd.len = len;
  circuit->passwd.type = ISIS_PASSWD_TYPE_CLEARTXT;
  strncpy((char *)circuit->passwd.passwd, argv[0], 255);

  return CMD_SUCCESS;
}

DEFUN(no_isis_passwd,
      no_isis_passwd_cmd,
      "no isis password",
      NO_STR
      "IS-IS commands\n"
      "Configure the authentication password for a circuit\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(&circuit->passwd, 0, sizeof(struct isis_passwd));

  return CMD_SUCCESS;
}

DEFUN(no_isis_passwd_h3c,
      no_isis_passwd_h3c_cmd,
      "undo isis authentication-mode",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set the type and password of ISIS interface authentication\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  memset(&circuit->passwd, 0, sizeof(struct isis_passwd));

  return CMD_SUCCESS;
}

DEFUN(isis_priority,
      isis_priority_cmd,
      "isis priority <0-127>",
      "IS-IS commands\n"
      "Set priority for Designated Router election\n"
      "Priority value\n")
{
  int prio;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  if (circuit == NULL)
  {
    vty_warning_out(vty, "ISIS disable in this interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->circ_type == CIRCUIT_T_P2P)
  {
    vty_warning_out(vty, "This interface is p2p type,priority is invalid!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  prio = atoi(argv[0]);
  if (prio < MIN_PRIORITY || prio > MAX_PRIORITY)
  {
    vty_warning_out(vty, "Invalid priority %d - should be <0-127>%s",
                    prio, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->priority[0] = prio;
  circuit->priority[1] = prio;

  return CMD_SUCCESS;
}

DEFUN(isis_priority_h3c,
      isis_priority_h3c_cmd,
      "isis dis-priority <0-127>",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set interface DIS election priority\n"
      "Value of priority\n")
{
  int prio;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  if (circuit == NULL)
  {
    vty_warning_out(vty, "ISIS disable in this interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->circ_type == CIRCUIT_T_P2P)
  {
    vty_warning_out(vty, "This interface is p2p type,priority is invalid!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  prio = atoi(argv[0]);
  if (prio < MIN_PRIORITY || prio > MAX_PRIORITY)
  {
    vty_warning_out(vty, "Invalid priority %d - should be <0-127>%s",
                    prio, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->priority[0] = prio;
  circuit->priority[1] = prio;

  return CMD_SUCCESS;
}

DEFUN(no_isis_priority,
      no_isis_priority_cmd,
      "no isis priority",
      NO_STR
      "IS-IS commands\n"
      "Set priority for Designated Router election\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->circ_type == CIRCUIT_T_P2P)
  {
    vty_warning_out(vty, "This interface is p2p type,priority is invalid!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->priority[0] = DEFAULT_PRIORITY;
  circuit->priority[1] = DEFAULT_PRIORITY;

  return CMD_SUCCESS;
}

DEFUN(no_isis_priority_h3c,
      no_isis_priority_h3c_cmd,
      "undo isis dis-priority",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set interface DIS election priority\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (circuit->circ_type == CIRCUIT_T_P2P)
  {
    vty_warning_out(vty, "This interface is p2p type,priority is invalid!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->priority[0] = DEFAULT_PRIORITY;
  circuit->priority[1] = DEFAULT_PRIORITY;

  return CMD_SUCCESS;
}

DEFUN(isis_priority_l1,
      isis_priority_l1_cmd,
      "isis priority <0-127> level-1",
      "IS-IS commands\n"
      "Set priority for Designated Router election\n"
      "Priority value\n"
      "Specify priority for level-1 routing\n")
{
  int prio;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  prio = atoi(argv[0]);
  if (prio < MIN_PRIORITY || prio > MAX_PRIORITY)
  {
    vty_warning_out(vty, "Invalid priority %d - should be <0-127>%s",
                    prio, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->priority[0] = prio;

  return CMD_SUCCESS;
}

DEFUN(isis_priority_l1_h3c,
      isis_priority_l1_h3c_cmd,
      "isis dis-priority <0-127> level-1",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set interface DIS election priority\n"
      "Value of priority\n"
      "Level-1\n")
{
  int prio;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  prio = atoi(argv[0]);
  if (prio < MIN_PRIORITY || prio > MAX_PRIORITY)
  {
    vty_warning_out(vty, "Invalid priority %d - should be <0-127>%s",
                    prio, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->priority[0] = prio;

  return CMD_SUCCESS;
}

DEFUN(no_isis_priority_l1,
      no_isis_priority_l1_cmd,
      "no isis priority level-1",
      NO_STR
      "IS-IS commands\n"
      "Set priority for Designated Router election\n"
      "Specify priority for level-1 routing\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->priority[0] = DEFAULT_PRIORITY;

  return CMD_SUCCESS;
}

DEFUN(no_isis_priority_l1_h3c,
      no_isis_priority_l1_h3c_cmd,
      "undo isis dis-priority level-1",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set interface DIS election priority\n"
      "Level-1\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->priority[0] = DEFAULT_PRIORITY;

  return CMD_SUCCESS;
}

DEFUN(isis_priority_l2,
      isis_priority_l2_cmd,
      "isis priority <0-127> level-2",
      "IS-IS commands\n"
      "Set priority for Designated Router election\n"
      "Priority value\n"
      "Specify priority for level-2 routing\n")
{
  int prio;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  prio = atoi(argv[0]);
  if (prio < MIN_PRIORITY || prio > MAX_PRIORITY)
  {
    vty_out(vty, "Invalid priority %d - should be <0-127>%s",
            prio, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->priority[1] = prio;

  return CMD_SUCCESS;
}

DEFUN(isis_priority_l2_h3c,
      isis_priority_l2_h3c_cmd,
      "isis dis-priority <0-127> level-2",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set interface DIS election priority\n"
      "Value of priority\n"
      "Level-2\n")
{
  int prio;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  prio = atoi(argv[0]);
  if (prio < MIN_PRIORITY || prio > MAX_PRIORITY)
  {
    vty_out(vty, "Invalid priority %d - should be <0-127>%s",
            prio, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->priority[1] = prio;

  return CMD_SUCCESS;
}

DEFUN(no_isis_priority_l2,
      no_isis_priority_l2_cmd,
      "no isis priority level-2",
      NO_STR
      "IS-IS commands\n"
      "Set priority for Designated Router election\n"
      "Specify priority for level-2 routing\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->priority[1] = DEFAULT_PRIORITY;

  return CMD_SUCCESS;
}

DEFUN(no_isis_priority_l2_h3c,
      no_isis_priority_l2_h3c_cmd,
      "undo isis dis-priority level-2",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set interface DIS election priority\n"
      "Level-2\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->priority[1] = DEFAULT_PRIORITY;

  return CMD_SUCCESS;
}

/* Metric command */
DEFUN(isis_metric,
      isis_metric_cmd,
      "isis metric <1-16777215>",
      "IS-IS commands\n"
      "Set default metric for circuit\n"
      "Default metric value ,narrow <1-63>\n")
{
  int met;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  met = atoi(argv[0]);

  /* RFC3787 section 5.1 */
  if (circuit->area && circuit->area->oldmetric == 1 &&
      (met > MAX_NARROW_LINK_METRIC || met < 1))
  {
    vty_error_out(vty, "Invalid metric %d - should be <1-63> "
                       "when narrow metric type enabled%s",
                  met, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* RFC4444 */
  if (circuit->area && circuit->area->newmetric == 1 &&
      (met > MAX_WIDE_LINK_METRIC || met < 1))
  {
    vty_error_out(vty, "Invalid metric %d - should be <1-16777215> "
                       "when wide metric type enabled%s",
                  met, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->te_metric[0] = met;
  circuit->te_metric[1] = met;

  circuit->te_metric_v6[0] = met;
  circuit->te_metric_v6[1] = met;

  circuit->metrics[0].metric_default = met;
  circuit->metrics[1].metric_default = met;
  circuit->ipv4_auto_cost_flag = 0;

  circuit->metrics_v6[0].metric_default = met;
  circuit->metrics_v6[1].metric_default = met;
  circuit->ipv6_auto_cost_flag = 0;

  if (circuit->area)
    lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);

  return CMD_SUCCESS;
}

DEFUN(isis_metric_h3c,
      isis_metric_h3c_cmd,
      "isis [ipv6] cost <1-16777215>",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Configure IPv6 commands for IS-IS\n"
      "Set the cost value\n"
      "Cost value ,narrow <1-63>\n")
{
  int met;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  met = atoi(argv[1]);

  /* RFC3787 section 5.1 */
  if (circuit->area && circuit->area->oldmetric == 1 &&
      (met > MAX_NARROW_LINK_METRIC || met < 1))
  {
    vty_error_out(vty, "Invalid metric %d - should be <1-63> "
                       "when narrow metric type enabled%s",
                  met, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* RFC4444 */
  if (circuit->area && circuit->area->newmetric == 1 &&
      (met > MAX_WIDE_LINK_METRIC || met < 1))
  {
    vty_error_out(vty, "Invalid metric %d - should be <1-16777215> "
                       "when wide metric type enabled%s",
                  met, VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!argv[0])
  {
    if (circuit->te_metric[0] == (u_int32_t)met &&
        circuit->te_metric[1] == (u_int32_t)met &&
        circuit->metrics[0].metric_default == met &&
        circuit->metrics[1].metric_default == met &&
        circuit->ipv4_auto_cost_flag == 0)
      return CMD_SUCCESS;

    circuit->te_metric[0] = met;
    circuit->te_metric[1] = met;
    circuit->metrics[0].metric_default = met;
    circuit->metrics[1].metric_default = met;
    circuit->ipv4_auto_cost_flag = 0;
  }
  else
  {
    if (circuit->te_metric_v6[0] == (u_int32_t)met &&
        circuit->te_metric_v6[1] == (u_int32_t)met &&
        circuit->metrics_v6[0].metric_default == met &&
        circuit->metrics_v6[1].metric_default == met &&
        circuit->ipv6_auto_cost_flag == 0)
      return CMD_SUCCESS;

    circuit->te_metric_v6[0] = met;
    circuit->te_metric_v6[1] = met;
    circuit->metrics_v6[0].metric_default = met;
    circuit->metrics_v6[1].metric_default = met;
    circuit->ipv6_auto_cost_flag = 0;
  }
  if (circuit->area)
    lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);

  return CMD_SUCCESS;
}

DEFUN(no_isis_metric,
      no_isis_metric_cmd,
      "no isis metric",
      NO_STR
      "IS-IS commands\n"
      "Set default metric for circuit\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->te_metric[0] = DEFAULT_CIRCUIT_METRIC;
  circuit->te_metric[1] = DEFAULT_CIRCUIT_METRIC;
  circuit->metrics[0].metric_default = DEFAULT_CIRCUIT_METRIC;
  circuit->metrics[1].metric_default = DEFAULT_CIRCUIT_METRIC;

  circuit->te_metric_v6[0] = DEFAULT_CIRCUIT_METRIC;
  circuit->te_metric_v6[1] = DEFAULT_CIRCUIT_METRIC;
  circuit->metrics_v6[0].metric_default = DEFAULT_CIRCUIT_METRIC;
  circuit->metrics_v6[1].metric_default = DEFAULT_CIRCUIT_METRIC;

  if (circuit->area)
  {
    if (circuit->area->ref_bandwidth_ipv4 != 0)
    {
      circuit->ipv4_auto_cost_flag = 1;
      isis_set_interface_autocost(circuit->area, circuit->area->ref_bandwidth_ipv4, AF_INET);
    }

    if (circuit->area->ref_bandwidth_ipv6 != 0)
    {
      circuit->ipv6_auto_cost_flag = 1;
      isis_set_interface_autocost(circuit->area, circuit->area->ref_bandwidth_ipv6, AFI_IP6);
    }

    lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);
  }

  return CMD_SUCCESS;
}

DEFUN(no_isis_metric_h3c,
      no_isis_metric_h3c_cmd,
      "undo isis [ipv6] cost",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Configure IPv6 commands for IS-IS\n"
      "Set the cost value\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!argv[0])
  {
    /* add this judgement for saving resource */
    if (circuit->te_metric[0] == DEFAULT_CIRCUIT_METRIC &&
        circuit->te_metric[1] == DEFAULT_CIRCUIT_METRIC &&
        circuit->metrics[0].metric_default == DEFAULT_CIRCUIT_METRIC &&
        circuit->metrics[1].metric_default == DEFAULT_CIRCUIT_METRIC &&
        circuit->ipv4_auto_cost_flag == 0)
    {
      return CMD_SUCCESS;
    }
    circuit->te_metric[0] = DEFAULT_CIRCUIT_METRIC;
    circuit->te_metric[1] = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics[0].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics[1].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->ipv4_auto_cost_flag = 0;
  }
  else
  {
    /* add this judgement for saving resource */
    if (circuit->te_metric_v6[0] == DEFAULT_CIRCUIT_METRIC &&
        circuit->te_metric_v6[1] == DEFAULT_CIRCUIT_METRIC &&
        circuit->metrics_v6[0].metric_default == DEFAULT_CIRCUIT_METRIC &&
        circuit->metrics_v6[1].metric_default == DEFAULT_CIRCUIT_METRIC &&
        circuit->ipv6_auto_cost_flag == 0)
    {
      return CMD_SUCCESS;
    }
    circuit->te_metric_v6[0] = DEFAULT_CIRCUIT_METRIC;
    circuit->te_metric_v6[1] = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics_v6[0].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->metrics_v6[1].metric_default = DEFAULT_CIRCUIT_METRIC;
    circuit->ipv6_auto_cost_flag = 0;
  }

  if (circuit->area)
  {
    if (circuit->area->ref_bandwidth_ipv4 != 0 && !argv[0])
    {
      circuit->ipv4_auto_cost_flag = 1;
      isis_set_interface_autocost(circuit->area, circuit->area->ref_bandwidth_ipv4, AF_INET);
    }

    if (circuit->area->ref_bandwidth_ipv6 != 0 && argv[0])
    {
      circuit->ipv6_auto_cost_flag = 1;
      isis_set_interface_autocost(circuit->area, circuit->area->ref_bandwidth_ipv6, AF_INET6);
    }

    lsp_regenerate_schedule(circuit->area, circuit->is_type, 0);
  }
  return CMD_SUCCESS;
}
/*
DEFUN (isis_metric_l1,
       isis_metric_l1_cmd,
       "isis metric <1-16777215> level-1",
       "IS-IS commands\n"
       "Set default metric for circuit\n"
       "Default metric value\n"
       "Specify metric for level-1 routing\n")
{
  int met;
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode ;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  met = atoi (argv[0]);
*/
/* RFC3787 section 5.1 */
/*	   
  if (circuit->area && circuit->area->oldmetric == 1 &&
      (met > MAX_NARROW_LINK_METRIC|| met < 1) )
    {
      vty_warning_out (vty, "Invalid metric %d - should be <1-63> "
               "when narrow metric type enabled%s",
               met, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }
*/
/* RFC4444 */
/*
  if (circuit->area && circuit->area->newmetric == 1 &&
      (met > MAX_WIDE_LINK_METRIC || met < 1) )
    {
      vty_warning_out (vty, "Invalid metric %d - should be <1-16777215> "
               "when wide metric type enabled%s",
               met, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }

  circuit->te_metric[0] = met;
  circuit->metrics[0].metric_default = met;

  if (circuit->area)
    lsp_regenerate_schedule (circuit->area, IS_LEVEL_1, 0);

  return CMD_SUCCESS;
}

DEFUN (no_isis_metric_l1,
       no_isis_metric_l1_cmd,
       "no isis metric level-1",
       NO_STR
       "IS-IS commands\n"
       "Set default metric for circuit\n"
       "Specify metric for level-1 routing\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  circuit->te_metric[0] = DEFAULT_CIRCUIT_METRIC;
  circuit->metrics[0].metric_default = DEFAULT_CIRCUIT_METRIC;

  if (circuit->area)
    lsp_regenerate_schedule (circuit->area, IS_LEVEL_1, 0);

  return CMD_SUCCESS;
}

DEFUN (isis_metric_l2,
       isis_metric_l2_cmd,
       "isis metric <1-16777215> level-2",
       "IS-IS commands\n"
       "Set default metric for circuit\n"
       "Default metric value\n"
       "Specify metric for level-2 routing\n")
{
  int met;
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }

  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  met = atoi (argv[0]);
*/
/* RFC3787 section 5.1 */
/*
  if (circuit->area && circuit->area->oldmetric == 1 &&
      (met > MAX_NARROW_LINK_METRIC || met < 1) )
    {
      vty_warning_out (vty, "Invalid metric %d - should be <1-63> "
               "when narrow metric type enabled%s",
               met, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }
*/
/* RFC4444 */
/*
  if (circuit->area && circuit->area->newmetric == 1 &&
      (met > MAX_WIDE_LINK_METRIC || met < 1) )
    {
      vty_warning_out (vty, "Invalid metric %d - should be <1-16777215> "
               "when wide metric type enabled%s",
               met, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }

  circuit->te_metric[1] = met;
  circuit->metrics[1].metric_default = met;

  if (circuit->area)
    lsp_regenerate_schedule (circuit->area, IS_LEVEL_2, 0);

  return CMD_SUCCESS;
}

DEFUN (no_isis_metric_l2,
       no_isis_metric_l2_cmd,
       "no isis metric level-2",
       NO_STR
       "IS-IS commands\n"
       "Set default metric for circuit\n"
       "Specify metric for level-2 routing\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  circuit->te_metric[1] = DEFAULT_CIRCUIT_METRIC;
  circuit->metrics[1].metric_default = DEFAULT_CIRCUIT_METRIC;

  if (circuit->area)
    lsp_regenerate_schedule (circuit->area, IS_LEVEL_2, 0);

  return CMD_SUCCESS;
}
*/
/* end of metrics */

DEFUN(isis_hello_interval,
      isis_hello_interval_cmd,
      "isis hello-interval <1-3600>",
      "IS-IS commands\n"
      "Set Hello interval\n"
      "Hello interval value\n"
      "Holdtime 1 seconds, interval depends on multiplier\n")
{
  int interval;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  interval = atoi(argv[0]);
  if (interval < MIN_HELLO_INTERVAL || interval > MAX_HELLO_INTERVAL)
  {
    vty_warning_out(vty, "Invalid hello-interval %d - should be <1-3600>%s",
                    interval, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (interval * circuit->hello_multiplier[0] > 65535 && interval * circuit->hello_multiplier[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval * circuit->hello_multiplier[0] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval * circuit->hello_multiplier[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_interval[0] = (u_int16_t)interval;
  circuit->hello_interval[1] = (u_int16_t)interval;

  return CMD_SUCCESS;
}

DEFUN(isis_hello_interval_h3c,
      isis_hello_interval_h3c_cmd,
      "isis timer hello <1-3600>",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS timer on interface\n"
      "Set hello packet sending interval\n"
      "Hello interval value (Seconds)\n")
{
  int interval;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  interval = atoi(argv[0]);
  if (interval < MIN_HELLO_INTERVAL || interval > MAX_HELLO_INTERVAL)
  {
    vty_warning_out(vty, "Invalid hello-interval %d - should be <1-3600>%s",
                    interval, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (interval * circuit->hello_multiplier[0] > 65535 && interval * circuit->hello_multiplier[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval * circuit->hello_multiplier[0] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (interval * circuit->hello_multiplier[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_interval[0] = (u_int16_t)interval;
  circuit->hello_interval[1] = (u_int16_t)interval;

  return CMD_SUCCESS;
}

DEFUN(no_isis_hello_interval,
      no_isis_hello_interval_cmd,
      "no isis hello-interval",
      NO_STR
      "IS-IS commands\n"
      "Set Hello interval\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_interval[0] = DEFAULT_HELLO_INTERVAL;
  circuit->hello_interval[1] = DEFAULT_HELLO_INTERVAL;

  return CMD_SUCCESS;
}

DEFUN(no_isis_hello_interval_h3c,
      no_isis_hello_interval_h3c_cmd,
      "undo isis timer hello",
      NO_STR
      "IS-IS commands\n"
      "Set Hello interval\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "get the interface mode timeout!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_interval[0] = DEFAULT_HELLO_INTERVAL;
  circuit->hello_interval[1] = DEFAULT_HELLO_INTERVAL;

  return CMD_SUCCESS;
}

/*
DEFUN (isis_hello_interval_l1,
       isis_hello_interval_l1_cmd,
       "isis hello-interval <1-600> level-1",
       "IS-IS commands\n"
       "Set Hello interval\n"
       "Hello interval value\n"
       "Holdtime 1 second, interval depends on multiplier\n"
       "Specify hello-interval for level-1 IIHs\n")
{
  long interval;
  struct isis_circuit *circuit = isis_circuit_lookup (vty);

  uint8_t ifmode ;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }  


  interval = atoi (argv[0]);
  if (interval < MIN_HELLO_INTERVAL || interval > MAX_HELLO_INTERVAL)
    {
      vty_warning_out (vty, "Invalid hello-interval %ld - should be <1-600>%s",
               interval, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }
  
  if(interval * circuit->hello_multiplier[0] > 65535 )
  {
	vty_error_out(vty,"ISIS Level-1 holding time reached the upper limit (65535)%s",VTY_NEWLINE);
	return CMD_SUCCESS;
  }

  circuit->hello_interval[0] = (u_int16_t) interval;

  return CMD_SUCCESS;
}

DEFUN (no_isis_hello_interval_l1,
       no_isis_hello_interval_l1_cmd,
       "no isis hello-interval level-1",
       NO_STR
       "IS-IS commands\n"
       "Set Hello interval\n"
       "Specify hello-interval for level-1 IIHs\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }

  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }  

  circuit->hello_interval[0] = DEFAULT_HELLO_INTERVAL;

  return CMD_SUCCESS;
}

#if 0 
DEFUN (isis_hello_interval_l2,
       isis_hello_interval_l2_cmd,
       "isis hello-interval <1-600> level-2",
       "IS-IS commands\n"
       "Set Hello interval\n"
       "Hello interval value\n"
       "Holdtime 1 second, interval depends on multiplier\n"
       "Specify hello-interval for level-2 IIHs\n")
{
  long interval;
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }  


  interval = atoi (argv[0]);
  if (interval < MIN_HELLO_INTERVAL || interval > MAX_HELLO_INTERVAL)
    {
      vty_warning_out (vty, "Invalid hello-interval %ld - should be <1-600>%s",
               interval, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }
  
  if(interval * circuit->hello_multiplier[1] > 65535 )
  {
	vty_error_out(vty,"ISIS Level-2 holding time reached the upper limit (65535)%s",VTY_NEWLINE);
	return CMD_SUCCESS;
  }

  circuit->hello_interval[1] = (u_int16_t) interval;

  return CMD_SUCCESS;
}

DEFUN (no_isis_hello_interval_l2,
       no_isis_hello_interval_l2_cmd,
       "no isis hello-interval level-2",
       NO_STR
       "IS-IS commands\n"
       "Set Hello interval\n"
       "Specify hello-interval for level-2 IIHs\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }

  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }  


  circuit->hello_interval[1] = DEFAULT_HELLO_INTERVAL;

  return CMD_SUCCESS;
}
#endif
*/

DEFUN(isis_hello_multiplier,
      isis_hello_multiplier_cmd,
      "isis hello-multiplier <3-100>",
      "IS-IS commands\n"
      "Set multiplier for Hello holding time\n"
      "Hello multiplier value\n")
{
  int mult;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  mult = atoi(argv[0]);
  if (mult < MIN_HELLO_MULTIPLIER || mult > MAX_HELLO_MULTIPLIER)
  {
    vty_warning_out(vty, "Invalid hello-multiplier %d - should be <3-100>%s",
                    mult, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (mult * circuit->hello_interval[0] > 65535 && mult * circuit->hello_interval[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (mult * circuit->hello_interval[0] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (mult * circuit->hello_interval[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_multiplier[0] = (u_int16_t)mult;
  circuit->hello_multiplier[1] = (u_int16_t)mult;

  return CMD_SUCCESS;
}

DEFUN(isis_hello_multiplier_h3c,
      isis_hello_multiplier_h3c_cmd,
      "isis timer holding-multiplier <3-100>",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS timer on interface\n"
      "Set holding multiplier value\n"
      "Holding multiplier value\n")
{
  int mult;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  mult = atoi(argv[0]);
  if (mult < MIN_HELLO_MULTIPLIER || mult > MAX_HELLO_MULTIPLIER)
  {
    vty_warning_out(vty, "Invalid hello-multiplier %d - should be <3-100>%s",
                    mult, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  if (mult * circuit->hello_interval[0] > 65535 && mult * circuit->hello_interval[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (mult * circuit->hello_interval[0] > 65535)
  {
    vty_error_out(vty, "ISIS Level-1 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (mult * circuit->hello_interval[1] > 65535)
  {
    vty_error_out(vty, "ISIS Level-2 holding time reached the upper limit (65535)%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_multiplier[0] = (u_int16_t)mult;
  circuit->hello_multiplier[1] = (u_int16_t)mult;

  return CMD_SUCCESS;
}

DEFUN(no_isis_hello_multiplier,
      no_isis_hello_multiplier_cmd,
      "no isis hello-multiplier",
      NO_STR
      "IS-IS commands\n"
      "Set multiplier for Hello holding time\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_multiplier[0] = DEFAULT_HELLO_MULTIPLIER;
  circuit->hello_multiplier[1] = DEFAULT_HELLO_MULTIPLIER;

  return CMD_SUCCESS;
}

DEFUN(no_isis_hello_multiplier_h3c,
      no_isis_hello_multiplier_h3c_cmd,
      "undo isis timer holding-multiplier",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS timer on interface\n"
      "Set holding multiplier value\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->hello_multiplier[0] = DEFAULT_HELLO_MULTIPLIER;
  circuit->hello_multiplier[1] = DEFAULT_HELLO_MULTIPLIER;

  return CMD_SUCCESS;
}
/*
DEFUN (isis_hello_multiplier_l1,
       isis_hello_multiplier_l1_cmd,
       "isis hello-multiplier <2-100> level-1",
       "IS-IS commands\n"
       "Set multiplier for Hello holding time\n"
       "Hello multiplier value\n"
       "Specify hello multiplier for level-1 IIHs\n")
{
  int mult;
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode ;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"isis get interface mode timeout error!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }

  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  mult = atoi (argv[0]);
  if (mult < MIN_HELLO_MULTIPLIER || mult > MAX_HELLO_MULTIPLIER)
    {
      vty_warning_out (vty, "Invalid hello-multiplier %d - should be <2-100>%s",
               mult, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }
  
  if(mult * circuit->hello_interval[0] > 65535 )
  {
	vty_error_out(vty,"ISIS Level-1 holding time reached the upper limit (65535)%s",VTY_NEWLINE);
	return CMD_SUCCESS;
  }

  circuit->hello_multiplier[0] = (u_int16_t) mult;

  return CMD_SUCCESS;
}

DEFUN (no_isis_hello_multiplier_l1,
       no_isis_hello_multiplier_l1_cmd,
       "no isis hello-multiplier level-1",
       NO_STR
       "IS-IS commands\n"
       "Set multiplier for Hello holding time\n"
       "Specify hello multiplier for level-1 IIHs\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode ;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"isis get interface mode timeout error!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }  


  circuit->hello_multiplier[0] = DEFAULT_HELLO_MULTIPLIER;

  return CMD_SUCCESS;
}

DEFUN (isis_hello_multiplier_l2,
       isis_hello_multiplier_l2_cmd,
       "isis hello-multiplier <2-100> level-2",
       "IS-IS commands\n"
       "Set multiplier for Hello holding time\n"
       "Hello multiplier value\n"
       "Specify hello multiplier for level-2 IIHs\n")
{
  int mult;
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"isis get interface mode timeout error!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  mult = atoi (argv[0]);
  if (mult < MIN_HELLO_MULTIPLIER || mult > MAX_HELLO_MULTIPLIER)
    {
      vty_warning_out (vty, "Invalid hello-multiplier %d - should be <2-100>%s",
               mult, VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }
  
  if(mult * circuit->hello_interval[1] > 65535 )
  {
	vty_error_out(vty,"ISIS Level-2 holding time reached the upper limit (65535)%s",VTY_NEWLINE);
	return CMD_SUCCESS;
  }

  circuit->hello_multiplier[1] = (u_int16_t) mult;

  return CMD_SUCCESS;
}

DEFUN (no_isis_hello_multiplier_l2,
       no_isis_hello_multiplier_l2_cmd,
       "no isis hello-multiplier level-2",
       NO_STR
       "IS-IS commands\n"
       "Set multiplier for Hello holding time\n"
       "Specify hello multiplier for level-2 IIHs\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup (vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if(ifm_get_mode(ifindex, MODULE_ID_ISIS,&ifmode) != 0)
  {
	vty_error_out(vty,"isis get interface mode timeout error!%s", VTY_NEWLINE);			
	return CMD_WARNING;
  }


  if(ifmode != IFNET_MODE_L3)
  {
  	vty_warning_out(vty,"This if mode is not l3!%s",VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit)
  {
  	vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
  	return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
	vty_warning_out (vty, "This interface is not enable!%s", VTY_NEWLINE);
	return CMD_SUCCESS;
  }   

  circuit->hello_multiplier[1] = DEFAULT_HELLO_MULTIPLIER;

  return CMD_SUCCESS;
}
*/
DEFUN(hello_padding_enable,
      hello_padding_enable_cmd,
      "hello padding enable",
      "IS-IS commands\n"
      "Add padding to IS-IS hello packets\n"
      "Pad hello packets\n"
      "<cr>\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->pad_hellos = 1;

  return CMD_SUCCESS;
}

DEFUN(hello_padding_enable_h3c,
      hello_padding_enable_h3c_cmd,
      "isis padding-hello",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Configure to send hello PDUs with padding TLV\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->pad_hellos = 1;

  return CMD_SUCCESS;
}

DEFUN(no_hello_padding_enable,
      no_hello_padding_enable_cmd,
      "no hello padding enable",
      NO_STR
      "IS-IS commands\n"
      "Add padding to IS-IS hello packets\n"
      "Pad hello packets\n"
      "<cr>\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->pad_hellos = 0;

  return CMD_SUCCESS;
}

DEFUN(no_hello_padding_enable_h3c,
      no_hello_padding_enable_h3c_cmd,
      "undo isis padding-hello",
      "Cancel current setting"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Configure to send hello PDUs with padding TLV\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->pad_hellos = 0;

  return CMD_SUCCESS;
}

DEFUN(csnp_interval,
      csnp_interval_cmd,
      "isis csnp-interval <1-3600>",
      "IS-IS commands\n"
      "Set CSNP interval in seconds\n"
      "CSNP interval value\n")
{
  unsigned long interval;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  interval = atol(argv[0]);
  if (interval < MIN_CSNP_INTERVAL || interval > MAX_CSNP_INTERVAL)
  {
    vty_warning_out(vty, "Invalid csnp-interval %lu - should be <1-600>%s",
                    interval, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->csnp_interval[0] = (u_int16_t)interval;
  circuit->csnp_interval[1] = (u_int16_t)interval;

  return CMD_SUCCESS;
}

DEFUN(csnp_interval_h3c,
      csnp_interval_h3c_cmd,
      "isis timer csnp <1-3600>",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS timer on interface\n"
      "Set CSNP packet sending interval\n"
      "CSNP packet sending interval value (Seconds)\n")
{
  unsigned long interval;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  interval = atol(argv[0]);
  if (interval < MIN_CSNP_INTERVAL || interval > MAX_CSNP_INTERVAL)
  {
    vty_warning_out(vty, "Invalid csnp-interval %lu - should be <1-600>%s",
                    interval, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->csnp_interval[0] = (u_int16_t)interval;
  circuit->csnp_interval[1] = (u_int16_t)interval;

  return CMD_SUCCESS;
}

DEFUN(no_csnp_interval,
      no_csnp_interval_cmd,
      "no isis csnp-interval",
      NO_STR
      "IS-IS commands\n"
      "Set CSNP interval in seconds\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->csnp_interval[0] = DEFAULT_CSNP_INTERVAL;
  circuit->csnp_interval[1] = DEFAULT_CSNP_INTERVAL;

  return CMD_SUCCESS;
}

DEFUN(no_csnp_interval_h3c,
      no_csnp_interval_h3c_cmd,
      "undo isis timer csnp",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Set ISIS timer on interface\n"
      "Set CSNP interval in seconds\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->csnp_interval[0] = DEFAULT_CSNP_INTERVAL;
  circuit->csnp_interval[1] = DEFAULT_CSNP_INTERVAL;

  return CMD_SUCCESS;
}

DEFUN(psnp_interval,
      psnp_interval_cmd,
      "isis psnp-interval <1-3600>",
      "IS-IS commands\n"
      "Set PSNP interval in seconds\n"
      "PSNP interval value\n")
{
  unsigned long interval;
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }
  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  interval = atol(argv[0]);
  if (interval < MIN_PSNP_INTERVAL || interval > MAX_PSNP_INTERVAL)
  {
    vty_warning_out(vty, "Invalid psnp-interval %lu - should be <1-120>%s",
                    interval, VTY_NEWLINE);
    return CMD_ERR_AMBIGUOUS;
  }

  circuit->psnp_interval[0] = (u_int16_t)interval;
  circuit->psnp_interval[1] = (u_int16_t)interval;

  return CMD_SUCCESS;
}

DEFUN(no_psnp_interval,
      no_psnp_interval_cmd,
      "no isis psnp-interval",
      NO_STR
      "IS-IS commands\n"
      "Set PSNP interval in seconds\n")
{
  struct isis_circuit *circuit = isis_circuit_lookup(vty);
  uint8_t ifmode;
  uint32_t ifindex;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit->psnp_interval[0] = DEFAULT_PSNP_INTERVAL;
  circuit->psnp_interval[1] = DEFAULT_PSNP_INTERVAL;

  return CMD_SUCCESS;
}

struct cmd_node interface_node = {
    INTERFACE_NODE,
    "%s(config-if)# ",
    1,
};

DEFUN(isis_network,
      isis_network_cmd,
      "isis network p2p",
      "IS-IS commands\n"
      "Set network type\n"
      "point-to-point network type\n")
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  char ifname_tmp[INTERFACE_NAMSIZ];
  uint32_t ifindex;
  vrf_id_t vrf_id = VRF_DEFAULT;
  uint8_t ifmode;
  int level = 0;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp,
                               strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                               vrf_id);
  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_scan_by_ifp(ifp);

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* RFC5309 section 4 */
  if (circuit->circ_type == CIRCUIT_T_P2P)
    return CMD_SUCCESS;

  if (circuit->state != C_STATE_UP)
  {
    circuit->circ_type = CIRCUIT_T_P2P;
    circuit->circ_type_config = CIRCUIT_T_P2P;
  }
  else
  {
    struct isis_area *area = circuit->area;
    if (!if_is_broadcast(circuit->interface))
    {
      vty_warning_out(vty, "isis network point-to-point "
                           "is valid only on broadcast interfaces%s",
                      VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }

    level = circuit->is_type;
    isis_csm_state_change(ISIS_DISABLE, circuit, area);
    isis_circuit_conf_ago(circuit);
    circuit->circ_type = CIRCUIT_T_P2P;
    circuit->circ_type_config = CIRCUIT_T_P2P;
    isis_csm_state_change(ISIS_ENABLE, circuit, area);
    circuit->is_type = level;
  }

  return CMD_SUCCESS;
}

DEFUN(isis_network_h3c,
      isis_network_h3c_cmd,
      "isis circuit-type p2p",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Change the network type of the circuit\n"
      "Change the network type of the circuit to P2P\n")
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  char ifname_tmp[INTERFACE_NAMSIZ];
  uint32_t ifindex;
  vrf_id_t vrf_id = VRF_DEFAULT;
  uint8_t ifmode;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);

  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  //ifp = (struct interface *) vty->index;
  ifindex = (uint32_t)vty->index;

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp,
                               strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                               vrf_id);
  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_scan_by_ifp(ifp);

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* RFC5309 section 4 */
  if (circuit->circ_type == CIRCUIT_T_P2P)
    return CMD_SUCCESS;

  if (circuit->state != C_STATE_UP)
  {
    circuit->circ_type = CIRCUIT_T_P2P;
    circuit->circ_type_config = CIRCUIT_T_P2P;
  }
  else
  {
    struct isis_area *area = circuit->area;
    if (!if_is_broadcast(circuit->interface))
    {
      vty_warning_out(vty, "isis network point-to-point "
                           "is valid only on broadcast interfaces%s",
                      VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }

    isis_csm_state_change(ISIS_DISABLE, circuit, area);
    isis_circuit_conf_ago(circuit);
    circuit->circ_type = CIRCUIT_T_P2P;
    circuit->circ_type_config = CIRCUIT_T_P2P;
    isis_csm_state_change(ISIS_ENABLE, circuit, area);
  }

  return CMD_SUCCESS;
}

DEFUN(no_isis_network,
      no_isis_network_cmd,
      "no isis network p2p",
      NO_STR
      "IS-IS commands\n"
      "Set network type for circuit\n"
      "point-to-point network type\n")
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  char ifname_tmp[INTERFACE_NAMSIZ];
  uint32_t ifindex;
  vrf_id_t vrf_id = VRF_DEFAULT;
  uint8_t ifmode;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  //ifp = (struct interface *) vty->index;
  ifindex = (uint32_t)vty->index;

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp,
                               strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                               vrf_id);
  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_scan_by_ifp(ifp);

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* RFC5309 section 4 */
  if (circuit->circ_type == CIRCUIT_T_BROADCAST)
    return CMD_SUCCESS;

  if (circuit->state != C_STATE_UP)
  {
    circuit->circ_type = CIRCUIT_T_BROADCAST;
    circuit->circ_type_config = CIRCUIT_T_BROADCAST;
  }
  else
  {
    struct isis_area *area = circuit->area;
    if (circuit->interface &&
        !if_is_broadcast(circuit->interface))
    {
      vty_warning_out(vty, "no isis network point-to-point "
                           "is valid only on broadcast interfaces%s",
                      VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }

    isis_csm_state_change(ISIS_DISABLE, circuit, area);
    isis_circuit_conf_ago(circuit);
    circuit->circ_type = CIRCUIT_T_BROADCAST;
    circuit->circ_type_config = CIRCUIT_T_BROADCAST;
    isis_csm_state_change(ISIS_ENABLE, circuit, area);
  }

  return CMD_SUCCESS;
}

DEFUN(no_isis_network_h3c,
      no_isis_network_h3c_cmd,
      "undo isis circuit-type",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Change the network type of the circuit\n")
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  char ifname_tmp[INTERFACE_NAMSIZ];
  uint32_t ifindex;
  vrf_id_t vrf_id = VRF_DEFAULT;
  uint8_t ifmode;

  ifindex = (uint32_t)vty->index;
  VTY_CHECK_NM_INTERFACE(ifindex);
  if (ifm_get_mode(ifindex, MODULE_ID_ISIS, &ifmode) != 0)
  {
    vty_error_out(vty, "isis get interface mode timeout error!%s", VTY_NEWLINE);
    return CMD_WARNING;
  }

  if (ifmode != IFNET_MODE_L3)
  {
    vty_warning_out(vty, "This if mode is not l3!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  //ifp = (struct interface *) vty->index;
  ifindex = (uint32_t)vty->index;

  ifm_get_name_by_ifindex(ifindex, ifname_tmp);

  /*according to the interface name look up the struct pointer ifp */
  ifp = if_get_by_name_len_vrf(ifname_tmp,
                               strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                               vrf_id);
  if (!ifp)
  {
    vty_warning_out(vty, "Invalid interface %s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  circuit = circuit_scan_by_ifp(ifp);

  if (!circuit)
  {
    vty_warning_out(vty, "circuit is NULL!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  if (!circuit->ip_router && !circuit->ipv6_router)
  {
    vty_warning_out(vty, "This interface is not enable!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
  }

  /* RFC5309 section 4 */
  if (circuit->circ_type == CIRCUIT_T_BROADCAST)
    return CMD_SUCCESS;

  if (circuit->state != C_STATE_UP)
  {
    circuit->circ_type = CIRCUIT_T_BROADCAST;
    circuit->circ_type_config = CIRCUIT_T_BROADCAST;
  }
  else
  {
    struct isis_area *area = circuit->area;
    if (circuit->interface &&
        !if_is_broadcast(circuit->interface))
    {
      vty_warning_out(vty, "no isis network point-to-point "
                           "is valid only on broadcast interfaces%s",
                      VTY_NEWLINE);
      return CMD_ERR_AMBIGUOUS;
    }

    isis_csm_state_change(ISIS_DISABLE, circuit, area);
    isis_circuit_conf_ago(circuit);
    circuit->circ_type = CIRCUIT_T_BROADCAST;
    circuit->circ_type_config = CIRCUIT_T_BROADCAST;
    isis_csm_state_change(ISIS_ENABLE, circuit, area);
  }

  return CMD_SUCCESS;
}

int isis_if_new_hook(struct interface *ifp)
{
  return 0;
}

int isis_if_delete_hook(struct interface *ifp)
{
  struct isis_circuit *circuit;
  /* Clean up the circuit data */
  if (ifp && ifp->info)
  {
    circuit = ifp->info;
    isis_csm_state_change(IF_DOWN_FROM_Z, circuit, circuit->area);
    isis_csm_state_change(ISIS_DISABLE, circuit, circuit->area);
  }

  return 0;
}

void isis_vlanif_init(void)
{
  /* "isis network" commands. */

  install_element(VLANIF_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_circuit_type_cmd, CMD_SYNC);
  //install_element (PHYSICAL_IF_NODE, &no_isis_circuit_type_cmd);

  install_element(VLANIF_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

#if 0
	install_element (PHYSICAL_IF_NODE, &isis_hello_padding_cmd);
	install_element (PHYSICAL_IF_NODE, &no_isis_hello_padding_cmd);
#endif
  install_element(VLANIF_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(VLANIF_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_h3c_cmd);
  install_element(VLANIF_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(VLANIF_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);
}

void isis_l3if_init(void)
{
  ifm_l3if_init(isis_interface_config_write);

  /* "isis network" commands. */

  install_element(PHYSICAL_IF_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_circuit_type_cmd, CMD_SYNC);
  //install_element (PHYSICAL_IF_NODE, &no_isis_circuit_type_cmd);

  install_element(PHYSICAL_IF_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

#if 0
	install_element (PHYSICAL_IF_NODE, &isis_hello_padding_cmd);
	install_element (PHYSICAL_IF_NODE, &no_isis_hello_padding_cmd);
#endif
  install_element(PHYSICAL_IF_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(PHYSICAL_IF_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_h3c_cmd);
  install_element(PHYSICAL_IF_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_IF_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);
}

void isis_l2if_init(void)
{
  install_element(PHYSICAL_SUBIF_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_circuit_type_cmd, CMD_SYNC);
  //install_element (PHYSICAL_SUBIF_NODE, &no_isis_circuit_type_cmd);

  install_element(PHYSICAL_SUBIF_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(PHYSICAL_SUBIF_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_h3c_cmd);
  install_element(PHYSICAL_SUBIF_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(PHYSICAL_SUBIF_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);
}

void isis_l2trunk_init(void)
{
  //ifm_l3if_init(isis_interface_config_write);

  /* "isis network" commands. */

  install_element(TRUNK_IF_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_circuit_type_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(TRUNK_IF_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_h3c_cmd);
  install_element(TRUNK_IF_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_IF_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);
}

void isis_l3trunk_init(void)
{
  install_element(TRUNK_SUBIF_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_circuit_type_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(TRUNK_SUBIF_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_h3c_cmd);
  install_element(TRUNK_SUBIF_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(TRUNK_SUBIF_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);
}

void isis_loopback_init(void)
{
  install_element(LOOPBACK_IF_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_circuit_type_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(LOOPBACK_IF_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_h3c_cmd);
  install_element(LOOPBACK_IF_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(LOOPBACK_IF_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);
}

void isis_circuit_init()
{

  /* Initialize Zebra interface data structure */
  if_add_hook(IF_NEW_HOOK, isis_if_new_hook);
  if_add_hook(IF_DELETE_HOOK, isis_if_delete_hook);

  /* Install interface node */
  install_node(&interface_node, NULL);
  install_element(CONFIG_NODE, &interface_cmd, CMD_SYNC);
  install_element(CONFIG_NODE, &no_interface_cmd, CMD_SYNC);

  install_default(INTERFACE_NODE);
#if 1
  install_element(INTERFACE_NODE, &interface_desc_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_interface_desc_cmd, CMD_SYNC);
#if 1
  install_element(INTERFACE_NODE, &isis_enable_instance_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_enable_instance_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_enable_ipv6_instance_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_enable_ipv6_instance_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_passive_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_passive_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_circuit_type_cmd, CMD_SYNC);
  //install_element (INTERFACE_NODE, &no_isis_circuit_type_cmd);

  install_element(INTERFACE_NODE, &isis_passwd_clear_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_passwd_md5_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_passwd_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_priority_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_priority_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_priority_l1_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_priority_l1_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_priority_l2_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_priority_l2_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_metric_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_metric_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_hello_interval_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_hello_interval_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_hello_multiplier_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_hello_multiplier_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &hello_padding_enable_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_hello_padding_enable_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &csnp_interval_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_csnp_interval_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &psnp_interval_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_psnp_interval_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_network_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_network_cmd, CMD_SYNC);

  install_element(INTERFACE_NODE, &isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_enable_instance_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_circuit_type_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_passwd_clear_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_passwd_md5_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_passwd_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_priority_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_priority_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_priority_l1_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_priority_l2_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_metric_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_metric_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_hello_interval_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_hello_multiplier_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_hello_padding_enable_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_csnp_interval_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &isis_network_h3c_cmd, CMD_SYNC);
  install_element(INTERFACE_NODE, &no_isis_network_h3c_cmd, CMD_SYNC);

#endif
#endif
  //isis_vlanif_init();

  isis_l3if_init();
  isis_l2if_init();

  isis_l2trunk_init();
  isis_l3trunk_init();

  isis_loopback_init();
  isis_vlanif_init();
}
