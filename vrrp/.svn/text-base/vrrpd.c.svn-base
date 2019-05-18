/*
 * Soft:        Vrrpd is an implementation of VRRPv2 as specified in rfc2338.
 *              VRRP is a protocol which elect a master server on a LAN. If the
 *              master fails, a backup server takes over.
 *              The original implementation has been made by jerome etienne.
 *
 * Version:     $Id: vrrpd.c,v 1.41 2004/08/02 10:03:54 mjh Exp $
 *
 * Author:      Jerome Etienne, <jetienne@arobas.net>
 *
 * Contributor: Alexandre Cassen, <acassen@linux-vs.org>
 *
 * Changes:
 *              Alexandre Cassen : 2001/05/23 :
 *               <+> Added IPSEC-AH support using HMAC-96bits digest with
 *                   anti-replay. 
 *                   -- rfc2401 / rfc2402 / rfc2102
 *                      draft-paridaens-xcast-sec-framework-01.txt
 *               <+> Added manipulated buffer memory allocation in state_mast
 *                   & state_back.
 *               <+> Added incoming packet check integrity for VRID.
 *                   The ipaddr asssociated with a VRID must be the same as 
 *                   the ones present in the MASTER VRRP advert.
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

/* system include */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
            
#include <zebra.h>
#include "version.h"
#include "getopt.h"
#include "thread.h"
#include "if.h"
#include "command.h"
#include "memory.h"
#include "prefix.h"
#include "keychain.h"
#include "linklist.h"

/* local include */
#include "vrrpd.h"

int sock_send = -1;

//#define PKT_BUFFER_LEN 1500
//static char pkt_buffer[PKT_BUFFER_LEN];

int ip_id = 0;  /* to have my own ip_id creates collision with kernel ip->id
                 ** but it should be ok because the packets are unlikely to be
                 ** fragmented (they are non routable and small) */
                /* WORK: this packet isnt routed, i can check the outgoing MTU
                 ** to warn the user only if the outoing mtu is too small */


static void master_to_back( vrrp_rt *vsrv );
static void mast_recv_pkt (vrrp_rt *vsrv, vrrp_pkt *hd, struct iphdr *iph );
static void back_recv_pkt (vrrp_rt *vsrv, vrrp_pkt *hd );
static int down_timeout (struct thread *th);
static int adver_timeout (struct thread *th);
static int sock_read (struct thread *th);


#if 0
uint32_t VRRP_TIMER_CLK( void )
{
	struct timeval tv;
        gettimeofday(&tv, NULL );
        return tv.tv_sec*VRRP_TIMER_HZ+tv.tv_usec;
}
#endif

/****************************************************************
 NAME  : in_csum        00/05/10 20:12:20
 AIM  : compute a IP checksum
 REMARK  : from kuznet's iputils
****************************************************************/
static u_short in_csum( u_short *addr, int len, u_short csum)
{
  register int nleft = len;
  const u_short *w = addr;
  register u_short answer;
  register int sum = csum;

  /*
   *  Our algorithm is simple, using a 32 bit accumulator (sum),
   *  we add sequential 16 bit words to it, and at the end, fold
   *  back all the carry bits from the top 16 bits into the lower
   *  16 bits.
   */
  while (nleft > 1)  {
    sum += *w++;
    nleft -= 2;
  }

  /* mop up an odd byte, if necessary */
  if (nleft == 1)
    sum += htons(*(u_char *)w << 8);

  /*
   * add back carry outs from top 16 bits to low 16 bits
   */
  sum = (sum >> 16) + (sum & 0xffff);  /* add hi 16 to low 16 */
  sum += (sum >> 16);      /* add carry */
  answer = ~sum;           /* truncate to 16 bits */

  return (answer);
}


/****************************************************************
 NAME  : ipaddr_ops        00/02/08 06:51:32
 AIM  :
 REMARK  :
****************************************************************/
static int ipaddr_ops( vrrp_rt *vsrv, int addF )
{
  int  err  = 0;
  struct listnode *node;
  vip_addr *vaddr;
  unsigned char hwaddr[6];
  vrrp_if *vif = vsrv->vif;

  /* source address --rfc2338.7.3 */
  if(vsrv->no_vmac) /*usr real mac*/
  {
      memcpy(hwaddr, vif->hwaddr, 6);
  }
  else
  {
      memcpy(hwaddr, vsrv->vmac, 6);
  }

  /* add or delete virtual ip address */
  LIST_LOOP(vsrv->vaddr, vaddr, node)
  {
    if(!vaddr->deletable)  
    {
	continue;
    }
    
    if(send_vg_info (vaddr->inaddr.s_addr, hwaddr, vsrv->vrid, vif->ifindex, addF) < 0)
    //if(sifproxyarp(vaddr->inaddr.s_addr, vsrv->vif->ifname) < 0)
    //if(ipaddr_op(ifidx, vaddr->inaddr.s_addr, vaddr->masklen, addF) < 0)
    {
      err = 1;
      vaddr->deletable = 0;
      zdebug(DEBUG_VRRP_EVENT, "%s arp %s to %s failed\n", addF ? "Add" : "Remove",
		     inet_ntoa(vaddr->inaddr), vif->ifname);
      zlog_warn("%s arp %s to %s failed!\n", addF ? "Add" : "Remove",
		     inet_ntoa(vaddr->inaddr), vif->ifname);
    }
    else{
      vaddr->deletable = 1;
      zdebug(DEBUG_VRRP_EVENT, "%s arp %s to %s succeed\n", addF ? "Add" : "Remove",
		     inet_ntoa(vaddr->inaddr), vif->ifname);
      DBG(zlog_info("%s arp %s to %s succeed!\n", addF ? "Add" : "Remove",
		     inet_ntoa(vaddr->inaddr), vif->ifname));
    }
  }
  
  return err;
}


/****************************************************************
 NAME  : vrrp_hd_len        00/02/02 15:16:23
 AIM  : return the vrrp header size in byte
 REMARK  :
****************************************************************/
static int vrrp_hd_len(int naddr)
{
  return sizeof(vrrp_pkt) + naddr*sizeof(uint32_t) + VRRP_AUTH_LEN;
}


/****************************************************************
 NAME  : vrrp_in_chk_ipsecah        01/05/23 15:55:23
 AIM  : check ipsec ah. return 0 for a valid pkt, != 0 else.
 CONTRIB  : Alexandre Cassen : 2001/05/23
            SA lookup, sequence number verif, ICV verif
****************************************************************/
static int vrrp_in_chk_ipsecah( vrrp_if *vif, char *buffer)
{
  struct iphdr *ip = (struct iphdr*)(buffer);
  ipsec_ah *ah = (ipsec_ah *)((char *)ip + (ip->ihl<<2));
  vrrp_pkt *hd = (vrrp_pkt *)((char *)ah + sizeof(ipsec_ah));
  unsigned char *digest;
  uint32_t backup_auth_data[3];

  /* first verify that the SPI value is equal to src IP */
  if( ah->spi != ip->saddr ) {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: invalid IPSEC SPI value %d and expect %d",ip->saddr,ah->spi);
    return -1;
  }

#if 0  
  /* then proceed with the sequence number to prevent against replay attack.
  ** in inbound processing, we increment seq_number counter to audit sender counter. */
  vif->ipsecah_counter.seq_number++;

  if( ah->seq_number >= vif->ipsecah_counter.seq_number ) 
  {
    //zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: SEQUENCE NUMBER : %d\n",ah->seq_number);
    vif->ipsecah_counter.seq_number = ah->seq_number;
  } 
  else 
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: IPSEC AH sequence number %d already proceeded. Packet droped",ah->seq_number);
    return -1;
  }
#endif

  /* then compute a ICV to compare with the one present in AH pkt.
  ** alloc a temp memory space to stock the ip mutable fields */
  digest = (unsigned char *)malloc(16*sizeof(unsigned char *));
  assert(digest);
  memset(digest, 0, 16*sizeof(unsigned char *));

  /* zero the ip mutable fields */
  ip->tos = 0;
  ip->id = 0;
  ip->frag_off = 0;
  ip->check = 0;
  memcpy(backup_auth_data, ah->auth_data, sizeof(ah->auth_data));
  memset(ah->auth_data, 0, sizeof(ah->auth_data));

  /* Compute the ICV */
  hmac_md5(buffer, sizeof(struct iphdr) + sizeof(ipsec_ah) + vrrp_hd_len(hd->naddr),
           vif->auth_data, VRRP_AUTH_LEN, digest);

  if (memcmp(backup_auth_data, digest, HMAC_MD5_TRUNC) != 0) {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: invalid IPSEC HMAC-MD5 value. Due to fields mutation or bad password !");
    return -1;
  }

  free(digest);
  return 0;
}

/****************************************************************
 NAME  : vrrp_in_chk_vips        01/05/23 15:55:23
 AIM  : check if ipaddr is present in VIP buffer
 CONTRIB  : Alexandre Cassen : 2001/05/23
****************************************************************/
static int vrrp_in_chk_vips(int naddr, uint32_t ipaddr, unsigned char *buffer)
{
  int i;
  uint32_t ipbuf;

  for (i=0; i < naddr; i++) {
    bcopy(buffer + i*sizeof(uint32_t), &ipbuf, sizeof(uint32_t));
    if (ipaddr == ipbuf) 
	return 0;
  }

  return -1;
}


/****************************************************************
 NAME  : vrrp_in_chk        01/05/23 12:54:54
 AIM  : check a incoming packet. return 0 if the pkt is valid, != 0 else
 REMARK  : rfc2338.7.1
 CHANGES : Alexandre Cassen : 2001/05/23 :
             <+> Added IPSEC AH checks
             <+> Added check for IPs addr associated with he VRID
****************************************************************/
static int vrrp_in_chk(vrrp_if *vif, unsigned char *buffer, vrrp_rt **pvrt, vrrp_pkt **phd)
{
  struct iphdr *ip = (struct iphdr*)(buffer);
  int ihl = ip->ihl << 2;
  ipsec_ah *ah;
  //struct listnode *vrt_node;
  int vrid;
  unsigned char *vips;
  struct listnode *node;
  vip_addr *vaddr;
  vrrp_rt *vrt = NULL;
  vrrp_pkt *hd = NULL;

  /* MUST verify that the IP TTL is 255 */
  if( ip->ttl != VRRP_IP_TTL ) {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: error ttl");
    return -1;
  }

  /* MUST verify that the received packet length is greater than or equal to the VRRP header */
  if((ntohs(ip->tot_len)-ihl) <= sizeof(vrrp_pkt))
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: too short");
    return -1;
  }
  
  /*get vrrp packet from ip packet*/
  if (vif->auth_type == VRRP_AUTH_AH) 
  {
    ah = (ipsec_ah *)(buffer + sizeof(struct iphdr));
    hd = (vrrp_pkt *)(buffer + ihl + sizeof(ipsec_ah));
  } 
  else {
    hd = (vrrp_pkt *)(buffer + ihl);
  }

  /* MUST verify the VRRP version */
  if( (hd->vers_type >> 4) != VRRP_VERSION ){
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: version mismatch");
    return -1;
  }

  /* MUST perform authentication specified by Auth Type */
  /* check the authentication type */
  if(vif->auth_type != hd->auth_type)
  {    
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: auth type mismatch!");
    return -1;
  }

  /* check the authentication if it is a passwd */
  if(hd->auth_type == VRRP_AUTH_PASS)
  {
    char  *passwd = (char *)ip + ntohs(ip->tot_len) - VRRP_AUTH_LEN;
    if(memcmp(passwd, vif->auth_data, VRRP_AUTH_LEN))
    {
      zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: passwd mismatch!");
      return -1;
    }
  }
  /* check the authenicaion if it is ipsec ah */
  else if( hd->auth_type == VRRP_AUTH_AH )
  {
    if(vrrp_in_chk_ipsecah (vif, buffer) < 0)
 	return -1;
  }

  vrid = hd->vrid;
  
  /* MUST verify that the VRID is valid on the receiving interface */
  if(vrid < 1 || vrid > 255)
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: error vrid!");
    return -1;
  }

  vrt = vrrp_router[vrid - 1]; 
  if(!vrt || vrt->vif != vif)
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: vrid mismatch!");
    return -1;
  }

  /* MAY verify that the IP address(es) associated with the VRID are valid */
  if (vrt->vaddr->count != hd->naddr) 
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: ip number count mismatch!");
    return -1;
  }

  /* WORK: MUST verify the VRRP checksum */
  if(in_csum((u_short*)hd, vrrp_hd_len(hd->naddr), 0))
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: error checksum" );
    return -1;
  }

  /* pointer to vrrp vips pkt zone */
  vips = (unsigned char *)((char *)hd + sizeof(vrrp_pkt));

  LIST_LOOP(vrt->vaddr, vaddr, node)
  {
     if (vrrp_in_chk_vips(hd->naddr, vaddr->inaddr.s_addr, vips) < 0) 
     {
        zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: ip address mismatch");
	return -1;
     }
  }

  /* MUST verify that the Adver Interval in the packet is the same as
  ** the locally configured for this virtual router */
  if(vrt->adver_int != hd->adver_int)
  {
    zdebug(DEBUG_VRRP_PACKET, "Recv vrrp packet: advertis-time mismatch");
    return -1; 
  }

  *pvrt = vrt;
  *phd = hd;
  return 0;
}

/****************************************************************
 NAME  : vrrp_build_dlt      00/02/02 14:39:18
 AIM  :
 REMARK  : rfc2338.7.3
****************************************************************/
static void vrrp_build_eth_header( vrrp_rt *vsrv, char *buffer, int buflen )
{
  /* hardcoded for ethernet */
  struct ether_header * eth = (struct ether_header *)buffer;

  /* destination address --rfc1122.6.4*/
  eth->ether_dhost[0]  = 0x01;
  eth->ether_dhost[1]  = 0x00;
  eth->ether_dhost[2]  = 0x5E;
  eth->ether_dhost[3]  = (INADDR_VRRP_GROUP >> 16) & 0x7F;
  eth->ether_dhost[4]  = (INADDR_VRRP_GROUP >>  8) & 0xFF;
  eth->ether_dhost[5]  =  INADDR_VRRP_GROUP        & 0xFF;

  /* source address --rfc2338.7.3 */
  if(vsrv->no_vmac) /*usr real mac*/
  {
      memcpy(eth->ether_shost, vsrv->vif->hwaddr, ETH_ALEN);
  }
  else
  {
      memcpy(eth->ether_shost, vsrv->vmac, ETH_ALEN);
  }
  
  /* type */
  eth->ether_type = htons(ETHERTYPE_IP);
}

/****************************************************************
 NAME  : vrrp_build_ip        00/02/02 14:39:18
 AIM  : build a ip packet
 REMARK  :
 CHANGES : Alexandre Cassen : 2001/05/23 :
             <+> IPSEC AH protocol handling
****************************************************************/
static void vrrp_build_ip_header( vrrp_rt *vsrv, char *buffer, int buflen )
{
  vrrp_if *vif = vsrv->vif;
  struct iphdr * ip = (struct iphdr *)(buffer);
  ip->ihl    = 5;
  ip->version  = 4;
  ip->tos    = 0;
  ip->tot_len  = ip->ihl*4 + vrrp_hd_len(vsrv->vaddr->count);
  ip->tot_len  = htons(ip->tot_len);
  ip->id    = ++ip_id;
  ip->frag_off  = 0;
  ip->ttl    = VRRP_IP_TTL;
  
  /* fill protocol type --rfc2402.2 */
  ip->protocol  = (vif->auth_type == VRRP_AUTH_AH)?IPPROTO_IPSEC_AH:IPPROTO_VRRP;
  ip->saddr  = htonl(vif->ipaddr);
  ip->daddr  = htonl(INADDR_VRRP_GROUP);

  /* checksum must be done last */
  ip->check  = in_csum( (u_short*)ip, ip->ihl*4, 0 );
}

/****************************************************************
 NAME  : vrrp_build_ipsecah        01/05/23 15:55:23
 AIM  : build a ipsec ah packet
 CONTRIB  : Alexandre Cassen : 2001/05/23
****************************************************************/
static void vrrp_build_ipsecah( vrrp_rt *vsrv, char *buffer, int buflen )
{
  vrrp_if *vif = vsrv->vif;
	
  ICV_mutable_fields *ip_mutable_fields;
  unsigned char *digest;
  struct iphdr *ip = (struct iphdr *)(buffer);
  ipsec_ah *ah = (ipsec_ah *)(buffer+sizeof(struct iphdr));

  /* alloc a temp memory space to stock the ip mutable fields */
  ip_mutable_fields = calloc(sizeof(ICV_mutable_fields), 1);
  assert(ip_mutable_fields);
  memset(ip_mutable_fields, 0, sizeof(ICV_mutable_fields));

  /* fill in next header filed --rfc2402.2.1 */
  ah->next_header = IPPROTO_VRRP;

  /* update IP header total length value */
  ip->tot_len = ip->ihl*4 + sizeof(ipsec_ah) + vrrp_hd_len(vsrv->vaddr->count);
  ip->tot_len = htons(ip->tot_len);

  /* update ip checksum */
  ip->check = 0;
  ip->check = in_csum( (u_short*)ip, ip->ihl*4, 0 );

  /* backup the ip mutable fields */
  ip_mutable_fields->tos = ip->tos;
  ip_mutable_fields->id = ip->id;
  ip_mutable_fields->frag_off = ip->frag_off;
  ip_mutable_fields->check = ip->check;

  /* zero the ip mutable fields */
  ip->tos=0;
  ip->id=0;
  ip->frag_off=0;
  ip->check=0;

  /* fill in the Payload len field */
  ah->payload_len = IPSEC_AH_PLEN;

  /* The SPI value is filled with the ip header source address.
     SPI uniquely identify the Security Association (SA). This value
     is chosen by the recipient itself when setting up the SA. In a 
     multicast environment, this becomes unfeasible.

     If left to the sender, the choice of the SPI value should be done
     so by the sender that it cannot possibly conflict with SPI values
     chosen by other entities sending IPSEC traffic to any of the receivers.
     To overpass this problem, the rule I have chosen to implement here is
     that the SPI value chosen by the sender is based on unique information
     such as its IP address.
     -- INTERNET draft : <draft-paridaens-xcast-sec-framework-01.txt>
  */
  ah->spi = ip->saddr;

  /* Processing sequence number.
     Cycled assumed if 0xFFFFFFFD reached. So the MASTER state is free for another srv.
     Here can result a flapping MASTER state owner when max seq_number value reached.
     => Much work needed here.
     In the current implementation if counter has cycled, we stop sending adverts and 
     become BACKUP. If all the master are down we reset the counter for becoming MASTER.
  */
#if 0  
  if (vif->ipsecah_counter.seq_number > 0xFFFFFFFD) {
    vif->ipsecah_counter.cycle = 1;
  } 
  else 
#endif	  
  {
    vif->ipsecah_counter.seq_number++; /*send seq_number*/
  }

  ah->seq_number = vif->ipsecah_counter.seq_number;

  /* Compute the ICV & trunc the digest to 96bits
     => No padding needed.
     -- rfc2402.3.3.3.1.1.1 & rfc2401.5
  */
  digest=(unsigned char *)malloc(16*sizeof(unsigned char *));
  assert( digest );
  memset(digest, 0, 16*sizeof(unsigned char *));
  hmac_md5(buffer, buflen, vif->auth_data, VRRP_AUTH_LEN, digest);
  memcpy(ah->auth_data, digest, HMAC_MD5_TRUNC);

  /* Restore the ip mutable fields */
  ip->tos = ip_mutable_fields->tos;
  ip->id = ip_mutable_fields->id;
  ip->frag_off = ip_mutable_fields->frag_off;
  ip->check = ip_mutable_fields->check;

  free(ip_mutable_fields);
  free(digest);
}

/****************************************************************
 NAME  : vrrp_build_vrrp      00/02/02 14:39:18
 AIM  :
 REMARK  :
****************************************************************/
static int vrrp_build_vrrp_header( vrrp_rt *vsrv, int prio, char *buffer, int buflen )
{
  int  i = 0;
  vrrp_if   *vif  = vsrv->vif;
  vrrp_pkt *hd  = (vrrp_pkt *)buffer;
  uint32_t *iparr  = (uint32_t *)((char *)hd+sizeof(*hd));

  struct listnode *node;
  vip_addr *vaddr;
 
  hd->vers_type  = (VRRP_VERSION<<4) | VRRP_PKT_ADVERT;
  hd->vrid  = vsrv->vrid;
  hd->priority  = prio;
  hd->naddr  = vsrv->vaddr->count;
  hd->adver_int  = vsrv->adver_int;
  hd->auth_type  = vif->auth_type;

  /* copy the ip addresses */
  LIST_LOOP(vsrv->vaddr, vaddr, node)
  {
       iparr[i] = vaddr->inaddr.s_addr;
       i++;
  }

  /* copy the passwd if the authentication is VRRP_AH_PASS */
  if( vif->auth_type == VRRP_AUTH_PASS )
  {
    char  *pw  = (char *)hd + sizeof(*hd) + vsrv->vaddr->count*4;
    memcpy(pw, vif->auth_data, VRRP_AUTH_LEN);
  }
  
  hd->chksum = in_csum((u_short*)hd, vrrp_hd_len(vsrv->vaddr->count), 0);
  return(0);
}

/****************************************************************
 NAME  : vrrp_set_ptk        00/02/02 13:33:32
 AIM  : build a advertissement packet
 REMARK  : 
 CHANGES : Alexandre Cassen : 2001/05/23 :
             <+> IPSEC AH header processing
****************************************************************/
static void vrrp_build_pkt( vrrp_rt *vsrv, int prio, char *buffer, int buflen )
{
  char *bufptr = buffer;
  vrrp_if *vif = vsrv->vif;

  /* build the ethernet header */
  vrrp_build_eth_header( vsrv, buffer, buflen );

  /* build the ip header */
  buffer += ETHER_HDR_LEN;
  buflen -= ETHER_HDR_LEN;
  vrrp_build_ip_header( vsrv, buffer, buflen );

  /* build the vrrp header */
  buffer += sizeof(struct iphdr);
  if (vif->auth_type == VRRP_AUTH_AH)
    buffer += sizeof(ipsec_ah);
  buflen -= sizeof(struct iphdr);
  if (vif->auth_type == VRRP_AUTH_AH)
    buflen -= sizeof(ipsec_ah);
  vrrp_build_vrrp_header( vsrv, prio, buffer, buflen );

  /* build the IPSEC AH header */
  if (vif->auth_type == VRRP_AUTH_AH) {
    bufptr += ETHER_HDR_LEN;
    buflen += sizeof(ipsec_ah) + sizeof(struct iphdr);;
    vrrp_build_ipsecah( vsrv, bufptr, buflen );
  }
}


/****************************************************************
 NAME  : vrrp_send_pkt        00/02/06 16:37:10
 AIM  :
 REMARK  :
****************************************************************/
static int vrrp_send_pkt( vrrp_rt *vsrv, char *buffer, int buflen )
{
  struct sockaddr from;
  int  len;

  /* build the address */
  memset( &from, 0 , sizeof(from));
  strcpy( from.sa_data, vsrv->vif->ifname );

  /* send the data */
  len = sendto( sock_send, buffer, buflen, 0, &from, sizeof(from) );

  return len;
}


static int vrrp_send_adv( vrrp_rt *vsrv, int prio )
{
  vrrp_if *vif = vsrv->vif;
  int  buflen, ret;
  char *buffer;

  /* alloc the memory */
  buflen = ETHER_HDR_LEN + sizeof(struct iphdr) + vrrp_hd_len(vsrv->vaddr->count);
  if (vif->auth_type == VRRP_AUTH_AH)
    buflen += sizeof(ipsec_ah);

  buffer = calloc( buflen, 1 );
  assert( buffer );
  memset(buffer,0,buflen);

  /* build the packet  */
  vrrp_build_pkt( vsrv, prio, buffer, buflen );

  /* send it */
  ret = vrrp_send_pkt( vsrv, buffer, buflen );
  zdebug(DEBUG_VRRP_PACKET, "Vrrp router %d send : ADVERTISEMENT", vsrv->vrid);

  /* build the memory */
  free( buffer );
  return ret;
}


/****************************************************************
 NAME  : init_virtual_srv      00/02/06 09:18:02
 AIM  :
 REMARK  :
****************************************************************/
void init_virtual_srv( vrrp_rt *vsrv )
{
  memset(vsrv, 0, sizeof(vrrp_rt));
  vsrv->state  = VRRP_STATE_INIT;
  vsrv->priority  = VRRP_PRIO_DFL;
  vsrv->cfg_priority  = VRRP_PRIO_DFL;
  vsrv->adver_int  = VRRP_ADVER_DFL;
  vsrv->preempt  = VRRP_PREEMPT_DFL;
  vsrv->no_vmac = 1;
  
  vsrv->vaddr = list_new();

  //vsrv->sockfd = -1;
  //vsrv->read_thread = NULL;
  vsrv->down_timer = NULL;
  vsrv->adver_timer = NULL;
  vsrv->vif = NULL;
}

/****************************************************************
 NAME  : send_gratuitous_arp      00/05/11 11:56:30
 AIM  :
 REMARK  : rfc0826
  : WORK: ugly because heavily hardcoded for ethernet
****************************************************************/
static int send_gratuitous_arp( vrrp_rt *vsrv, int addr, int vAddrF )
{
struct m_arphdr
{
    unsigned short int ar_hrd;          /* Format of hardware address.  */
    unsigned short int ar_pro;          /* Format of protocol address.  */
    unsigned char ar_hln;               /* Length of hardware address.  */
    unsigned char ar_pln;               /* Length of protocol address.  */
    unsigned short int ar_op;           /* ARP opcode (command).  */

    /* Ethernet looks like this : This bit is variable sized however...  */
    unsigned char __ar_sha[ETH_ALEN];   /* Sender hardware address.  */
    unsigned char __ar_sip[4];          /* Sender IP address.  */
    unsigned char __ar_tha[ETH_ALEN];   /* Target hardware address.  */
    unsigned char __ar_tip[4];          /* Target IP address.  */
};

  char  buf[sizeof(struct m_arphdr)+ETHER_HDR_LEN];
  char  buflen  = sizeof(struct m_arphdr)+ETHER_HDR_LEN;
  struct ether_header *eth = (struct ether_header *)buf;
  struct m_arphdr  *arph = (struct m_arphdr *)(buf + ETHER_HDR_LEN);
  unsigned char  *hwaddr = vAddrF ? vsrv->vmac : vsrv->vif->hwaddr;
  int  hwlen = ETH_ALEN;
  int ret;

  /* hardcoded for ethernet */
  memset(eth->ether_dhost, 0xFF, ETH_ALEN);
  memcpy(eth->ether_shost, hwaddr, hwlen);
  eth->ether_type = htons(ETHERTYPE_ARP);

  /* build the arp payload */
  memset(arph, 0, sizeof(*arph));
  arph->ar_hrd  = htons(ARPHRD_ETHER);
  arph->ar_pro  = htons(ETHERTYPE_IP);
  arph->ar_hln  = 6;
  arph->ar_pln  = 4;
  arph->ar_op  = htons(ARPOP_REQUEST);
  memcpy(arph->__ar_sha, hwaddr, hwlen);

  //addr = htonl(addr);
  memcpy( arph->__ar_sip, &addr, sizeof(addr) );
  memcpy( arph->__ar_tip, &addr, sizeof(addr) );

  ret = vrrp_send_pkt( vsrv, buf, buflen );
  zdebug(DEBUG_VRRP_EVENT, "Vrrp send gratuitous arp for %s, mac=%02x:%02x:%02x:%02x:%02x:%02x", 
		  ip_ntoa(addr), hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
  DBG(zlog_info("Vrrp send gratuitous arp for %s, mac=%02x:%02x:%02x:%02x:%02x:%02x", 
		  ip_ntoa(addr), hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]));

  return ret;
}


/****************************************************************
 NAME  : state_gotomaster      00/02/07 00:15:26
 AIM  :
 REMARK  : called when the state is now MASTER
****************************************************************/
void state_goto_master( vrrp_rt *vsrv )
{
  vrrp_if  *vif = vsrv->vif;
  struct listnode *node = NULL;
  vip_addr *vaddr = NULL;

  /* set the VRRP virtual MAC address -- rfc2338.7.3 */
  if( !vsrv->no_vmac )
  {
    hwaddr_set(vif->ifname, vif->hw_type, vsrv->vmac, 6);/*set vmac*/
    //rcvhwaddr_op(vif->ifname, vif->hwaddr, sizeof(vif->hwaddr), 1);

    /* restore routing table */
    //rt_restore(vif->rt, vif->ifname);

    /* send gratuitous ARP for all interface addresses to update the cache of remote hosts */
    {
        int i, naddr;
	uint32_t  addr[1024];
    	naddr = ipaddr_list(ifname_to_idx(vif->ifname), addr, sizeof(addr)/sizeof(addr[0]));

    	for( i = 0; i < naddr; i++ )
      	   send_gratuitous_arp( vsrv, htonl(addr[i]), 1 );
    }
  }

  /* add the ip addresses ????? */
  ipaddr_ops(vsrv, 1);

  /* send an advertisement */
  vrrp_send_adv( vsrv, vsrv->priority );

  /* send gratuitous arp for each virtual ip */
  LIST_LOOP(vsrv->vaddr, vaddr, node)
  {
    send_gratuitous_arp(vsrv, vaddr->inaddr.s_addr, !vsrv->no_vmac);
  }
  
  /* init the struct */
  vsrv->adver_timer = thread_add_timer(master, adver_timeout, vsrv, vsrv->adver_int);

  vsrv->state = VRRP_STATE_MAST;
}

/****************************************************************
 NAME  : state_leavemaster      00/02/07 00:15:26
 AIM  :
 REMARK  : called when the state is no more MASTER
****************************************************************/
void state_leave_master( vrrp_rt *vsrv, int advF )
{
  uint32_t  addr[1024];
  vrrp_if    *vif = vsrv->vif;

  /* restore the original MAC addresses */
  if( !vsrv->no_vmac )
  {
    hwaddr_set(vif->ifname, vif->hw_type, vif->hwaddr, 6);/*set real mac*/
    //rcvhwaddr_op(vif->ifname, vif->hwaddr, sizeof(vif->hwaddr), 0);

    /* restore routing table */
    //rt_restore(vif->rt, vif->ifname);
  }

  /* remove the added virtual ip addresses and restore old interface address */
  ipaddr_ops( vsrv, 0 );

  /* if we stop vrrpd, warn the other routers to speed up the recovery */
  if( advF ){
    zdebug(DEBUG_VRRP_EVENT, "Vrrp router %d send: STOP ADVERTISEMENT", vsrv->vrid);
    DBG(zlog_info("Vrrp router %d send: STOP ADVERTISEMENT", vsrv->vrid));
    vrrp_send_adv( vsrv, VRRP_PRIO_STOP );
  }

  /* send gratuitous ARP for all the non-vrrp ip addresses to update
  ** the cache of remote hosts using these addresses */
  if( !vsrv->no_vmac )
  {
    int i, naddr;
    naddr = ipaddr_list(ifname_to_idx(vif->ifname), addr, sizeof(addr)/sizeof(addr[0]));

    for( i = 0; i < naddr; i++ )
      send_gratuitous_arp( vsrv, htonl(addr[i]), 0 );
  }

  if (vsrv->adver_timer)
  {
     THREAD_OFF(vsrv->adver_timer);
     vsrv->adver_timer = NULL;
  }

  /* If becoming MASTER in IPSEC AH AUTH, we reset the anti-replay */
  if (vif->ipsecah_counter.cycle) {
    vif->ipsecah_counter.cycle = 0;
    vif->ipsecah_counter.seq_number = 0;
  }
}

/****************************************************************
 NAME  : state_init        00/02/07 00:15:26
 AIM  :
 REMARK  : rfc2338.6.4.1
****************************************************************/
void state_init( vrrp_rt *vsrv )
{
  if(vsrv->priority == VRRP_PRIO_OWNER)
  {
    zdebug(DEBUG_VRRP_STATE, "Vrrp router %d is ip owner", vsrv->vrid);
    DBG(zlog_info("Vrrp router %d is ip owner", vsrv->vrid));

    zdebug(DEBUG_VRRP_STATE, "Vrrp router %d state : INIT to MASTER", vsrv->vrid);
    zlog_notice("Vrrp router %d state : INIT to MASTER", vsrv->vrid);

    state_goto_master( vsrv );
  } 
  else 
  {
    int delay = 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);
    vsrv->down_timer = thread_add_timer(master, down_timeout, vsrv, delay);
    vsrv->state = VRRP_STATE_BACK;
    
    zdebug(DEBUG_VRRP_STATE, "Vrrp router %d state : INIT to BACKUP", vsrv->vrid);
    zlog_notice("Vrrp router %d state : INIT to BACKUP", vsrv->vrid);
  }
}


/****************************************************************
 NAME  : open_sock        00/02/07 12:40:00
 AIM  : open the socket and join the multicast group.
 REMARK  :
 CHANGES : Alexandre Cassen : 2001/05/23 :
             <+> Added IPSEC AH socket support
****************************************************************/
int open_vrrp_sock( vrrp_if *vif )
{
  //struct  ip_mreq req;
  struct  ip_mreqn req;
  int  ret;
  
  if(vif->sockfd > 0)
    return 0;
  
  /* open the socket */
  if (vif->auth_type == VRRP_AUTH_AH)
 	vif->sockfd = socket( AF_INET, SOCK_RAW, IPPROTO_IPSEC_AH );
  else
  	vif->sockfd = socket( AF_INET, SOCK_RAW, IPPROTO_VRRP );

  if( vif->sockfd < 0 )
  {
    zlog_err("Create vrrp socket failed: %s(%d)\n", strerror(errno), errno);
    return -1;
  }
  
  /* join the multicast group */
  memset(&req, 0, sizeof (req));
  req.imr_multiaddr.s_addr = htonl(INADDR_VRRP_GROUP);
  //req.imr_interface.s_addr = vif->ipaddr;
  req.imr_address.s_addr = vif->ipaddr;
  req.imr_ifindex = vif->ifindex;
  ret = setsockopt(vif->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		   (char *)&req, sizeof(struct ip_mreq));
  if( ret < 0 )
  {
    close(vif->sockfd);
    vif->sockfd = -1;
    zlog_err("Vrrp socket join muticast group failed: %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  DBG(zlog_info("Open vrrp sock!"));
  vif->read_thread = thread_add_read(master, sock_read, vif, vif->sockfd);
	 
  return 0;
}


void close_vrrp_sock(vrrp_if *vif)
{
    if (vif->read_thread)
    {
        THREAD_OFF(vif->read_thread);
        vif->read_thread = NULL;
    }

    if (vif->sockfd> 0)
    {
  	DBG(zlog_info("Close vrrp sock!"));
        close (vif->sockfd);
        vif->sockfd = -1;
    }
}


/* call back function when vif->sockfd is ready */ 
static int sock_read (struct thread *th)
{
   char	buf[1024];
   vrrp_if *vif = (vrrp_if *)th->arg;
   int fd = vif->sockfd;  
   int len = 0;
   struct iphdr	*iph = NULL;
   vrrp_pkt *hd = NULL;
   vrrp_rt *vsrv = NULL;
   
   len = read( fd, buf, sizeof(buf) );
   if(len <= 0)
   {
	zdebug(DEBUG_VRRP_PACKET, "Vrrp rcv packet error: len < 0");
	goto fail;
   }
   
   if(vrrp_in_chk(vif, buf, &vsrv, &hd) < 0)
   {
	goto fail;
   }

   zdebug(DEBUG_VRRP_PACKET, "Vrrp rcv packet (%d bytes)", len);

   if ( vsrv->state == VRRP_STATE_BACK )
        back_recv_pkt (vsrv, hd);
   else if (vsrv->state == VRRP_STATE_MAST)
   {
   	iph = (struct iphdr *)buf;
        mast_recv_pkt (vsrv, hd, iph);
   }
   
fail:
   vif->read_thread = thread_add_read(master, sock_read, vif, fd);
   return 0;
}


/* call back function when advertisement timer expires */
static int adver_timeout (struct thread *th)
{
   vrrp_rt *vsrv = (vrrp_rt *)th->arg;

   vsrv->adver_timer = NULL;

   if (vsrv->state == VRRP_STATE_MAST)
   {
         vrrp_send_adv( vsrv, vsrv->priority );
         vsrv->adver_timer = thread_add_timer (master, adver_timeout, vsrv, vsrv->adver_int);
   }

   return 0;
}


/* call back function when ms_down_timer expires */
static int down_timeout (struct thread *th)
{
  vrrp_rt *vsrv = (vrrp_rt *)th->arg;

  vsrv->down_timer = NULL;

  if (vsrv->state == VRRP_STATE_BACK)
  {
        zdebug(DEBUG_VRRP_STATE, "Vrrp router %d state : BACKUP to MASTER", vsrv->vrid);
        zlog_notice("Vrrp router %d state : BACKUP to MASTER", vsrv->vrid);
  	state_goto_master( vsrv );
  }

  return 0;
}


/* vrrp router receives multicast packet when it's in back state */
static void back_recv_pkt (vrrp_rt *vsrv, vrrp_pkt *hd )
{
  int delay;

  if ( hd->priority == 0 ) 
  {
     delay = VRRP_TIMER_SKEW(vsrv);
     if (vsrv->down_timer)
	THREAD_OFF(vsrv->down_timer);
     vsrv->down_timer = thread_add_timer (master, down_timeout, vsrv, delay );
  } 
  else if( !vsrv->preempt || hd->priority >= vsrv->priority ) 
  {
     delay = 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);
     if (vsrv->down_timer)
	THREAD_OFF(vsrv->down_timer);
     vsrv->down_timer = thread_add_timer (master, down_timeout, vsrv, delay );
  }

  return;
}


/* vrrp router receives multicast packet when it's in master state */
static void mast_recv_pkt (vrrp_rt *vsrv, vrrp_pkt *hd, struct iphdr *iph )
{
  uint32_t ip1, ip2;

  ip1 = iph->saddr;
  ip2 = ntohl(vsrv->vif->ipaddr);
  
  DBG(zlog_info("Master recv packet: his->priority=%d, my->priority=%d, his->saddr=%u, my->ifaddr=%u", hd->priority, vsrv->priority, ip1, ip2));
  
  if( hd->priority == 0 )
  {
      zdebug(DEBUG_VRRP_EVENT, "Master %d recv packet: STOP ADVERTISE", vsrv->vrid);
      DBG(zlog_info("Master %d recv packet: STOP ADVERTISE", vsrv->vrid));
      vrrp_send_adv( vsrv, vsrv->priority );
      if (vsrv->adver_timer)
	   THREAD_OFF(vsrv->adver_timer);
      vsrv->adver_timer = thread_add_timer (master, adver_timeout, vsrv, vsrv->adver_int);
  }
  else if( hd->priority > vsrv->priority )
  {
      DBG(zlog_info("Master %d recv packet: his priority %d > my priority %d", 
		 vsrv->vrid, hd->priority, vsrv->priority));
      zdebug(DEBUG_VRRP_EVENT, "Master %d recv packet: his priority %d > my priority %d", 
		 vsrv->vrid, hd->priority, vsrv->priority);
      master_to_back(vsrv);
  }
  else if( (hd->priority == vsrv->priority) && (ip1 > ip2) )
  {
      DBG(zlog_info("Master %d recv packet: his ip %u > my ip %u", 
		 vsrv->vrid, ip1, ip2));
      zdebug(DEBUG_VRRP_EVENT, "Master %d recv packet: his ip %u > my ip %u", 
		 vsrv->vrid, ip1, ip2);
      master_to_back(vsrv);
  }
 
  {  
  struct listnode *node = NULL;
  vip_addr *vaddr = NULL;
  /* send gratuitous arp for each virtual ip */
  LIST_LOOP(vsrv->vaddr, vaddr, node)
  {
      send_gratuitous_arp(vsrv, vaddr->inaddr.s_addr, !vsrv->no_vmac);
  }
  }

  return;
}


/* the operation when state transmits from master to back */
static void master_to_back( vrrp_rt *vsrv )
{
    int delay = 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);

    if (vsrv->adver_timer)
    {
	THREAD_OFF(vsrv->adver_timer);
	vsrv->adver_timer = NULL;
    }
    
    if (vsrv->down_timer)
	THREAD_OFF(vsrv->down_timer);
    vsrv->down_timer = thread_add_timer (master, down_timeout, vsrv, delay);

    zdebug(DEBUG_VRRP_STATE, "Vrrp router %d state : MASTER to BACKUP", vsrv->vrid);
    zlog_notice("Vrrp router %d state : MASTER to BACKUP", vsrv->vrid);

    state_leave_master( vsrv, 0 );
    vsrv->state	= VRRP_STATE_BACK;
}


int vrrp_is_owner(uint32_t ipaddr, vrrp_rt *vsrv)
{
    vip_addr *vaddr;
    struct listnode  *vaddr_node;
    //int ip_owner = 0;
    
    LIST_LOOP(vsrv->vaddr, vaddr, vaddr_node)
    {
        if (ipaddr == vaddr->inaddr.s_addr)
        {
	     vaddr->deletable = 0;
             vsrv->ip_owner = 1;
        }
	else 
	     vaddr->deletable = 1;
    }

    return vsrv->ip_owner;
}


int vrrp_check_samenet(vrrp_rt *vsrv, vrrp_if *vif)
{
    vip_addr *vaddr;
    struct listnode  *vaddr_node;

    LIST_LOOP(vsrv->vaddr, vaddr, vaddr_node)
    {
	if(convert2netip (vaddr->inaddr.s_addr, vif->masklen) 
	   != convert2netip (vif->ipaddr, vif->masklen))
        {
             zdebug(DEBUG_VRRP_STATE, "Vrrp router %d not in the same net of interface %s!", vsrv->vrid, vif->ifname);
             return -1;
        }
    }

    return 0;
}


int check_vrrp_config(vrrp_rt *vsrv)
{
    if(!vsrv->vaddr || !vsrv->vaddr->count){
     	zdebug(DEBUG_VRRP_STATE, "vrrp router %d has no ip address!%s", vsrv->vrid);
	return -1;
    }

    return 0;
}


int vrrp_router_enable(vrrp_rt *vsrv, vrrp_if *vif)
{
    if(vsrv->vif)
	return;

    /* check temp */
    if(check_vrrp_config(vsrv) < 0)
    {
         goto fail;
    }

    if(vrrp_check_samenet(vsrv, vif) < 0)
    {
        goto fail;
    }

    if(vif->sockfd < 0)
    {
    	if(open_vrrp_sock(vif) < 0)
    	{
            goto fail;
    	}
    }
   
    zdebug(DEBUG_VRRP_EVENT, "Enable vrrp router %d succeed", vsrv->vrid);
    //zlog_notice("Enable vrrp router %d succeed", vsrv->vrid);

    if(vrrp_is_owner(vif->ipaddr, vsrv))
    {
	vsrv->priority = VRRP_PRIO_OWNER;
    }
    else
	vsrv->priority = vsrv->cfg_priority;	    

    vsrv->vif = vif;
    state_init( vsrv );
    return 0;

fail: 
    zdebug(DEBUG_VRRP_EVENT, "Enable vrrp router %d failed!", vsrv->vrid);
    //zlog_notice("Enable vrrp router %d failed!", vsrv->vrid);
    return -1;
}


void vrrp_router_disable(vrrp_rt *vsrv)
{
    if(vsrv->vif == NULL)
	return;
	    
    zdebug(DEBUG_VRRP_EVENT, "Disable vrrp router %d", vsrv->vrid);
    zlog_notice("Disable vrrp router %d", vsrv->vrid);
	
    if (vsrv->state == VRRP_STATE_MAST)
         state_leave_master( vsrv, 1 );
    
    if (vsrv->down_timer)
    {
        THREAD_OFF(vsrv->down_timer);
        vsrv->down_timer = NULL;
    }

    if(vsrv->adver_timer)
    {
        THREAD_OFF(vsrv->adver_timer);
        vsrv->adver_timer = NULL;
    }

    vsrv->vif = NULL;
    vsrv->state = VRRP_STATE_INIT;
    vsrv->priority = vsrv->cfg_priority;
    vsrv->ip_owner = 0;
}


void vrrp_stop_all_routers()
{
    int i;
    vrrp_rt *vsrv;

    DBG(zlog_info("Disable all vrrp router"));

    for (i=0; i<255; i++)
    {
	vsrv = vrrp_router[i];
	if(vsrv && vsrv->vif)
            vrrp_router_disable(vrrp_router[i]);
    }
}


/* transmit virtual group info from user space to the kernel with the help of 
    netlink socket. The five params are the values need to be transmitted. */ 
int send_vg_info (uint32_t vip, u_char *vmac, int vid, uint32_t ifidx, int vopt)
{
        struct rtnl_handle      *rth = &rtnl_sock;
	struct {
		struct nlmsghdr 	n;
		struct vrrpmsg 	        vrrp;
		char   			buf[256];
	} req;

	memset(&req, 0, sizeof(req));
	req.n.nlmsg_len		= NLMSG_LENGTH(sizeof(struct vrrpmsg));
	req.n.nlmsg_flags	= NLM_F_REQUEST;
	req.n.nlmsg_type	= RTM_ADD_VRRP;
	req.vrrp.family	= AF_INET;
	req.vrrp.vip	= vip;
	req.vrrp.vid	= vid;
	req.vrrp.ifidx	= ifidx;
	req.vrrp.vopt	= vopt;
	memcpy(req.vrrp.mac, vmac, 6);
	
	if (rtnl_talk(rth, &req.n, 0, 0, NULL, NULL, NULL) < 0)
	{
		DBG(zlog_info("Send vrrp msg to kernel failed: %s(%d)!", strerror(errno), errno));
		return -1;
	}

	DBG(zlog_info("Send vrrp msg to kernel succeed!"));
	return (0);
}

int open_send_sock ()
{
    DBG(zlog_info("Open vrrp send sock"));

    /* Get an internet socket for doing socket ioctls. */
    sock_send = socket(PF_PACKET, SOCK_PACKET, 0x300); /* 0x300 is magic */
    if (sock_send < 0)
        zlog_err("Create vrrp send socket failed: %s(%d)", strerror(errno), errno);

    return sock_send;
}


void close_send_sock()
{
    if(sock_send >= 0)
        close(sock_send);

    sock_send = -1;
}

