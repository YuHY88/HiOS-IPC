/*  file : harbour_ac/src/zebra/vrrpd/utils.c
 *        
*/ 

#include "../config.h"
#include <zebra.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <asm/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>

#include "command.h"
#include "if.h"
#include "prefix.h"
#include "memory.h"
#include "network.h"
#include "table.h"
#include "stream.h"
#include "thread.h"
#include "zclient.h"
#include "filter.h"
#include "sockopt.h"
#include "zebra/connected.h"
#include "linklist.h"
//#include "common.h"

#include "log.h"
#include "zlog.h"

#include "utils.h"


/*@brief 返回在字符串s中最后出现字符c的位置给ptr
* @param ptr -- 返回的指针
* @param s -- 检查的字符串
* @param c -- 出现的字符
* @retval -1 在检查的字符串中找不到该字符
*/
#define STRRCHR(ptr, s, c) \
	if ( (ptr = (char *)strrchr(s, c)) == NULL) \
	{	return -1; } 

#define STRTOL(A, ptr) \
	if ( *ptr == '\0' ) { \
		return -1; \
	} \
	else { \
		A = strtol(ptr, (char **)NULL, 10); \
	}
	
#define STRCKE(ptr) \
        if ((*ptr == '0') && (*(ptr+1) != '\0')) \
         {return -1;}


int sock_ioctl = -1;


int open_ioctl_sock ()
{
    /* Get an internet socket for doing socket ioctls. */
    sock_ioctl = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ioctl < 0)
        zlog_err("Couldn't create IP socket: %m(%d)", errno);

    DBG(zlog_info("Open ioctl_sock"));

    return sock_ioctl;
}


void close_ioctl_sock()
{
    if(sock_ioctl >= 0)
	close(sock_ioctl);

    sock_ioctl = -1;
}


char *ip_ntoa(uint32_t ip)
{
  static char buf[20];
  unsigned char *bytep;

  bytep = (unsigned char *) &(ip);
  sprintf(buf, "%d.%d.%d.%d", bytep[0], bytep[1], bytep[2], bytep[3]);
  return buf;
}


/* get a network ip from an ip address and a masklen */
uint32_t convert2netip (uint32_t ip, int masklen)
{
    struct in_addr temp_ip;

    masklen2ip(masklen, &temp_ip);
    return (ip & temp_ip.s_addr);
}


/**
* @brief 检查ip是否是真正的IP地址，即是否符合A.B.C.D的形式
*
* @param ip -- 要检查的IP地址
* @retval 1  是
* @retval -1 不是
*/		
int is_bad_ip_address(char *ip)
{
	char *ptr, buf[20];
	int A, B, C, D;
	
    	//not number
	if ((ip[0] < 48) || (ip[0] > 57))
           return 1;
	
	strncpy(buf, ip, 20);
	buf[19] = 0;
	if ( buf != NULL )
	{
		//find the first '.'
		STRRCHR(ptr, buf, '.');
		*ptr = '\0';
		++ptr;

		//find the second '.'
       		STRCKE(ptr);
		STRTOL(D, ptr);
		STRRCHR(ptr, buf, '.');
		*ptr = '\0';
		++ptr;

		//find the third '.'
        	STRCKE(ptr);
		STRTOL(C, ptr);
		STRRCHR(ptr, buf, '.');
		*ptr = '\0';
		++ptr;

		//find the forth '.'
        	STRCKE(ptr);
		STRTOL(B, ptr);
		STRTOL(A, buf);
		ptr = buf;
        	STRCKE(ptr);

		//if ((A > 255) || (B > 255) || (C > 255) || (D > 255) || (D == 0))
		if ((A > 255) || (B > 255) || (C > 255) || (D > 255))
			return 1;
		else 
			return 0;
	}

	return 1;
}


/*check if the ip address include '.'*/
int is_address(char *address)
{
	char address_temp[20];  
	 
	strncpy(address_temp, address, 20);
	address_temp[19] = 0;

	/*didn't find the '.'*/
	if (strrchr(address_temp, '.') == NULL)
		return 0;
	else 
		return 1;   
}


int getmacbyip(int sock, struct in_addr *ip, char *interface, unsigned char *mac)
{
	int ret;
        struct arpreq   arpreq;
        struct sockaddr_in  *sin;

        bzero(&arpreq, sizeof( struct arpreq));
        sin = (struct sockaddr_in *) &arpreq.arp_pa;
        bzero(sin, sizeof(struct sockaddr_in));
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = ip->s_addr;
        //strcpy(arpreq.arp_dev, interface);

        ret = ioctl(sock, SIOCGARP, &arpreq);
        if(!ret){
                memcpy(mac, arpreq.arp_ha.sa_data, 6);
        }

        return ret;
}


/*check if the addr_ip1 in the same network domain as net_ip */
int is_same_net(unsigned long ip1, unsigned long ip2, int masklen)
{
    struct in_addr netmask;

    if (ip1 == ip2)
	return 1;
 
    masklen_to_netip(masklen, &netmask);

    if((ip1 & netmask.s_addr) == (ip2 & netmask.s_addr))
   	return 1;
    else 
 	return 0;
}


/* convert masklen to netmask */
void masklen_to_netip (int masklen, struct in_addr *netmask)
{  
   u_char *pnt;  
   int bit;  
   int offset;        
   unsigned char maskbit[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0,  0xf8, 0xfc, 0xfe, 0xff};
   
   memset (netmask, 0, sizeof (struct in_addr));  
   pnt = (unsigned char *) netmask;		    
   offset = masklen / 8;  
   bit = masklen % 8;     

   while (offset--)	  
    *pnt++ = 0xff;		  

   if (bit)      
  	*pnt = maskbit[bit];
}


/*convert ip and masklen to broadcast ip address*/
void masklen_to_broadip (unsigned long ip, int masklen, unsigned long *broadip)
{
   struct in_addr netmask;  
   unsigned long net_ip;
   
   masklen_to_netip (masklen, &netmask); 
   net_ip = ip & netmask.s_addr; 

   *broadip = net_ip ^ (~(netmask.s_addr)); 

   return; 
}


/****************************************************************
 NAME  : get_dev_from_ip      00/02/08 06:51:32
 AIM  :
 REMARK  :
****************************************************************/
uint32_t ifname_to_ip( char *ifname )
{
  struct ifreq  ifr;
  uint32_t  addr  = 0;

  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
  if (ioctl(sock_ioctl, SIOCGIFADDR, (char *)&ifr) == 0)
  {
    struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
    addr = sin->sin_addr.s_addr;
  }

  return addr;
}


/****************************************************************
 NAME  : get_dev_from_ip      00/02/08 06:51:32
 AIM  :
 REMARK  :
****************************************************************/
int ifname_to_idx( char *ifname )
{
  struct ifreq  ifr;
  int    ifindex = -1;

  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
  if (ioctl(sock_ioctl, SIOCGIFINDEX, (char *)&ifr) == 0)
    ifindex = ifr.ifr_ifindex;

  return ifindex;
}


/****************************************************************
 NAME  : rcvhwaddr_op        00/02/08 06:51:32
 AIM  :
 REMARK  :
****************************************************************/
int rcvhwaddr_op( char *ifname, char *addr, int addrlen, int addF )
{
  struct ifreq  ifr;
  int    ret;

  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
  memcpy( ifr.ifr_hwaddr.sa_data, addr, addrlen );
  ifr.ifr_hwaddr.sa_family = AF_UNSPEC;
  ret = ioctl(sock_ioctl, addF ? SIOCADDMULTI : SIOCDELMULTI, (char *)&ifr);
  if( ret ){
    zlog_warn("Can't %s on %s. errno=%d\n",
               addF ? "SIOCADDMULTI" : "SIOCDELMULTI", ifname, errno );
  }

  return ret;
}


/****************************************************************
 NAME  : hwaddr_set        00/02/08 06:51:32
 AIM  :
 REMARK  : linux refuse to change the hwaddr if the interface is up
****************************************************************/
int hwaddr_set( char *ifname, uint16_t hw_type, unsigned char *addr, int addrlen )
{
  struct ifreq  ifr;
  unsigned long  flags;
  int    ret;
  
  DBG(zlog_info("set mac %02x:%2x:%02x:%02x:%02x:%02x to interface %s",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], ifname));

  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

  /* get the flags */
  ret = ioctl(sock_ioctl, SIOCGIFFLAGS, (char *)&ifr);
  if(ret)   goto end;
  flags = ifr.ifr_flags;

  /* set the interface down */
  ifr.ifr_flags &= ~IFF_UP;
  ret = ioctl(sock_ioctl, SIOCSIFFLAGS, (char *)&ifr);
  if( ret )  goto end;

  /* change the hwaddr */
  memcpy( ifr.ifr_hwaddr.sa_data, addr, addrlen );
  //ifr.ifr_hwaddr.sa_family = AF_UNIX;
  ifr.ifr_hwaddr.sa_family = hw_type;
  ret = ioctl(sock_ioctl, SIOCSIFHWADDR, (char *)&ifr);
  if( ret )  goto end;

  /* set the interface up */
  ifr.ifr_flags = flags;
  ret = ioctl(sock_ioctl, SIOCSIFFLAGS, (char *)&ifr);
  if( ret )  goto end;

end:
  if( ret )
        zlog_warn("SET hwaddr faile: errno=%d\n", errno);

  return ret;
}


/****************************************************************
 NAME  : hwaddr_get        00/02/08 06:51:32
 AIM  :
 REMARK  :
****************************************************************/
int hwaddr_get( char *ifname, char *addr, int addrlen )
{
  struct ifreq  ifr;
  int    ret;
  
  memset(&ifr.ifr_hwaddr, 0, sizeof(struct sockaddr));
  strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
  ret = ioctl(sock_ioctl, SIOCGIFHWADDR, (char *)&ifr);
  if (ret >= 0)
  	memcpy( addr, ifr.ifr_hwaddr.sa_data, addrlen );

  DBG(zlog_info("Get hwaddr: %02x:%02x:%02x:%02x:%02x:%02x\n", addr[0], addr[1], addr[2], addr[3], addr[
4], addr[5]));

  return ret;
}


/********************************************************************
 *
 * sifproxyarp - Make a proxy ARP entry for the ipaddr.
 */
int sifproxyarp (uint32_t ipaddr, char const *ifname)
{
    	struct arpreq arpreq;

    	memset (&arpreq, '\0', sizeof(arpreq));
	SET_SA_FAMILY(arpreq.arp_pa, AF_INET);
	SIN_ADDR(arpreq.arp_pa) = ipaddr;
	arpreq.arp_flags = ATF_PERM | ATF_PUBL;
	
	/*
 	* Get the hardware address of an interface on the same subnet
 	* as ipaddr
 	*/
	if (get_ether_addr(&arpreq.arp_ha, ifname) < 0) 
	{
	    zlog_warn("Can't get hardware address");
	    return -1;
	}

	strlcpy(arpreq.arp_dev, ifname, sizeof(arpreq.arp_dev));

	if (ioctl(sock_ioctl, SIOCSARP, (caddr_t)&arpreq) < 0) {
	    zlog_err("Set arp failed: %m");
	    return -1;
	}

        return 0;
}

/********************************************************************
 *
 * cifproxyarp - Delete the proxy ARP entry for the ipaddr.
 */
int cifproxyarp (uint32_t ipaddr, char const *ifname)
{
    	struct arpreq arpreq;

	memset (&arpreq, '\0', sizeof(arpreq));
	SET_SA_FAMILY(arpreq.arp_pa, AF_INET);
	SIN_ADDR(arpreq.arp_pa) = ipaddr;
	arpreq.arp_flags = ATF_PERM | ATF_PUBL;
	strlcpy(arpreq.arp_dev, ifname, sizeof(arpreq.arp_dev));

	if (ioctl(sock_ioctl, SIOCDARP, (caddr_t)&arpreq) < 0) {
	    zlog_err("Delete arp failed: %m");
	    return -1;
	}
    	
	return 0;
}


/********************************************************************
 *
 * get_ether_addr - get the hardware address of an interface 
 */
int get_ether_addr (struct sockaddr *hwaddr, char const *ifname)
{
    struct ifreq ifr;
    int ret;
    
    memset(&ifr.ifr_hwaddr, 0, sizeof(struct sockaddr));
    strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
    ret = ioctl(sock_ioctl, SIOCGIFHWADDR, (char *)&ifr);
    if (ret < 0)
    {
	zlog_err("Get interface %s hardware address failed: %m", ifr.ifr_name);
	return -1;
    }
    memcpy (hwaddr, &ifr.ifr_hwaddr, sizeof (struct sockaddr));

    DBG(zlog_info("proxy arp: found hwaddr %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
		(int) ((unsigned char *) &hwaddr->sa_data)[0],
		(int) ((unsigned char *) &hwaddr->sa_data)[1],
		(int) ((unsigned char *) &hwaddr->sa_data)[2],
		(int) ((unsigned char *) &hwaddr->sa_data)[3],
		(int) ((unsigned char *) &hwaddr->sa_data)[4],
		(int) ((unsigned char *) &hwaddr->sa_data)[5],
		(int) ((unsigned char *) &hwaddr->sa_data)[6],
		(int) ((unsigned char *) &hwaddr->sa_data)[7]));

    return 0;
}


/********************************************************************
 *
 * get_ether_addr - get the hardware address of an interface on the
 * the same subnet as ipaddr.
 */
int get_if_byip (u_int32_t ipaddr, struct sockaddr *hwaddr, char *name, int namelen)
{
    struct ifreq *ifr, *ifend;
    u_int32_t ina, mask;
    char *aliasp;
    struct ifreq ifreq, bestifreq;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];

    u_int32_t bestmask=0;
    int found_interface = 0;

    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if (ioctl(sock_ioctl, SIOCGIFCONF, &ifc) < 0) {
	zlog_err("Get interface list failed: %s", strerror(errno));
	return -1;
    }

    DBG(zlog_info("scanning %d interfaces for IP %s",
		ifc.ifc_len / sizeof(struct ifreq), ip_ntoa(ipaddr)));

    /*
     * Scan through looking for an interface with an Internet
     * address on the same subnet as `ipaddr'.
    */
    ifend = ifs + (ifc.ifc_len / sizeof(struct ifreq));
    for (ifr = ifc.ifc_req; ifr < ifend; ifr++) 
    {
	if (ifr->ifr_addr.sa_family == AF_INET) 
  	{
	    if(strncmp(ifr->ifr_name, "ppp", 3) == 0)
		continue;

	    ina = SIN_ADDR(ifr->ifr_addr);
	    strlcpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));
	    DBG(zlog_info("examining interface %s", ifreq.ifr_name));

	    /* Check that the interface is up, and not point-to-point nor loopback. */
	    if (ioctl(sock_ioctl, SIOCGIFFLAGS, &ifreq) < 0)
		continue;

	    if (((ifreq.ifr_flags ^ FLAGS_GOOD) & FLAGS_MASK) != 0)
		continue;
	
	    /* Get its netmask and check that it's on the right subnet. */
	    if (ioctl(sock_ioctl, SIOCGIFNETMASK, &ifreq) < 0)
		continue;

	    mask = SIN_ADDR(ifreq.ifr_addr);
	    DBG(zlog_info("interface addr %s mask %d", ip_ntoa(ina), ntohl(mask)));

	    if (((ipaddr ^ ina) & mask) != 0)
		continue; /* no match */

	    /* matched */
	    if (mask >= bestmask) 
 	    {
		/* Compare using >= instead of > -- it is possible for
		   an interface to have a netmask of 0.0.0.0 */
		found_interface = 1;
		bestifreq = ifreq;
		bestmask = mask;
	    }
	}
    }

    if (!found_interface) 
	return -1;

    strlcpy(name, bestifreq.ifr_name, namelen);

#if 0
    /* trim off the :1 in eth0:1 */
    aliasp = strchr(name, ':');
    if (aliasp != 0)
	*aliasp = 0;
#endif

    DBG(zlog_info("Found interface %s for ip %s", name, ip_ntoa(ipaddr)));
   
    /* Now get the hardware address */
    memset (&bestifreq.ifr_hwaddr, 0, sizeof (struct sockaddr));
    if (ioctl (sock_ioctl, SIOCGIFHWADDR, &bestifreq) < 0) {
	zlog_err("Get interface hardware address failed: %s", strerror(errno));
	return -1;
    }

    memcpy (hwaddr, &bestifreq.ifr_hwaddr, sizeof (struct sockaddr));

    DBG(zlog_info("found hwaddr %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
		(int) ((unsigned char *) &hwaddr->sa_data)[0],
		(int) ((unsigned char *) &hwaddr->sa_data)[1],
		(int) ((unsigned char *) &hwaddr->sa_data)[2],
		(int) ((unsigned char *) &hwaddr->sa_data)[3],
		(int) ((unsigned char *) &hwaddr->sa_data)[4],
		(int) ((unsigned char *) &hwaddr->sa_data)[5],
		(int) ((unsigned char *) &hwaddr->sa_data)[6],
		(int) ((unsigned char *) &hwaddr->sa_data)[7]));
    return 0;
}


