#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/inet_ip.h>
#include <lib/ether.h>
#include <lib/vty.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include "ftm/pkt_icmp.h"
#include "ping_cmd.h"
#include "ping.h"
#include "ping6.h"


/**
 * @brief      <+IP trace route command+>
 * @param[in ] <+dip、sip、maxttl、send pkt count、timeout value、packet size、vpn+>
 * @param[out] <+result about link status+>
 * @return     <+errno+>
 * @author     jinlei sun
 * @date       Tue Apr 10 17:40:21 CST 2018
 * @note       <+this function include icmp and udp probe method+>
 */
DEFUN(tracert,
  	tracert_cmd,
  	"tracert {ip} A.B.C.D {-a A.B.C.D|-m <1-255>|-c <1-255>|-w <1-30>|-s <20-9500>|vpn <1-1024>}",
  	"tracert\n"
  	"Tracert ip address\n"
  	"ip address\n"
    "Set source IP address, the default is the IP address of the output interface\n"
	"source-ip-address\n"		
	"Max time to live, the default is 30\n"
	"max-ttl\n"	
  	"Number of probe packet, the default is 3\n"
	"count value\n"	
  	"Timeout in seconds to wait for each reply, the default is 1s\n"
  	"timeout value\n"
	"Specify the number of data bytes to be sent, the default is 12bytes\n"
	"packet size\n"
  	"Vpn\n"
  	"vpn value\n"	
  	)
{
	uint32_t time = 1;
	uint32_t size = 20;
	uint32_t vpn = 0;	
    struct inet_addr dip;
    struct inet_addr sip;		
	uint32_t maxttl = 30;	
	uint32_t count = 3;
    uint32_t buffer_len = 0;
    uint8_t  ip_lowbit = 0;
    char addr[20];
    struct ping_info *pingInfo = NULL;

    memset(&sip, 0, sizeof(struct inet_addr));
    memset(&dip, 0, sizeof(struct inet_addr));
    dip.addr.ipv4 = inet_strtoipv4((char *)argv[1]);
    inet_ipv4tostr(dip.addr.ipv4, addr);
    
	if(NULL != argv[2])
	{
        sip.addr.ipv4 = inet_strtoipv4((char *)argv[2]);
	}
	
	if(NULL != argv[3])
	{	
		maxttl = atoi(argv[3]);				
	}	

	if(NULL != argv[4])
	{	
		count = atoi(argv[4]);				
	}
    
	if(NULL != argv[5])
	{	
		time = atoi(argv[5]);				
	}
    
	if(NULL != argv[6])
	{	
		size = atoi(argv[6]);				
	}
    
	if (NULL != argv[7])
	{		
    	vpn = (uint32_t)atoi(argv[7]);
	}

    if (vpn > VPN_SUPPORT)
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }

	/* 127.0.0.0为一般故障 */
	if(dip.addr.ipv4 == 0x7F000000)
	{
		vty_error_out(vty, "General fault!%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

    if ((0 != sip.addr.ipv4) && !inet_valid_ipv4(sip.addr.ipv4))
    {
        vty_error_out(vty,"Sip is invalid! Please check out input!%s",VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    ip_lowbit = sip.addr.ipv4 & (0x000000ff);
    if ((0 != sip.addr.ipv4) && ((0 == ip_lowbit) || (0xff == ip_lowbit)))
    {
        vty_warning_out(vty,"The specified sip maybe invalid!%s",VTY_NEWLINE);
    }
    
    if (!inet_valid_ipv4(dip.addr.ipv4))
    {
        vty_error_out(vty,"Dip is invalid! Please check out input!%s",VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    pingInfo = XMALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == pingInfo )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    memset(pingInfo, 0, sizeof(struct ping_info));

    pingInfo->waittime = time;
    pingInfo->size = size;
	pingInfo->vpn = vpn;
    pingInfo->ttl = maxttl;	
    pingInfo->destip.addr.ipv4 = dip.addr.ipv4;
	pingInfo->srcip.addr.ipv4 = sip.addr.ipv4;
    pingInfo->pvty = vty;
    pingInfo->count = count;
    pingInfo->type = TRACE_CMD;

    /*回显字符串长度(ttl 3byte + 时间7byte/个+ 回显ip 16byte)
        如: 30 1111ms 192.192.192.192 */
    buffer_len = ECHO_MAX_STR_LEN(count) ; 

    /*统计信息结构体初始化*/
    pingInfo->stats = XMALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == pingInfo->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    memset(pingInfo->stats,0,sizeof(struct ping_cnt));

    /*每跳trace 结果缓存区域 */
    pingInfo->stats->buffer = XMALLOC(MTYPE_PING,buffer_len);
    if(NULL == pingInfo->stats->buffer)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    memset(pingInfo->stats->buffer,0,buffer_len);
    pingInfo->stats->ip = 1;
    
    //icmp_ping_send_request(pingInfo);//发ip报文 

	pingInfo->type = TRACE_UDP_CMD;
	vty_out(vty,"Tracing route to %s over a maximum of %d hops%s",addr,maxttl,VTY_NEWLINE);
	usleep(500000);
	trace_udp_send(pingInfo);//发送udp报文

    return CMD_WAIT;
}



/**
 * @brief      <+IP ping command+>
 * @param[in ] <+dip、sip、send pkt count、specified ttl、timeout value、packet size、frag flag、vpn+>
 * @param[out] <+result about link status+>
 * @return     <+errno+>
 * @author     jinlei sun
 * @date       Tue Apr 10 17:40:21 CST 2018
 * @note       <+icmp ping+>
 */
DEFUN(ping,
        ping_cmd,
        "ping {ip} A.B.C.D {-a A.B.C.D|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|-f <0-1>|vpn <1-1024>}",
        "ping\n"
        "Ping ip address\n"
        "Ip address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Set Don't Fragment flag in packet (IPv4-only)\n"
        "frag flag 0:frag 1:no frag\n"  
        "Vpn\n"
        "vpn value\n"    
        )
{
    uint32_t count = 5;
    uint32_t time = 1;
    uint32_t size = 20; 
    uint16_t vpn = 0;   
    uint32_t ttl = 255;
    uint32_t fragflag = 0;
    uint32_t dip = 0;
    uint32_t sip = 0;
    struct ping_info *pingInfo = NULL;
    char addr[20];  
    uint8_t ip_lowbit = 0;
    
    memset(&sip, 0, sizeof(struct inet_addr));
    memset(&dip, 0, sizeof(struct inet_addr));
    dip = inet_strtoipv4((char *)argv[1]);
    inet_ipv4tostr(dip, addr);
    
    if(NULL != argv[2])
    {
        sip = inet_strtoipv4((char *)argv[2]);
    }   
        
    if(NULL != argv[3])
    {
        count = atoi(argv[3]);       
    }
        
    if(NULL != argv[4])
    {   
        ttl = atoi(argv[4]);               
    }   
           
    if(NULL != argv[5])
    {   
        time = atoi(argv[5]);             
    }
    
    if(NULL != argv[6])
    {
        size = atoi(argv[6]);                  
    }
        
    if(NULL != argv[7])
    {
        fragflag = atoi(argv[7]);   
    }   
    if(NULL != argv[8])
    {       
        vpn = (uint32_t)atoi(argv[8]);
    }   
    
    if (vpn > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    /* 127.0.0.0为一般故障 */
    if(dip == 0x7F000000)
    {
        vty_error_out(vty, "General fault!%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    if ((0 != sip) && !inet_valid_ipv4(sip))
    {
        vty_error_out(vty,"Sip is invalid! Please check out input!%s",VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    ip_lowbit = sip & (0x000000ff);
    if ((0 != sip) && ((0 == ip_lowbit) || (0xff == ip_lowbit)))
    {
        vty_warning_out(vty,"The specified sip maybe invalid!%s",VTY_NEWLINE);
    }
    
    if (!inet_valid_ipv4(dip))
    {
        vty_error_out(vty,"Dip is invalid! Please check out input!%s",VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    /*填充控制信息结构*/
    pingInfo = XMALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == pingInfo )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    memset(pingInfo, 0, sizeof(struct ping_info));

    pingInfo->waittime = time;
    pingInfo->size = size;
	pingInfo->vpn = vpn;
    pingInfo->ttl = ttl;	
    pingInfo->destip.addr.ipv4 = dip;
	pingInfo->srcip.addr.ipv4 = sip;
    pingInfo->pvty = vty;
    pingInfo->count = count;
    pingInfo->type = PING_CMD;

	if(fragflag == 1)
    {
		pingInfo->fragflag = 0x4000;
	}
    
    /*统计信息结构体初始化*/
    pingInfo->stats = XMALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == pingInfo->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    memset(pingInfo->stats,0,sizeof(struct ping_cnt));
    
    vty_out(vty,"Ping %s: %d data bytes,press CTRL_C to break%s",addr,size, VTY_NEWLINE);
	usleep(500000);
	
	/*发送第一个icmp 请求包*/
    icmp_ping_send_request(pingInfo);

	return CMD_WAIT;


}


/**
 * @brief      <+IPv6 ping command+>
 * @param[in ] <+dip6、sip6、send pkt countp、specified ttl、timeout value、packet size、frag flag、vpn+>
 * @param[out] <+result about link status+>
 * @return     <+errno+>
 * @author     jinlei sun
 * @date       Tue Apr 10 17:40:21 CST 2018
 * @note       <+icmpv6 ping+>
 */
DEFUN(ping6,
        ping6_cmd,
        "ping ipv6 X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>}",
        "ping\n"
        "Ping ipv6\n"
        "Ipv6 address format X:X:X:X:X:X:X:X\n"
        "Select source Ipv6 address, the default is the Ipv6 address of the output interface\n"
        "source-ipv6-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value\n"    
        )
{
    uint32_t count = 5;
    uint32_t time = 1;
    uint32_t size = 20; 
    uint16_t vpn = 0;   
    uint32_t ttl = 255;
    struct ipv6_addr dip6;
    struct ipv6_addr sip6;
    struct ping_info *ping6Info = NULL;
    int8_t ret = 0;

    memset(&sip6, 0, sizeof(struct ipv6_addr));
    memset(&dip6, 0, sizeof(struct ipv6_addr));
    
    ret = ndp_ipv6_str_check((char *)argv[0]);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty, "The destination IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (0 == inet_pton (AF_INET6, (char *)argv[0], dip6.ipv6))
	{
        vty_error_out(vty, "The destination IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if (ipv6_is_zero(&dip6))
	{
		vty_error_out(vty, "Destination address cannot be the unspecified address!\n%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    if(NULL != argv[1])
    {
        ret = ndp_ipv6_str_check((char *)argv[1]);
	    if (ERRNO_SUCCESS != ret)
	    {
            vty_error_out(vty, "The source IPV6 address is invalid.%s", VTY_NEWLINE);
		    return CMD_WARNING;
	    }
    
        if (0 == inet_pton (AF_INET6, (char *)argv[1], sip6.ipv6))
		{
            vty_error_out(vty, "The source IPV6 address is invalid.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

        if (ERRNO_FAIL == ipv6_sip_check(&sip6, vty))
        {
            return CMD_WARNING;
        }

        if (ipv6_is_linklocal(&dip6) && !ipv6_is_linklocal(&sip6) && !ipv6_is_sitelocal(&sip6) 
            && !ipv6_is_multicast(&sip6) && !ipv6_is_zero(&sip6))
        {
            vty_error_out(vty,"The destination IPv6 address must be a global unicast address.%s",VTY_NEWLINE);
            return CMD_WARNING;
        }
    }   
        
    if(NULL != argv[2])
    {
        count = atoi(argv[2]);       
    }
        
    if(NULL != argv[3])
    {   
        ttl = atoi(argv[3]);               
    }   
           
    if(NULL != argv[4])
    {   
        time = atoi(argv[4]);             
    }
    
    if(NULL != argv[5])
    {
        size = atoi(argv[5]);                  
    }
        
    if(NULL != argv[6])
    {       
        vpn = (uint32_t)atoi(argv[6]);
    }   
   
    if (vpn > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    if (ipv6_is_linklocal(&dip6))
    {
        vty_error_out(vty,"Please specify an interface name for the link-local address!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(ipv6_is_loopback(&dip6))
    {
        vty_error_out(vty, "General fault!%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    /*填充控制信息结构*/
    ping6Info = XMALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == ping6Info )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return ERRNO_MALLOC;
    }
    memset(ping6Info, 0, sizeof(struct ping_info));

    ping6Info->waittime = time;
    ping6Info->size = size;
	ping6Info->vpn = vpn;
    ping6Info->ttl = ttl;	
    IPV6_ADDR_COPY(ping6Info->destip.addr.ipv6, &dip6);
    IPV6_ADDR_COPY(ping6Info->srcip.addr.ipv6, &sip6);
    ping6Info->pvty = vty;
    ping6Info->count = count;
    ping6Info->type = PING6_CMD;

    
    /*统计信息结构体初始化*/
    ping6Info->stats = XMALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == ping6Info->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return ERRNO_MALLOC;
    }
    memset(ping6Info->stats,0,sizeof(struct ping_cnt));
    
    vty_out(vty,"Ping %s: %d data bytes,press CTRL_C to break%s", argv[0], size, VTY_NEWLINE);
	usleep(500000);
	
	/*发送第一个icmp 请求包*/
	icmpv6_ping_send_request(ping6Info);

	
	return CMD_WAIT;


}



/**
 * @brief      <+IPv6 ping command+>
 * @param[in ] <+dip6、sip6、send pkt countp、specified ttl、timeout value、packet size、frag flag、vpn、packet outif+>
 * @param[out] <+result about link status+>
 * @return     <+errno+>
 * @author     jinlei sun
 * @date       Tue Apr 10 17:40:21 CST 2018
 * @note       <+icmpv6 ping+>
 */
DEFUN(ping6_intf,
        ping6_intf_cmd,
        "ping ipv6  X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>} interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK | vlanif <1-4094>}",
        "ping\n"
        "Ping ipv6 address\n"
        "Ipv6 address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"       
        "Specify the number of echo requests to be sent, the default is 3\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value <1-128>\n"
        INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    	CLI_INTERFACE_TRUNK_STR
    	CLI_INTERFACE_TRUNK_VHELP_STR
    	CLI_INTERFACE_VLANIF_STR
    	CLI_INTERFACE_VLANIF_VHELP_STR
        )
{
    uint32_t count = 5;
    uint32_t time = 1;
    uint32_t size = 20; 
    uint16_t vpn = 0;   
    uint32_t ttl = 255;
    uint32_t ifindex = 0;
    struct ipv6_addr dip6;
    struct ipv6_addr sip6;
    struct ping_info *ping6Info = NULL;  
    int8_t ret = 0;

    memset(&sip6, 0, sizeof(struct ipv6_addr));
    memset(&dip6, 0, sizeof(struct ipv6_addr));
    
    ret = ndp_ipv6_str_check((char *)argv[0]);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty, "The destination IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (0 == inet_pton (AF_INET6, (char *)argv[0], dip6.ipv6))
	{
        vty_error_out(vty, "The destination IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if (ipv6_is_zero(&dip6))
	{
		vty_error_out(vty, "Destination address cannot be the unspecified address!\n%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
    if(NULL != argv[1])
    {
        ret = ndp_ipv6_str_check((char *)argv[0]);
	    if (ERRNO_SUCCESS != ret)
	    {
            vty_error_out(vty, "The source IPV6 address is invalid.%s", VTY_NEWLINE);
		    return CMD_WARNING;
	    }

    	if (0 == inet_pton (AF_INET6, (char *)argv[1], sip6.ipv6))
		{
            vty_error_out(vty, "The source IPV6 address is invalid.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
    
        if (ERRNO_FAIL == ipv6_sip_check(&sip6, vty))
        {
            return CMD_WARNING;
        }
        
        if (ipv6_is_linklocal(&dip6) && !ipv6_is_linklocal(&sip6) && !ipv6_is_sitelocal(&sip6) 
            && !ipv6_is_multicast(&sip6) && !ipv6_is_zero(&sip6))
        {
            vty_error_out(vty,"The destination IPv6 address must be a global unicast address.%s",VTY_NEWLINE);
            return CMD_WARNING;
        }
    }   
        
    if(NULL != argv[2])
    {
        count = atoi(argv[2]);       
    }
        
    if(NULL != argv[3])
    {   
        ttl = atoi(argv[3]);               
    }   
           
    if(NULL != argv[4])
    {   
        time = atoi(argv[4]);             
    }
    
    if(NULL != argv[5])
    {
        size = atoi(argv[5]);                  
    }

    if(NULL != argv[6])
    {       
        vpn = (uint32_t)atoi(argv[6]);
    }   

    if (NULL != argv[7] )
    {
	    ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[7]);
    }
    else if(NULL != argv[8])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[8]);
    }
    else if(NULL != argv[9])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[9]);
    }
	else if(NULL != argv[10])
	{
		ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[10]);
	}
	else if(NULL != argv[11])
	{
		ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[11]);
	}

    if (!ipv6_is_linklocal(&dip6) && (ifindex != 0))
    {
        vty_error_out(vty,"No need to specify an interface for the global unicast address!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ipv6_is_linklocal(&dip6) && (ifindex == 0))
    {
        vty_error_out(vty,"Please specify an interface name for the link-local address!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    

    /*填充控制信息结构*/
    ping6Info = XMALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == ping6Info )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    memset(ping6Info, 0, sizeof(struct ping_info));

    ping6Info->time = time;
    ping6Info->size = size;
	ping6Info->vpn = vpn;
    ping6Info->ttl = ttl;	
    IPV6_ADDR_COPY(ping6Info->destip.addr.ipv6, &dip6);
    IPV6_ADDR_COPY(ping6Info->srcip.addr.ipv6, &sip6);
    ping6Info->pvty = vty;
    ping6Info->count = count;
    ping6Info->type = PING6_CMD;
    ping6Info->ifindex = ifindex;

    
    /*统计信息结构体初始化*/
    ping6Info->stats = XMALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == ping6Info->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    memset(ping6Info->stats, 0, sizeof(struct ping_cnt));
    
    vty_out(vty,"Ping %s: %d data bytes,press CTRL_C to break%s", argv[0], size, VTY_NEWLINE);
	usleep(500000);
	/*发送第一个icmp 请求包*/
    icmpv6_ping_send_request(ping6Info);  

	return CMD_WAIT;

}

DEFUN (ping_debug_fun,
	ping_debug_cmd,
	"debug ping (enable|disable)",
	"Debug information to moniter\n"
	"Programe name\n"
	"PING debug enable\n"
	"PING debug disable\n")
{
	int zlog_num = 0;
	
	
	if(!strncmp(argv[1], "ping", 2))
	{
		zlog_debug_set( vty, PING_DBG_ALL, !strncmp(argv[0], "enable", 3));

		return CMD_SUCCESS;
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ping_debug_fun,
		 show_ping_debug_cmd,
		 "show pingd debug",
		 SHOW_STR
		 "PING"
		 "Debug status\n")
{


	int type_num = 0;

	vty_out(vty, "debug type		  status %s", VTY_NEWLINE);

	
	vty_out(vty, "%-15s	  %-10s %s", "pingd", 
		!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	

	return CMD_SUCCESS;
}


#if 0
DEFUN(h3c_ping,
      h3c_ping_cmd,
      "ping {-a A.B.C.D|-c <1-4294967295>|-f |-h <1-255> |-m <1-65535> |-p <0-FFFFFFFF> |-q |-r |-s <20-8100> |-t <0-65535> |-tos <0-255> |-vpn-instance STRING<1-31>} A.B.C.D",
      "Ping function\n"
      "Specify the source IP address\n"
      "Source IP address\n"
      "Specify the number of echo requests\n"
      "Number of echo requests\n"
      "Specify packets not to be fragmented\n"
      "Specify the TTL value\n"
      "TTL value\n"
      "Specify the interval for sending echo requests\n"
      "No more than 8 ""pad"" hexadecimal characters to fill out the sent packet. For example, -p f2 will fill the sent packet with 000000f2 repeatedly\n"
      "Display only summary\n"
      "Record route. Include the RECORD_ROUTE option in the ECHO_REQUEST packets and display the route\n"
      "Specify the payload length\n"
      "Payload length in bytes\n"
      "Specify the wait time for each reply\n"
      "Wait time in milliseconds"
      "Specify the TOS value\n"
      "Specify a VPN instance\n"
      "VPN instance name\n"
      "IP address\n"
     )
{
    #if 0
    uint8_t i = 0;
    printf("argc:%d\n",argc);
    for(i = 0;i<argc;i++)
        printf("argc[%d]:%s  ",i,argv[i]);
    printf("\n");
    #endif

    struct ping_info *pingInfo = NULL;
    uint32_t dip = 0,sip = 0;
    uint32_t count = 5;
    uint8_t  frag = 0;
    uint8_t  ttl = 0xff;
    //uint16_t interval = 0;
    //uint32_t pad = 0;
    //uint8_t  echo_sunmmary = 0;
    //uint8_t  record_route = 0;
    uint16_t pkt_size = 56;
    uint16_t timeout = 2;
    //uint8_t  tos = 0;
    uint32_t vpn = 0;

    dip = inet_strtoipv4((char *)argv[12]); 
    if (NULL != argv[0])
    {
        sip = inet_strtoipv4((char *)argv[0]);
    }

    if (NULL != argv[1])
    {
        count = atoi(argv[1]);       
    }

    if (NULL != argv[2])
    {
        frag = 1;
    }

    if (NULL != argv[3])
    {
        ttl = atoi(argv[3]);
    }

    if (NULL != argv[4])
    {
        //interval = atoi(argv[4]);
    }

    if (NULL != argv[5]) 
    {
        //pad = atoi(argv[5]);
    }
    
    if (NULL != argv[6])
    {
        //echo_sunmmary = 1;        
    }

    if (NULL != argv[7])
    {
        //record_route = 1;
    }

    if (NULL != argv[8])
    {
        pkt_size = atoi(argv[8]); 
    }

    if (NULL != argv[9])
    {
        timeout = atoi(argv[9]); 
    }

    if (NULL != argv[10])
    {
        //tos = atoi(argv[10]);
    }

    if (NULL != argv[11])
    {
        vpn = atoi(argv[11]);
    }

    /*填充控制信息结构*/
    pingInfo = XCALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == pingInfo )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    pingInfo->srcip.addr.ipv4 = sip;
    pingInfo->destip.addr.ipv4 = dip;
    pingInfo->count = count;
    pingInfo->fragflag = frag;
    pingInfo->ttl = ttl;
    //pingInfo->interval = interval;
    //pingInfo->pad = pad;
    //pingInfo->echo_sunmmary = echo_sunmmary;
    //pingInfo->record_route_flag = record_route;
    pingInfo->size = pkt_size;
    pingInfo->time = timeout;
    //pingInfo->tos = tos;
    pingInfo->vpn = vpn;
    pingInfo->pvty = vty;
    pingInfo->type = PING_CMD;

    /*统计信息结构体初始化*/
    pingInfo->stats = XCALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == pingInfo->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    icmp_ping_send_request(pingInfo);
    
    vty_out(vty,"Ping %s: %d data bytes,press CTRL_C to break%s",argv[12], pkt_size, VTY_NEWLINE);

	return CMD_WAIT;
}


DEFUN(h3c_ping_sb_ip,
      h3c_ping_sb_ip_cmd,
      "ping ip {-a A.B.C.D|-c <1-4294967295>|-f |-h <1-255> |-m <1-65535> |-p <0-FFFFFFFF> |-q |-r |-s <20-8100> |-t <0-65535> |-tos <0-255> |-vpn-instance STRING<1-31>} A.B.C.D",
      "Ping function\n"
      "IP information\n"
      "Specify the source IP address\n"
      "Source IP address\n"
      "Specify the number of echo requests\n"
      "Number of echo requests\n"
      "Specify packets not to be fragmented\n"
      "Specify the TTL value\n"
      "TTL value\n"
      "Specify the interval for sending echo requests\n"
      "No more than 8 ""pad"" hexadecimal characters to fill out the sent packet. For example, -p f2 will fill the sent packet with 000000f2 repeatedly\n"
      "Display only summary\n"
      "Record route. Include the RECORD_ROUTE option in the ECHO_REQUEST packets and display the route\n"
      "Specify the payload length\n"
      "Payload length in bytes\n"
      "Specify the wait time for each reply\n"
      "Wait time in milliseconds"
      "Specify the TOS value\n"
      "Specify a VPN instance\n"
      "VPN instance name\n"
      "IP address\n"
     )
{
#if 0
    uint8_t i = 0;
    printf("argc:%d\n",argc);
    for(i = 0;i<argc;i++)
        printf("argc[%d]:%s  ",i,argv[i]);
    printf("\n");
#endif

    struct ping_info *pingInfo = NULL;
    uint32_t dip = 0,sip = 0;
    uint32_t count = 5;
    uint8_t  frag = 0;
    uint8_t  ttl = 0xff;
    //uint16_t interval = 0;
    //uint32_t pad = 0;
    //uint8_t  echo_sunmmary = 0;
    //uint8_t  record_route = 0;
    uint16_t pkt_size = 56;
    uint16_t timeout = 2;
    //uint8_t  tos = 0;
    uint32_t vpn = 0;

    dip = inet_strtoipv4((char *)argv[12]); 
    if (NULL != argv[0])
    {
        sip = inet_strtoipv4((char *)argv[0]);
    }

    if (NULL != argv[1])
    {
        count = atoi(argv[1]); 
    }

    if (NULL != argv[2])
    {
        frag = 1;
    }

    if (NULL != argv[3])
    {
        ttl = atoi(argv[3]);
    }

    if (NULL != argv[4])
    {
        //interval = atoi(argv[4]);
    }

    if (NULL != argv[5]) 
    {
        //pad = atoi(argv[5]);
    }
    
    if (NULL != argv[6])
    {
        //echo_sunmmary = 1;        
    }

    if (NULL != argv[7])
    {
        //record_route = 1;
    }

    if (NULL != argv[8])
    {
        pkt_size = atoi(argv[8]); 
    }

    if (NULL != argv[9])
    {
        timeout = atoi(argv[9]); 
    }

    if (NULL != argv[10])
    {
        //tos = atoi(argv[10]);
    }

    if (NULL != argv[11])
    {
        vpn = atoi(argv[11]);
    }

    /*填充控制信息结构*/
    pingInfo = XCALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == pingInfo )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    pingInfo->srcip.addr.ipv4 = sip;
    pingInfo->destip.addr.ipv4 = dip;
    pingInfo->count = count;
    pingInfo->fragflag = frag;
    pingInfo->ttl = ttl;
    //pingInfo->interval = interval;
    //pingInfo->pad = pad;
    //pingInfo->echo_sunmmary = echo_sunmmary;
    //pingInfo->record_route_flag = record_route;
    pingInfo->size = pkt_size;
    pingInfo->time = timeout;
    //pingInfo->tos = tos;
    pingInfo->vpn = vpn;
    pingInfo->pvty = vty;
    pingInfo->type = PING_CMD;

    /*统计信息结构体初始化*/
    pingInfo->stats = XCALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == pingInfo->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    icmp_ping_send_request(pingInfo);
    
    vty_out(vty,"Ping %s: %d data bytes,press CTRL_C to break%s",argv[12], pkt_size, VTY_NEWLINE);

	return CMD_WAIT;
}

DEFUN(h3c_trace,
      h3c_trace_cmd,
      //"tracert {-a A.B.C.D |-f <1-255> |-m <1-255> |-p <1-65535> |-q <1-65535> |-t <0-255> |-w <1-65535>|-vpn-instance STRING<1-31>[-resolve-as (global|none|vpn)]} A.B.C.D",
      "tracert {-a A.B.C.D |-f <1-255> |-m <1-255> |-p <1-65535> |-q <1-65535> |-t <0-255> |-w <1-65535>|-vpn-instance STRING<1-31>} A.B.C.D",
      "Tracert function\n"
      "Specify the source IP address used by TRACERT\n"
      "Source IP address\n"
      "Specify the TTL value for the first packet\n"
      "TTL value for the first packet\n"
      "Specify the maximum TTL value\n"
      "Maximum TTL value\n"
      "Specify the destination UDP port number\n"
      "Destination UDP port number\n"
      "Specify the number of probe packets sent each time\n"
      "Number of probe packets\n"
      "Set the Type of Service (ToS) value\n"
      "ToS value\n"
      "Set the timeout to wait for each reply\n"
      "Timeout in milliseconds\n"
      "IP address or hostname of the destination device\n"
    )
{
#if 0
    uint8_t i = 0;
    printf("argc:%d\n",argc);
    for(i = 0;i<argc;i++)
        printf("argc[%d]:%s  ",i,argv[i]);
    printf("\n");
#endif
   
    uint32_t sip = 0,dip = 0;
    //uint8_t  first_pkt_ttl = 0;
    uint8_t max_ttl = 30;
    //uint16_t dst_port = 0;
    uint8_t  count = 3;
    //uint8_t  tos = 0;
    uint8_t  timeout = 1;
    uint32_t vpn = 0;
    struct ping_info *pingInfo = NULL;
    uint8_t buffer_len = 0;
    uint8_t size = 20;

    if (NULL != argv[8])
    {
        dip = inet_strtoipv4((char *)argv[8]);
    }

    if (NULL != argv[0])
    {
        dip = inet_strtoipv4((char *)argv[0]);
    }

    if (NULL != argv[1])
    {
        //first_pkt_ttl = atoi(argv[1]);
    }

    if (NULL != argv[2])
    {
        max_ttl = atoi(argv[2]);
    }

    if (NULL != argv[3])
    {
        //dst_port = atoi(argv[3]);
    }

    if (NULL != argv[4])
    {
        count = atoi(argv[4]);
    }

    if (NULL != argv[5])
    {
        //tos = atoi(argv[5]);
    }

    if (NULL != argv[6])
    {
        timeout = atoi(argv[6]);
    }

    if (NULL != argv[7])
    {
        vpn = atoi(argv[7]);
    }

    pingInfo = XCALLOC(MTYPE_PING,sizeof(struct ping_info));
    if ( NULL == pingInfo )
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    pingInfo->srcip.addr.ipv4 = sip;
    pingInfo->destip.addr.ipv4 = dip;
    //pingInfo->first_pkt_ttl = first_pkt_ttl;
    pingInfo->ttl = max_ttl;
    //pingInfo->dst_port = dst_port;
    pingInfo->count = count;
    //pingInfo->tos = tos;
    pingInfo->time = timeout;
    pingInfo->vpn = vpn;
    pingInfo->type = TRACE_UDP_CMD;
    pingInfo->pvty = vty;
    pingInfo->size = size;
    
    pingInfo->stats = XCALLOC(MTYPE_PING,sizeof(struct ping_cnt));
    if(NULL == pingInfo->stats)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    /*每跳trace 结果缓存区域 */
    buffer_len = ECHO_MAX_STR_LEN(count); 
    pingInfo->stats->buffer = XCALLOC(MTYPE_PING,buffer_len);
    if(NULL == pingInfo->stats->buffer)
    {
        vty_error_out(vty, "Malloc failure.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    pingInfo->stats->ip = 1;
    
	trace_udp_send(pingInfo);//发送udp报文
    
    vty_out(vty,"Tracing route to %s over a maximum of %d hops%s",argv[8],max_ttl,VTY_NEWLINE);

    return CMD_WAIT;
}
#endif


#if 0
/*unused just for blablabla..........
 *ipv6 global unicast :  2000::/3 (range 2000::0~~~~3fff:ffff....ffff)
 * */
static uint8_t ping6_ipv6_is_global_unicast(struct ipv6_addr *ipv6addr)
{
    return ((ipv6addr->ipv6[0] & 0xe0) == 0x20);
}
#endif

uint8_t ipv6_sip_check(struct ipv6_addr *sip6,struct vty *vty)
{
    if ((NULL == sip6) || (NULL == vty))
    {
        return ERRNO_FAIL;
    }
    
    if (ipv6_is_zero(sip6))
    {
        vty_error_out(vty,"Source IPv6 address is unspecified.%s",VTY_NEWLINE);
        return ERRNO_FAIL;
    }

    if (ipv6_is_sitelocal(sip6) || ipv6_is_loopback(sip6))
    {
        vty_error_out(vty,"Invalid source IPv6 address.%s",VTY_NEWLINE);
        return ERRNO_FAIL;
    }

    if (ipv6_is_linklocal(sip6) || ipv6_is_multicast(sip6))
    {
        vty_error_out(vty,"Source IPv6 address must be a global unicast address.%s",VTY_NEWLINE);
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      <+ping command init+>
 * @param[in ] <+none+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Tue Apr 10 17:40:21 CST 2018
 * @note       <+install all cmd+>
 */
void ping_CmdInit(void)
{
	install_element (CONFIG_NODE, &ping_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &tracert_cmd, CMD_LOCAL);	
    install_element (CONFIG_NODE, &ping6_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &ping6_intf_cmd, CMD_LOCAL);

	
	install_element (CONFIG_NODE, &ping_debug_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ping_debug_cmd, CMD_LOCAL);
#if 0
    install_element (CONFIG_NODE, &h3c_ping_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &h3c_ping_sb_ip_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &h3c_trace_cmd, CMD_LOCAL);
#endif
}


