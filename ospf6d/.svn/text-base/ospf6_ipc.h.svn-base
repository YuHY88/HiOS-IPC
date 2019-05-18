

#ifndef _OSPF6_IPC_H
#define _OSPF6_IPC_H

#include "ospf6_top.h"


/*interface state add by tai*/
#define IFF_UP      0x0001      /* interface is up */
#define   IFF_BROADCAST   0x0002      /* broadcast address valid */
//#define   IFF_DEBUG   0x0004      /* turn on debugging */
#define IFF_LOOPBACK    0x0008      /* is a loopback net */
//#define   IFF_POINTOPOINT 0x0010      /* interface is point-to-point link */
//#define   IFF_NOTRAILERS  0x0020      /* avoid use of trailers */
#define IFF_RUNNING 0x0040      /* resources allocated */
//#define   IFF_NOARP   0x0080      /* no address resolution protocol */
//#define   IFF_PROMISC 0x0100      /* receive all packets */
//#define   IFF_ALLMULTI    0x0200      /* receive all multicast packets */
//#define   IFF_OACTIVE 0x0400      /* transmission in progress */
//#define   IFF_SIMPLEX 0x0800      /* can't hear own transmissions */
//#define   IFF_LINK0   0x1000      /* per link layer defined bit */
//#define   IFF_LINK1   0x2000      /* per link layer defined bit */
//#define   IFF_LINK2   0x4000      /* per link layer defined bit */
//#define   IFF_MULTICAST   0x8000      /* supports multicast */

/* Zebra API message flag. */
#define ZAPI_MESSAGE_NEXTHOP  0x01
#define ZAPI_MESSAGE_IFINDEX  0x02
#define ZAPI_MESSAGE_DISTANCE 0x04
#define ZAPI_MESSAGE_METRIC   0x08
#define ZAPI_MESSAGE_MTU      0x10



extern void ospf6_ipc_init(void);
extern int ospf6_redistribute_send(int , struct ospf6 *, int);


#endif

