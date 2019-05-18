/*
 * IS-IS Rout(e)ing protocol - isisd.h   
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

#ifndef ISISD_H
#define ISISD_H

#define ISISD_VERSION "0.0.7"

#include "isisd/isis_redist.h"
#include "isisd/isis_route.h"


/* uncomment if you are a developer in bug hunt */
/* #define EXTREME_DEBUG  */
/* #define EXTREME_TLV_DEBUG */
#define DEFAULT_DISTANCE 115
#define ISIS_DEFAULT_BANDWIDTH 1000
#define ISIS_VPN_DEFAULT 0
#define SHOW_ISIS_ROUTE_V4_HEADER \
  "Codes: R - RIP, O - OSPF, I - IS-IS, B - BGP%s",VTY_NEWLINE

#define ISIS_GET_IPV6_PREFIX(NAME,V,STR)                                       \
do {                                                                             \
  int retv;                                                                   \
  retv = str2prefix_ipv6 ((STR), &(V));                                       \
  if (retv <= 0)                                                              \
    {                                                                         \
      vty_out (vty, "%% Invalid %s value%s", NAME, VTY_NEWLINE);              \
      return CMD_WARNING;                                                     \
    }                                                                         \
} while (0)


#define VTY_CHECK_ISIS_ADDRESS(NAME,STR)                                      \
{                                                                             \
    int pnt ;                                                                                                                                       \
    int a,b,c,d;                                                                                                                                \
    pnt = sscanf(STR,"%d.%d.%d.%d",&a,&b,&c,&d);                                                                \
    if(pnt != 4)                                                                                                                                \
    {                                                                                                                                                       \
        vty_error_out(vty, "Please specify %s by A.B.C.D%s",NAME,VTY_NEWLINE);     \
        return CMD_WARNING;                                                                                                             \
    }                                                                                                                                                       \
}   


/* ISIS master for system wide configuration and variables. */
struct isis_master
{
    TIMERID t_thread_routefifo;	/*�첽����route thread*/
  struct fifo routefifo;	/*�첽����route fifo*/ 
  struct fifo routefifo_rcv;	/*this fifo is for receiving route from route*/
  struct fifo ifm_event_fifo;	/*�첽�ӿ��¼�*/
};

extern struct isis_master isis_m;

struct isis
{
  u_long process_id;
  //int sysid_set;
  //u_char sysid[ISIS_SYS_ID_LEN];	/* SystemID for this IS */
  u_int32_t router_id;          /* Router ID from zebra */
  struct list *area_list;	/* list of IS-IS areas */
  struct list *init_circ_list;
  struct list *nexthops;	/* IPv4 next hops from this IS */
#if 1
  struct list *nexthops6;	/* IPv6 next hops from this IS */
#endif				/* 0 */
  u_char max_area_addrs;	/* maximumAreaAdresses */
  struct area_addr *man_area_addrs;	/* manualAreaAddresses */
  u_int32_t debugs;		/* bitmap for debug */
  time_t uptime;		/* when did we start */
    TIMERID t_dync_clean;	/* dynamic hostname cache cleanup thread */
  struct route_table *ext_info[REDIST_PROTOCOL_COUNT];
};

extern struct isis *isis;


#define DEFAULT_LSP_MTU 1497
struct isis_area
{
  struct isis *isis;				              /* back pointer */
  dict_t *lspdb[ISIS_LEVELS];			          /* link-state dbs */
  u_char sysid[ISIS_SYS_ID_LEN];
  int sysid_set;
  uint32_t ref_bandwidth_ipv4; 
  uint32_t ref_bandwidth_ipv6; 
  struct isis_spftree *spftree[ISIS_LEVELS];	  /* The v4 SPTs */
  struct route_table *route_table[ISIS_LEVELS];	  /* IPv4 routes */
  u_char force_l2_lsp_refresh;	                  /* l1 route change, L-1-2 router must refresh l2 lsp */
  struct isis_spftree *spftree6[ISIS_LEVELS];	  /* The v6 SPTs */
  struct route_table *route_table6[ISIS_LEVELS];  /* IPv6 routes */
  unsigned int lsp_mtu;				              /* Size of LSPs to generate */
  struct list *circuit_list;	                  /* IS-IS circuits */
  struct flags flags;
  TIMERID t_tick;	/* LSP walker */
  TIMERID t_lsp_refresh[ISIS_LEVELS];
  /* t_lsp_refresh is used in two ways:
   * a) regular refresh of LSPs
   * b) (possibly throttled) updates to LSPs
   *
   * The lsp_regenerate_pending flag tracks whether the timer is active
   * for the a) or the b) case.
   *
   * It is of utmost importance to clear this flag when the timer is
   * rescheduled for normal refresh, because otherwise, updates will
   * be delayed until the next regular refresh.
   */
  int lsp_regenerate_pending[ISIS_LEVELS];

  /*
   * Configurables 
   */
  struct isis_passwd area_passwd;
  struct isis_passwd domain_passwd;
  /* do we support dynamic hostnames?  */
  char dynhostname;
  /* do we support new style metrics?  */
  char newmetric;
  char oldmetric;
  char compatible;
  /* identifies the routing instance   */
  char *area_tag;
  /* area addresses for this area      */
  struct list *area_addrs;
  u_int16_t max_lsp_lifetime[ISIS_LEVELS];
  char is_type;			/* level-1 level-1-2 or level-2-only */
  /* are we overloaded? */
  char overload_bit;
  /* L1/L2 router identifier for inter-area traffic */
  char attached_bit;
  u_int16_t lsp_refresh[ISIS_LEVELS];
  /*the interval will be change when set lifetime , when no life time is setted ,refresh interval must be recover */
  u_int16_t lsp_refresh_ago;
  /* minimum time allowed before lsp retransmission */
  u_int16_t lsp_gen_interval[ISIS_LEVELS];
  /* min interval between between consequtive SPFs */
  u_int16_t min_spf_interval[ISIS_LEVELS];
  /* the percentage of LSP mtu size used, before generating a new frag */
  int lsp_frag_threshold;
  int ip_circuits;
  int force_spf_l1;
  int force_spf_l2; 

  int gr;
  u_int16_t gr_interval;
  u_int16_t vpn ;
  int distance4;
  int distance6;
  int area_import_to_l1;
  int area_import_to_l2;
  
  int area_import_to_l1_v6;
  int area_import_to_l2_v6;

  struct list *summary;
  struct list *summary_l1;
  struct list *summary_l2;
  //struct list *summary_l12;
  //struct route_table *route_table;
  
  struct list *summary_ipv6;
  struct list *summary_ipv6_l1;
  struct list *summary_ipv6_l2;
  
  /* logging adjacency changes? */
  u_char log_adj_changes;
  int ipv6_circuits;
  
  int force_spf_v6_l1;
  int force_spf_v6_l2; 
  /* Counters */
  u_int32_t circuit_state_changes;
  //struct isis_redist redist_settings[REDIST_PROTOCOL_COUNT][ISIS_LEVELS];
  //struct list * redist_settings_l1;
  //struct list * redist_settings_l2;
  struct list * redist_settings;
  struct list * redist_settings_v6;

  struct route_table *ext_reach[REDIST_PROTOCOL_COUNT][ISIS_LEVELS];

#ifdef TOPOLOGY_GENERATE
  struct list *topology;
  u_char topology_baseis[ISIS_SYS_ID_LEN];  /* IS for the first IS emulated. */
  char *topology_basedynh;                /* Dynamic hostname base. */
  char top_params[200];                   /* FIXME: what is reasonable? */
#endif /* TOPOLOGY_GENERATE */
};

void isis_init (void);
void isis_new(unsigned long);
struct isis_area *isis_area_create(const char *,u_int16_t vpn);
struct isis_area *isis_area_lookup (const char *);
int isis_area_get (struct vty *vty, const char *area_tag,u_int16_t vpn);
void print_debug(struct vty *, int, int);
void show_l2_route(struct vty *vty,struct isis_area *area);
void show_l1_route(struct vty *vty,struct isis_area *area);

int summary_ipv6_add(struct prefix_ipv6 *p, struct isis_area *area, int level);
int summary_ipv6_list_add(struct prefix_ipv6 *p, struct isis_area *area, int level);
void summary_ipv6_regenerate(struct isis_area *area, int level, struct prefix_ipv6 *p);
int summary_ipv6_extend_check(struct isis_area *area, int level, struct prefix_ipv6 *p);
void summary_ipv6_lsp_list_regenerate(struct isis_area *area, int level, struct prefix_ipv6 *p);
int summary_ipv6_route_table_check(struct isis_area *area,int level,struct prefix_ipv6 *p);
int summary_ipv6_del(struct prefix_ipv6 *p, struct isis_area *area, int level);


int summary_add(struct prefix_ipv4 *p, struct isis_area *area, int level);
void summary_regenerate(struct isis_area *area, int level, struct prefix_ipv4 *p);
int summary_del(struct prefix_ipv4 *p, struct isis_area *area, int level);
int summary_list_add(struct prefix_ipv4 *p, struct isis_area *area, int level);
int summary_extend_check(struct isis_area *area, int level, struct prefix_ipv4 *p);
int summary_route_table_check(struct isis_area *area,int level,struct prefix_ipv4 *p);
void summary_lsp_list_regenerate(struct isis_area *area, int level, struct prefix_ipv4 *p);
int isis_set_interface_autocost(struct isis_area * area, uint32_t refbw , int afi);

void isis_route_validate_distance(struct isis_area *area, int family);
int show_isis_instance_interface_common (struct vty *vty, struct isis_area *area, char detail);
int show_isis_instance_neighbor_common (struct vty *vty, struct isis_area *area, char detail);
void isis_redist_reconf(struct isis_area * area);
void isis_password_encode (char *dpw, char *spw);
void isis_password_decode (char *dpw, char *spw);
int isis_area_destroy_without_vty (const char *area_tag);
void isis_circuit_metric_reconf(struct isis_area * area);
void isis_show_route (struct vty *vty,struct route_node *rn,struct isis_route_info *route_info,struct isis_area *area);
void isis_show_ipv6_route (struct vty *vty,struct route_node *rn,struct isis_route_info *route_info,struct isis_area *area);
void show_ipv6_route(struct vty *vty,struct isis_area *area,int level);



/* Master of threads. */
extern struct thread_master *master;
/*
#define DEBUG_ADJ_PACKETS                (1<<0)
#define DEBUG_CHECKSUM_ERRORS            (1<<1)
#define DEBUG_LOCAL_UPDATES              (1<<2)
#define DEBUG_PROTOCOL_ERRORS            (1<<3)
#define DEBUG_SNP_PACKETS                (1<<4)
#define DEBUG_UPDATE_PACKETS             (1<<5)
#define DEBUG_SPF_EVENTS                 (1<<6)
#define DEBUG_SPF_STATS                  (1<<7)
#define DEBUG_SPF_TRIGGERS               (1<<8)
#define DEBUG_RTE_EVENTS                 (1<<9)
#define DEBUG_EVENTS                     (1<<10)
#define DEBUG_ZEBRA                      (1<<11)
#define DEBUG_PACKET_DUMP                (1<<12)
#define DEBUG_LSP_GEN                    (1<<13)
#define DEBUG_LSP_SCHED                  (1<<14)
*/

/* 按位定义debug信息的类型 */
#define ISIS_DEBUG_TYPE_HELLO		(1 << 0)
#define ISIS_DEBUG_TYPE_SPF		    (1 << 1)
#define ISIS_DEBUG_TYPE_LSP		    (1 << 2)
#define ISIS_DEBUG_TYPE_MSG		    (1 << 3)
#define ISIS_DEBUG_TYPE_EVENTS		(1 << 4)
#define ISIS_DEBUG_TYPE_ROUTE		(1 << 5)
#define ISIS_DEBUG_TYPE_NEIGHBOR	(1 << 6)
#define ISIS_DEBUG_TYPE_CSNP	    (1 << 7)
#define ISIS_DEBUG_TYPE_PSNP	    (1 << 8)
#define ISIS_DEBUG_TYPE_RECEIVE		(1 << 9)
#define ISIS_DEBUG_TYPE_SEND		(1 << 10)
#define ISIS_DEBUG_TYPE_TLV         (1 << 11)

#define ISIS_DEBUG_TYPE_ALL			(ISIS_DEBUG_TYPE_HELLO |\
									 ISIS_DEBUG_TYPE_SPF |\
									 ISIS_DEBUG_TYPE_LSP |\
									 ISIS_DEBUG_TYPE_MSG |\
									 ISIS_DEBUG_TYPE_EVENTS |\
									 ISIS_DEBUG_TYPE_ROUTE |\
									 ISIS_DEBUG_TYPE_NEIGHBOR |\
									 ISIS_DEBUG_TYPE_CSNP |\
									 ISIS_DEBUG_TYPE_PSNP |\
									 ISIS_DEBUG_TYPE_RECEIVE |\
									 ISIS_DEBUG_TYPE_TLV |\
									 ISIS_DEBUG_TYPE_SEND)	//所有debug
#define lsp_debug(...) \
  do \
    { \
      if (isis->debugs & ISIS_DEBUG_TYPE_LSP ) \
        zlog_debug(ISIS_DEBUG_TYPE_LSP,__VA_ARGS__); \
    } \
  while (0)

#define sched_debug(...) \
  do \
    { \
      if (isis->debugs & ISIS_DEBUG_TYPE_LSP ) \
        zlog_debug(ISIS_DEBUG_TYPE_LSP,__VA_ARGS__); \
    } \
  while (0)

#define THREAD_TIMER_ON_NEW(master,timerId,func,arg,time) \
	do { \
	  if (! timerId) \
		timerId = high_pre_timer_add (master, LIB_TIMER_TYPE_NOLOOP ,func, arg, time * 1000); \
	} while (0)
  
#define THREAD_TIMER_MSEC_ON_NEW(master,timerId,func,arg,time) \
	do { \
	  if (! timerId) \
		timerId = high_pre_timer_add (master, LIB_TIMER_TYPE_NOLOOP, func, arg, time); \
	} while (0)

#define THREAD_TIMER_OFF_NEW(timerId) \
	do{ \
		if(timerId) \
		{ \
			high_pre_timer_delete(timerId); \
			timerId = 0; \
		} \
	}while(0)
#endif /* ISISD_H */
