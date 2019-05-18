/*
 * IS-IS Rout(e)ing protocol - isis_pfpacket.c
 *
 * Copyright (C) 2001,2002    Sampo Saaristo
 *                            Tampere University of Technology      
 *                            Institute of Communications Engineering
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
#if ISIS_METHOD == ISIS_METHOD_PFPACKET
#include <net/ethernet.h>	/* the L2 protocols */
#include <netpacket/packet.h>

#include "log.h"
#include "network.h"
#include "stream.h"
#include "if.h"

#include "isisd/dict.h"
#include "isisd/include-netbsd/iso.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_flags.h"
#include "isisd/isisd.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_network.h"
#include "isisd/isis_pdu.h"

#include "privs.h"
#include "pkt_buffer.h"
#include "module_id.h"

extern struct isis_statistics isis_sta;
/*
 * Table 9 - Architectural constants for use with ISO 8802 subnetworks
 * ISO 10589 - 8.4.8
 */

u_char ALL_L1_ISS[6] = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x14 };
u_char ALL_L2_ISS[6] = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x15 };
u_char ALL_ISS[6] = { 0x09, 0x00, 0x2B, 0x00, 0x00, 0x05 };
u_char ALL_ESS[6] = { 0x09, 0x00, 0x2B, 0x00, 0x00, 0x04 };


//static uint8_t discard_buff[8192];
//static uint8_t sock_buff[8192];

int hello_init(struct isis_circuit *circuit);


/*
 * if level is 0 we are joining p2p multicast
 * FIXME: and the p2p multicast being ???
 */
static int
isis_multicast_join (int fd, int registerto, int if_num)
{
  struct packet_mreq mreq;

  memset (&mreq, 0, sizeof (mreq));
  mreq.mr_ifindex = if_num;
  if (registerto)
    {
      mreq.mr_type = PACKET_MR_MULTICAST;
      mreq.mr_alen = ETH_ALEN;
      if (registerto == 1)
	memcpy (&mreq.mr_address, ALL_L1_ISS, ETH_ALEN);
      else if (registerto == 2)
	memcpy (&mreq.mr_address, ALL_L2_ISS, ETH_ALEN);
      else if (registerto == 3)
	memcpy (&mreq.mr_address, ALL_ISS, ETH_ALEN);
      else
	memcpy (&mreq.mr_address, ALL_ESS, ETH_ALEN);

    }
    else
    {
        mreq.mr_type = PACKET_MR_ALLMULTI;
    }

    zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"isis_multicast_join(): fd=%d, reg_to=%d, if_num=%d, "
                "address = %02x:%02x:%02x:%02x:%02x:%02x",
                fd, registerto, if_num, mreq.mr_address[0], mreq.mr_address[1],
                mreq.mr_address[2], mreq.mr_address[3], mreq.mr_address[4],
                mreq.mr_address[5]);

    if (setsockopt (fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq,
                    sizeof (struct packet_mreq)))
    {
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"isis_multicast_join(): setsockopt(): %s", safe_strerror (errno));
        return ISIS_WARNING;
    }

    return ISIS_OK;
}

static int
open_packet_socket (struct isis_circuit *circuit)
{
  struct sockaddr_ll s_addr;
  int fd, retval = ISIS_OK;

  fd = socket (PF_PACKET, SOCK_DGRAM, htons (ETH_P_ALL));
  if (fd < 0)
    {
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"open_packet_socket(): socket() failed %s",
                    safe_strerror (errno));
        return ISIS_WARNING;
    }

  /*
   * Bind to the physical interface
   */
  memset (&s_addr, 0, sizeof (struct sockaddr_ll));
  s_addr.sll_family = AF_PACKET;
  s_addr.sll_protocol = htons (ETH_P_ALL);
  s_addr.sll_ifindex = circuit->interface->ifindex;

  if (bind (fd, (struct sockaddr *) (&s_addr),
	    sizeof (struct sockaddr_ll)) < 0)
    {
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"open_packet_socket(): bind() failed: %s", safe_strerror (errno));
        close (fd);
        return ISIS_WARNING;
    }

  circuit->fd = fd;

  if (if_is_broadcast (circuit->interface))
    {
      /*
       * Join to multicast groups
       * according to
       * 8.4.2 - Broadcast subnetwork IIH PDUs
       * FIXME: is there a case only one will fail??
       */
      /* joining ALL_L1_ISS */
      retval |= isis_multicast_join (circuit->fd, 1,
                                      circuit->interface->ifindex);
      /* joining ALL_L2_ISS */
      retval |= isis_multicast_join (circuit->fd, 2,
                                      circuit->interface->ifindex);
      /* joining ALL_ISS (used in RFC 5309 p2p-over-lan as well) */
      retval |= isis_multicast_join (circuit->fd, 3,
                                    circuit->interface->ifindex);
    }
  else
    {
      retval =
        isis_multicast_join (circuit->fd, 0, circuit->interface->ifindex);
    }

  return retval;
}

/*
 * Create the socket and set the tx/rx funcs
 */
int hello_init(struct isis_circuit *circuit)
{
	int retval = ISIS_OK;

	if (if_is_broadcast (circuit->interface))
	{
	    //printf("%s():%d\n", __FUNCTION__, __LINE__);
		circuit->tx = isis_send_pdu_bcast;
		circuit->rx = isis_recv_pdu_bcast;
	}
	else if (if_is_pointopoint (circuit->interface))
	{
		circuit->tx = isis_send_pdu_p2p;
		circuit->rx = isis_recv_pdu_p2p;
	}
	else
    {
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"isis_sock_init(): unknown circuit type");
        retval = ISIS_WARNING;
        goto end;
    }
	
end:
	
	  return retval;

}
int
isis_sock_init (struct isis_circuit *circuit)
{
  int retval = ISIS_OK;

  //printf("%s():%d\n", __FUNCTION__, __LINE__);

  retval = open_packet_socket (circuit);

  //printf("%s():%d  ret = %d\n", __FUNCTION__, __LINE__,retval);

  if (retval != ISIS_OK)
    {
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"%s: could not initialize the socket", __func__);
        goto end;
    }

  /* Assign Rx and Tx callbacks are based on real if type */
  if (if_is_broadcast (circuit->interface))
    {
      circuit->tx = isis_send_pdu_bcast;
      circuit->rx = isis_recv_pdu_bcast;
    }
  else if (if_is_pointopoint (circuit->interface))
    {
      circuit->tx = isis_send_pdu_p2p;
      circuit->rx = isis_recv_pdu_p2p;
    }
  else
    {
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"isis_sock_init(): unknown circuit type");
        retval = ISIS_WARNING;
        goto end;
    }

end:

  return retval;
}

static inline int
llc_check (u_char * llc)
{
  if (*llc != ISO_SAP || *(llc + 1) != ISO_SAP || *(llc + 2) != 3)
    return 0;

  return 1;
}

int
isis_recv_pdu_bcast (struct isis_circuit *circuit, u_char * ssnpa)
{
#if 0
 // int bytesread, addr_len;
  //struct sockaddr_ll s_addr;
  u_char llc[LLC_LEN];
  int i;
 // static int y = 0;
//  y++;
 // if(y < 10)
 // printf("%s():%d\n", __FUNCTION__, __LINE__);
  
  rcv_buf = pkt_rcv(MODULE_ID_ISIS);
  
//  if(y < 10)
//    printf("%s():%d\n", __FUNCTION__, __LINE__);

  if(rcv_buf == NULL)
  {
  	 //printf("rcv pdu suc----%s(),%d\n",__FUNCTION__,__LINE__);
	 return ISIS_WARNING;
  }
  else
  {
  	printf("pkt_rcv suc\n");	
  }
	
  pkt_dump(rcv_buf->data,rcv_buf->data_len);
	
#if 0
	if ((rcv_buf == NULL) || (rcv_buf->in_ifindex != (int)circuit->interface->ifindex))
    {
    //printf("%s():%d\n", __FUNCTION__, __LINE__);
      if (rcv_buf == NULL)
      {
       zlog_debug ("isis_recv_packet_bcast(): ifname %s, fd %d, "
                  "recvfrom(): %s",
                   circuit->interface->name, circuit->fd,safe_strerror (errno));
      }
      if (rcv_buf->in_ifindex != (int)circuit->interface->ifindex)
      {
         zlog_debug("packet is received on multiple interfaces: "
                   "rcv interface %d, circuit interface %d, ",
                   rcv_buf->in_ifindex, circuit->interface->ifindex);
      }

      /* get rid of the packet */
      //bytesread = recvfrom (circuit->fd, discard_buff, sizeof (discard_buff),
      //                      MSG_DONTWAIT, (struct sockaddr *) &s_addr,
      //                      (socklen_t *) &addr_len);
	 // printf("%s():%d\n", __FUNCTION__, __LINE__);
       return ISIS_WARNING;
    }
#endif
	ssnpa = rcv_buf->cb.ethcb.smac;
	pkt_dump(ssnpa,6);

	memcpy(llc,rcv_buf->data,3);
	pkt_dump(llc,3);

	if (!llc_check (llc))
	{
		printf("%s():%d\n", __FUNCTION__, __LINE__);
		return ISIS_WARNING;
	}

	stream_write (circuit->rcv_stream, rcv_buf->data, rcv_buf->data_len);
	printf("%s():%d\n", __FUNCTION__, __LINE__);
#if 0
  if ((bytesread < 0) || (s_addr.sll_ifindex != (int)circuit->interface->ifindex))
    {
    //printf("%s():%d\n", __FUNCTION__, __LINE__);
      if (bytesread < 0)
        {
          zlog_debug ("isis_recv_packet_bcast(): ifname %s, fd %d, "
                     "bytesread %d, recvfrom(): %s",
                     circuit->interface->name, circuit->fd, bytesread,
                     safe_strerror (errno));
        }
      if (s_addr.sll_ifindex != (int)circuit->interface->ifindex)
        {
          zlog_debug("packet is received on multiple interfaces: "
                    "socket interface %d, circuit interface %d, "
                    "packet type %u",
                    s_addr.sll_ifindex, circuit->interface->ifindex,
                    s_addr.sll_pkttype);
        }

      /* get rid of the packet */
      //bytesread = recvfrom (circuit->fd, discard_buff, sizeof (discard_buff),
      //                      MSG_DONTWAIT, (struct sockaddr *) &s_addr,
      //                      (socklen_t *) &addr_len);
	 // printf("%s():%d\n", __FUNCTION__, __LINE__);
      return ISIS_WARNING;
    }
#endif
  /*
   * Filtering by llc field, discard packets sent by this host (other circuit)
   */
#if 0
  if (!llc_check (llc) || s_addr.sll_pkttype == PACKET_OUTGOING)
  {
    printf("%s():%d\n", __FUNCTION__, __LINE__);
      /*  Read the packet into discard buff */
      bytesread = recvfrom (circuit->fd, discard_buff, sizeof (discard_buff),
                            MSG_DONTWAIT, (struct sockaddr *) &s_addr,
                            (socklen_t *) &addr_len);
      if (bytesread < 0)
	zlog_debug ("isis_recv_pdu_bcast(): recvfrom() failed");
      return ISIS_WARNING;
  }
#endif
  /* on lan we have to read to the static buff first */
#if 0
  bytesread = recvfrom (circuit->fd, sock_buff, sizeof (sock_buff), MSG_DONTWAIT,
			(struct sockaddr *) &s_addr, (socklen_t *) &addr_len);
  if (bytesread < 0)
    {
      zlog_debug ("isis_recv_pdu_bcast(): recvfrom() failed");
      return ISIS_WARNING;
    }

  /* then we lose the LLC */
  stream_write (circuit->rcv_stream, sock_buff + LLC_LEN, bytesread - LLC_LEN);
   
  memcpy (ssnpa, &s_addr.sll_addr, s_addr.sll_halen);
 #endif
  // stream_forward(circuit->rcv_stream ,6);
   //stream_get(ssnpa,circuit->rcv_stream,6);
#endif
  return ISIS_OK;
}

int
isis_recv_pdu_p2p (struct isis_circuit *circuit, u_char * ssnpa)
{
#if 0
    u_char llc[LLC_LEN];

    struct pkt_buffer * rcv_buf;

    rcv_buf= pkt_rcv(MODULE_ID_ISIS);

    if(rcv_buf == NULL)
    {
        return ISIS_WARNING;
    }

    ssnpa = rcv_buf->cb.ethcb.smac;
    memcpy (llc, rcv_buf->data, 3);

    zlog_debug (ISIS_DEBUG_TYPE_RECEIVE,"ssnpa:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", ssnpa[0], ssnpa[1], ssnpa[2], ssnpa[3], ssnpa[4], ssnpa[5]);

    if (!llc_check (llc))
	{
		return ISIS_WARNING;
    }
    stream_write (circuit->rcv_stream, rcv_buf->data, rcv_buf->data_len);

    memset (&s_addr, 0, sizeof (struct sockaddr_ll));
  addr_len = sizeof (s_addr);

  /* we can read directly to the stream */
  bytesread = stream_recvfrom (circuit->rcv_stream, circuit->fd,
                               circuit->interface->mtu, 0,
                               (struct sockaddr *) &s_addr, 
                               (socklen_t *) &addr_len);

  if (s_addr.sll_pkttype == PACKET_OUTGOING)
    {
      /*  Read the packet into discard buff */
      bytesread = recvfrom (circuit->fd, discard_buff, sizeof (discard_buff),
                            MSG_DONTWAIT, (struct sockaddr *) &s_addr,
                            (socklen_t *) &addr_len);
      if (bytesread < 0)
	zlog_debug ("isis_recv_pdu_p2p(): recvfrom() failed");
      return ISIS_WARNING;
    }

  /* If we don't have protocol type 0x00FE which is
   * ISO over GRE we exit with pain :)
   */
  if (ntohs (s_addr.sll_protocol) != 0x00FE)
    {
      zlog_debug ("isis_recv_pdu_p2p(): protocol mismatch(): %X",
		 ntohs (s_addr.sll_protocol));
      return ISIS_WARNING;
    }

  memcpy (ssnpa, &s_addr.sll_addr, s_addr.sll_halen);
#endif

  return ISIS_OK;
}

int
isis_send_pdu_bcast (struct isis_circuit *circuit, int level)
{
    /* we need to do the LLC in here because of P2P circuits, which will
      * not need it
      */
    ssize_t rv;
    union pkt_control pktcontrol;
    struct stream *pdata;
    u_int16_t length;

    length = LLC_LEN + stream_get_endp(circuit->snd_stream);
    memset(&pktcontrol, 0, sizeof(union pkt_control));
    pdata = stream_new(sizeof(union pkt_control) + length);

    /* RFC5309 section 4.1 recommends ALL_ISS */
 
    if (circuit->circ_type == CIRCUIT_T_P2P)
    {
      	pktcontrol.ethcb.dmac[0] = ALL_ISS[0];
      	pktcontrol.ethcb.dmac[1] = ALL_ISS[1];
      	pktcontrol.ethcb.dmac[2] = ALL_ISS[2];
      	pktcontrol.ethcb.dmac[3] = ALL_ISS[3];
      	pktcontrol.ethcb.dmac[4] = ALL_ISS[4];
      	pktcontrol.ethcb.dmac[5] = ALL_ISS[5];
      	
    }
    else if (level == 1)
    {
        pktcontrol.ethcb.dmac[0] = ALL_L1_ISS[0];
      	pktcontrol.ethcb.dmac[1] = ALL_L1_ISS[1];
      	pktcontrol.ethcb.dmac[2] = ALL_L1_ISS[2];
      	pktcontrol.ethcb.dmac[3] = ALL_L1_ISS[3];
      	pktcontrol.ethcb.dmac[4] = ALL_L1_ISS[4];
      	pktcontrol.ethcb.dmac[5] = ALL_L1_ISS[5];
    }
    else
    {
        pktcontrol.ethcb.dmac[0] = ALL_L2_ISS[0];
      	pktcontrol.ethcb.dmac[1] = ALL_L2_ISS[1];
      	pktcontrol.ethcb.dmac[2] = ALL_L2_ISS[2];
      	pktcontrol.ethcb.dmac[3] = ALL_L2_ISS[3];
      	pktcontrol.ethcb.dmac[4] = ALL_L2_ISS[4];
      	pktcontrol.ethcb.dmac[5] = ALL_L2_ISS[5];
    }

    /*Length*/
	if(circuit->interface->mtu > 1500)
	{
	  pktcontrol.ethcb.ethtype = 0x8870;
	}
	else
	{
	  pktcontrol.ethcb.ethtype = length;
	}
    
	pktcontrol.ethcb.smac_valid = 0;
	pktcontrol.ethcb.is_changed = 1;
    pktcontrol.ethcb.cos = 3;

    /*           LLC              */
    stream_putc(pdata,0xFE);
    stream_putc(pdata,0xFE);
    stream_putc(pdata,0x03);

    stream_put(pdata ,STREAM_DATA(circuit->snd_stream)
		             ,stream_get_endp(circuit->snd_stream));
  
    pktcontrol.ethcb.ifindex = circuit->interface->ifindex;

    rv = pkt_send(PKT_TYPE_ETH, &pktcontrol, STREAM_DATA(pdata), length);
    stream_free(pdata); 

    if (rv < 0)
    {
      zlog_err("IS-IS pfpacket: could not transmit packet on %s: %s",
               circuit->interface->name, safe_strerror(errno));
      if (ERRNO_IO_RETRY(errno))
        return ISIS_WARNING;
      return ISIS_ERROR;
    }

    isis_sta.isis_send++;
  
    return ISIS_OK;
}

int
isis_send_pdu_p2p (struct isis_circuit *circuit, int level)
{
    ssize_t rv;
    int i;
	union pkt_control pktcontrol;
	struct stream *pdata;
    u_int16_t length;
	
	length = stream_get_endp(circuit->snd_stream);

    pdata = stream_new(sizeof(union pkt_control) + 14 + length);

    stream_put(pdata,&pktcontrol,sizeof(union pkt_control));

    if (level == 1)
        stream_put (pdata, ALL_L1_ISS,6);
    else
      stream_put (pdata, ALL_L2_ISS,6);

	for (i = 0; i < circuit->interface->hw_addr_len; i++)
	{
	  stream_putc(pdata, circuit->interface->hw_addr[i]);
	}
	if(length > 1500)
	{
	   stream_putw(pdata ,0x8870);
	}
	else
	{
	  stream_putw(pdata ,length);
	}

	stream_putc(pdata,0xFE);
	stream_putc(pdata,0xFE);
	stream_putc(pdata,0x03);
  
	stream_put(pdata ,STREAM_DATA(circuit->snd_stream)
		             ,stream_get_endp(circuit->snd_stream));
  
	//pktcontrol.rawcb.ifindex = circuit->interface->ifindex;
	
    //rv = pkt_send(PKT_TYPE_RAW, &pktcontrol, STREAM_DATA(pdata), 14 + length);
    rv = ipc_send_msg_n2(pdata,sizeof(union pkt_control) + 14 + length, 1, MODULE_ID_FTM, MODULE_ID_ISIS,
                         IPC_TYPE_PACKET, PKT_TYPE_RAW, 0, 0);

    stream_free(pdata);

    if (rv < 0)
    {
      zlog_err("IS-IS pfpacket: could not transmit packet on %s: %s",
                  circuit->interface->name, safe_strerror(errno));
      if (ERRNO_IO_RETRY(errno))
          return ISIS_WARNING;
        return ISIS_ERROR;
    }
    isis_sta.isis_send++;
    return ISIS_OK;
}

#endif /* ISIS_METHOD == ISIS_METHOD_PFPACKET */
