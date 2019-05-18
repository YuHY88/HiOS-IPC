/*
 * Soft:        Vrrpd is an implementation of VRRPv2 as specified in rfc2338.
 *              VRRP is a protocol which elect a master server on a LAN. If the
 *              master fails, a backup server takes over.
 *              The original implementation has been made by jerome etienne.
 *
 * Version:     $Id: vrrpd.h,v 1.13 2004/06/16 07:10:25 mjh Exp $
 *
 * Author:      Jerome Etienne, <jetienne@arobas.net>
 *
 * Contributor: Alexandre Cassen, <acassen@linux-vs.org>
 *
 * Changes:
 *              Alexandre Cassen : 2001/05/23 :
 *               <+> Added IPSEC AH sequence number counter in vrrp_rt.
 *
 *              This program is distributed in the hope that it will be useful, 
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *              See the GNU General Public License for more details.
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 */

#ifndef __VRRP_H__
#define __VRRP_H__

/* system include */
#include <stdint.h>
#include "linklist.h"
#include "utils.h"

#define VRRPD_VERSION	"0.6"


/* local include */
#include "iproute.h"
#include "ipaddr.h"
#include "ipsecah.h"

typedef struct {	/* rfc2338.5.1 */
	uint8_t		vers_type;	/* 0-3=type, 4-7=version */
	uint8_t		vrid;		/* virtual router id */
	uint8_t		priority;	/* router priority */
	uint8_t		naddr;		/* address counter */
	uint8_t		auth_type;	/* authentification type */
	uint8_t		adver_int;	/* advertissement interval(in sec) */
	uint16_t	chksum;		/* checksum (ip-like one) */

	/* here <naddr> ip addresses */
        /* here authentification infos */
} vrrp_pkt;

/* protocol constants */
#define INADDR_VRRP_GROUP 0xe0000012	/* multicast addr - rfc2338.5.2.2 */
#define VRRP_IP_TTL	255	/* in and out pkt ttl -- rfc2338.5.2.3 */
#define IPPROTO_VRRP	112	/* IP protocol number -- rfc2338.5.2.4*/
#define VRRP_VERSION	2	/* current version -- rfc2338.5.3.1 */
#define VRRP_PKT_ADVERT	1	/* packet type -- rfc2338.5.3.2 */
#define VRRP_PRIO_OWNER	255	/* priority of the ip owner -- rfc2338.5.3.4 */
#define VRRP_PRIO_DFL	100	/* default priority -- rfc2338.5.3.4 */
#define VRRP_PRIO_STOP	0	/* priority to stop -- rfc2338.5.3.4 */
#define VRRP_AUTH_NONE	0	/* no authentification -- rfc2338.5.3.6 */
#define VRRP_AUTH_PASS	1	/* password authentification -- rfc2338.5.3.6 */
#define VRRP_AUTH_AH	2	/* AH(IPSec) authentification - rfc2338.5.3.6 */
#define VRRP_ADVER_DFL	1	/* advert. interval (in sec) -- rfc2338.5.3.7 */
#define VRRP_PREEMPT_DFL 1	/* rfc2338.6.1.2.Preempt_Mode */

#define VRRP_AUTH_LEN	8


/* parameters per interface -- rfc2338.6.1.1 */
typedef struct {
      uint32_t        ifindex;        /* interface index */
      uint32_t        ipaddr;         /* interface ip address */
      int             masklen;
      int	      hw_type;
      u_char          hwaddr[ETH_ALEN+1];
      char            ifname[INTERFACE_NAMSIZ + 1]; /* interface name */

      int	      auth_type;	/* authentification type: VRRP_AUTH_* */
      uint8_t	      auth_data[VRRP_AUTH_LEN + 1];	/* authentification data */
      seq_counter     ipsecah_counter; /* IPSEC AH counter def --rfc2402.3.3.2 */

      int 	      up;       /*interface is up*/
      int             sockfd;         /* the socket descriptor */
      struct thread   *read_thread;

      struct list     *vrt_list;
      //struct          rt_entry *rt;   /* interface routing table */
} vrrp_if;


/*vrrp router ip address */
typedef struct {
	struct in_addr  inaddr;
	//int 	        masklen;
	int		deletable;	/* TRUE if one of my primary addr */
} vip_addr;


/* parameters per virtual router -- rfc2338.6.1.2 */
typedef struct {	
	int	vrid;		/* virtual id. from 1(!) to 255 */
	int	priority;	/* priority value */
	int 	cfg_priority;
	int 	ip_owner;
	int	state;		/* internal state (init/backup/master) */
	//int	enable;
	int	preempt;	/* true if a higher prio preempt a lower one */
	int	no_vmac;	/* dont handle the virtual MAC --rfc2338.7.3 */
	struct  list *vaddr;    /* point on the ip address array */

	uint8_t vmac[6];        /* virtual mac for vrrp router */

	int	adver_int;	/* delay between advertisements(in sec) */	
	int	ms_down_time;

        struct thread *down_timer;
	struct thread *adver_timer;

	vrrp_if	*vif; 	    	/* interface parameters */
} vrrp_rt;

/* VRRP state machine -- rfc2338.6.4 */
#define VRRP_STATE_INIT	1	/* rfc2338.6.4.1 */
#define VRRP_STATE_BACK	2	/* rfc2338.6.4.2 */
#define VRRP_STATE_MAST	3	/* rfc2338.6.4.3 */

#define VRRP_AUTH_LEN	8

#define VRRP_IS_BAD_VID(id) ((id)<1 || (id)>255)	/* rfc2338.6.1.vrid */
#define VRRP_IS_BAD_PRIORITY(p) ((p)<1 || (p)>255)	/* rfc2338.6.1.prio */
#define VRRP_IS_BAD_ADVERT_INT(d) ((d)<1)

#if 0
/* use the 'tcp sequence number arithmetic' to handle the wraparound.
** VRRP_TIMER_SUB: <0 if t1 precedes t2, =0 if t1 equals t2, >0 if t1 follows t2
*/
#define VRRP_TIMER_SET( val, delta )	(val) = VRRP_TIMER_CLK() + (delta)
#define VRRP_TIMER_SUB( t1, t2 ) ((int32_t)(((uint32_t)t1)-((uint32_t)t2)))
#define VRRP_TIMER_DELTA( val )		VRRP_TIMER_SUB( val, VRRP_TIMER_CLK() )
#define VRRP_TIMER_EXPIRED( val )	((val) && VRRP_TIMER_DELTA(val)<=0)
#define VRRP_TIMER_CLR( val ) 		(val) = 0
#define VRRP_TIMER_IS_RUNNING( val )	(val)
#endif

#define VRRP_TIMER_SKEW( srv ) ((256-(srv)->priority)*(srv)->adver_int/256) 

#define VRRP_MIN( a , b )	( (a) < (b) ? (a) : (b) )
//#define VRRP_MAX( a , b )	( (a) > (b) ? (a) : (b) )

/* vrrpd_main.c */
void vrrp_init(void);
void vrrp_die();
struct thread_master *master;

/* vrrpd.c */
vrrp_rt *vrrp_router[255];
unsigned char vrrp_hwaddr[6];  //WORK: lame hardcoded for ethernet

void init_virtual_srv( vrrp_rt *vsrv );
void state_goto_master( vrrp_rt *vsrv );
void state_leave_master( vrrp_rt *vsrv, int advF );
void state_init( vrrp_rt *vsrv );

void vrrp_stop_all_routers();
void vrrp_router_disable(vrrp_rt *vsrv);
int vrrp_router_enable(vrrp_rt *vsrv, vrrp_if *vif);
int send_vg_info (uint32_t vip, u_char *vmac, int vid, uint32_t ifidx, int vopt);
int open_send_sock ();
void close_send_sock();
void close_vrrp_sock(vrrp_if *vif);
int open_vrrp_sock( vrrp_if *vif );
int vrrp_is_owner(uint32_t ipaddr, vrrp_rt *vsrv);
int vrrp_check_samenet(vrrp_rt *vsrv, vrrp_if *vif);
int check_vrrp_config(vrrp_rt *vsrv);


/* iprout.c */
char *ip_ntoa(uint32_t ip);


#endif	/* __VRRP_H__ */


